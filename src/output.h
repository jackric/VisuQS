#include <cassert>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <GL/glut.h>


#ifndef OUTPUT_H
#define OUTPUT_H

void spewWireFrameEPS(FILE * file, int doSort, GLint size, GLfloat * buffer, char *creator);
//void printBuffer(GLint size, GLfloat * buffer);

#endif // OUTPUT_H
