#===-- lib/Solvers/Makefile ----------------------------------------------===#
#
#                     The LAV Software Verification Tool
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
#===----------------------------------------------------------------------===#

#
# Relative path to the top of the source tree.
#
LEVEL=../..

PARALLEL_DIRS=
USEDLIBS = expression.a 
LIBRARYNAME=solvers
BUILD_ARCHIVE=1

include $(LEVEL)/Makefile.common

ifeq ($(ENABLE_Z3), 1)
        CXX.Flags += -I$(PROJ_SRC_ROOT)/solvers/z3/include
endif

ifeq ($(ENABLE_BOOLECTOR), 1)
        CXX.Flags += -I$(PROJ_SRC_ROOT)/solvers/boolector/include
endif

ifeq ($(ENABLE_YICES), 1)
        CXX.Flags += -I$(PROJ_SRC_ROOT)/solvers/yices/include
endif

ifeq ($(ENABLE_MATHSAT), 1)
        CXX.Flags += -I$(PROJ_SRC_ROOT)/solvers/mathsat/include
endif

