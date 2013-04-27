/****************************************************************************
Program:        VisuQS.LX / VQS.exe
File:           output.cpp / output.h
Description:    Produces EPS output from frameBuffer
Author:         Mark Kilgard
Source:         http://www.opengl.org/resources/code/rendering/mjktips/Feedback.html
****************************************************************************/


#include "output.h"
#include <cstdio>
#include <cmath>
#include <GL/glut.h>

using namespace std;

/* OpenGL's GL_3D_COLOR feedback vertex format. */
typedef struct _Feedback3Dcolor
{
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat red;
    GLfloat green;
    GLfloat blue;
    GLfloat alpha;
} Feedback3Dcolor;

//int blackBackground = 0;  /* Initially use a white background. */
//int lighting = 0;       /* Initially disable . */
//int polygonMode = 1;    /* Initially show wireframe. */
//int object = 1;         /* Initially show the torus. */

//GLfloat angle = 0.0;    /* Angle of rotation for object. */
//int moving, begin;      /* For interactive object rotation. */
int size = 1;           /* Size of lines and points. */

/* How many feedback buffer GLfloats each of the three objects need. */
//int objectComplexity[] =
//{6000, 14000, 380000};  /* Teapot requires ~1.5 megabytes for
//                           its feedback results! */

/* render gets called both by "display" (in OpenGL render mode)
   and by "outputEPS" (in OpenGL feedback mode). */

/*
void
render(void)
{
  glPushMatrix();
  glRotatef(angle, 0.0, 1.0, 0.0);
  switch (object) {
  case 0:
    //glutSolidSphere(1.0, 10, 10);
    glutSolidCube(1.0);
    break;
  case 1:
    glutSolidTorus(0.5, 1.0, 15, 15);
    break;
  case 2:
    glutSolidTeapot(1.0);
    break;
  }
  glPopMatrix();
}

void
display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  render();
  glutSwapBuffers();
}
*/

//void
//updateBackground(void)
//{
//  if (blackBackground) {
//    /* Clear to black. */
//    glClearColor(0.0, 0.0, 0.0, 1.0);
//  } else {
//    /* Clear to white. */
//    glClearColor(1.0, 1.0, 1.0, 1.0);
//  }
//}

//void
//updateLighting(void)
//{
//  if (lighting) {
//    glEnable(GL_LIGHTING);
//  } else {
//    glDisable(GL_LIGHTING);
//  }
//}

//void
//updatePolygonMode(void)
//{
//  switch (polygonMode) {
//  case 0:
//    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
//    break;
//  case 1:
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    break;
//  case 2:
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//    break;
//  }
//}

/* Write contents of one vertex to stdout. */
void
print3DcolorVertex(GLint size, GLint * count,
                   GLfloat * buffer)
{
    int i;

    printf("  ");
    for (i = 0; i < 7; i++)
    {
        printf("%4.2f ", buffer[size - (*count)]);
        *count = *count - 1;
    }
    printf("\n");
}

void
printBuffer(GLint size, GLfloat * buffer)
{
    GLint count;
    int token, nvertices;

    count = size;
    while (count)
    {
        token = (int)buffer[size - count];
        count--;
        switch (token)
        {
        case GL_PASS_THROUGH_TOKEN:
            printf("GL_PASS_THROUGH_TOKEN\n");
            printf("  %4.2f\n", buffer[size - count]);
            count--;
            break;
        case GL_POINT_TOKEN:
            printf("GL_POINT_TOKEN\n");
            print3DcolorVertex(size, &count, buffer);
            break;
        case GL_LINE_TOKEN:
            printf("GL_LINE_TOKEN\n");
            print3DcolorVertex(size, &count, buffer);
            print3DcolorVertex(size, &count, buffer);
            break;
        case GL_LINE_RESET_TOKEN:
            printf("GL_LINE_RESET_TOKEN\n");
            print3DcolorVertex(size, &count, buffer);
            print3DcolorVertex(size, &count, buffer);
            break;
        case GL_POLYGON_TOKEN:
            printf("GL_POLYGON_TOKEN\n");
            nvertices = (int)buffer[size - count];
            count--;
            for (; nvertices > 0; nvertices--)
            {
                print3DcolorVertex(size, &count, buffer);
            }
        }
    }
}

GLfloat pointSize;

