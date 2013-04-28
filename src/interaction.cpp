#include <GL/glut.h>
#include "main.h"
#include "readInput.h"
#include "debug.h"
#include "capture.h"

Input _incommands;

void Mouse(int button, int state, int x, int y)
{

    DEBUG("Mouse()");

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


void Keyboard(unsigned char key, int x, int y)
{

    DEBUG("Keyboard()");

    if (_incommands.getoutput() == 0)
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
            capture(1, counter);
            break;
        case 'r':
            init();
            break;
        case 'w':
            glutReshapeWindow(250, 250);
            break;
        case 't':
            antiAliasDraw();
            capture(0, counter);
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
            system("rm -r seq > /dev/null 2> /dev/null");
            quit(0);
            break;
        case 'q': // escape key
            system("rm -r seq > /dev/null 2> /dev/null");
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
        capture(0, counter);
        break;
    case 1: //EPS capture
        antiAliasDraw();
        capture(1, counter);
        break;
    case 2: //exit
        quit(0);
        break;
    }
}
