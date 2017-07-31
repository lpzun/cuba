###########################################################################
# This is the makefile for boolean program parser: BoPP                   #
# 																		  #
# @author: 	Peizun Liu													  #
# @Copyright: 	It belongs to Thomas Wahl's group in CAR Lab, CCIS, NEU	  #
###########################################################################

#========================================
# macro definition		#
#========================================
# Override these variables (or add new ones) locally
BASE 		= bopp
#DIR
#EXPDIR
ILIBS		=#
IINCLUDE    =# -I $(SATDIR)/include/#                                      

ISTD	    = -std=c++11

#### flex
FLEXSUFF	= lex
FLEXCOMP	= flex 
FLEXOBJ 	= $(BASE).$(FLEXSUFF)
FEXPORT		= lex.yy.cc

#### bison
BISONSUFF	= y
BISONCOMP 	= bison
BISONOBJ 	= $(BASE).$(BISONSUFF)
BISONFLAG 	= -d
# the export of flex and bison
BEXPORT		= $(BASE).tab.cc $(BASE).tab.hh location.hh position.hh stack.hh

#### C compiler
SRCDIR       = ../
SRCDIRS      = $(shell find $(SRCDIR) -name '*.$(CSUFF)' -exec dirname {} \; | uniq)
SUPPORT     = bopp.cc bopp.hh

CBASE 		= $(BASE)
CCOMP 		= g++ #the name of the compiler
CCOBJS 		= $(BASE).tab.cc $(FEXPORT) bopp.cc
DEBUG 		= -g
CFLAGS 		= -Wall -c $(DEBUG) $(ISTD)#
LFLAGS 		= -Wall $(DEBUG) $(ISTD)#
CEXPORT 	= *.h *.cfg *.loc *.taf

#========================================
# main cmds		#
# $(CCOMP) $(LFLAGS) $(CCOBJS) -o $(CBASE) -lfl #does not work on Mac if append -lfl
#========================================

all:
	$(FLEXCOMP) -o $(FEXPORT) $(FLEXOBJ)
	$(BISONCOMP) $(BISONFLAG) $(BISONOBJ)
	$(CCOMP) $(LFLAGS) $(CCOBJS) -o $(CBASE)

#========================================
# Cleaning 		#
#========================================
clean:
	rm -f $(FEXPORT) $(BEXPORT) $(BASE).cc

distclean: clean
	rm -f $(CBASE) $(CEXPORT) *~
	rm -rf *.dSYM
