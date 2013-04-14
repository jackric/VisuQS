/****************************************************************************
Program:        VisuQS.LX / VQS.exe
File:           createInput.cpp
Description:    Creates default input file for VQS prog.
Author:         Richard Marriott / Luke Gyngell
Environment:    Borland C++BuilderX-Personal V.1.0.0.1786
Notes:          Must be run before VQS prog. can be used
Revision:       $Revision: 1.2 $
****************************************************************************/

#include "createInput.h"

void createInput()
{
  char visMethod = 'F';
  int output = 0;
  int stillFormat = 1;
  int width = 640;
  int height = 480;
  int framerate = 40;
  int duration = 10;
  int offset = 0;
  float start_r = 1.5;
  float start_phi = 0.0;
  float start_theta = 90.0;
  float end_r = 1.5;
  float end_phi = 180.0;
  float end_theta = 90.0;
  int fly = 0;
  int flip = 0;
  float elev_there = 0.0;
  int full = 1;
  float elev_back = 0.0;
  float x_tran = 0.0;
  float y_tran = 0.0;
  float z_tran = 0.0;
  float bg_r = 0.0;
  float bg_g = 0.0;
  float bg_b = 0.0;
  int text = 1;
  float text_r = 1.0;
  float text_g = 1.0;
  float text_b = 1.0;
  float threshold = 1.0;
  int colourMethod = 0;
  int antiA = 3;
  int periodic = 0;

  #ifdef _DEBUG
    //cout << "\ncreateInput(): $Revision: 1.2 $"<<flush;
    //-- annoying error when compiling, not needed for now so commented out, JB	--//
  #endif

//-- ditto above, not actually needed and causing compile problems, JB --//
/* 
  ofstream outfile("VQSinput.txt");

  outfile << visMethod << "\t# Visualisation method: 'B'=Bubbles, 'C'=Cubes, 'F'=Fog\n"
          << colourMethod << "\t# Colouring method: '0'=Graded, '1'=Fire\n"
          << periodic << "\t# Periodic plotting: '0'=Single cube, '1'=Atmosphere\n"
          << threshold << "\t# Probability threshold - N.B. '1.0' (max) defaults to average\n"
          << output << "\t# Output type: '0'=Interactive, '1'=Still, '2'=Sequence of stills, '3'=.avi\n"
          << stillFormat << "\t# Output format: '0'=.tga, '1'=.eps\n"
          << width << "\t# Width of image in pixels (0 for max screen resolution)\n"
          << height << "\t# Height of image in pixels (0 for max screen resolution)\n"
          << framerate << "\t# Frame rate\n"
	  << duration << "\t# Duration of animation (seconds)\n"
	  << offset << "\t# For batch processing, set to 0 if running locally\n" //added for batch processing, JB
          << antiA << "\t# Level of antialiasing\n"
          << start_r << "\t# Start-r parameter\n"
          << start_phi << "\t# Start-phi parameter (longitude)\n"
          << start_theta << "\t# Start-theta parameter (latitude)\n"
          << end_r << "\t# End-r parameter\n"
          << end_phi << "\t# End-phi parameter (longitude)\n"
          << end_theta << "\t# End-theta parameter (latitude)\n"
          << fly << "\t# Fly-through flag: '0'=Rotation, '1'=Fly-through\n"
          << flip << "\t# Flip flag (Always faces centre on fly-through): '0'=Normal, '1'=Flip\n"
          << elev_there << "\t# Angle of elevation of rotation towards end point\n"
          << full << "\t# Full-rotation flag: '0'=Start to End, '1'=Full rotation\n"
          << elev_back << "\t# Angle of elevation of rotation back from end point\n"
          << x_tran << "\t# X-coord. of cube relative to centre of rotation\n"
          << y_tran << "\t# Y-coord. of cube relative to centre of rotation\n"
          << z_tran << "\t# Z-coord. of cube relative to centre of rotation\n"
          << bg_r << "\t# R value of background colour\n"
          << bg_g << "\t# G value of background colour\n"
          << bg_b << "\t# B value of background colour\n"
          << text << "\t# Text flag: '0'=Text off, '1'=Text on\n"
          << text_r << "\t# R value of text colour\n"
          << text_g << "\t# G value of text colour\n"
          << text_b << "\t# B value of text colour\n";*/
}
