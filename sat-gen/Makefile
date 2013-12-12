BOOST  = /cygdrive/d/libs/boost
SRC    = src
BIN    = bin
INC    = -I$(BOOST)
CARGS  = -Wall -Wno-uninitialized -O3 $(INC) 
LDARGS =

all: $(BIN)/gen

$(BIN)/gen: $(BIN)/main.o $(BIN)/clauses.o $(BIN)/generate.o $(BIN)/checker.o $(BIN)/reduce.o $(BIN)/tools.o
	g++ -o $(BIN)/gen $(BIN)/main.o $(BIN)/clauses.o $(BIN)/generate.o $(BIN)/checker.o $(BIN)/reduce.o $(BIN)/tools.o

$(BIN)/main.o: $(SRC)/main.cpp $(SRC)/reduce.h $(SRC)/consts.h $(SRC)/clauses.h $(SRC)/generate.h $(SRC)/checker.h $(SRC)/tools.h
	g++ $(CARGS) -c -o $(BIN)/main.o $(SRC)/main.cpp
    
$(BIN)/clauses.o: $(SRC)/clauses.cpp $(SRC)/consts.h $(SRC)/clauses.h
	g++ $(CARGS) -c -o $(BIN)/clauses.o $(SRC)/clauses.cpp

$(BIN)/generate.o: $(SRC)/generate.cpp $(SRC)/consts.h $(SRC)/clauses.h $(SRC)/generate.h $(SRC)/checker.h
	g++ $(CARGS) -c -o $(BIN)/generate.o $(SRC)/generate.cpp
    
$(BIN)/checker.o: $(SRC)/checker.cpp $(SRC)/consts.h $(SRC)/checker.h $(SRC)/clauses.h
	g++ $(CARGS) -c -o $(BIN)/checker.o $(SRC)/checker.cpp

$(BIN)/reduce.o: $(SRC)/reduce.cpp $(SRC)/reduce.h $(SRC)/consts.h $(SRC)/checker.h $(SRC)/clauses.h
	g++ $(CARGS) -c -o $(BIN)/reduce.o $(SRC)/reduce.cpp
  
$(BIN)/tools.o: $(SRC)/tools.cpp $(SRC)/tools.h
	g++ $(CARGS) -c -o $(BIN)/tools.o $(SRC)/tools.cpp
  
clean:
	rm -f bin/*.o bin/gen