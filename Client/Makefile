# Fichier Makefile

CC = gcc
CFLAGS = -g -Wall -D _SVID_SOURCE 
CFLAG2 = -c -Wall -D _SVID_SOURCE
OBJETS = CL.o moniteur.o

all: CL

CL : $(OBJETS)
	$(CC) $(CFLAGS) $(OBJETS) -o CL

CL.o : CL.c CL_include
	$(CC) $(CFLAGS) CL.c -o CL.o

moniteur.o : moniteur.c CL_include moniteur.h
	$(CC) $(CFLAG2) moniteur.c -o moniteur.o

clean:
	@rm -f *.o CL
