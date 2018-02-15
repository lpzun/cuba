###########################################################################
# A makefile for documentation.
#
###########################################################################
BASE     = cubadoc
DOCCOMP  = doxygen
DOXYFILE = cuba.doxyfile
DOCDIR   = html
OBJECTS  = doc

default: $(BASE)

new: clean default

##################################
# Targets Region (do not change) #
##################################
$(BASE): doxygen $(DOXYFILE)

############################
# Cleaning (do not change) #
############################
clean: CLEANOBJS
     
CLEANOBJS:
	@$(call clean-obj)
	
# description: for cleaning all objects
define clean-obj
	rm -rf $(DOCDIR)
endef