#pragma once


static int counter;

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
}
void quit(int exitStatus);
void antiAliasDraw(void);
void init(void);
