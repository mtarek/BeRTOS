#!/usr/bin/env python
# encoding: utf-8
#
# Copyright 2008 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# $Id:$
#
# Author: Lorenzo Berni <duplo@develer.com>
#

import os
import fnmatch
import glob
import re
import shutil

import const
import codelite_project
import DefineException

def isBertosDir(directory):
   return os.path.exists(directory + "/VERSION")

def bertosVersion(directory):
   return open(directory + "/VERSION").readline().strip()

def createBertosProject(project_info):
    directory = project_info.info("PROJECT_PATH")
    sources_dir = project_info.info("SOURCES_PATH")
    if not os.path.isdir(directory):
        os.mkdir(directory)
    f = open(directory + "/project.bertos", "w")
    f.write(repr(project_info))
    f.close()
    ## Destination source dir
    srcdir = directory + "/bertos"
    shutil.rmtree(srcdir, True)
    shutil.copytree(sources_dir + "/bertos", srcdir)
    ## Destination makefile
    makefile = directory + "/Makefile"
    if os.path.exists(makefile):
        os.remove(makefile)
    makefile = open("mktemplates/Makefile").read()
    makefile = makefileGenerator(project_info, makefile)
    open(directory + "/Makefile", "w").write(makefile)
    ## Destination project dir
    prjdir = directory + "/" + os.path.basename(directory)
    shutil.rmtree(prjdir, True)
    os.mkdir(prjdir)
    ## Destination configurations files
    cfgdir = prjdir + "/cfg"
    shutil.rmtree(cfgdir, True)
    os.mkdir(cfgdir)
    for configuration, information in project_info.info("CONFIGURATIONS").items():
        string = open(sources_dir + "/" + configuration, "r").read()
        for start, parameter in information["paramlist"]:
            infos = information[parameter]
            value = infos["value"]
            if "type" in infos["informations"] and infos["informations"]["type"] == "autoenabled":
                value = "1"
            if "unsigned" in infos["informations"].keys() and infos["informations"]["unsigned"]:
                value += "U"
            if "long" in infos["informations"].keys() and infos["informations"]["long"]:
                value += "L"
            string = sub(string, parameter, value)
        f = open(cfgdir + "/" + os.path.basename(configuration), "w")
        f.write(string)
        f.close()
    ## Destinatio mk file
    makefile = open("mktemplates/template.mk", "r").read()
    makefile = mkGenerator(project_info, makefile)
    open(prjdir + "/" + os.path.basename(prjdir) + ".mk", "w").write(makefile)
    ## Destination main.c file
    main = open("srctemplates/main.c", "r").read()
    open(prjdir + "/main.c", "w").write(main)
    if "codelite" in project_info.info("OUTPUT"):
        workspace = codeliteWorkspaceGenerator(project_info)
        open(directory + "/" + os.path.basename(prjdir) + ".workspace", "w").write(workspace)
        project = codeliteProjectGenerator(project_info)
        open(directory + "/" + os.path.basename(prjdir) + ".project", "w").write(project)

def mkGenerator(project_info, makefile):
    """
    Generates the mk file for the current project.
    """
    mk_data = {}
    mk_data["$pname"] = os.path.basename(project_info.info("PROJECT_PATH"))
    mk_data["$cpuname"] = project_info.info("CPU_INFOS")["CORE_CPU"]
    mk_data["$cflags"] = " ".join(project_info.info("CPU_INFOS")["C_FLAGS"])
    mk_data["$ldflags"] = " ".join(project_info.info("CPU_INFOS")["LD_FLAGS"])
    mk_data["$csrc"], mk_data["$pcsrc"], mk_data["$asrc"], mk_data["$constants"] = csrcGenerator(project_info)
    mk_data["$prefix"] = project_info.info("TOOLCHAIN")["path"].split("gcc")[0]
    mk_data["$suffix"] = project_info.info("TOOLCHAIN")["path"].split("gcc")[1]
    mk_data["$cross"] = project_info.info("TOOLCHAIN")["path"].split("gcc")[0]
    mk_data["$main"] = os.path.basename(project_info.info("PROJECT_PATH")) + "/main.c"
    for key in mk_data:
        while makefile.find(key) != -1:
            makefile = makefile.replace(key, mk_data[key])
    return makefile

def makefileGenerator(project_info, makefile):
    """
    Generate the Makefile for the current project.
    """
    # TODO: write a general function that works for both the mk file and the Makefile
    while makefile.find("project_name") != -1:
        makefile = makefile.replace("project_name", os.path.basename(project_info.info("PROJECT_PATH")))
    return makefile

