SHARED_SRCS = soundlib.c soundlib.h joypadlib.c joypadlib.h
SAMPLE_SRCS = main_32_sounds.c main_left_right.c main_frequency.c main_callback.c main_callbacks.c main_instances.c main_3d.c main_sound_play.c
ALL_SRCS = $(SAMPLE_SRCS) $(SHARED_SRCS)

SHARED_OBJS = soundlib.o joypadlib.o
SAMPLE_OBJS = main_32_sounds.o main_left_right.o main_frequency.o main_callback.o main_callbacks.o main_instances.o main_3d.o main_sound_play.o
SAMPLE_PROGS = main_32_sounds main_left_right main_frequency main_callback main_callbacks main_instances main_3d main_sound_play
ALL_OBJS = $(SAMPLE_OBJS) $(SHARED_OBJS)

OTHER_FILES = Makefile doc.htm 1.wav 2.wav 3.wav 4.wav 5.wav engine-stereo.wav leftright-stereo.wav outofhere-mono.wav

ALL_PACKAGE_FILES = $(ALL_SRCS) $(OTHER_FILES)
PACKAGE_NAME = soundlib-0.1.tar

main_32_sounds: main_32_sounds.o $(SHARED_OBJS)
	gcc -o main_32_sounds main_32_sounds.o $(SHARED_OBJS)

main_left_right: main_left_right.o $(SHARED_OBJS)
	gcc -o main_left_right main_left_right.o $(SHARED_OBJS)

main_frequency: main_frequency.o $(SHARED_OBJS)
	gcc -o main_frequency main_frequency.o $(SHARED_OBJS)

main_callback: main_callback.o $(SHARED_OBJS)
	gcc -o main_callback main_callback.o $(SHARED_OBJS)

main_callbacks: main_callbacks.o $(SHARED_OBJS)
	gcc -o main_callbacks main_callbacks.o $(SHARED_OBJS)

main_instances: main_instances.o $(SHARED_OBJS)
	gcc -o main_instances main_instances.o $(SHARED_OBJS)

main_3d: main_3d.o $(SHARED_OBJS)
	gcc -o main_3d main_3d.o $(SHARED_OBJS)

main_sound_play: main_sound_play.o $(SHARED_OBJS)
	gcc -o main_sound_play main_sound_play.o $(SHARED_OBJS) -lvorbisidec -lmad

package: 
	tar -cf $(PACKAGE_NAME) $(ALL_PACKAGE_FILES)	

list_package: package
	tar -tvf $(PACKAGE_NAME)

output_package: package
	cp $(PACKAGE_NAME) ../$(PACKAGE_NAME)

clean:
	rm -f $(SAMPLE_PROGS)
	rm -f $(ALL_OBJS)
	rm -f $(PACKAGE_NAME)
