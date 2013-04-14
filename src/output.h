#pragma once

#include <cassert>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <GL/glut.h>


void spewWireFrameEPS(FILE * file, int doSort, GLint size, GLfloat * buffer, char *creator);
//void printBuffer(GLint size, GLfloat * buffer);
