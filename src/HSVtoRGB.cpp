/****************************************************************************
Program:        VisuQS.LX / VQS.exe
File:           HSVtoRGB.cpp
Description:    Used to implement colour gradients
Author:         E. Christopher Dyken
Environment:    (Edited using) Borland C++BuilderX-Personal V.1.0.0.1786
Source:         www.uio.no/studier/emner/matnat/ifi/INF3320/h04/undervisningsmateriale/oblig1.pdf
****************************************************************************/

#include "HSVtoRGB.h"
#include <iostream>

const double pi = 3.141592654;

void hsv_to_rgb(float* rgb, float* hsv)
{
  float h,s,v,f,p,q,t;
  int i;

  h = *hsv;
  s = 1.0;
  v = 1.0;

  if(s <= FLT_EPSILON)
  {
    for(i=0; i<3; i++)
      rgb[i] = v;
  }
  else
  {
    while(h<0.0)
      h+=360.0;
    while(h>=360.0)
      h-=360.0;

    h/=60.0;
    i = (int)floor((double)h); // floorf undefined (RM)
    f = h-(float)i;
    p = v*(1.0-s);
    q = v*(1.0-(s*f));
    t = v*(1.0-(s*(1.0-f)));

    switch(i)
    {
      case 0: rgb[0] = v; rgb[1] = t; rgb[2] = p; break;
      case 1: rgb[0] = q; rgb[1] = v; rgb[2] = p; break;
      case 2: rgb[0] = p; rgb[1] = v; rgb[2] = t; break;
      case 3: rgb[0] = p; rgb[1] = q; rgb[2] = v; break;
      case 4: rgb[0] = t; rgb[1] = p; rgb[2] = v; break;
      case 5: rgb[0] = v; rgb[1] = p; rgb[2] = q; break;
    }
  }
}
