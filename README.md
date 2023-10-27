# Checkpoint 1

This project is split into three parts:
1. Lexer - tokenizes input
2. Parser - uses lexer, expects/goes through S-expression, makes AST, prints infix expression and evaluated value
3. Calc - uses lexer, expects/goes through infix expression, makes AST, prints infix expression and evaluated value
   
# Lexer 
Split into 3 files:
1. lex.h - header file of all fucntion and class declarations
2. lexfunctions.cpp - implementation of functions
3. lex.cpp - takes in input, uses functions, and produces output

The lexer takes in an input and prints out all of the tokens (parenthesis, operator, number) with their corresponding row and column # in the input. To do so the lexer uses these helper functions:
- createTokens() - takes in a line of input and creates tokens and adds them to token vector
- addEndToken() - when input is finished, adds endtoken
- lexer() - takes input and uses previous two functions to return a completed vector of tokens
- printToken() - prints tokens

To build the lexer run the following command:
```
make
```
Run using:
```
./lex.o
```
Once running, input S-expression and the tokens will be created and printed!


# Parser 
Split into 3 files: 
1. parse.h - header file of all fucntion and class declarations
2. parsefunctions.cpp - implementation of functions
3. parse.cpp - takes in input, uses functions, and produces output
   
The parser takes in a vector of tokens and contructs an AST. The operators and numbers are saved into nodes with the operands becoming the operator's children. If there are vairables in the tree, they are saved to a variable vector in case they needed to be used later. This tree can then be printed in infix form and evaluated. The parser uses an AST class which includes the node struct, a destructor, and constructor. The parser also uses the following helper functions:

- lexer() - gives vector of tokens to parse
- expressionChecker() - checks that valid expression can be created from the tokens
- createAST() creates - and AST from tokens
- parser() - combines the previous two functions running expressionChecker() first then createAST()
- printInfix() - prints out AST in infix form
- evaulate() - evaluates expression represented by the AST


# Checkpoint 2 
1. Updating the lexer and first parser to support assignment and variables --updated the old lex files
2. Creating an infix parser

# Infix Parser
Split into 3 files:
1. calc.h - header file of all fucntion and class declarations
2. calcfunctions.cpp - implementation of functions
3. calc.cpp - takes in input, uses functions, and produces output

The Infix Parser produces the same output as the first Parser. The main difference is how it takes in input and treats errors. This parser takes in one line of input, runs, and aks for another line of input. The program repeats this process until the user stops inputting. If there is an error in the input calc outputs an error, skips the rest of functions, and asks for another input. 

The infix parser uses the following functions:
- createtokens() - takes in input and returns vector of tokens
- addEnd() - adds an END token to the vector of tokens
- expressionChecker2() - takes in a vector and throws if there is an error with the input
- findMatchingParenth() - used by the expressionChecker to make sure the parenthesis are valid, returns the index of ")"
- build() - takes in a vector of tokens and returns and AST2 pointer
- precedence() - takes in a vector of tokens and finds the token of least precedence -- used by the build() function
- infixString() - takes in a AST2 pointer and returns a string of contents
- evaluate() - takes in a AST2 pointer and returns a double of the result

To build the infix parser run the following command:
```
make
```
Run using:
```
./parse.o
```
Once running, input infix-expression and tokens will be created, AST will be created, expression will be printed in infix form, and AST will be evaluated/printed!


