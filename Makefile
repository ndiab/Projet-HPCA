CXX=gcc -Wall -g -O3
CXX2= nvcc -g -O3

CPPFLAGS=-Iinc


FILES=Rectangle_Maximal load algo1 algo2
OBJECTS=$(addsuffix .o, $(FILES))
BIN=bin/run

all : $(BIN)

$(BIN) : $(addprefix obj/, $(OBJECTS))
	$(CXX2) -o $@ $^

obj/%.o : src/%.cu
	$(CXX2) -c -o $@ $^ $(CPPFLAGS) 

#obj/%.o : src/%.c
#	$(CXX) -c -o $@ $^ $(CPPFLAGS) 

exec : 
	./bin/run ex_N100_res6741 2 0
clean :
	rm -f bin/* obj/*
