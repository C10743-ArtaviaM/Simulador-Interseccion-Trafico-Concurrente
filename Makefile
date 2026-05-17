CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
LIBS = -lpthread

all: interseccion

interseccion: interseccion.c
	$(CC) $(CFLAGS) interseccion.c -o interseccion $(LIBS)

fase1: interseccion.c
	$(CC) $(CFLAGS) -DSOLO_FASE1 interseccion.c -o interseccion $(LIBS)

fase2: interseccion.c
	$(CC) $(CFLAGS) -DSOLO_FASE2 interseccion.c -o interseccion $(LIBS)

run: interseccion
	./interseccion

clean:
	rm -f interseccion