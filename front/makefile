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

# object dir
BINDIR       = bin
OBJDIR       = obj

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

#### C++
CSUFF        = cc
#CSUFF       = c

#### C++ compiler
SRCDIR       = .#
SRCDIRS      = $(shell find $(SRCDIR) -name '*.$(CSUFF)' -exec dirname {} \; | uniq)

CBASE 		= $(BASE)
CCOMP 		= g++ #the name of the compiler

SOURCES     = $(shell find $(SRCDIR) -name '*.$(CSUFF)') #$(wildcard *.$(CSUFF))#
LOBJECTS    = $(patsubst %.$(CSUFF),$(OBJDIR)/%.o, $(SOURCES:.$(CSUFF)=.o)) #             list of local files that will be compiled and linked into executable
OBJECTS      = $(LOBJECTS) #$(BASE).tab.cc $(FEXPORT) main.cc cfg.cc aide.cc cfg.cc

DEBUG 		= -g
CFLAGS 		= -Wall -c -Wno-deprecated-register $(DEBUG) $(ISTD)#
LFLAGS 		= -Wall $(DEBUG) $(ISTD)#

#========================================
# main cmds: Compiling		#
# $(CCOMP) $(LFLAGS) $(CCOBJS) -o $(CBASE) -lfl #does not work on Mac if append -lfl
#========================================
ifeq ($(CSUFF),cc)
  HSUFF        = hh
  TSUFF        = CC
  CCOMP        = g++
else
  HSUFF        = h
  TSUFF        = C
  CCOMP        = gcc
endif

DEFAULT      = $(BASE)

all:
	$(FLEXCOMP) -o $(FEXPORT) $(FLEXOBJ)
	$(BISONCOMP) $(BISONFLAG) $(BISONOBJ)
	
$(DEFAULT): $(OBJECTS)
	$(CCOMP) $(LFLAGS) $(OBJECTS) -o $@
	
$(OBJECTS): %.o: %.$(CSUFF) $(HEADERS)
	$(CCOMP) $(CFLAGS) $< -c -o $@

#========================================
# Cleaning 		#
#========================================
#### For cleaning
CEXPORT 	= *.h *.cfg *.loc *.taf

clean:
	rm -f $(FEXPORT) $(BEXPORT)

distclean: clean CLEANOBJS
	rm -f $(CBASE) $(CEXPORT) *~
	rm -rf *.dSYM
	
CLEANOBJS:
	@$(call clean-obj)
	
# description: for cleaning all objects
define clean-obj
	find . -name '*.o' -type f -delete
	find . -name '*.~' -type f -delete
endef