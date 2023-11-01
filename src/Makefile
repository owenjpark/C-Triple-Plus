CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17

all: lex.o parse.o calc.o format.o

lex.o: lib/lexfunctions.cpp lex.cpp
	$(CXX) $(CXXFLAGS) lib/lexfunctions.cpp lex.cpp -o lex.o

parse.o: lib/lexfunctions.cpp lib/parsefunctions.cpp parse.cpp
	$(CXX) $(CXXFLAGS) lib/lexfunctions.cpp lib/parsefunctions.cpp parse.cpp -o parse.o

calc.o: lib/lexfunctions.cpp lib/calcfunctions.cpp calc.cpp
	$(CXX) $(CXXFLAGS) lib/lexfunctions.cpp lib/calcfunctions.cpp calc.cpp -o calc.o

format.o: lib/lexfunctions.cpp lib/calcfunctions.cpp lib/formatfunctions.cpp format.cpp
	$(CXX) $(CXXFLAGS) lib/lexfunctions.cpp lib/calcfunctions.cpp lib/formatfunctions.cpp format.cpp -o format.o
	
clean:
	rm *.o