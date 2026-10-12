CC			= g++

TARGET		= Md2Animation

$(TARGET):	Main.o Init.o Md2.o
	$(CC) -o $(TARGET) Main.o Init.o Md2.o `sdl-config --libs` -lGL -lGLU -lm

Main.o:		Main.cpp main.h
	$(CC) -c Main.cpp `sdl-config --cflags`

Md2.o:	    Md2.cpp Md2.h
	$(CC) -c Md2.cpp `sdl-config --cflags`

Init.o:		Init.cpp main.h
	$(CC) -c Init.cpp `sdl-config --cflags`

clean:
	rm -f $(TARGET) Main.o Init.o Md2.o core
