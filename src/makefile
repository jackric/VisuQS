EXE_DIR = ../VQSEXE/
EXE = VisuQS.LX
CPP = g++

#CPPFLAGS = -Wno-deprecated -fpermissive -D_LINUX -D_DEBUG# debugging
CPPFLAGS = -Wno-deprecated -fpermissive -D_LINUX -O3 # release

LIBFLAGS  = -L/usr/X11R6/lib -lglut -lGL -lGLU -lX11 -lXmu -lXi

$(EXE):	main.o createInput.o readData.o readInput.o quickSort.o output.o HSVtoRGB.o

	$(CPP) $(CPPFLAGS) -o $(EXE) \
	main.o createInput.o readData.o readInput.o quickSort.o output.o HSVtoRGB.o $(LIBFLAGS)
	mv *.o $(EXE_DIR)
	mv $(EXE) $(EXE_DIR)

main.o:		main.cpp
	$(CPP) $(CPPFLAGS) -c main.cpp

createInput.o:	createInput.cpp
	$(CPP) $(CPPFLAGS) -c createInput.cpp

readData.o:	readData.cpp
	$(CPP) $(CPPFLAGS) -c readData.cpp

readInput.o:	readInput.cpp
	$(CPP) $(CPPFLAGS) -c readInput.cpp

quickSort.o:	quickSort.cpp
	$(CPP) $(CPPFLAGS) -c quickSort.cpp

output.o:	output.cpp
	$(CPP) $(CPPFLAGS) -c output.cpp

HSVtoRGB.o:	HSVtoRGB.cpp
	$(CPP) $(CPPFLAGS) -c HSVtoRGB.cpp

clean:
	cd $(EXE_DIR)
	rm *.o