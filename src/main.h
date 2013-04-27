#pragma once

static char *revisionSTR= "$Revision: 1.2 $";
static char *authorSTR= "$Author: phuhgh $";
static char *dateSTR= "$Date: 2009/11/11 23:58:33 $";
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
