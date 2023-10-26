CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17

all: lex.o parse.o calc.o

lex.o: lib/lexfunctions.cpp lex.cpp
	$(CXX) $(CXXFLAGS) lib/lexfunctions.cpp lex.cpp -o lex.o

parse.o: lib/lexfunctions.cpp lib/parsefunctions.cpp parse.cpp
	$(CXX) $(CXXFLAGS) lib/lexfunctions.cpp lib/parsefunctions.cpp parse.cpp -o parse.o

calc.o: lib/lexfunctions.cpp calc.cpp
	$(CXX) $(CXXFLAGS) lib/lexfunctions.cpp calc.cpp -o calc.o
	
clean:
	rm *.o