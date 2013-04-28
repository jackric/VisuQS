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


char origDir[256];

void opengl_init()
{
    int flags[] =
    {
        GL_DEPTH_TEST, GL_LIGHTING, GL_COLOR_MATERIAL, GL_CULL_FACE
    };

    for(unsigned int i=0; i<sizeof(flags); i++)
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


    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // black background
    glClearColor (0, 0, 0, 0.0);
    glClearAccum(0.0, 0.0, 0.0, 0.0);

}


void init(void)
{
    DEBUG("init()");


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
        break;
        case DrawStyles::Bubbles:
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
        break;
        case DrawStyles::Fog:
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

    unit.drawCrystal();

    glPopMatrix();
}


void simpleDraw()
{
    DEBUG("simpleDraw()");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    render();
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

    init();
    opengl_init();


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
