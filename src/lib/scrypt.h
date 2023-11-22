#ifndef SCRYPT_H
#define SCRYPT_H

#include "lex.h"
#include "calc.h"
#include <memory>



// main functions
shared_ptr<AST2::Node> buildProgram(const vector<token> &vec);

variable runProgram(const shared_ptr<AST2::Node> &node, vector<variable> &variables);

// helper functions


vector<token> parseBlock(unsigned &i, const vector<token> &vec);

bool elseIf (const vector<token> &vec, unsigned &i, shared_ptr<AST2::Node> &node);

bool enterStatement (const shared_ptr<AST2::Node> &root, vector<variable> &variables);



#endif