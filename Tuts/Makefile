#manage make utility for all tutorials

#Do not forget to correctly set variable to locate sps2 directory
#this version point to my sps2 directory.
SPS2DIR = /Develop/sps2dev-0.3.0a
SOURCE = 01-FirstCube 02-ControllerManager 01-FirstCube_sps2 \
	 02-MovingCube 02-ControllerData 03-TexturedCube \
	 04-MacroMode 04-MicroMode

all:
	make -C 01-FirstCube 
	make -C 01-FirstCube_sps2 
	make -C 02-ControllerManager 
	make -C 02-MovingCube 
	make -C 02-ControllerData 
	make -C 03-TexturedCube 
	make -C 04-MacroMode 
	make -C 04-MicroMode 	 
clean:
	make -C 01-FirstCube clean
	make -C 01-FirstCube_sps2 clean
	make -C 02-ControllerManager clean
	make -C 02-ControllerData clean
	make -C 02-MovingCube clean
	make -C 03-TexturedCube clean
	make -C 04-MacroMode  clean
	make -C 04-MicroMode clean 
	
package:
	tar czvf Tutorials.tgz $(SOURCE) Makefile README
