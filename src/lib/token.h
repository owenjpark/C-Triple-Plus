#ifndef CALC_H
#define CALC_H

#include <string>

struct Token {
    int line;
    int column;
    std::string text;
};

struct leftparenthesis: public Token {
};

struct rightparenthesis: public Token {
};

struct plus: public Token {
};

struct minus: public Token {
};

struct multiplication: public Token {
};

struct division: public Token {
};

struct floatingnum: public Token {
};

struct END: public Token {

};

#endif