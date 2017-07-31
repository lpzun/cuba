/*******************************************************************************
 ** Name:    	BoPP: parser
 ** Authors: 	Peizun Liu
 ** Version: 	0.3
 ** Copyright: 	It belongs to Thomas Wahl's group in CAR Lab, CCIS, NEU
 ** Date:       Summer 2013
 ** Description: BoPP is a Boolean Program Parser written by C++. BoPP aims to
 *		parse (SatAbs format) Boolean programs and build its control 
 *		flow graph and the corresponding weakest precondition formula
 *              for each statement.
 ******************************************************************************/

1. BoPP:

1.1 USAGE: 

    step 1: put BoPP.sh, BoPP.jar and BoPP into a same directory, and then

    step 2: execute ./BoPP.sh <input-file>

1.2 DESCRIPTION: it includes two parts: BoPP.jar and BoPP. 

2. Preprocessor: BoPP.jar: 
    
2.1 DESCRIPTION: To translate (SatAbs format) Boolean programs to our format.

2.2 USAGE:

    The usage of the preprocessor separately:

	java -jar BoPP.jar [-h] <-f input-file>

NOTE: the input should be the SatAbs format file.


3. Parser: BoPP

3.1 DESCRIPTION: To build its control flow graph and the corresponding weakest 
    precondition formula for each statement.

3.1 USAGE:

    The usage of the parser separately:

	cat <input-file> | ./BoPP [-dimacs] [-cfg file] [-taf file]

3.2 COMMAND:

   	1. input-file: the Boolean program you want to parse. 

	2. -dimacs output the WP into a DIMACS CNF format

	3. -cfg: to specify the file name for the control flow graph (cfg).

   	4. -taf: to specify the file name for the target thread state file.

3.3 OUTPUT:
 
	1. cfg file: to store control flow graph, initial thread states, candidate pc, 
	   WP, etc.
 
	2. taf file: to store target thread states.

NOTE: the default cfg file name is "bp.cfg", while the default target file is 
"bp.taf".
