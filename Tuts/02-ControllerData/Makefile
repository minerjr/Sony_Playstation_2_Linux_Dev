TARGET = ControllerData

# Which compiler
CC = g++

LIBS   = 

.SUFFIXES: .c .cpp .s .vsm .dsm .o

SOURCE = main.cpp 
OBJS = main.o

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
	rm -f $(TARGET).tgz 
	rm -f -r html 
	rm -f -r rtf
package:
	tar czvf $(TARGET).tgz $(SOURCE) Makefile

run: $(TARGET)
	./ControllerData
-include .depend

