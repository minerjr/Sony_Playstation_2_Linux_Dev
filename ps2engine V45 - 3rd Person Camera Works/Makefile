SOURCES 	= dma4kpage dmamemory dmapacket dmascheduler vumgr matrixfunc \
				matrix addregisters custommodel md2file ms3dfile bitmap texture joypadlib \
				PS2Pad input timestamp perftime gameclock action actionlist collision \
				demon event acquaintance area gossip gossipengine timelineevent timelineday timeline \
				npc customimage keyframe q3bspfile level
OBJECTS 	= $(addsuffix .o, $(basename $(SOURCES)))
TARGET  	= libps2engine

VU1MODULES	= trans
VU1SOURCES	= $(addsuffix .vsm, $(VU1MODULES))
VU1CODECS	= $(addsuffix .vu, $(VU1MODULES))

CFLAGS  	= $(OPT) -I../sps2dev-0.3.0a
CXXFLAGS 	= $(CFLAGS) 

LDFLAGS  	= -L~/sps2dev-0.3.0a 
LIBS		= -lm -lsps2util -lps2dev
OPT		= -g -Wall -Wmissing-prototypes 

CXX		= c++
CC		= cc
CP		= /bin/cp -fauv
RM		= /bin/rm -f
MKDIR		= /bin/mkdir -p
OBJCOPY 	= objcopy
VU_AS 		= ee-dvp-as
NM 		= nm

INC_INSTALL 	= ../include
LIB_INSTALL 	= ../lib


all: depend lib vu1codecs main

opt:
	make all OPT="-O2 -fomit-frame-pointer -ffast-math"

lib: $(TARGET).a

$(TARGET).a: $(OBJECTS)
	ar -rus $@ $^

vu1codecs: $(VU1CODECS)


main : main.o $(TARGET).a
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	- $(RM) $(DEPENDS) *.a *.o *.vu *.vh *.lst libexec/*/* main 
run:	all
	./main
	
install: lib $(INC_INSTALL) $(LIB_INSTALL)
	$(CP) $(wildcard *.h) $(INC_INSTALL)
	$(CP) $(TARGET).a $(LIB_INSTALL)

$(INSTALL_INCLUDE_PATH):
	$(MKDIR) $@

$(INSTALL_LIB_PATH):
	$(MKDIR) $@

depend: $(DEPENDS)

#
# depend
#
DEPENDS = $(addprefix ., $(OBJECTS:.o=.d))

.%.d : %.c
	$(CC) $(CFLAGS) -MM $< | sed 's/$*.o/& $@/g' > $@

#.%.d : %.cpp
#	$(CXX) $(CXXFLAGS) -MM $< | sed 's/$*.o/& $@/g' > $@

.vsm.vu:
	$(VU_AS) -g -o $*.vo $< > $*.lst
	@$(NM) -g -P $*.vo | sed -e 's/^/#define /' -e 's/[^ ] 00\+/0x0/' >$*.vh
	@$(OBJCOPY) -Obinary $*.vo $@
	@$(RM) *.vo
	@$(CP) $@ libexec/vu1


-include $(DEPENDS)

.PHONY: all install lib clean as dist
.SUFFIXES: .d .vsm .vu
