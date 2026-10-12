include ../config.mak

LIBNAME = libmpflac.a

CFLAGS = $(OPTFLAGS) -I. -I..

SRCS = dither.c replaygain_synthesis.c
ifeq ($(MPFLAC),full)
SRCS += bitbuffer.c bitmath.c cpu.c crc.c fixed.c format.c lpc.c stream_decoder.c metadata_object.c
CFLAGS += -DUSE_MPFLAC_DECODER=1
endif
OBJS = $(SRCS:.c=.o)


.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

$(LIBNAME): $(OBJS)
	$(AR) r $(LIBNAME) $(OBJS)

all: $(LIBNAME)

clean:
	rm -f *.o *.a *~

distclean:
	rm -f Makefile.bak *.o *.a *~ .depend

dep: depend

depend:
	$(CC) -MM $(CFLAGS) $(SRCS) 1>.depend

ifneq ($(wildcard .depend),)
include .depend
endif

