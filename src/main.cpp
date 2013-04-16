/****************************************************************************
Program:        VisuQS.LX / VQS.exe  Visualising Quantum States in Disordered Systems
File:           main.cpp
Description:    Displays probability distribution of a 1, 2, or 3 dimensional
                data set on screen, with user-defined visualisation settings
Author:         Richard Marriott / Luke Gyngell
Environment:    Borland C++BuilderX-Personal V.1.0.0.1786 / g++ (GCC) 4.0.1
Notes:          Data read from \"VQSdatafile.txt\" in format \"(x) (y) (z) psi\"
                Editable input parameters read from \"VQSinput.txt\"
                Both files must lie in directory from which program is run
Revision:       $Revision: 1.5 $
****************************************************************************/

#include "main.h"
#include "readData.h"
#include "readInput.h"
#include "HSVtoRGB.h"
#include "output.h"
#include "jitter.h"
#include "debug.h"
#include "util.h"
#include <iostream>
#include <GL/glut.h>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <time.h>

// #include <cstring>

#ifdef _WINDOWS
#include <conio.h>
#endif
#ifdef _LINUX
#include <curses.h>
#endif
using namespace std;



// Global objects, structures & variables
Input incommands;
bool flip = incommands.getflip();
DrawStyles::Enum visMeth;


psiArray data;

int visStatus;
int moving, startx, starty, n;
float angle, angle2, radius;
const double pi = 3.141592654;
#define PI_ 3.14159265358979323846
int counter, flycounter;

float rgb[3];
int end;
bool flownflag;
float rubberneck=0.0;
int modAngle, modAngle2;
bool flipped;
float threshold;
bool vidCreated = false;
bool altCodec = false;
int frames;
void outputEPS(int size, int doSort, char *filename);

/***** Arbitrary parameters *****/
float cutFrac = 0.13; // Carefully chosen to cut red from the BOTTOM of the scale
float overlap = 0.07; //  Size of objects is increased by this factor to incite overlapping
float cr = 1.0; // Central Range for Fly-through
float nearClipPlane = 0.001;
float farClipPlane = 3100.0; // Must be beyond the depth of the text
// Lighting variables
float ambient_light[] = {0.45, 0.55, 0.60};
float source_light[] = {0.9, 0.8, 0.8};
float light_pos[] = {400.0, 500.0, 300.0, 1.0};
float mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
float mat_shininess[] = { 25.0 };
float fogColor[]= {incommands.getbg_r(), incommands.getbg_g(), incommands.getbg_b(), 1.0};

char origDir[256];


void init(void)
{
    DEBUG("init()");

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, source_light);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glEnable(GL_LIGHT0);
    if(visMeth == DrawStyles::Bubbles)
    {
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    }
    if(incommands.getperiodic())
    {
        glFogi(GL_FOG_MODE, GL_EXP2);               // Fog Mode: GL_EXP, GL_EXP2 or GL_LINEAR
        glFogfv(GL_FOG_COLOR, fogColor);		// Set Fog Color
        glFogf(GL_FOG_DENSITY, 1.0f);		// How Dense Will The Fog Be
        glHint(GL_FOG_HINT, GL_NICEST);	        // Fog Hint Value: GL_NICEST, GL_FASTEST or GL_DONT_CARE
        glFogf(GL_FOG_START, 0.0f);		        // Fog Start Depth
        glFogf(GL_FOG_END, 3.0f);			// Fog End Depth
        glEnable(GL_FOG);				// Enables GL_FOG
    }


    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glEnable (GL_CULL_FACE);

    glClearColor (incommands.getbg_r(), incommands.getbg_g(), incommands.getbg_b(), 0.0);
    glClearAccum(0.0, 0.0, 0.0, 0.0);

    if(incommands.getthreshold() == 1.0)
    {
        threshold = (float)1.0/(pow((double)data.extent, data.dimension));
    }
    else
    {
        threshold = incommands.getthreshold()*data.max;
    }



    frames = (int)( incommands.getframerate()*incommands.getduration() );
    if(!incommands.getfly() && incommands.getfull())
    {
        end = frames;
    }
    else
    {
        end = frames + 1;
    }

    flycounter=0;
    counter=0;

    radius = incommands.getstart_r();
    flownflag = false;
    angle = incommands.getstart_phi();
    angle2 = incommands.getstart_theta();
    rubberneck = 0.0;
    flipped = false;

#ifdef _WINDOWS
    system("del /q /s seq > NUL 2> NUL");
    system("rd /q /s seq 2> NUL");
#endif
#ifdef _LINUX
    system("rm -r seq > /dev/null 2> /dev/null");
#endif

}

#pragma argsused

void quit(int exitStatus)
{
    DEBUG("quit()");

    if (glutGetWindow() != 0)
    {
        glutDestroyWindow(glutGetWindow());
    }

    if (exitStatus == 0)
    {
        cout << "\n\nquit(): Program finished successfully";
    }
    else
    {
        cout << "\n\nquit(): Program stopped due to errors";
    }

    cout<<"\n";
    std::exit(exitStatus);
}


// Source: http://www.lighthouse3d.com/opengl/glut/index.php?strokefont
void screenText(float x, float y, float z, char* text)
{

    int newlineCount = 0;
    glPushMatrix();

    glDisable(GL_DEPTH_TEST); // Keeps text on-top
    glDisable(GL_LIGHTING);   // Brings text out of shadow
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

    if(incommands.getperiodic())
        glEnable(GL_FOG);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
}


void accFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble d_near, GLdouble d_far, GLdouble pixdx, GLdouble pixdy, GLdouble eyedx, GLdouble eyedy, GLdouble focus)
{
    GLdouble xwsize, ywsize;
    GLdouble dx, dy;
    GLint viewport[4];
    glGetIntegerv (GL_VIEWPORT, viewport);

    DEBUG("accFrustum()");


    xwsize = right - left;
    ywsize = top - bottom;
    dx = -(pixdx*xwsize/(GLdouble) viewport[2] + eyedx*d_near/focus);
    dy = -(pixdy*ywsize/(GLdouble) viewport[3] + eyedy*d_near/focus);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum (left + dx, right + dx, bottom + dy, top + dy, d_near, d_far);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef (-eyedx, -eyedy, 0.0);
}

