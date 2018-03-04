# Generic doxygen Makefile

BASEDIR = doc
HTMLDIR = html

DOXYGEN = doxygen

DOXYFILE = cuba.doxyfile

DOCUMENTATION = doc



#####################################
# 2. Private Region (do not change) #
#####################################

.PHONY: all clean

all: $(DOCUMENTATION)

new: clean all

distnew: distclean all


# Add new targets locally. This is included after 'default' above, so that the default remains the default.
-include makefile-local-targets

unexport MAKEFLAGS # do not export variables by default (only those mentioned in EXPORT)


##################################
# Targets Region (do not change) #
##################################

$(DOCUMENTATION): 
	$(DOXYGEN) $(DOXYFILE)
	
############################
# Cleaning (do not change) #
############################
clean:
	rm -rf $(HTMLDIR)