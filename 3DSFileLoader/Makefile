CC			= g++

TARGET		= 3DSLoader

$(TARGET):	Main.o Init.o 3ds.o
	$(CC) -o $(TARGET) Main.o Init.o 3ds.o `sdl-config --libs` -lGL -lGLU -lm

Main.o:		Main.cpp main.h
	$(CC) -c Main.cpp `sdl-config --cflags`

3ds.o:	    3ds.cpp 3ds.h
	$(CC) -c 3ds.cpp `sdl-config --cflags`

Init.o:		Init.cpp main.h
	$(CC) -c Init.cpp `sdl-config --cflags`

clean:
	rm -f $(TARGET) Main.o Init.o 3ds.o core
