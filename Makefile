
DIRS = C cuda

all : 
	for d in $(DIRS); do (cd $$d; ${MAKE} all);done

exec : 
	for d in $(DIRS); do (cd $$d; ${MAKE} run);done
clean :
	for d in $(DIRS); do (cd $$d; ${MAKE} clean);done