def csrcGenerator(project_info):
    modules = project_info.info("MODULES")
    files = project_info.info("FILES")
    if "harvard" in project_info.info("CPU_INFOS")["CPU_TAGS"]:
        harvard = True
    else:
        harvard = False
    ## file to be included in CSRC variable
    csrc = []
    ## file to be included in PCSRC variable
    pcsrc = []
    ## files to be included in CPPASRC variable
    asrc = []
    ## constants to be included at the beginning of the makefile
    constants = {}
    for module, information in modules.items():
        module_files = set([])
        dependency_files = set([])
        ## assembly sources
        asm_files = set([])
        if information["enabled"]:
            if "constants" in information:
                constants.update(information["constants"])
            cfiles, sfiles = findModuleFiles(module, project_info)
            module_files |= set(cfiles)
            asm_files |= set(sfiles)
            for file_dependency in information["depends"]:
                if file_dependency in files:
                    dependencyCFiles, dependencySFiles = findModuleFiles(file_dependency, project_info)
                    dependency_files |= set(dependencyCFiles)
                    asm_files |= set(dependencySFiles)
            for file in module_files:
                if not harvard or "harvard" not in information or information["harvard"] == "both":
                    csrc.append(file)
                if harvard and "harvard" in information:
                    pcsrc.append(file)
            for file in dependency_files:
                csrc.append(file)
            for file in asm_files:
                asrc.append(file)
    csrc = " \\\n\t".join(csrc) + " \\"
    pcsrc = " \\\n\t".join(pcsrc) + " \\"
    asrc = " \\\n\t".join(asrc) + " \\"
    constants = "\n".join([os.path.basename(project_info.info("PROJECT_PATH")) + "_" + key + " = " + str(value) for key, value in constants.items()])
    return csrc, pcsrc, asrc, constants
    
def findModuleFiles(module, project_info):
    ## Find the files related to the selected module
    cfiles = []
    sfiles = []
    ## .c files related to the module and the cpu architecture
    for filename, path in findDefinitions(module + ".c", project_info) + \
            findDefinitions(module + "_" + project_info.info("CPU_INFOS")["TOOLCHAIN"] + ".c", project_info):
        path = path.replace(project_info.info("SOURCES_PATH") + "/", "")
        cfiles.append(path + "/" + filename)
    ## .s files related to the module and the cpu architecture
    for filename, path in findDefinitions(module + ".s", project_info) + \
            findDefinitions(module + "_" + project_info.info("CPU_INFOS")["TOOLCHAIN"] + ".s", project_info) + \
            findDefinitions(module + ".S", project_info) + \
            findDefinitions(module + "_" + project_info.info("CPU_INFOS")["TOOLCHAIN"] + ".S", project_info):
        path = path.replace(project_info.info("SOURCES_PATH") + "/", "")
        sfiles.append(path + "/" + filename)
    ## .c and .s files related to the module and the cpu tags
    for tag in project_info.info("CPU_INFOS")["CPU_TAGS"]:
        for filename, path in findDefinitions(module + "_" + tag + ".c", project_info):
            path = path.replace(project_info.info("SOURCES_PATH") + "/", "")
            cfiles.append(path + "/" + filename)
        for filename, path in findDefinitions(module + "_" + tag + ".s", project_info) + \
                findDefinitions(module + "_" + tag + ".S", project_info):
            path = path.replace(project_info.info("SOURCES_PATH") + "/", "")
            sfiles.append(path + "/" + filename)
    return cfiles, sfiles

def codeliteProjectGenerator(project_info):
    template = open("cltemplates/bertos.project").read()
    filelist = "\n".join(codelite_project.clFiles(codelite_project.findSources(project_info.info("PROJECT_PATH")), project_info.info("PROJECT_PATH")))
    while template.find("$filelist") != -1:
        template = template.replace("$filelist", filelist)
    project_name = os.path.basename(project_info.info("PROJECT_PATH"))
    while template.find("$project") != -1:
        template = template.replace("$project", project_name)
    return template

def codeliteWorkspaceGenerator(project_info):
    template = open("cltemplates/bertos.workspace").read()
    project_name = os.path.basename(project_info.info("PROJECT_PATH"))
    while template.find("$project") != -1:
        template = template.replace("$project", project_name)
    return template
    
