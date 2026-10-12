TARGET = MicroMode

# Which compiler
CC = g++
#SPS2DIR = ~/sps2dev-0.3.0a
LIBS   = -lm -lsps2util -L$(SPS2DIR) #libs and includes


.SUFFIXES: .c .cpp .s .vsm .dsm .o .bin

SOURCE = main.cpp Matrix44Operation.cpp Matrix44.cpp Vector4.cpp \
	 TextureManager.cpp Screenshot.cpp \
	 PS2Math.h Joypad.cpp Joypad.h JoypadConst.h \
	 TextureManager.h PS2Utils.h Screenshot.h \
	 BasicTransform.vsm
OBJS = main.o Matrix44Operation.o Matrix44.o Vector4.o Joypad.o TextureManager.o \
       Screenshot.o 
BIN  = BasicTransform.bin

# Options for development
CFLAGS = -g -Wall -fno-common -ffast-math  -I$(SPS2DIR) 

$(TARGET): $(OBJS) $(BIN)
	$(CC) -o $@ $(OBJS) $(LIBS)

.cpp.o:
	$(CC) -c $< $(CFLAGS) -o $@
	
.c.o:
	$(CC) -c $< $(CFLAGS) -o $@

# Rule for compiling .vsm -> bin	
.vsm.bin:
	ee-dvp-as -o $*.vo_ $*.vsm
	objcopy -Obinary $*.vo_ $*.bin
	rm $*.vo_
	
depend:
	$(CC) $(CFLAGS) -MM *.cpp > .depend

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
	rm -f $(BIN)
	rm -f Makefile.bak
	rm -f .depend
	rm -f $(TARGET).tgz 
	rm -f -r html 
	rm -f -r rtf
	rm -f *tga
	rm -f *bak
	rm -f -r *_
package:
	tar czvf $(TARGET).tgz $(SOURCE) Makefile

run: $(TARGET)
	./MicroMode
-include .depend

