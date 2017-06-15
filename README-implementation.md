Some comments on the tool. 

- already reported: empty stacks are not displayed by your tool.
-- REPLY: I'll add this back soon. 

===========================================================================

In the help output, what is the diff between "source.pds" and "--input-file" ?

Here is a patch for some textual change I would make in the help:

---------------------------------------------------------------------------
--- utils/cmd.cc	(revision 91)
+++ utils/cmd.cc	(working copy)
@@ -338,9 +338,9 @@
 	this->add_option(get_opt_index(opt_type::CON), "-k", "--ctx-bound",
 			"the bound of contexts", "1");
 	this->add_option(get_opt_index(opt_type::CON), "-n", "--threads",
-			"the number of threads, used only for parameterized systems", "1");
+			"the number of threads (used only for parameterized systems)", "1");
 	this->add_switch(get_opt_index(opt_type::CON), "-x", "--explicit",
-			"an explicit forward search but probably non-terminate");
+			"an explicit forward search (may not terminate)");
 
 	/// other options
 	this->add_switch(get_opt_index(opt_type::OTHER), "-cmd", "--cmd-line",
---------------------------------------------------------------------------

A segementation fault occurs when k = 1: need to fix it.