static char *gouraudtriangleEPS[] =
{
    "/bd{bind def}bind def /triangle { aload pop   setrgbcolor  aload pop 5 3",
    "roll 4 2 roll 3 2 roll exch moveto lineto lineto closepath fill } bd",
    "/computediff1 { 2 copy sub abs threshold ge {pop pop pop true} { exch 2",
    "index sub abs threshold ge { pop pop true} { sub abs threshold ge } ifelse",
    "} ifelse } bd /computediff3 { 3 copy 0 get 3 1 roll 0 get 3 1 roll 0 get",
    "computediff1 {true} { 3 copy 1 get 3 1 roll 1 get 3 1 roll 1 get",
    "computediff1 {true} { 3 copy 2 get 3 1 roll  2 get 3 1 roll 2 get",
    "computediff1 } ifelse } ifelse } bd /middlecolor { aload pop 4 -1 roll",
    "aload pop 4 -1 roll add 2 div 5 1 roll 3 -1 roll add 2 div 3 1 roll add 2",
    "div 3 1 roll exch 3 array astore } bd /gouraudtriangle { computediff3 { 4",
    "-1 roll aload 7 1 roll 6 -1 roll pop 3 -1 roll pop add 2 div 3 1 roll add",
    "2 div exch 3 -1 roll aload 7 1 roll exch pop 4 -1 roll pop add 2 div 3 1",
    "roll add 2 div exch 3 -1 roll aload 7 1 roll pop 3 -1 roll pop add 2 div 3",
    "1 roll add 2 div exch 7 3 roll 10 -3 roll dup 3 index middlecolor 4 1 roll",
    "2 copy middlecolor 4 1 roll 3 copy pop middlecolor 4 1 roll 13 -1 roll",
    "aload pop 17 index 6 index 15 index 19 index 6 index 17 index 6 array",
    "astore 10 index 10 index 14 index gouraudtriangle 17 index 5 index 17",
    "index 19 index 5 index 19 index 6 array astore 10 index 9 index 13 index",
    "gouraudtriangle 13 index 16 index 5 index 15 index 18 index 5 index 6",
    "array astore 12 index 12 index 9 index gouraudtriangle 17 index 16 index",
    "15 index 19 index 18 index 17 index 6 array astore 10 index 12 index 14",
    "index gouraudtriangle 18 {pop} repeat } { aload pop 5 3 roll aload pop 7 3",
    "roll aload pop 9 3 roll 4 index 6 index 4 index add add 3 div 10 1 roll 7",
    "index 5 index 3 index add add 3 div 10 1 roll 6 index 4 index 2 index add",
    "add 3 div 10 1 roll 9 {pop} repeat 3 array astore triangle } ifelse } bd",
    NULL
};

