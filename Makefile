export PKG_CONFIG_PATH=/home/tapir/Projects/libs/raylib/build/raylib
PKGS=raylib
CFLAGS=-Wall -Wextra -ggdb -pedantic -std=c11 `pkg-config --cflags --static $(PKGS)`
LIBS=`pkg-config --libs --static $(PKGS)`

all: main test1 test2

test2: test2.c
	$(CC) $(CFLAGS) -o test2 test2.c $(LIBS) -lm
test1: test1.c
	$(CC) $(CFLAGS) -o test1 test1.c $(LIBS) -lm
main: main.c
	$(CC) $(CFLAGS) -o main main.c $(LIBS) -lm
