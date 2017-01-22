CXX= nvcc -g -O3

CPPFLAGS=-Iinc  --generate-code arch=compute_35,code=sm_35

FILES=Rectangle_Maximal load algo1 algo2
OBJECTS=$(addsuffix .o, $(FILES))
BIN= bin/run

all : $(BIN)

$(BIN) : $(addprefix obj/, $(OBJECTS))
	$(CXX) -o $@ $^

obj/%.o : src/%.cu
	$(CXX) -c -o $@ $^ $(CPPFLAGS) 	

run :
	./bin/run /users/Enseignants/fortin/Public/HPCA_oct2016/Projet/JeuxDeDonnees/Perf/ex_N131072_alea 2
	./bin/run /users/Enseignants/fortin/Public/HPCA_oct2016/Projet/JeuxDeDonnees/Verif/ex_N500_res7854 1

clean :
	rm -f bin/* obj/*
