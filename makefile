OBJ= list.o aStar.o status.o map.o
aStar : $(OBJ)	
	gcc -o aStar $(OBJ)
	aStar Rennes Lyon

make clean :
	rm -f *.o *.exe

list.o: ./status/status.h list.h
	gcc -c list.c

aStar.o : list.h ./status/status.h map.h
	gcc -c aStar.c

status.o : status.h 
	gcc -c status.c

map.o : map.h ./status/status.h
	gcc -c map.c