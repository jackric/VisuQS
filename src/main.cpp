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

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cfloat>
#include <GL/glut.h>
#include <time.h>

#include "constants.h"
#include "readData.h"
#include "capture.h"
#include "jitter.h"
#include "debug.h"
#include "util.h"
#include "HSVtoRGB.h"
#include "readInput.h"
#include "output.h"
#include "annotations.h"
#include "main.h"
#include "interaction.h"

// #include <cstring>



#include <curses.h>

using namespace std;
Input incommands;



// Global objects, structures & variables
bool flip = incommands.getflip();
DrawStyles::Enum visMeth = incommands.getvisMethod();

float fogColor[]= {incommands.getbg_r(), incommands.getbg_g(), incommands.getbg_b(), 1.0};

psiArray data;

int visStatus;
const double pi = 3.141592654;
#define PI_ 3.14159265358979323846
int flycounter;

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


char origDir[256];



void init(void)
{
    DEBUG("init()");

    int flags[] = {
        GL_DEPTH_TEST, GL_LIGHTING, GL_COLOR_MATERIAL, GL_CULL_FACE};

    for(int i=0; i<sizeof(flags);i++)
    {
        glEnable(flags[i]);
    }


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


    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);


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



    system("rm -r seq > /dev/null 2> /dev/null");


}


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
    psiArray data;

public:
    Cell(psiArray inData) // constructor
    {
        data = inData;
    }



    void draw(float rgb[3], float prob)
    {
        max = data.max;
        min = data.min;
        float size, reducedMax, opacity;

        switch(visMeth)
        {
        case DrawStyles::Cubes:
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
            break;
        case DrawStyles::Bubbles:
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
            break;
        case DrawStyles::Fog:
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
            break;
        default:
            cout<<"\ncell(): error --- visualisation method parameter not recognised";
            quit(1);
        }
    }
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
                        this->draw(rgb, data.matrix3d[i][j][k]);
                    }

                    glPopMatrix();
                }
            }
        }

    }
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
        default:
            cout<<"\ncell(): error --- dimension should be 1-3";
            quit(1);
        }
        glPopMatrix();
        glColor3f(incommands.gettext_r(), incommands.gettext_g(), incommands.gettext_b());
        glutWireCube (1.0);
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
    Cell unit(data);

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
                    unit.drawCrystal();
                    glPopMatrix();
                }
            }
        }
    }
    else
    {
        unit.drawCrystal();
    }

    glPopMatrix();
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
        drawColourScale(incommands, threshold, data.max);

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
        capture(incommands.getformat(), counter);
        quit(0);
    }
    if (incommands.getoutput() > 1)
    {
        capture(incommands.getoutput(), counter);
    }



}



void idle()
{
    DEBUG("idle()");


    if(incommands.getfly())
        fly();
    else
        rotate();

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



void visFunc(int stat)
{
    visStatus = stat;

}

int main(int argcp, char** argv)
{
    DEBUG("main()");

    if (incommands.validateInput() == 1)
    {
        printf("you have invalid input in VQSinput.txt!!\nQuitting...\n");
        quit(1);
    }


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

    glPopMatrix();


    glutMainLoop();

    return 0;
}
