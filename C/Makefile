CXX=mpicc -Wall -g -O3 -fopenmp 


CPPFLAGS=-Iinc -fopenmp

FILES=Rectangle_Maximal load algo1 algo2 dvpr
OBJECTS=$(addsuffix .o, $(FILES))
BIN=bin/run

all : $(BIN)

$(BIN) : $(addprefix obj/, $(OBJECTS))
	$(CXX) -o $@ $^

obj/%.o : src/%.c
	$(CXX) -c -o $@ $^ $(CPPFLAGS) 

run :
	./bin/run ../Ressources/ex_N100000_res100000 3


clean :
	rm -f bin/* obj/*