void accPerspective(GLdouble fovy, GLdouble aspect, GLdouble d_near, GLdouble d_far, GLdouble pixdx, GLdouble pixdy, GLdouble eyedx, GLdouble eyedy, GLdouble focus)
{
    GLdouble fov2,left,right,bottom,top;

    DEBUG("accPerspective()");

    fov2 = ((fovy*PI_) / 180.0) / 2.0;
    top = d_near / (cos(fov2) / sin(fov2));
    bottom = -top;
    right = top * aspect;
    left = -right;
    accFrustum (left, right, bottom, top, d_near, d_far, pixdx, pixdy, eyedx, eyedy, focus);
}

class Cell
{
private:
    float max;
    float min;

public:
    Cell() // constructor
    {


    }



    void draw(float rgb[3], float prob)
    {
        max = data.max;
        min = data.min;
        float size, reducedMax, opacity;

        if (visMeth == DrawStyles::Cubes)
        {
            size = (float)pow((double)prob, 1.0/3); // size = side-length of cube of volume prob
            reducedMax = (float)pow((double)max, 1.0/3);
            size = size/(data.extent*reducedMax); // Scales cubes to within cells of side-length reducedMax
            size = size * (1 + overlap*data.extent); // Increases size by arbitrary amount to incite overlapping

            Cell::maybeColorize(prob, max, rgb);

            glColor3f(rgb[0], rgb[1], rgb[2]);
            glutSolidCube(size);

            // Black outline on sites that are really large relatively
            if ( (float)pow((double)prob, 1.0/2) > ((float)pow((double)data.max, 1.0/2)*0.36788) ) // 1/e
            {
                glColor3f(0.0, 0.0, 0.0);
                glutWireCube(size);
            }
        }
        else if (visMeth == DrawStyles::Bubbles) // Draw Bubble
        {
            size = (prob/max)/((4.0/3)*pi);
            size = (float)pow((double)size, 1.0/3);
            size = size/(data.extent);

            size = size * (1 + overlap*data.extent); // Increases size by arbitrary amount to incite overlapping

            glPushMatrix();

            Cell::maybeColorize(prob, max, rgb);

            glRotatef( 90.0, 1.0, 0.0, 0.0 );
            glColor3f(rgb[0], rgb[1], rgb[2]);
            GLUquadricObj*  q = gluNewQuadric ( );
            gluQuadricDrawStyle ( q, GLU_FILL );
            gluQuadricNormals   ( q, GLU_SMOOTH );
            gluSphere ( q, size, 20, 20 );
            gluDeleteQuadric ( q );

            glPopMatrix();
        }
        else if (visMeth == DrawStyles::Fog)
        {
            size = 1.0/data.extent;

            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            opacity = log(pow((double)prob,1.0/2));
            reducedMax = log(pow((double)max,1.0/2));


            Cell::maybeColorize(prob, max, rgb);

            glColor4f(rgb[0], rgb[1], rgb[2], reducedMax/opacity);
            glutSolidCube(size);

            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            cout<<"\ncell(): error --- visualisation method parameter not recognised";
            quit(1);
        }
    }
private:
    void maybeColorize(float prob, float max, float rgb[3])
    {
        if(incommands.getcolourMethod())
        {
            float hue;
            hue = 360.0 * (1.0-( log(max)/log(prob) ));
            hue = hue*(1.0 - cutFrac);

            hsv_to_rgb(rgb, &hue);
        }
    }
};

Cell unit;

void draw3d()
{
    int x, y, z;
    float hue;
    for (int i=0; i<data.extent; i++)
    {
        for (int j=0; j<data.extent; j++)
        {
            for (int k=0; k<data.extent; k++)
            {
                glPushMatrix();

                if(visMeth == DrawStyles::Fog)
                {
                    x = data.coords3d[i][j][k][0];
                    y = data.coords3d[i][j][k][1];
                    z = data.coords3d[i][j][k][2];
                    glTranslatef( (float)x/data.extent, (float)y/data.extent, (float)z/data.extent );
                }
                else // NOT Fog
                {
                    glTranslatef( (float)i/data.extent, (float)j/data.extent, (float)k/data.extent );
                }

                if(visMeth == DrawStyles::Fog) // Decides colour if 'depth-graduated'
                    hue = (data.extent-(z+1))*(360.0/data.extent); // Fog is sorted and uses
                else                                             // different depth coordinate, 'z'
                    hue = (data.extent-(k+1))*(360.0/data.extent);

                float rgb[3];
                hue = hue*(1.0 - cutFrac);
                hsv_to_rgb(&rgb[0], &hue);

                //Test the current site, if prob greater than threshold then draw it.
                if (data.matrix3d[i][j][k] > threshold)
                {
                    unit.draw(rgb, data.matrix3d[i][j][k]);
                }

                glPopMatrix();
            }
        }
    }

}

/*
void draw2d()
{
    for (int i=0; i<data.extent; i++)
    {
            for (int j=0; j<data.extent; j++) {
                    glPushMatrix();

                    if(incommands.getvisMethod() == 'F')
                    {
                        x = data.coords2d[i][j][0];
                        y = data.coords2d[i][j][1];
                        glTranslatef( (float)x/data.extent, (float)y/data.extent, 0.0);
                    }
                    else // NOT Fog
                        glTranslatef( (float)i/data.extent, (float)j/data.extent, 0.0);

                    if (data.matrix2d[i][j] > threshold)
                        unit.draw(r, g, b, data.matrix2d[i][j]);

                    glPopMatrix();
    }
    }

}

void draw1d()
{
    for (int i=0; i<data.extent; i++) {
    glPushMatrix();

    if(incommands.getvisMethod() == 'F')
    {
        x = data.coords1d[i];
        glTranslatef( (float)x/data.extent, 0.0, 0.0);
    }
    else // NOT Fog
        glTranslatef( (float)i/data.extent, 0.0, 0.0);

    if (data.matrix1d[i] > threshold)
        unit.draw(r, g, b, data.matrix1d[i]);
    glPopMatrix();
  }

}
*/

