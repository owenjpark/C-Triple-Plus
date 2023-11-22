#ifndef SCRYPT_H
#define SCRYPT_H

#include "lex.h"
#include "calc.h"



// main functions
shared_ptr<AST3::Node> buildProgram(const vector<token> &vec);

void runProgram(const shared_ptr<AST3::Node> &node, vector<variable> &variables);

// helper functions
shared_ptr<AST3::Node> ConvertAST2ToAST3(const shared_ptr<AST2::Node> &node2);

shared_ptr<AST2::Node> ConvertAST3ToAST2(const shared_ptr<AST3::Node> &node3);

vector<token> parseBlock(unsigned &i, const vector<token> &vec);

bool elseIf (const vector<token> &vec, unsigned &i, shared_ptr<AST3::Node> &node);

bool enterStatement (const shared_ptr<AST3::Node> &root, vector<variable> &variables);

#endif