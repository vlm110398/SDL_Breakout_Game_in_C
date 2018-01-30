CC=gcc

CFLAGS=-g -Wall -D_GNU_SOURCE=1 -D_REENTRANT -std=c90 -pedantic
#CFLAGS=-g -Wall -D_REENTRANT
MIXER_FLAGS=-lSDL2_mixer
IMAGE_FLAGS=-lSDL2_image
SFLAGS=-lSDL2main -lSDL2
SOURCES=06_moving_images.c
BINARIES=06_moving_images

#all: ex0 ex1 ex2 ex3 ex4 ex5 ex6 ex7 grafico texto
all: $(BINARIES)

06_moving_images: 06_moving_images.c
	$(CC) -o 06_moving_images 06_moving_images.c $(CFLAGS) $(SFLAGS) $(IMAGE_FLAGS) $(IMAGE_MIXER)

clean:
	rm -rf *.o *.exe *.bak *.c~ $(BINARIES) core a.out
