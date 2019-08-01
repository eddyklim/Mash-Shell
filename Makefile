CC=gcc
CFLAGS=-g -I. -Wall 
LDFLAGS=-lm

binaries=mash

all: $(binaries)

mash: mash.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
        
clean:
	$(RM) -f $(binaries) *.o


 

