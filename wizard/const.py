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

CPU_DEF = {
    "CPU_NAME": "",
    "CPU_DIR": "",
    "DEFINITION_PATH": "",
    "TOOLCHAIN": "",
    "CORE_CPU": "",
    "SCRIPT_DIR": "",
    "HW_DIR": "",
    "DRV_DIR": "",
    "C_FLAGS": [],
    "CPPA_FLAGS" : [],
    "CPP_FLAGS" : [],
    "LD_FLAGS" : [],
    "CPPA_SRC" : [],
    "SUPPORTED_DRV" : [],
    "PC_SRC" : [],
    "CPU_DESC" : []
}

CPU_DEFINITION = "*.cdef"

GCC_NAME = "*gcc*"

MODULE_CONFIGURATION = "cfg_*.h"