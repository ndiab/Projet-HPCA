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
	mpirun -hostfile hostfile -np 2 -npersocket 1 bin/run /users/Enseignants/fortin/Public/HPCA_oct2016/Projet/JeuxDeDonnees/Perf/ex_N67108864_alea 4
	./bin/run /users/Enseignants/fortin/Public/HPCA_oct2016/Projet/JeuxDeDonnees/Perf/ex_N67108864_alea 3
	./bin/run /users/Enseignants/fortin/Public/HPCA_oct2016/Projet/JeuxDeDonnees/Perf/ex_N131072_alea 2
	./bin/run /users/Enseignants/fortin/Public/HPCA_oct2016/Projet/JeuxDeDonnees/Verif/ex_N500_res7854 1
	


clean :
	rm -f bin/* obj/*
