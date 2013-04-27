#include <GL/glut.h>
#include "debug.h"
#include <cstdio>
#include <cmath>
#include "readInput.h"
#include "annotations.h"
#include "HSVtoRGB.h"
#include "util.h"
#include "constants.h"

void drawColourScale(Input incommands, float threshold, float max)
{
    int k;
    int sections = 300;
    float hue, r, g, b;
    float length = 1.85;
    char charMax[15], threeQuarters[15], half[15], quarter[15], bottom[15];
    float exponent;
    float rgb[3];

    DEBUG("drawColourScale()");

    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glLineWidth(8);
    glBegin(GL_LINES);

    for(k = 0; k < sections; k++)
    {
        hue = (360.0/sections)*(sections-k);
        if(incommands.getthreshold() == 1.0) // cuts bottom of scale off according to probability threshold
            hue = 360.0*(1.0 - (threshold/max)) - (360.0-hue)*(1.0-(threshold/max)) ;
        else
            hue = 360.0*(1.0 - incommands.getthreshold()) - (360.0-hue)*(1.0-incommands.getthreshold()) ;

        hue = hue*(1.0 - cutFrac); // cuts red off bottom of scale
        hsv_to_rgb(&rgb[0], &hue);
        r = rgb[0];
        g = rgb[1];
        b = rgb[2];

        glColor3f(r, g, b);
        glVertex3f(1.3, -length/2 + (length/sections)*k, -1.5);
        glVertex3f(1.3, -length/2 + (length/sections)*(k+1), -1.5);
    }
    glEnd();

    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glLineWidth(1);
    glBegin(GL_LINES);

    glColor3f(incommands.gettext_r(), incommands.gettext_g(), incommands.gettext_b());
    glVertex3f(1.34, -length/2, -1.5);
    glVertex3f(1.34, +length/2, -1.5);
    glVertex3f(1.34, +length/2, -1.5);
    glVertex3f(1.36, +length/2, -1.5);
    glVertex3f(1.34, +length/4, -1.5);
    glVertex3f(1.36, +length/4, -1.5);
    glVertex3f(1.34, 0.0, -1.5);
    glVertex3f(1.36, 0.0, -1.5);
    glVertex3f(1.34, -length/4, -1.5);
    glVertex3f(1.36, -length/4, -1.5);
    glVertex3f(1.34, -length/2, -1.5);
    glVertex3f(1.36, -length/2, -1.5);
    glEnd();

    if(threshold > 0.0)
    {
        sprintf(charMax, "%E", max);
        truncateExp(charMax);
        exponent = -log(max)/( (1-log(max)/log(threshold))*(1-0.75) - 1 );
        sprintf(threeQuarters, "%E", exp(exponent) );
        truncateExp(threeQuarters);
        exponent = -log(max)/( (1-log(max)/log(threshold))*(1-0.5) - 1 );
        sprintf(half, "%E", exp(exponent) );
        truncateExp(half);
        exponent = -log(max)/( (1-log(max)/log(threshold))*(1-0.25) - 1 );
        sprintf(quarter, "%E", exp(exponent) );
        truncateExp(quarter);
        sprintf(bottom, "%E", threshold);
        truncateExp(bottom);
    }
    else
    {
        exponent = log(max);

        sprintf(charMax, "%E", max);
        truncateExp(charMax);
        sprintf(threeQuarters, "%E", exp(exponent/0.75) );
        truncateExp(threeQuarters);
        sprintf(half, "%E", exp(exponent/0.5) );
        truncateExp(half);
        sprintf(quarter, "%E", exp(exponent/0.25) );
        truncateExp(quarter);
        sprintf(bottom, "%E", threshold);
        truncateExp(bottom);
    }

    screenText(2745, 2050, -3000, "|Psi|^2");
    screenText(2745, 1792, -3000, charMax);
    screenText(2745, 877, -3000, threeQuarters);
    screenText(2745, -47, -3000, half);
    screenText(2745, -975, -3000, quarter);
    if(threshold == 0.0)
        screenText(2745, -1900, -3000, "0.00");
    else
        screenText(2745, -1900, -3000, bottom);
}
// Source: http://www.lighthouse3d.com/opengl/glut/index.php?strokefont
void screenText(float x, float y, float z, char* text)
{

    int newlineCount = 0;
    glPushMatrix();

    glDisable(GL_DEPTH_TEST); // Keeps text on-top
    glDisable(GL_LIGHTING);   // Brings text out of shadow
    bool fogWasEnabled = glIsEnabled(GL_FOG);
    glDisable(GL_FOG);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);

    glLineWidth(2.0);

    glTranslatef(x, y, z);
    for (text; *text != '\0'; text++)
    {
        if(*text == 0x0A) //if is \n (newline), then tranlslate down a line
        {
            newlineCount++;
            glPopMatrix();
            glTranslatef(x,y-(newlineCount * 180),z);
            glPushMatrix();
        }
        else
        {
            glutStrokeCharacter(GLUT_STROKE_ROMAN, *text);
        }
    }

    glLineWidth(1.0);

    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_BLEND);

    if (fogWasEnabled) {
        glEnable(GL_FOG);
    }
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
}