GLfloat *
spewPrimitiveEPS(FILE * file, GLfloat * loc)
{
    int token;
    int nvertices, i;
    GLfloat red, green, blue;
    int smooth;
    GLfloat dx, dy, dr, dg, db, absR, absG, absB, colormax;
    int steps;
    Feedback3Dcolor *vertex;
    GLfloat xstep, ystep, rstep, gstep, bstep;
    GLfloat xnext, ynext, rnext, gnext, bnext, distance;

    token = (int)*loc;
    loc++;
    switch (token)
    {
    case GL_LINE_RESET_TOKEN:
    case GL_LINE_TOKEN:
        vertex = (Feedback3Dcolor *) loc;

        dr = vertex[1].red - vertex[0].red;
        dg = vertex[1].green - vertex[0].green;
        db = vertex[1].blue - vertex[0].blue;

        if (dr != 0 || dg != 0 || db != 0)
        {
            /* Smooth shaded line. */
            dx = vertex[1].x - vertex[0].x;
            dy = vertex[1].y - vertex[0].y;
            distance = sqrt(dx * dx + dy * dy);

            absR = fabs(dr);
            absG = fabs(dg);
            absB = fabs(db);

#define Max(a,b) (((a)>(b))?(a):(b))

#define EPS_SMOOTH_LINE_FACTOR 0.06  /* Lower for better smooth

            lines. */

            colormax = Max(absR, Max(absG, absB));
            steps = (int)Max(1.0, colormax * distance * EPS_SMOOTH_LINE_FACTOR);

            xstep = dx / steps;
            ystep = dy / steps;

            rstep = dr / steps;
            gstep = dg / steps;
            bstep = db / steps;

            xnext = vertex[0].x;
            ynext = vertex[0].y;
            rnext = vertex[0].red;
            gnext = vertex[0].green;
            bnext = vertex[0].blue;

            /* Back up half a step; we want the end points to be
               exactly the their endpoint colors. */
            xnext -= xstep / 2.0;
            ynext -= ystep / 2.0;
            rnext -= rstep / 2.0;
            gnext -= gstep / 2.0;
            bnext -= bstep / 2.0;
        }
        else
        {
            /* Single color line. */
            steps = 0;
        }

        fprintf(file, "%g %g %g setrgbcolor\n",
                vertex[0].red, vertex[0].green, vertex[0].blue);
        fprintf(file, "%g %g moveto\n", vertex[0].x, vertex[0].y);

        for (i = 0; i < steps; i++)
        {
            xnext += xstep;
            ynext += ystep;
            rnext += rstep;
            gnext += gstep;
            bnext += bstep;
            fprintf(file, "%g %g lineto stroke\n", xnext, ynext);
            fprintf(file, "%g %g %g setrgbcolor\n", rnext, gnext, bnext);
            fprintf(file, "%g %g moveto\n", xnext, ynext);
        }
        fprintf(file, "%g %g lineto stroke\n", vertex[1].x, vertex[1].y);

        loc += 14;          /* Each vertex element in the feedback
                           buffer is 7 GLfloats. */

        break;
    case GL_POLYGON_TOKEN:
        nvertices = (int)*loc;
        loc++;

        vertex = (Feedback3Dcolor *) loc;

        if (nvertices > 0)
        {
            red = vertex[0].red;
            green = vertex[0].green;
            blue = vertex[0].blue;
            smooth = 0;
            for (i = 1; i < nvertices; i++)
            {
                if (red != vertex[i].red || green != vertex[i].green || blue != vertex[i].blue)
                {
                    smooth = 1;
                    break;
                }
            }
            if (smooth)
            {
                /* Smooth shaded polygon; varying colors at vetices. */
                //int triOffset;

                /* Break polygon into "nvertices-2" triangle fans. */
                for (i = 0; i < nvertices - 2; i++)
                {
                    //triOffset = i * 7;
                    fprintf(file, "[%g %g %g %g %g %g]",
                            vertex[0].x, vertex[i + 1].x, vertex[i + 2].x,
                            vertex[0].y, vertex[i + 1].y, vertex[i + 2].y);
                    fprintf(file, " [%g %g %g] [%g %g %g] [%g %g %g] gouraudtriangle\n",
                            vertex[0].red, vertex[0].green, vertex[0].blue,
                            vertex[i + 1].red, vertex[i + 1].green, vertex[i + 1].blue,
                            vertex[i + 2].red, vertex[i + 2].green, vertex[i + 2].blue);
                }
            }
            else
            {
                /* Flat shaded polygon; all vertex colors the same. */
                fprintf(file, "newpath\n");
                fprintf(file, "%g %g %g setrgbcolor\n", red, green, blue);

                /* Draw a filled triangle. */
                fprintf(file, "%g %g moveto\n", vertex[0].x, vertex[0].y);
                for (i = 1; i < nvertices; i++)
                {
                    fprintf(file, "%g %g lineto\n", vertex[i].x, vertex[i].y);
                }
                fprintf(file, "closepath fill\n\n");
            }
        }
        loc += nvertices * 7;  /* Each vertex element in the
                              feedback buffer is 7 GLfloats. */
        break;
    case GL_POINT_TOKEN:
        vertex = (Feedback3Dcolor *) loc;
        fprintf(file, "%g %g %g setrgbcolor\n", vertex[0].red, vertex[0].green, vertex[0].blue);
        fprintf(file, "%g %g %g 0 360 arc fill\n\n", vertex[0].x, vertex[0].y, pointSize / 2.0);
        loc += 7;           /* Each vertex element in the feedback
                           buffer is 7 GLfloats. */
        break;
    default:
        /* XXX Left as an excersie to the reader. */
        printf("Incomplete implementation.  Unexpected token (%d).\n", token);
        exit(1);
    }
    return loc;
}

void
spewUnsortedFeedback(FILE * file, GLint size, GLfloat * buffer)
{
    GLfloat *loc, *end;

    loc = buffer;
    end = buffer + size;
    while (loc < end)
    {
        loc = spewPrimitiveEPS(file, loc);
    }
}

typedef struct _DepthIndex
{
    GLfloat *ptr;
    GLfloat depth;
} DepthIndex;

static int
compare(const void *a, const void *b)
{
    DepthIndex *p1 = (DepthIndex *) a;
    DepthIndex *p2 = (DepthIndex *) b;
    GLfloat diff = p2->depth - p1->depth;

    if (diff > 0.0)
    {
        return 1;
    }
    else if (diff < 0.0)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}


#define EPS_GOURAUD_THRESHOLD 0.1  /* Lower for better (slower)

smooth shading. */



