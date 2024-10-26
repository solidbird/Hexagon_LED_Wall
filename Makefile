export PKG_CONFIG_PATH=/home/tapir/Projects/libs/raylib/build/raylib
PKGS=raylib
CFLAGS=-Wall -Wextra -ggdb -pedantic -std=c11 `pkg-config --cflags --static $(PKGS)`
LIBS=`pkg-config --libs --static $(PKGS)`
HEADER_IMPLEMENTATIONS=

all: main
 
main: main.c hexagon.c timer.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) -lm