def getSystemPath():
    path = os.environ["PATH"]
    if os.name == "nt":
        path = path.split(";")
    else:
        path = path.split(":")
    return path

def findToolchains(path_list):
    toolchains = []
    for element in path_list:
        for toolchain in glob.glob(element+ "/" + const.GCC_NAME):
            toolchains.append(toolchain)
    return list(set(toolchains))

def getToolchainInfo(output):
    info = {}
    expr = re.compile("Target: .*")
    target = expr.findall(output)
    if len(target) == 1:
        info["target"] = target[0].split("Target: ")[1]
    expr = re.compile("gcc version [0-9,.]*")
    version = expr.findall(output)
    if len(version) == 1:
        info["version"] = version[0].split("gcc version ")[1]
    expr = re.compile("gcc version [0-9,.]* \(.*\)")
    build = expr.findall(output)
    if len(build) == 1:
        build = build[0].split("gcc version ")[1]
        build = build[build.find("(") + 1 : build.find(")")]
        info["build"] = build
    expr = re.compile("Configured with: .*")
    configured = expr.findall(output)
    if len(configured) == 1:
        info["configured"] = configured[0].split("Configured with: ")[1]
    expr = re.compile("Thread model: .*")
    thread = expr.findall(output)
    if len(thread) == 1:
        info["thread"] = thread[0].split("Thread model: ")[1]
    return info

def loadSourceTree(project):
    fileList = [f for f in os.walk(project.info("SOURCES_PATH"))]
    project.setInfo("FILE_LIST", fileList)

def findDefinitions(ftype, project):
    L = project.info("FILE_LIST")
    definitions = []
    for element in L:
        for filename in element[2]:
            if fnmatch.fnmatch(filename, ftype):
                definitions.append((filename, element[0]))
    return definitions

def loadCpuInfos(project):
    cpuInfos = []
    for definition in findDefinitions(const.CPU_DEFINITION, project):
        cpuInfos.append(getInfos(definition))
    return cpuInfos

def getInfos(definition):
    D = {}
    D.update(const.CPU_DEF)
    def include(filename, dict = D, directory=definition[1]):
        execfile(directory + "/" + filename, {}, D)
    D["include"] = include
    include(definition[0], D)
    D["CPU_NAME"] = definition[0].split(".")[0]
    D["DEFINITION_PATH"] = definition[1] + "/" + definition[0]
    del D["include"]
    return D

def getCommentList(string):
    comment_list = re.findall(r"/\*{2}\s*([^*]*\*(?:[^/*][^*]*\*+)*)/", string)
    comment_list = [re.findall(r"^\s*\* *(.*?)$", comment, re.MULTILINE) for comment in comment_list]
    return comment_list

def loadModuleDefinition(first_comment):
    to_be_parsed = False
    module_definition = {}
    for num, line in enumerate(first_comment):
        index = line.find("$WIZ$")
        if index != -1:
            to_be_parsed = True
            try:
                exec line[index + len("$WIZ$ "):] in {}, module_definition
            except:
                raise ParseError(num, line[index:])
        elif line.find("\\brief") != -1:
            module_definition["module_description"] = line[line.find("\\brief") + len("\\brief "):]
    module_dict = {}
    if "module_name" in module_definition.keys():
        module_name = module_definition[const.MODULE_DEFINITION["module_name"]]
        del module_definition[const.MODULE_DEFINITION["module_name"]]
        module_dict[module_name] = {}
        if const.MODULE_DEFINITION["module_depends"] in module_definition.keys():
            if type(module_definition[const.MODULE_DEFINITION["module_depends"]]) == str:
                module_definition[const.MODULE_DEFINITION["module_depends"]] = (module_definition[const.MODULE_DEFINITION["module_depends"]],)
            module_dict[module_name]["depends"] = module_definition[const.MODULE_DEFINITION["module_depends"]]
            del module_definition[const.MODULE_DEFINITION["module_depends"]]
        else:
            module_dict[module_name]["depends"] = ()
        if const.MODULE_DEFINITION["module_configuration"] in module_definition.keys():
            module_dict[module_name]["configuration"] = module_definition[const.MODULE_DEFINITION["module_configuration"]]
            del module_definition[const.MODULE_DEFINITION["module_configuration"]]
        else:
            module_dict[module_name]["configuration"] = ""
        if "module_description" in module_definition.keys():
            module_dict[module_name]["description"] = module_definition["module_description"]
            del module_definition["module_description"]
        if const.MODULE_DEFINITION["module_harvard"] in module_definition.keys():
            harvard = module_definition[const.MODULE_DEFINITION["module_harvard"]]
            if harvard == "both" or harvard == "pgm_memory":
                module_dict[module_name]["harvard"] = harvard
            del module_definition[const.MODULE_DEFINITION["module_harvard"]]
        module_dict[module_name]["constants"] = module_definition
        module_dict[module_name]["enabled"] = False
    return to_be_parsed, module_dict

