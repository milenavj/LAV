#===-- tools/TestLAV/Makefile --------------------------------------------===#
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

TOOLNAME = Statistike
USEDLIBS = transformation.a misc.a
LINK_COMPONENTS = jit bitreader bitwriter ipo linker engine 

include $(LEVEL)/Makefile.common

LIBS += -lsolvers
LIBS += -lexpression
LIBS += -lgmp  
LIBS += -lgmpxx  
