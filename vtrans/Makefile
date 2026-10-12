TARGET = main

CC = g++

.SUFFIXES: .cpp .o .vcl

OBJS = main.o ctexture.o sps2wrap.o vu1code.o 

LIBS = -lm -lsps2util

CFLAGS = -g -Wall -Werror -ffast-math -fsingle-precision-constant

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

.cpp.o:
	$(CC) -c $< $(CFLAGS) -o $@

# Rule for compiling .vcl -> .o	
.vcl.o:
	vcl -g -o$*.vsm $*.vcl
	ee-dvp-as -o $*.vo_ $*.vsm
	objcopy -Obinary $*.vo_ $*.bin_
	./bin2as $* $*.bin_ > $*.a_
	as -mcpu=r5900 -KPIC -mips2 -o $*.o $*.a_
	rm $*.vo_ $*.bin_ $*.a_

*.vcl: bin2as
bin2as:  bin2as.cpp
	cc -o bin2as bin2as.cpp

# create the dependancy file	
depend:
	$(CC) $(CFLAGS) -MM *.cpp > .depend

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
	rm -f .depend
	rm -f *.vsm

#include the dependencies into the build
-include .depend