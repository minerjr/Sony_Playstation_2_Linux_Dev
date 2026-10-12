TARGET = TexturedCube

# Which compiler
CC = g++
#SPS2DIR = /Develop/sps2dev-0.3.0a
LIBS   = -lm -lsps2util -L$(SPS2DIR) #libs and includes


.SUFFIXES: .c .cpp .s .vsm .dsm .o

SOURCE = main.cpp Matrix44Operation.cpp Matrix44.cpp Vector4.cpp \
	 TextureManager.cpp Screenshot.cpp \
	 PS2Math.h Joypad.cpp Joypad.h JoypadConst.h \
	 TextureManager.h PS2Utils.h Screenshot.h
OBJS = main.o Matrix44Operation.o Matrix44.o Vector4.o Joypad.o TextureManager.o \
       Screenshot.o

# Options for development
CFLAGS = -g -Wall -fno-common -ffast-math  -I$(SPS2DIR) 

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
	rm -f $(TARGET).tgz 
	rm -f -r html 
	rm -f -r rtf
	rm -f -r *tga
package:
	tar czvf $(TARGET).tgz $(SOURCE) Makefile

run: $(TARGET)
	./TexturedCube
-include .depend