def loadDefineLists(comment_list):
    define_list = {}
    for comment in comment_list:
        for num, line in enumerate(comment):
            index = line.find("$WIZ$")
            if index != -1:
                try:
                    exec line[index + len("$WIZ$ "):] in {}, define_list
                except:
                    raise ParseError(num, line[index:])
    for key, value in define_list.items():
        if type(value) == str:
            define_list[key] = (value,)
    return define_list

def getDescriptionInformations(comment): 
    """ 
    Take the doxygen comment and strip the wizard informations, returning the tuple 
    (comment, wizard_information) 
    """
    brief = ""
    description = ""
    information = {}
    for num, line in enumerate(comment):
        index = line.find("$WIZ$")
        if index != -1:
            if len(brief) == 0:
                brief += line[:index].strip()
            else:
                description += " " + line[:index]
            try:
                exec line[index + len("$WIZ$ "):] in {}, information
            except:
                raise ParseError(num, line[index:])
        else:
            if len(brief) == 0:
                brief += line.strip()
            else:
                description += " " + line
                description = description.strip()
    return brief.strip(), description.strip(), information

def getDefinitionBlocks(text):
    """
    Take a text and return a list of tuple (description, name-value).
    """
    block = []
    block_tmp = re.finditer(r"/\*{2}\s*([^*]*\*(?:[^/*][^*]*\*+)*)/\s*#define\s+((?:[^/]*?/?)+)\s*?(?:/{2,3}[^<].*?)?$", text, re.MULTILINE)
    for match in block_tmp:
        # Only the first element is needed
        comment = match.group(1)
        define = match.group(2)
        start = match.start()
        block.append(([re.findall(r"^\s*\* *(.*?)$", line, re.MULTILINE)[0] for line in comment.splitlines()], define, start))
    for match in re.finditer(r"/{3}\s*([^<].*?)\s*#define\s+((?:[^/]*?/?)+)\s*?(?:/{2,3}[^<].*?)?$", text, re.MULTILINE):
        comment = match.group(1)
        define = match.group(2)
        start = match.start()
        block.append(([comment], define, start))
    for match in re.finditer(r"#define\s*(.*?)\s*/{3}<\s*(.+?)\s*?(?:/{2,3}[^<].*?)?$", text, re.MULTILINE):
        comment = match.group(2)
        define = match.group(1)
        start = match.start()
        block.append(([comment], define, start))
    return block

def loadModuleData(project):
    module_info_dict = {}
    list_info_dict = {}
    configuration_info_dict = {}
    file_dict = {}
    for filename, path in findDefinitions("*.h", project) + findDefinitions("*.c", project) + findDefinitions("*.s", project) + findDefinitions("*.S", project):
        comment_list = getCommentList(open(path + "/" + filename, "r").read())
        if len(comment_list) > 0:
            module_info = {}
            configuration_info = {}
            try:
                to_be_parsed, module_dict = loadModuleDefinition(comment_list[0])
            except ParseError, err:
                raise DefineException.ModuleDefineException(path, err.line_number, err.line)
            for module, information in module_dict.items():
                if "depends" not in information:
                    information["depends"] = ()
                information["depends"] += (filename.split(".")[0],)
                information["category"] = os.path.basename(path)
                if "configuration" in information.keys() and len(information["configuration"]):
                    configuration = module_dict[module]["configuration"]
                    try:
                        configuration_info[configuration] = loadConfigurationInfos(project.info("SOURCES_PATH") + "/" + configuration)
                    except ParseError, err:
                        raise DefineException.ConfigurationDefineException(project.info("SOURCES_PATH") + "/" + configuration, err.line_number, err.line)
            module_info_dict.update(module_dict)
            configuration_info_dict.update(configuration_info)
            if to_be_parsed:
                try:
                    list_dict = loadDefineLists(comment_list[1:])
                    list_info_dict.update(list_dict)
                except ParseError, err:
                    raise DefineException.EnumDefineException(path, err.line_number, err.line)
    for filename, path in findDefinitions("*_" + project.info("CPU_INFOS")["TOOLCHAIN"] + ".h", project):
        comment_list = getCommentList(open(path + "/" + filename, "r").read())
        list_info_dict.update(loadDefineLists(comment_list))
    for tag in project.info("CPU_INFOS")["CPU_TAGS"]:
        for filename, path in findDefinitions("*_" + tag + ".h", project):
            comment_list = getCommentList(open(path + "/" + filename, "r").read())
            list_info_dict.update(loadDefineLists(comment_list))
    project.setInfo("MODULES", module_info_dict)
    project.setInfo("LISTS", list_info_dict)
    project.setInfo("CONFIGURATIONS", configuration_info_dict)
    project.setInfo("FILES", file_dict)
    