void drawCrystal()
{
    float r,g,b;
    float translationFactor = (1.0/(float)data.extent) + ((float)data.extent-3.0)/(2.0*(float)data.extent);

    DEBUG("drawCrystal()");


    glPushMatrix();

    if ( data.dimension == 3 )
        glTranslatef( -translationFactor, -translationFactor, -translationFactor); // translates entire plot to keep
    if ( data.dimension == 2 )                                                   // it central and within outline cube
        glTranslatef( -translationFactor, -translationFactor, 0);
    if ( data.dimension == 1 )
        glTranslatef( -translationFactor, 0, 0);

    r = 0.0;
    g = 0.0;
    b = 1.0;

    switch (data.dimension)
    {
    case 3:
        draw3d();
        break;
        /*
        case 2:
        draw2d();
        break;
        case 1:
        draw1d();
        break;
        */
    default:
        cout<<"\ncell(): error --- dimension should be 1-3";
        quit(1);
    }

    glPopMatrix();

    glColor3f(incommands.gettext_r(), incommands.gettext_g(), incommands.gettext_b());

    if(data.dimension == 3 && incommands.getcolourMethod() && !incommands.getperiodic() )
        glutWireCube (1.0);
    else if(data.dimension == 3 && !incommands.getcolourMethod() && !incommands.getperiodic() )
    {
        glBegin(GL_LINE_STRIP);       // Plots wire-cube with spaces for the z-scales
        glVertex3f(-0.5, 0.5, 0.5);
        glVertex3f(-0.5, -0.5, 0.5);
        glVertex3f(0.5, -0.5, 0.5);
        glVertex3f(0.5, 0.5, 0.5);
        glVertex3f(-0.5, 0.5, 0.5);
        glVertex3f(-0.5, 0.5, -0.5);
        glVertex3f(-0.5, -0.5, -0.5);
        glVertex3f(0.5, -0.5, -0.5);
        glVertex3f(0.5, 0.5, -0.5);
        glVertex3f(0.5, 0.5, 0.5);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(-0.5, 0.5, -0.5);
        glVertex3f(0.5, 0.5, -0.5);
        glEnd();
    }
}



