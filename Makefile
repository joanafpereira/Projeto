CFLAGS=-Wall -Wextra -pedantic -O2
LIBS=-lm -lcurses

jogo: jogo.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm jogo *.o
