#
# $Id$
# Copyright 2002, 2003, 2004, 2006 Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Author: Bernie Innocenti <bernie@codewiz.org>
#

# Set to 1 for verbose build output, 0 for terse output
V := 0

default: all

include bertos/config.mk

#Include subtargets
include examples/demo/demo.mk
#include examples/at91sam7s/at91sam7s.mk
#include examples/triface/triface.mk
include test/libunittest.mk

include bertos/rules.mk