void drawZscale() // Needs to be plotted after 'crystal' to avoid blending in with the data
{
    float r,g,b;
    float translationFactor = (1.0/(float)data.extent) + ((float)data.extent-3.0)/(2.0*(float)data.extent);
    float hue;
    int x, y, z;

    DEBUG("drawZscale()");

    glPushMatrix();

    glTranslatef( -translationFactor, -translationFactor, -translationFactor); // translates entire plot to keep

    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glLineWidth(6);

    for (int i=0; i<data.extent; i++)
    {
        for (int j=0; j<data.extent; j++)
        {
            for (int k=0; k<data.extent; k++)
            {
                hue = (data.extent-(k+1))*(360.0/data.extent);
                hue = hue*(1.0 - cutFrac);

                hsv_to_rgb(&rgb[0], &hue);
                r = rgb[0];
                g = rgb[1];
                b = rgb[2];

                glBegin(GL_LINES);
                glColor3f(r, g, b);
                glVertex3f(-(0.5/(float)data.extent), -(0.5/(float)data.extent), (float)k/data.extent -(0.5/(float)data.extent));
                glVertex3f(-(0.5/(float)data.extent), -(0.5/(float)data.extent), ((float)k/data.extent)+1.0/data.extent -(0.5/(float)data.extent));
                glVertex3f(1.0 -(0.5/(float)data.extent), -(0.5/(float)data.extent), (float)k/data.extent -(0.5/(float)data.extent));
                glVertex3f(1.0 -(0.5/(float)data.extent), -(0.5/(float)data.extent), ((float)k/data.extent)+1.0/data.extent -(0.5/(float)data.extent));
                glEnd();
            }
        }

    }

    glLineWidth(1);
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

int adjustRounding(float coord)
{
    if (coord < 0)
    {
        return (int) coord - 0.5;
    }
    else if (coord > 0 )
    {
        return (int) coord + 0.5;
    }
}

void adjustRange(float coord, int base, int *from, int *until)
{

    *from = base - 1;
    *until = base + 2;
    if(radius > 0)
    {
        if(coord > 0)
        {
            *from = base - 2;
            *until = base + 2;
        }
        else if(coord < 0)
        {
            *from = base - 1;
            *until = base + 3;
        }
        else
        {
            *from = base - 2;
            *until = base + 3;
        }
    }
    else
    {
        if(coord > 0)
        {
            *from = base - 1;
            *until = base + 3;
        }
        else if(coord < 0)
        {
            *from = base - 2;
            *until = base + 2;
        }
        else
        {
            *from = base - 2;
            *until = base + 3;
        }
    }

}

void render()
{

    DEBUG("render()");

    glPushMatrix();

    glRotatef ( -rubberneck, 0.0, 1.0, 0.0 );

    glTranslatef(0.0, 0.0, -radius);

    glRotatef ( -angle2+90.0, 1.0, 0.0, 0.0 ); // Do not change the order (first)
    glRotatef ( -angle, 0.0, 1.0, 0.0 ); // (second)

    glTranslatef(incommands.getx_tran(), incommands.gety_tran(), incommands.getz_tran());

    if(incommands.getperiodic()) // Plots crystal periodically
    {
        float xCoord, yCoord, zCoord;
        int intX, intY, intZ;
        int fromX, fromY, fromZ;
        int untilX, untilY, untilZ;
        xCoord = radius*sin((pi/180.0)*angle2)*sin((pi/180.0)*angle) - incommands.getx_tran();
        yCoord = radius*cos((pi/180.0)*angle2) - incommands.gety_tran();
        zCoord = radius*sin((pi/180.0)*angle2)*cos((pi/180.0)*angle) - incommands.getz_tran();

        intX = adjustRounding(xCoord);
        intY = adjustRounding(yCoord);
        intZ = adjustRounding(zCoord);


        adjustRange(xCoord, intX, &fromX, &untilX);
        adjustRange(yCoord, intY, &fromY, &untilY);
        adjustRange(zCoord, intZ, &fromZ, &untilZ);
        for(int x = fromX; x < untilX; x++)
        {
            for(int y = fromY; y < untilY; y++)
            {
                for(int z = fromZ; z < untilZ; z++)
                {
                    glPushMatrix();
                    glTranslatef((float)x, (float)y, (float)z);
                    drawCrystal();
                    glPopMatrix();
                }
            }
        }
    }
    else
    {
        drawCrystal();

        if( !incommands.getcolourMethod() && (data.dimension == 3) )
            drawZscale();
    }

    glPopMatrix();
}




void drawColourScale()
{
    int k;
    int sections = 300;
    float hue, r, g, b;
    float length = 1.85;
    char charMax[15], threeQuarters[15], half[15], quarter[15], bottom[15];
    float exponent;

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
            hue = 360.0*(1.0 - (threshold/data.max)) - (360.0-hue)*(1.0-(threshold/data.max)) ;
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
        sprintf(charMax, "%E", data.max);
        truncateExp(charMax);
        exponent = -log(data.max)/( (1-log(data.max)/log(threshold))*(1-0.75) - 1 );
        sprintf(threeQuarters, "%E", exp(exponent) );
        truncateExp(threeQuarters);
        exponent = -log(data.max)/( (1-log(data.max)/log(threshold))*(1-0.5) - 1 );
        sprintf(half, "%E", exp(exponent) );
        truncateExp(half);
        exponent = -log(data.max)/( (1-log(data.max)/log(threshold))*(1-0.25) - 1 );
        sprintf(quarter, "%E", exp(exponent) );
        truncateExp(quarter);
        sprintf(bottom, "%E", threshold);
        truncateExp(bottom);
    }
    else
    {
        exponent = log(data.max);

        sprintf(charMax, "%E", data.max);
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

void makeFileName(char* address)
{
    char rank[12];
    char dimension[12];
    char start_r[12];
    char start_phi[12];
    char start_theta[12];
    char end_r[12];
    char end_phi[12];
    char end_theta[12];

    DEBUG("makeFileName()");

#ifdef _WINDOWS
    itoa(data.extent, rank, 10);
    itoa(data.dimension, dimension, 10);
#endif
#ifdef _LINUX
    snprintf(rank, 9, "%d", data.extent);
    snprintf(dimension, 9, "%d", data.dimension);
#endif
    if(incommands.getoutput() != 0) //if not 'interactive mode...'
    {
        sprintf(start_r, "%f", incommands.getstart_r());
        sprintf(start_phi, "%f", incommands.getstart_phi());
        sprintf(start_theta, "%f", incommands.getstart_theta());
    }
    else //if interactive
    {
        if(radius < 0)
            sprintf(start_r, "%f", -radius);
        else
            sprintf(start_r, "%f", radius);
        sprintf(start_phi, "%d", (int)modAngle);
        sprintf(start_theta, "%d", (int)modAngle2);
    }
    sprintf(end_r, "%f", incommands.getend_r());
    sprintf(end_phi, "%f", incommands.getend_phi());
    sprintf(end_theta, "%f", incommands.getend_theta());

    removeZeros(start_r);
    removeZeros(start_phi);
    removeZeros(start_theta);
    removeZeros(end_r);
    removeZeros(end_phi);
    removeZeros(end_theta);

//create folders first
    if (incommands.getoutput() == 3) //create movie
    {
        strcpy(address, "Animations/");
#ifdef _WINDOWS
        system("mkdir Animations 2> NUL");
#endif
#ifdef _LINUX
        system("mkdir Animations 2> /dev/null");
#endif
    }
    else     //create stills
    {
#ifdef _WINDOWS
        strcpy(address, "Stills\\");
        system("mkdir Stills 2> NUL");
#endif
#ifdef _LINUX
        strcpy(address, "Stills/");
        system("mkdir Stills 2> /dev/null");
#endif
    }

    strcat(address, rank); //append "system size" to path
    if (incommands.getoutput() != 2) //if not STILLS
    {
        strcat(address, "^");
    }
    else
    {
        strcat(address, "-");
    }

    strcat(address, dimension);
    strcat(address, "-");

    if(visMeth == DrawStyles::Fog)
        strcat(address, "F");
    else if(visMeth == DrawStyles::Cubes)
        strcat(address, "C");
    else
        strcat(address, "B");

    if(incommands.getcolourMethod())
        strcat(address, "F-");
    else
        strcat(address, "G-");

    strcat(address, start_r);
    strcat(address, "-");
    strcat(address, start_phi);
    strcat(address, "-");
    strcat(address, start_theta);

    if((incommands.getoutput() > 1) && incommands.getfly())
        strcat(address, "-fly-");
    else if((incommands.getoutput() > 1) && !incommands.getfly())
        strcat(address, "-rot-");

    if(incommands.getoutput() > 1)
    {
        strcat(address, end_r);
        strcat(address, "-");
        strcat(address, end_phi);
        strcat(address, "-");
        strcat(address, end_theta);
    }

    if((incommands.getoutput() > 1) && !incommands.getfly() && incommands.getfull())
        strcat(address, "-full");



    if(incommands.getoutput() == 3)
    {
        strcat(address, ".avi");
    }
}


void outputTGA(int x, int y, int w, int h, const char *fname)
{
    unsigned char *image = NULL;

    DEBUG("outputTGA()");

// Reserve some memory
    image = (unsigned char*)malloc(sizeof(unsigned char)*w*h*3);
    if (!image)
    {
        fprintf(stderr, "Unable to allocate image buffer\n");
//ret = -1;
    }
    else
    {
        glReadPixels(x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);

        for (int i = 0; i < w*h*3; i = i + 3)
        {
            GLfloat temp = image[i];
            image[i] = image[i+2];
            image[i+2] = (unsigned char)temp;
        }
        int i;
        FILE *fp;

// See http://www.dcs.ed.ac.uk/home/mxr/gfx/2d/TGA.txt  for spec
        struct TGAHeader
        {
            unsigned char idfield_len;
            unsigned char cmap_type;
            unsigned char image_type;
            unsigned char cmap_spec[5];
            unsigned char x_orig[2];
            unsigned char y_orig[2];
            unsigned char width[2];
            unsigned char height[2];
            unsigned char pixel_size;
            unsigned char image_desc;
        } header; //__attribute__((packed)) header;
// Open file
        fp = fopen(fname, "wb");

        if (!fp)
        {
            fprintf(stderr, "Unable to open %s for writing\n", fname);
        }
        else
        {
// Construct header
            header.idfield_len = 0;
            header.cmap_type = 0;
            header.image_type = 2;
            for (i = 0; i < 5; i++)
            {
                header.cmap_spec[i] = 0;
            }
            for (i = 0; i < 2; i++)
            {
                header.x_orig[i] = 0;
                header.y_orig[i] = 0;
            }
// Lo bits
            header.width[0] = w & 0xFF;
// Hi bits
            header.width[1] = (w >> 8) & 0xFF;
            header.height[0] = h & 0xFF;
            header.height[1] = (h >> 8) & 0xFF;
            header.pixel_size = 24;
            header.image_desc = 0;
// Output header
            fwrite(&header, sizeof(header), 1, fp);
// Output image
            fwrite(image, sizeof(unsigned char), w*h*3, fp);
        }
        if (fp)
        {
            fclose(fp);
        }
        if (image)
        {
            free(image);
        }

//Build a command string to convert written TGA to PNG, then fire off in background
        char convertCmd[512];

        sprintf(convertCmd,"convert %s %s/`basename %s`.png && rm -f %s &",fname,origDir,fname,fname);
//sprintf(convertCmd,"echo convert %s %s/`basename %s`.png",fname,origDir,fname);
#ifdef _DEBUG
        printf("%s\n",convertCmd);
#endif

        system(convertCmd);

    }
}

void capture(int type)
{
    char fname[256];
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    DEBUG("capture()");

    switch(type)
    {
    case 0:
        makeFileName(fname);
        strcat(fname, ".tga");
#ifdef _DEBUG
        cout << "\nCreating image " << fname << flush;
#endif
        outputTGA(0, 0, viewport[2], viewport[3], fname);
#ifdef _DEBUG
        cout << "\nImage created successfully" << flush;
#endif
        break;
    case 1:
        makeFileName(fname);
        strcat(fname, ".eps");
#ifdef _DEBUG
        cout << "\nCreating image " << fname << flush;
#endif
        outputEPS(200000000, 1, fname);
#ifdef _DEBUG
        cout << "\nImage created successfully" << flush;
#endif
        break;
    case 2: //if making STILLS
        makeFileName(fname);
        if (counter == 1) //if this is the first frame
        {
            char dname[256];
//make a directory like "100-3-FF-1.5-0.0-90.0-rot-1.5-359.0-90.0-full"
            strcpy(dname, "mkdir ");
            strcat(dname, fname);
            cout << "\n" << dname;
#ifdef _WINDOWS
            strcat(dname, " 2> NUL");
            system(dname);
#endif
#ifdef _LINUX
            strcat(dname, " 2> /dev/null");
            system(dname);
#endif
        }
        if (counter-1 < end)
        {
            char screenNumber[9];
            char endFrameNum[9];
            char formatStr[9];
            char strPadWidth[5];

            int padWidth;

//Find out how long last frame is, so we can zero-pad accordingly.
//e.g. 334 gives 3, so 1 => 001.tga etc

            sprintf(endFrameNum,"%d",end);
            padWidth = strlen(endFrameNum);
//itoa(padWidth,strPadWidth,10);
            sprintf(strPadWidth,"%d",padWidth);

//Make a format string like %05d to zero pad 5 places for example
            strcpy(formatStr,"%0");
            strcat(formatStr,strPadWidth);
            strcat(formatStr,"d");


            snprintf(screenNumber, 9, formatStr, counter);

            strcat(fname, "/");
            strcat(fname, screenNumber);
            strcat(fname, ".tga");
#ifdef _DEBUG
            cout << "\nCreating image " << fname << flush;
#endif
            outputTGA(0, 0, viewport[2], viewport[3], fname);
#ifdef _DEBUG
            cout << "\nImage created successfully" << flush;
#endif
        }
        else
        {
            quit(0);
        }
        break;
    case 3:
        if (counter-1 < end)
        {
            if (counter == 1)
            {
                system("mkdir seq");
            }
            char screenNumber[9];

#ifdef _WINDOWS
            itoa(counter, screenNumber, 10);
#endif
#ifdef _LINUX
            snprintf(screenNumber, 9, "%d", counter);
#endif
            strcpy(fname, "seq/");
            strcat(fname, screenNumber);
            strcat(fname, ".tga");

            outputTGA(0, 0, viewport[2], viewport[3], fname);
        }
        break;
    default:
        break;
    }
}

void simpleDraw()
{
    char line1[] = "r = ";
    char line2[] = "phi = ";
    char line3[] = "theta = ";
    char charR[12], charPhi[6], charTheta[6];
    char* p;

    DEBUG("simpleDraw()");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    render();

    if(incommands.gettext()==1) // Puts text on-screen
    {
        glPushMatrix();
        glColor3f(incommands.gettext_r(), incommands.gettext_g(), incommands.gettext_b());

        if(incommands.getfly())
        {
            if(radius < 0) // Beyond centre
            {
                modAngle = (int)( 180.0 + angle );
                modAngle2 = (int)( 180.0 - angle2 );
            }
            else // Before centre
            {
                modAngle = (int)angle;
                modAngle2 = (int)angle2;
            }
        }
        else // rotation
        {
            modAngle = (int)angle;
            if(modAngle < 0)
                modAngle = -modAngle;

            modAngle2 = (int)angle2;
        }

        modAngle = modAngle % 360;        // Keeps 0 <= phi < 360 (on-screen)

        modAngle2 = (int)modAngle2 % 360;    // Keeps -180 < theta <= 180 (on-screen)
        if(modAngle2 < -179.9999999)
            modAngle2 = 180 + (modAngle2 % 180);
        if(modAngle2 > 180)
            modAngle2 = -180 + (modAngle2 % 180);

        float text_x_offset = -1850;
        float text_y_offset = -200;
        float text_z_offset = -1500;

        screenText(text_x_offset, text_y_offset, text_z_offset, &line3[0]);
        sprintf(charTheta, "%d", (int)modAngle2); // cast to get rid of dodgy numbers
        screenText(text_x_offset + 570 , text_y_offset, text_z_offset, charTheta);

        screenText(text_x_offset, text_y_offset + 180 , text_z_offset, &line2[0]);
        sprintf(charPhi, "%d", (int)modAngle);
        screenText(text_x_offset + 570 , text_y_offset + 180 , text_z_offset, charPhi);

        screenText(text_x_offset, text_y_offset + 360 , text_z_offset, &line1[0]);
        if(radius < 0)
            sprintf(charR, "%f", -radius);
        else
            sprintf(charR, "%f", radius);

        for(p = charR; *p != '\0'; p++)
        {
            if(*p == '.')
            {
                p = p + 3;
                *p = '\0';
                p--;
            }
        }
        screenText(text_x_offset + 570, text_y_offset+360, text_z_offset, charR);
        screenText(text_x_offset,text_y_offset+720,text_z_offset,"My test\nNewline");

        glPopMatrix(); // Do not remove this!!! lol
    }
    else if(incommands.gettext()==2) //we want to write contents of a text file
    {
        glPushMatrix();
        glColor3f(incommands.gettext_r(), incommands.gettext_g(), incommands.gettext_b());

        int fLen;
        char * sBuf;


        float text_x_offset = -1850;
        float text_y_offset = -90;
        float text_z_offset = -1500;

        //see if the program directory had a file called overlay_info.txt, and render it to screen
        char overlayFile[256];
        sprintf(overlayFile,"%s/overlay_info.txt",origDir);

        if( FileExists(overlayFile) )
        {
            int lineCount=0;
            char tmpLine[256];
            ifstream fOverlay;

            fOverlay.open(overlayFile,ios::binary);

            int i;
            for(i=0; i<=1; i++) //do 2 iterations, first time just count lines, 2nd pass render, offset accordingly
            {

                while( fOverlay.getline(tmpLine,256) )
                {

                    if(i!=0) //2nd pass
                    {

                        screenText(text_x_offset,text_y_offset - (180 * lineCount),text_z_offset,tmpLine);

                    }
                    lineCount++;
                }

                if(i==0)
                {
                    //This has the effect of centering the text around the middle of the screen
                    text_y_offset += 90 * lineCount;
                    //Clear the lineCount again so can be used for newline positioning ^^
                    lineCount = 0;


                    //File was read to get lineCount, now must reset pointers:
                    fOverlay.clear();
                    fOverlay.seekg(0,ios::beg);



                }

            }



        }

        glPopMatrix(); // Do not remove this!!! lol
    }

    if(incommands.getcolourMethod() && incommands.gettext())
        drawColourScale();

    glutSwapBuffers();
}


void antiAliasDraw(void)
{
    GLint viewport[4];
    int jitter;
    glGetIntegerv (GL_VIEWPORT, viewport);
    glClear(GL_ACCUM_BUFFER_BIT);

    DEBUG("antiAliasDraw()");

    jitter_point offset;
    for (jitter = 0; jitter < incommands.getantiA(); jitter++)
    {
        offset = jitterPosition(jitter, incommands.getantiA());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        accPerspective (80.0, (GLdouble) viewport[2]/(GLdouble) viewport[3], nearClipPlane, farClipPlane, offset.x, offset.y, 0.0, 0.0, 1.0);
        simpleDraw();
        glAccum(GL_ACCUM, 1.0/incommands.getantiA());
    }

    glAccum (GL_RETURN, 1.0);
    glFlush();

}

void rotate()
{
    float sine, sine_elevate;
    float theta_dif = incommands.getend_theta() - incommands.getstart_theta();

    float phi_dif = incommands.getend_phi() - incommands.getstart_phi();
    if(incommands.getend_phi() == incommands.getstart_phi())
        phi_dif = FLT_EPSILON;

    float r_dif = incommands.getend_r() - incommands.getstart_r();
    float floatNumStepsThere = (phi_dif/360)*frames;
    float floatNumStepsBack = (1.0-(phi_dif/360))*frames;

//--not sure of the validity of this, but worth a try--//
    float floatNumStepsThereOffset = (phi_dif/360);
    float floatNumStepsBackOffset = (1.0-(phi_dif/360));

    DEBUG("rotate()");

    if((counter) < end)
    {

        if(incommands.getfull())
        {
            if(counter <= (int)floatNumStepsThere)
            {
                sine = 0.5*sin((pi/180.0)*( counter*(360.0/frames)*(180.0/phi_dif) - 90.0 )) + 0.5;
                sine_elevate = 0.5*sin((pi/180.0)*( counter*(360.0/frames)*(360.0/phi_dif) -90.0)) + 0.5;
            }
            else if (counter > (int)floatNumStepsThere)
            {
                sine = 0.5*sin((pi/180)*( (counter-(int)floatNumStepsThere)*(360.0/frames)*(180.0/(360.0-phi_dif)) + 90.0)) + 0.5;
                sine_elevate = 0.5*sin((pi/180.0)*( (counter-(int)floatNumStepsThere)*(360.0/frames)*(360.0/(360.0-phi_dif)) - 90.0)) + 0.5;
            }

            angle = incommands.getstart_phi() + counter*(360.0/frames);
            angle2 = incommands.getstart_theta() + (sine * theta_dif);
            radius = incommands.getstart_r() + (sine * r_dif);

            if(counter <= (int)floatNumStepsThere)
                angle2 = angle2 - sine_elevate*incommands.getelev_there();
            else if (counter > (int)floatNumStepsThere)
                angle2 = angle2 - sine_elevate*incommands.getelev_back();
        }

        else
        {
            sine_elevate = sin((pi/180.0)*( counter*(phi_dif/frames)*(180.0/phi_dif) ));

            angle = incommands.getstart_phi() + counter*(phi_dif/frames);
            angle2 = incommands.getstart_theta() + counter*(theta_dif/frames) - sine_elevate*incommands.getelev_there();
            radius = incommands.getstart_r() + counter*(r_dif/frames);
        }

    }

    counter++;
}

void fly()
{
    float sine, rstep;
    int stepswithin;
    float phi_dif = incommands.getend_phi() - incommands.getstart_phi();
    float theta_change = incommands.getend_theta() + incommands.getstart_theta() -180.0;

    DEBUG("fly()");

    if(!flipped)
    {
        modAngle = (int)incommands.getstart_phi();
        modAngle2 = (int)incommands.getstart_theta();
    }
    else
    {
        modAngle = (int)incommands.getend_phi();
        modAngle2 = (int)incommands.getend_theta();
    }

    if(counter < end)
    {
        rstep = (incommands.getstart_r() + incommands.getend_r())/frames;
        radius = incommands.getstart_r() - counter*rstep;

        if (radius > -cr/2 && radius < cr/2)
        {
            flipped = true;

            stepswithin = (int)( frames * cr/(incommands.getstart_r() + incommands.getend_r()) ); // number of frames central range
            sine = 0.5*sin((pi/180.0)*( flycounter*180.0/stepswithin - 90.0 )) + 0.5;

            if(flip)
            {
                rubberneck = sine*180.0;
                if(phi_dif < 0)
                    rubberneck = -rubberneck;
            }

            if(phi_dif < 0)
                angle = incommands.getstart_phi() + sine*(180.0+phi_dif);
            else
                angle = incommands.getstart_phi() - sine*(180.0-phi_dif);

            angle2 = incommands.getstart_theta() - sine*theta_change;

            flycounter++;
        }
    }
    counter++;
}


void Draw()
{

    if (incommands.getoutput() == 0 || incommands.getantiA() == 1 || incommands.getformat() == 1)
    {
        simpleDraw();
    }
    else
    {
        antiAliasDraw();
    }

    if (incommands.getoutput() == 1 && counter == 1)
    {
        capture(incommands.getformat());
        quit(0);
    }
    if (incommands.getoutput() > 1)
    {
        capture(incommands.getoutput());
    }



}



void idle()
{
    DEBUG("idle()");


    if(incommands.getfly())
        fly();
    else
        rotate();

    if((incommands.getoutput() == 3) && (counter > end) && !vidCreated)
    {
#ifdef _WINDOWS
        createVCF();
        system("vdub.exe /s VQSconfig.vcf > NUL");
        system("rename seq aviSequence");
        if(!altCodec)
            system("del /q VQSconfig.vcf > NUL");
        vidCreated = true;
        cout << "\nAnimation successfully created";
#endif
#ifdef _LINUX
        system("mv seq aviSequence");
        cout<<"\navi output is not available under Linux.";
        cout<<"\nThe image sequence you have produced can be found in the 'aviSequence' folder.";
        cout<<"\nTo create a video file from this sequence we recommend using 'avidemux'.";
#endif
        quit(0);
    }

    glutPostRedisplay();
//This condition tests if the window is not visible on the screen, and if so, calls Draw()
//because GLUT would otherwise not render in the next cycle. Necessary for rendering with
//screen saver active, windows on top, etc.
    if( visStatus==GLUT_NOT_VISIBLE)
    {
        Draw();
    }

}

void Reshape(int w, int h)
{
    if(incommands.getoutput() != 0) //If we are to do renderings, ensure the window size is what the user wants.
    {

        if(w != incommands.getimageW() || h != incommands.getimageH())
        {
            cout<<"\nReshape: !! Window wasn't the size we want, going to resize...\n";
            glutReshapeWindow(incommands.getimageW(),incommands.getimageH());
            w=incommands.getimageW();
            h=incommands.getimageH();
        }
//return;
    }

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

#ifdef _DEBUG
    cout << "\nReshape(): $Revision: 1.5 $\n";
    printf("Window reshaped, new dimensions: %d x % d\n",w,h);
#endif

    if (h==0)
        gluPerspective(80, (float)w, nearClipPlane, farClipPlane);
    else
        gluPerspective(80, (float)w/(float)h, nearClipPlane, farClipPlane);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Mouse(int button, int state, int x, int y)
{

    DEBUG("Mouse()");

    if(counter >= end)
    {
        if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
        {
            moving = 1;
            startx = x;
            starty = y;
        }

        if (button == GLUT_LEFT_BUTTON && state ==GLUT_UP)
        {
            moving = 0;
        }
    }
}

void Motion (int x, int y)
{
    if (moving)
    {
        angle = angle  + (startx - x);
        angle2 = angle2 + (starty - y);
        startx = x;
        starty = y;
        glutPostRedisplay();
    }
}

void outputEPS(int size, int doSort, char *filename)
{
    GLfloat *feedbackBuffer;
    GLint returned = -1;
    FILE *file;

    DEBUG("outputEPS()");

    bool sizeIncrease = false;
    size = 100000000;
    while ( returned == -1 )
    {
        if (sizeIncrease)
            size = (int)(size * 1.5);
        sizeIncrease = true;
        cout << "\nSize set to:    " << size << flush;

        feedbackBuffer = (GLfloat *) calloc(size, sizeof(GLfloat));
        cout << "\nfeedbackBuffer= " << feedbackBuffer << flush;

        glFeedbackBuffer(size, GL_3D_COLOR, feedbackBuffer);
        (void) glRenderMode(GL_FEEDBACK);
        simpleDraw();
        returned = glRenderMode(GL_RENDER);
        cout << "\nReturned Value: " << returned << flush;
    }

    if (filename)
    {
        file = fopen(filename, "w");
        if (file)
        {
            spewWireFrameEPS(file, doSort, returned, feedbackBuffer, "rendereps");
        }
        else
        {
            printf("Could not open %s\n", filename);
        }
    }
    else
    {
        /* Helps debugging to be able to see the decode feedback
        buffer as text. */
//printBuffer(returned, feedbackBuffer);
    }
    free(feedbackBuffer);
}

void
Keyboard(unsigned char key, int x, int y)
{

    DEBUG("Keyboard()");

    if (incommands.getoutput() == 0)
    {
        switch(key)
        {
        case 27: // escape key
            std::exit(0);
            break;
        case 'q': // escape key
            std::exit(0);
            break;
        case 'f':
            glutFullScreen();
            break;
        case 'p':
            antiAliasDraw();
            capture(1);
            break;
        case 'r':
            init();
            break;
        case 'w':
            glutReshapeWindow(250, 250);
            break;
        case 't':
            antiAliasDraw();
            capture(0);
            break;
        case '+':
            radius = radius-0.1;
            break;
        case '-':
            radius = radius+0.1;
            break;
        default:
            break;
        }
    }
    else
    {
        switch(key)
        {
        case 27: // escape key
#ifdef _WINDOWS
            system("del /q /s seq > NUL 2> NUL");
            system("rd /q /s seq 2> NUL");
#endif
#ifdef _LINUX
            system("rm -r seq > /dev/null 2> /dev/null");
#endif
            quit(0);
            break;
        case 'q': // escape key
#ifdef _WINDOWS
            system("del /q /s seq > NUL 2> NUL");
            system("rd /q /s seq 2> NUL");
#endif
#ifdef _LINUX
            system("rm -r seq > /dev/null 2> /dev/null");
#endif
            quit(0);
            break;
        default:
            break;
        }
    }
}

void mouseButton(int value)
{

    DEBUG("mouseButton()");

    switch (value)
    {
    case 0: //TGA capture
        antiAliasDraw();
        capture(0);
        break;
    case 1: //EPS capture
        antiAliasDraw();
        capture(1);
        break;
    case 2: //exit
        quit(0);
        break;
    }
}

void consoleDump()
{

    DEBUG("consoleDump()");

    cout << "\n****************************************************************************\n";
    cout << "Program:        Visualising Quantum States in Disordered Systems\n";
    cout << "File:           $Id: main.cpp,v 1.5 2010/01/14 16:25:13 phuhdk Exp $\n";
    cout << "Description:    Displays probability distribution of a 1, 2, or 3 dimensional\n";
    cout << "                data set on screen, with user-defined visualisation settings\n";
    cout << "Authors:        Richard Marriott / Luke Gyngell\n";
#ifdef _WINDOWS
    cout << "Environment:    Borland C++BuilderX-Personal V.1.0.0.1786\n";
#endif
#ifdef _LINUX
    cout << "Environment:    g++ (GCC) 4.0.1 (4.0.1-5mdk for Mandriva Linux release 2006.0)\n";
#endif
    cout << "Notes:          Data read from \"VQSdatafile.txt\" in format \"(x) (y) (z) psi\"\n";
    cout << "                Editable input parameters read from \"VQSinput.txt\"\n";
    cout << "                Both files must lie in directory from which program is run\n";
    cout << "Build Date:     "<< revisionSTR << ", " << authorSTR << ", " << dateSTR << "\n";
    cout << "****************************************************************************\n";
    cout << "\n";

    ifstream inputFile("VQSinput.txt");
    cout << "Current settings of VQSinput.txt\n";
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    char *tempArray = new char[256];
    while ( !inputFile.eof() )
    {
        inputFile.getline(tempArray, 256);
        cout << "\n" << tempArray;
    }
    delete tempArray;
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" << flush;
}

void visFunc(int stat)
{
    visStatus = stat;

}

int main(int argcp, char** argv)
{
    DEBUG("main()");
    switch(incommands.getvisMethod())
    {
    case 'B':
        visMeth = DrawStyles::Bubbles;
        break;
    case 'C':
        visMeth = DrawStyles::Cubes;
        break;
    case 'F':
        visMeth = DrawStyles::Fog;
        break;
    }

#ifndef _WINDOWS
#ifndef _LINUX
    cout<<"\nYou must define your OS on compilation.";
    cout<<"\nUse either -D_WINDOWS or -D_LINUX";
    quit(1);
#endif
#endif


    if (incommands.validateInput() == 1)
    {
        printf("you have invalid input in VQSinput.txt!!\nQuitting...\n");
        quit(1);
    }

    consoleDump();

    data = readData();



    glutInit(&argcp, argv);
    glutInitWindowSize(incommands.getimageW(), incommands.getimageH());
    glutInitWindowPosition(0, 0);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Visualising Quantum States in Disordered Systems");
    if (incommands.getimageW() == 0 || incommands.getimageH() == 0)
    {
        cout<<"\nIgnoring width and height, going full screen!";
        glutFullScreen();
    }

//Get the dir the program was run in, so png files can be copied back later
    getcwd(origDir,256);
#ifdef _DEBUG
    printf("origDir: %s\n",origDir);
#endif

//Now attempt to make a working directory on the local RAMdisk to improve performance
//Get current timestamp for unique name...
    time_t seconds;
    seconds = time (NULL);
    char newDir[256];
    sprintf(newDir,"/dev/shm/VQS-%d",seconds);

    char tmpCmd[256];
    sprintf(tmpCmd,"mkdir -p %s",newDir);
    system(tmpCmd);

//Now chdir to the new directory, stills will be made here
    chdir(newDir);



    init();


    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutDisplayFunc(Draw);
    glutIdleFunc(idle);
    glutVisibilityFunc(visFunc);
//glutIdleFunc(NULL);

    if (incommands.getoutput() == 0) //if interactive mode
    {
        glutMouseFunc(Mouse);
        glutMotionFunc(Motion);
        glutCreateMenu(mouseButton);
        glutAddMenuEntry("Take TGA of current screen", 0);
        glutAddMenuEntry("Take EPS of current screen", 1);
        glutAddMenuEntry("Exit", 2);
        glutAttachMenu(GLUT_RIGHT_BUTTON);
    }
    glPopMatrix();


    glutMainLoop();

    return 0;
}
