CXX=gcc -Wall -g -O3
CXX2= nvcc -g -O3

CPPFLAGS=-Iinc  --generate-code arch=compute_35,code=sm_35

FILES=Rectangle_Maximal load algo1 algo2 dvpr
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
	./bin/run Ressources/ex_N200000_res75141975 3 0
clean :
	rm -f bin/* obj/*
