TARGET = FirstCube

# Which compiler
CC = g++

# libs and includes
LIBS = -lm -lps2dev 

.SUFFIXES: .c .cpp .s .vsm .dsm .o

SOURCE = main.cpp Matrix44Operation.cpp Matrix44.cpp Vector4.cpp PS2Math.h
OBJS = main.o Matrix44Operation.o Matrix44.o Vector4.o

# Options for development
CFLAGS = -g -Wall -fno-common -ffast-math -I ../..

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

.cpp.o:
	$(CC) -c $< $(CFLAGS) -o $@
	
.c.o:
	$(CC) -c $< $(CFLAGS) -o $@
	
depend:
	$(CC) $(CFLAGS) -MM *.cpp > .depend

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
	rm -f Makefile.bak
	rm -f .depend
	rm -f Dump.txt
	rm -f OutputDebug.txt 
	rm -f $(TARGET).tgz 
	rm -f -r html 
	rm -f -r rtf
package:
	tar czvf $(TARGET).tgz $(SOURCE) Makefile

run: $(TARGET)
	./FirstCube -ntsc
-include .depend