def formatParamNameValue(text):
    """
    Take the given string and return a tuple with the name of the parameter in the first position
    and the value in the second.
    """
    block = re.findall("\s*([^\s]+)\s*(.+?)\s*$", text, re.MULTILINE)
    return block[0]

def loadConfigurationInfos(path):
    """
    Return the module configurations found in the given file as a dict with the
    parameter name as key and a dict containig the fields above as value:
        "value": the value of the parameter
        "description": the description of the parameter
        "informations": a dict containig optional informations:
            "type": "int" | "boolean" | "enum"
            "min": the minimum value for integer parameters
            "max": the maximum value for integer parameters
            "long": boolean indicating if the num is a long
            "unsigned": boolean indicating if the num is an unsigned
            "value_list": the name of the enum for enum parameters
    """
    configuration_infos = {}
    configuration_infos["paramlist"] = []
    for comment, define, start in getDefinitionBlocks(open(path, "r").read()):
        name, value = formatParamNameValue(define)
        brief, description, informations = getDescriptionInformations(comment)
        configuration_infos["paramlist"].append((start, name))
        configuration_infos[name] = {}
        configuration_infos[name]["value"] = value
        configuration_infos[name]["informations"] = informations
        if not "type" in configuration_infos[name]["informations"]:
            configuration_infos[name]["informations"]["type"] = findParameterType(configuration_infos[name])
        if ("type" in configuration_infos[name]["informations"].keys() and
                configuration_infos[name]["informations"]["type"] == "int" and
                configuration_infos[name]["value"].find("L") != -1):
            configuration_infos[name]["informations"]["long"] = True
            configuration_infos[name]["value"] = configuration_infos[name]["value"].replace("L", "")
        if ("type" in configuration_infos[name]["informations"].keys() and
                configuration_infos[name]["informations"]["type"] == "int" and
                configuration_infos[name]["value"].find("U") != -1):
            configuration_infos[name]["informations"]["unsigned"] = True
            configuration_infos[name]["value"] = configuration_infos[name]["value"].replace("U", "")
        configuration_infos[name]["description"] = description
        configuration_infos[name]["brief"] = brief
    return configuration_infos

def findParameterType(parameter):
    if "value_list" in parameter["informations"]:
        return "enum"
    if "min" in parameter["informations"] or "max" in parameter["informations"] or re.match(r"^\d+U?L?$", parameter["value"]) != None:
        return "int"

def sub(string, parameter, value):
    """
    Substitute the given value at the given parameter define in the given string
    """
    return re.sub(r"(?P<define>#define\s+" + parameter + r"\s+)([^\s]+)", r"\g<define>" + value, string)

def isInt(informations):
    """
    Return True if the value is a simple int.
    """
    if ("long" not in informatios.keys() or not informations["long"]) and ("unsigned" not in informations.keys() or informations["unsigned"]):
        return True
    else:
        return False

def isLong(informations):
    """
    Return True if the value is a long.
    """
    if "long" in informations.keys() and informations["long"] and "unsigned" not in informations.keys():
        return True
    else:
        return False

def isUnsigned(informations):
    """
    Return True if the value is an unsigned.
    """
    if "unsigned" in informations.keys() and informations["unsigned"] and "long" not in informations.keys():
        return True
    else:
        return False

def isUnsignedLong(informations):
    """
    Return True if the value is an unsigned long.
    """
    if "unsigned" in informations.keys() and "long" in informations.keys() and informations["unsigned"] and informations["long"]:
        return True
    else:
        return False

class ParseError(Exception):
    def __init__(self, line_number, line):
        Exception.__init__(self)
        self.line_number = line_number
        self.line = line
