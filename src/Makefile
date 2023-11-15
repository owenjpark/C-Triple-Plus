CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17

TARGETS = lex parse calc format scrypt

all: $(TARGETS)

lex: lib/lexfunctions.cpp lex.cpp
	$(CXX) $(CXXFLAGS) lib/lexfunctions.cpp lex.cpp -o lex

parse: lib/lexfunctions.cpp lib/parsefunctions.cpp parse.cpp
	$(CXX) $(CXXFLAGS) lib/lexfunctions.cpp lib/parsefunctions.cpp parse.cpp -o parse

calc: lib/lexfunctions.cpp lib/calcfunctions.cpp calc.cpp
	$(CXX) $(CXXFLAGS) lib/lexfunctions.cpp lib/calcfunctions.cpp calc.cpp -o calc

format: lib/lexfunctions.cpp lib/calcfunctions.cpp lib/formatfunctions.cpp format.cpp
	$(CXX) $(CXXFLAGS) lib/lexfunctions.cpp lib/calcfunctions.cpp lib/formatfunctions.cpp format.cpp -o format

scrypt: lib/lexfunctions.cpp lib/calcfunctions.cpp lib/scryptFunctions.cpp scrypt.cpp
	$(CXX) $(CXXFLAGS) lib/lexfunctions.cpp lib/calcfunctions.cpp lib/scryptFunctions.cpp scrypt.cpp -o scrypt
clean:
	rm -f $(TARGETS)