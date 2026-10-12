##########################################################
# Makefile will build an executable made from C++ files.
# Henry S Fortuna
#
# To create the dependency file:
# make depend
#
# To build the executable:
# make
#
# To execute:
# make run
#
# To clean out all objects, targets and dependencies
# make clean
##########################################################

# The target name
TARGET = main 

# The compiler being used
CC = g++

# The known file extensions
.SUFFIXES: .cpp .o

# The object files to build
OBJS = main.o ctexture.o

# The libraries to link
LIBS = -lm -lsps2util

# The compiler flags to use
# -g - produce debugging information
# -Wall - turn most of the important warnings on
# -Werror - treat warnings as errors and abort compilation
# -ffast-math - allow less checking for faster maths routines
# - fsingle-precision-constant - use single precision constants
CFLAGS = -g -Wall -Werror -ffast-math -fsingle-precision-constant

# build the target file from the objects
$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS) $(LIBS)

# build the object files from the c source files
.cpp.o:
	$(CC) -c $< $(CFLAGS) -o $@

# create the dependancy file	
depend:
	$(CC) $(CFLAGS) -MM *.cpp > .depend
	
run:
	./main

# remove all the built files
clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
	rm -f .depend

#include the dependencies into the build
-include .depend