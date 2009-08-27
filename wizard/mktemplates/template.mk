#
# Copyright 2009 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Makefile template for BeRTOS wizard.
#
# Author: Lorenzo Berni <duplo@develer.com>
#
#

# Include the mk file generated by the wizard
include $pname/$pname_wiz.mk

# Files included by the user.
$pname_USER_CSRC = \
	$main \
	#

# Files included by the user.
$pname_USER_PCSRC = \
	#

# Files included by the user.
$pname_USER_CPPASRC = \
	#

# Files included by the user.
$pname_USER_CXXSRC = \
	#

# Files included by the user.
$pname_USER_ASRC = \
	#

# Programmer interface configuration, see http://dev.bertos.org/wiki/ProgrammerInterface for help
$pname_PROGRAMMER_TYPE = none
$pname_PROGRAMMER_PORT = none

####### End of configurable section, do not edit the lines below! #######

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
$pname_CSRC = $($pname_WIZARD_CSRC) $($pname_USER_CSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
$pname_PCSRC = $($pname_WIZARD_PCSRC) $($pname_USER_PCSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
$pname_CPPASRC = $($pname_WIZARD_CPPASRC) $($pname_USER_CPPASRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
$pname_CXXSRC = $($pname_WIZARD_CXXSRC) $($pname_USER_CXXSRC)

# Automatically generated by the wizard. PLEASE DO NOT EDIT!
$pname_ASRC = $($pname_WIZARD_ASRC) $($pname_USER_ASRC)

