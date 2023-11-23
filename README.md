This project is split into five parts:
1. Lexer - tokenizes input
2. Parser (S-expression parser) - uses lexer, expects/goes through S-expression, makes AST, prints infix expression and evaluated value
3. Calc (infix parser)- uses lexer, expects/goes through infix expression, makes AST, prints infix expression and evaluated value
4. Format - uses lexer and prints out input in the code-like format
5. Scrypt - uses lexer, expects/goes through expressions and statements, makes AST, and prints out print statements

# CheckPoint 4 File Updates 
The Lexer, Calc, Format, Scrypt helper files have all been updated to support function definitions and arrays 
## For the Lexer:
   - ';', "def", "null", brackets, and functions calls (the names) did not cause lexer errors and got assigned the correct type

## For 

# Lexer 
Split into 3 files:
1. lex.h - header file of all fucntion and class declarations
2. lexfunctions.cpp - implementation of functions
3. lex.cpp - takes in input, uses functions, and produces output

The lexer takes in an input and prints out all of the tokens with their corresponding row and column # in the input. The following tokens are listed below:
- Parentheses (`(` and `)`).
- Multiplication, division, and modulo (`*`, `/`, and `%`).
- Addition and subtraction (`+` and `-`).
- Ordered comparison (`<`, `<=`, `>`, and `>=`).
- Equality and inequality (`==` and `!=`).
- Logical and (`&`).
- Logical exclusive or (`^`).
- Logical inclusive or (`|`).
- Assignment (`=`).
- Statements (`if`, `else`, `while`)
- Brackets (`{` and `}`)
- Variables (string of consecutive characters starting with _ or character)
- Defintions ('def')
- Function names ('f()')
- Return ('return')
- Null ('null')

The lexer uses the following functions.
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

# Parser (S-expression parser)
Split into 3 files: 
1. parse.h - header file of all fucntion and class declarations
2. parsefunctions.cpp - implementation of functions
3. parse.cpp - takes in input, uses functions, and produces output
   
The parser takes in a vector of tokens and contructs an AST. The operators and numbers are saved into nodes with the operands becoming the operator's children. This tree can then be printed in infix form and evaluated. The parser uses an AST class which includes the node struct, a destructor, and constructor. 

The parser also uses the following functions:
- lexer() - gives vector of tokens to parse
- expressionChecker() - checks that valid expression can be created from the tokens
- createAST() creates - and AST from tokens
- parser() - combines the previous two functions running expressionChecker() first then createAST()
- printInfix() - prints out AST in infix form
- evaulate() - evaluates expression represented by the AST

**_NOTE:_** only handles parenthesis, numbers, addition/subtraction, multiplication/division, variables, and assignment

To build the parser run the following command:
```
make
```
Run using:
```
./parse.o
```
Once running, input S-expression and tokens will be created, AST will be created, expression will be printed in infix form, and AST will be evaluated/printed!

# Calc (infix parser)
Split into 3 files:
1. calc.h - header file of all function and class declarations
2. calcfunctions.cpp - implementation of functions
3. calc.cpp - takes in input, uses functions, and produces output

The infix iarser produces the same output as the first parser. The main difference is how it takes in input and treats errors. Instead of parsing S-expressions, it parses infix epxressions. The parser takes in one line of input, runs, and asks for another line of input. The program repeats this process until the user stops inputting. If there is an error in the input calc outputs an error, skips the rest of functions, and asks for another input. 

The infix parser uses the following functions:
- createtokens() - takes in input and returns vector of tokens
- addEnd() - adds an END token to the vector of tokens
- expressionChecker2() - takes in a vector and throws if there is an error with the input
- findMatchingParenth() - used by the expressionChecker to make sure the parenthesis are valid, returns the index of ")"
- build() - takes in a vector of tokens and returns and AST2 pointer
- precedence() - takes in a vector of tokens and finds the token of least precedence -- used by the build() function
- infixString() - takes in a AST2 pointer and returns a string of contents
- evaluate() - takes in a AST2 pointer and returns a double of the result

**_NOTE:_** handles everything S-expression parser does plus ordered comparison, equality/inequality, and logical operators
  
To build the calc run the following command:
```
make
```
Run using:
```
./calc.o
```
Once running, input infix expression and tokens will be created, AST will be created, expression will be printed in infix form, and AST will be evaluated/printed!

# Format
Split into 3 files:
1. format.h - header file of all function and class declarations
2. formatfunctions.cpp - implementation of functions
3. format.cpp - takes in input, uses functions, and produces output

Format uses the lexer to take in input and prints out input in the code-like format with proper all proper indentations. It does not evaluate any statements, it simply prints it.

Format uses the following funcitons:
- indent() - prints out proper indentation based on how nested a statement is
- printStatements - takes in vector from lexer and prints out input in code-like format

To build format run the following command:
```
make
```
Run using:
```
./format.o
```
Once running, input expressions and statements and your input will be printed out in code-like format!

# Scrypt
Split into 3 files:
1. scrypt.h - header file of all function and class declarations
2. scryptfunctions.cpp - implementation of functions
3. scrypt.cpp - takes in input, uses functions, and produces output

Scrypt uses lexer to take in input and functions like a basic programming language. It enters conditionals if conditions are true, keeps track of variables, and prints when called to print.

Scrypt uses the following functions:
- buildProgram() - takes in tokens from lexer and builds an AST tree to represent program
- runProgram() - evaluates the AST tree and prints out necessary print statements
- ConvertAST2toAST3() - converts expression trees to fit onto the new tree that holds both expressions and statements
- ConverstAST3toAST2() - converts new tree that holds both expressions and statements into just expression trees, necessary in order to use evaluate() from calc
- parseBlock() - returns a vector of tokens included in a block { }
- elseIf() - special function that helps deal with consecutive else ifs and else after else ifs when building program
- enterStatement() - checks to see if condition of a statement is true or false

To build scrypt run the following command:
```
make
```
Run using:
```
./scrypt.o
```
Once running, input expressions and statements and it behave like a simple programming langauge! It will output print statements and evaluate statements/expressions!

