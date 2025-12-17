# chest_uci 

chest_uci is a uci adapter for chest 3.19

It sits between a UCI Client like 'ChessX' and 'chest' - the chess problem solving engine.

## Copyright

All Rights Reserved.

Permission to use, copy, modify and distribute this software
for non-commercial purposes and without fee is hereby granted,
provided that the above copyright notice and this permission
notice appear in all copies. This software is provided "as is"
without express or implied warranty.

Note that all further recommended programs and sources have their 
own terms of use.

## Compiling 

make

OR 
 
gcc -O2 -o chest_uci chest_uci.c

chest_uci works nice for Mac OS, should work under Linux and might even work for Windoze.

## Binaries

The current Release is v1.0 and contains Sources, chest_uci and a complimentary dchest compilation.
https://github.com/Isarhamster/chest_uci/releases/tag/v1.0

## Operation

chest_uci was designed to be used with ChessX (see https://chessx.sourceforge.io)
but any UCI client should work with chest_uci.
chest_uci is configured as UCI engine inside ChessX and translates the weird and clumsy UCI
protocol into the even weirder and clumsier chest language.

So in addition to the chest_uci executable you need a chest-executable (usually dchest).
Configure the path to the chest excutable in the 'backend' option of chest-uci.
Default path is "./dchest" which assumes that you put the chest_uci into the same folder 
as dchest.
You can add commandline arguments like -r to the backend call. Do not use -b.

chest_uci has a second option 'job' which is directly passed to dchest 'j'.
Supported are the following operations:
- o	orthodox (normal)
- O	stalemate
- h	helpmate
- H	helpstalemate
- s selfmate
- S	selfstalemate 
	    
### Supported Options
- job
- backend    

## Debugging 

In case something does not work as expected, you can look into the process by activating
the Log output inside chessx (this will give you the transaction between ChessX and chest_uci).
Furthermore, chest_uci generates two files:
- chest_input.txt
- chest_output.txt

These files are used for communicating jobs and results between chest_uci and chest.
chest_uci only generates a list of info messages with solutions and advertises the last found 
solution as bestmove. In chest_output.txt you can find further explanations and insights
which cannot be transported using UCI.
	   
## Limitations / Issues

- chest_uci generates fake nodes information (chest does not generate node information and ChessX
really wants to have it)
- chest is not developed anymore, chest_uci supports only one chest version (that is the latest 3.19 
with the complete set of bugfixes up to March 8 2011).
- chest_uci does NOT support non-free versions after the final source code release, especially no
support for WinChest 5.2
- I like passing -rLS to chest and I might add the lines from the tree into the UCI information
in some future version
	    
## References

- UCI Client: https://chessx.sourceforge.io
- Chest Download: https://turbotm.de/~heiner/Chess/chest.html



