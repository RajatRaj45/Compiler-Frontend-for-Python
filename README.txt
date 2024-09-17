**********************README**********************

There are 2 code files:
    1. Lexer.cpp
    2. Parser.cpp

There are 2 text files which are used to exchange information between these code files:
    1. PreprocessedOutput.txt
    2. LexerOutput.txt

The input is to be given in a single python file:
    1. srcCode.py

This file should contain python code made of basic constructs as mentioned in the problem statement.

Rules for source code:
    1. Every if should have a corresponding else statement
    2. If statements should be of the following type: if(condition):
    3. Expressions should be only binary in nature i.e. a+3 or 4%2 ...
    4. There should not be any blank line in the source code including the end
    5. There should not be any comments in the source code

How to run the code:
    1. Write the python code in the srcCode.py file
    2. Run the Lexer.cpp file
    3. Run the Parser.cpp file

Interpreting Output:
    In case of a lexical error, the Lexer.cpp prints Lexical Error upon execution. The Parser need not be run in this case
    The LexerOutput.txt file contains the identified lexemes and their tokens
    If lexical analysis is successful, the Parser is run, it outputs the encountered Parsing error if any, else outputs Parsing Successful
    In case Lexical Analysis and Parsing is successful, the ParseTree.dot file contains the dot code for the generated parse tree.