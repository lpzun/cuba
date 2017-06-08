Some comments on the tool. I don't want to put this into the github, since
the repos is totally public, right?

- why is the github repos totally public? Is this a good idea at this
  development stage? can we share this exclusively between the two of us?

-- REPLY: Sure, I also prefer to make it private. Can you give me your 
   github account? So, I can add you as a collaborator. 

- I don't quite understand the help line

  -n [--threads] arg        the number of threads (default = 1)

  Look at stutter.pds. If I specify two PDSs but do not specify -n (hence
  n=1, by default), there will still be two threads, correct? so what does
  the -n really do? is it the number of threads PER PDS?

-- REPLY: Sorry for confusion. This is only for parameterized programs. I 
   will reorganized my helpful information. 

   For un-parameterized input, like stutter.pds, -n is useless. The number 
   of threads depends on how many PDAs you have in your input

--- This is fine. You can add to the help: "[only for parameterized
    programs; ignored for others]" or something.

- help line:

  -a [--target] arg         a target thread configuration (default = 0|0,1,2)

  using comma to separate local states. But when I use a comma, my machine
  freezes! In what you sent me via email you use a semicolon.

  So correct this. We have traditionally used comma -- any reason for
  semicolon?

  Also, I assume in the initial state each stack has exactly size 1,
  correct?

-- REPLY: No, stack sizes for initial configurations could be arbitrarily 
   large.

--- That is good, for flexible experimentation.

-- Comma is a delimiter for symbols in the same stack. But you are 
   right: (i) The help info is misleading; (ii) using comma to separate stack 
   content is not a good design. I will update this, and make the help info
   more readable.

--- I see. This actually makes sense (but is not understandable from the
    help). How about giving a *general* initial stack, like

    0|1,2;3,4

    Then you explain the diff between , and ; .

- on my desktop I cannot compile the program; it says: "-lz: lib not
  found". Any idea what I need to install?

  (Turns out: when I remove the -lz link flag, it still compiles [without
  the error].)

-- REPLY: No, -lz is useless for this project. I used it for our other project. 
   I thought I commented it away. Sorry for confusion. 

- already reported: empty stacks are not displayed by your tool. Let's
  discuss that -- I want to know why, and I want to say why it is important.

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
