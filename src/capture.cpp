#include <GL/glut.h>
#include "debug.h"
#include <cstdlib>
#include <iostream>
#include "readData.h"
#include "readInput.h"
#include "capture.h"


using namespace std;

void capture(int type, int counter)
{
    char fname[255];
    snprintf(fname, sizeof(fname), "frame_%d", counter);
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    DEBUG("capture()");

    switch(type)
    {
    case 0:
        strcat(fname, ".tga");
        DEBUG("Creating image %s", fname);
        outputTGA(0, 0, viewport[2], viewport[3], fname);
        DEBUG("Image created successfully");
        break;
    case 2: //if making STILLS
        snprintf(fname, sizeof(fname), "image_%d.tga", counter);
        DEBUG("Creating image %s", fname);
        outputTGA(0, 0, viewport[2], viewport[3], fname);
        DEBUG("Image created successfully");
        break;
    case 3:
        if (counter == 1)
        {
            system("mkdir seq");
        }
        char screenNumber[9];

        snprintf(screenNumber, sizeof(screenNumber), "%d", counter);
        strcpy(fname, "seq/");
        strcat(fname, screenNumber);
        strcat(fname, ".tga");

        outputTGA(0, 0, viewport[2], viewport[3], fname);
        break;
    default:
        break;
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


    }
}
