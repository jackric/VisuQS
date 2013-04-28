EXE_DIR = ../VQSEXE/
EXE = VisuQS.LX
CPP = g++

#LIBFLAGS  = -L/usr/X11/include -lglut -lOpenGL -lGLU -lX11 -lXmu -lXi
#_GL_DIR = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/System/Library/Frameworks/OpenGL.framework/Versions/A/OpenGL
LIBFLAGS  = -L/usr/X11/lib -L/usr/X11/include  -lglut  -lX11 -lXmu -lXi -framework OpenGL
CPPFLAGS = -Wall -pedantic -std=c99 -Wno-variadic-macros
debug: CPPFLAGS += -D_DEBUG
debug: executable

release: CPPFLAGS += -O3
release: executable

all: executable


.cpp.o:
	$(CPP) $(CPPFLAGS) -c -o $@ $<

#LIBFLAGS  = -L/usr/X11R6/lib -lglut -lGL -lGLU -lX11 -lXmu -lXi
OBJ_FILES = main.o readData.o readInput.o quickSort.o HSVtoRGB.o util.o capture.o annotations.o interaction.o


executable:	$(OBJ_FILES)

	$(CPP) $(CPPFLAGS) -o $(EXE) $(OBJ_FILES) $(LIBFLAGS)
	mv *.o $(EXE_DIR)
	mv $(EXE) $(EXE_DIR)


clean:
	cd $(EXE_DIR)
	rm *.o

style:
	astyle -n --style=allman *.cpp
