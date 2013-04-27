EXE_DIR = ../VQSEXE/
EXE = VisuQS.LX
CPP = g++

#LIBFLAGS  = -L/usr/X11/include -lglut -lOpenGL -lGLU -lX11 -lXmu -lXi
#_GL_DIR = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/System/Library/Frameworks/OpenGL.framework/Versions/A/OpenGL
LIBFLAGS  = -L/usr/X11/lib -L/usr/X11/include  -lglut  -lX11 -lXmu -lXi -framework OpenGL
CPPFLAGS = -Wfatal-errors -Wno-deprecated -fpermissive $(LIBFLAGS)
debug: CPPFLAGS += -D_DEBUG
debug: executable

release: CPPFLAGS += -O3
release: executable

all: executable



#LIBFLAGS  = -L/usr/X11R6/lib -lglut -lGL -lGLU -lX11 -lXmu -lXi
OBJ_FILES = main.o createInput.o readData.o readInput.o quickSort.o output.o HSVtoRGB.o util.o capture.o annotations.o

executable:	$(OBJ_FILES)

	$(CPP) $(CPPFLAGS) -o $(EXE) \
	$(OBJ_FILES) $(LIBFLAGS)
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

util.o:	util.cpp
	$(CPP) $(CPPFLAGS) -c util.cpp

capture.o:	capture.cpp
	$(CPP) $(CPPFLAGS) -c capture.cpp

annotations.o:	annotations.h
	$(CPP) $(CPPFLAGS) -c annotations.cpp

clean:
	cd $(EXE_DIR)
	rm *.o

style:
	astyle -n --style=allman *.cpp
