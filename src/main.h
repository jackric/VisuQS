#pragma once


static int counter;

static char *revisionSTR= "$Revision: 1.2 $";
static char *authorSTR= "$Author: phuhgh $";
static char *dateSTR= "$Date: 2009/11/11 23:58:33 $";
static int moving, startx, starty, n;
static float angle, angle2, radius;
struct drawSite
{
	int i;
	int j;
	int k;
	float r,g,b;
	float size;	
};

namespace DrawStyles {
    enum Enum {
        Bubbles, Fog, Cubes
    };
};
void quit(int exitStatus);
void antiAliasDraw(void);
void init(void);
