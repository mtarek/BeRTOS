#!/bin/bash
#
# Copyright Develer S.r.l. (http://www.develer.com/)
# All rights reserved.
#
# Author: Bernardo Innocenti <bernie@develer.com>
#
# $Id$
#
# $Log$
# Revision 1.2  2005/02/28 10:46:44  bernie
# Remove test binaries.
#
# Revision 1.1  2005/02/01 06:59:24  bernie
# Really trivial testsuite framework.
#


VERBOSE=1

CC=gcc
CFLAGS="-W -Wall -Wextra"

CXX=g++
CXXFLAGS="-W -Wall -Wextra"


for test in `find . -name "*_test.*"`; do
	[ $VERBOSE -gt 0 ] && echo "Running $test..."
	case "$test" in
	*.cpp)
		$CXX $CXXFLAGS $test -o test || exit 1
		./test || exit 1
		rm -f test
	;;
	*.c)
		$CC $CFLAGS $test -o test || exit 1
		./test || exit 1
		rm -f test
	;;
	esac
done

