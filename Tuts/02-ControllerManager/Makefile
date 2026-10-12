TARGET = ControllerManager

# Which compiler
CC = g++

LIBS   = 

.SUFFIXES: .c .cpp .s .vsm .dsm .o

SOURCE = main.cpp Joypad.cpp Joypad.h JoypadConst.h
OBJS = main.o Joypad.o

# Options for development
CFLAGS = -g -Wall -fno-common -ffast-math

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
	./ControllerManager
-include .depend

