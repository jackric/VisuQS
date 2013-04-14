/****************************************************************************
Program:        Read input file
File:           ReadInput.cpp / ReadInput.exe
Functions:      main()
Description:    Reads input file for VQS prog.
Author:         Richard Marriott
Environment:    Borland C++BuilderX-Personal V.1.0.0.1786
Notes:          Provides access of VQSinput to VQS prog.
Revision:       1
****************************************************************************/

#include <iostream>
#include <fstream>
using namespace std;

#define VQS_TEXT_FRAMEINFO 1
#define VQS_TEXT_NONE 0
#define VQS_TEXT_TEXTFILE 2



class Input
{
  private:
    char visMethod[3];
    char output[9];
    char format[9];
    char imageW[9];
    char imageH[9];
    char framerate[9];
    char duration[9];
    char start_r[9];
    char start_phi[9];
    char start_theta[9];
    char end_r[9];
    char end_phi[9];
    char end_theta[9];
    char fly[9];
    char flip[9];
    char elev_there[9];
    char full[9];
    char elev_back[9];
    char x_tran[9];
    char y_tran[9];
    char z_tran[9];
    char bg_r[9];
    char bg_g[9];
    char bg_b[9];
    char text[3];
    char text_r[9];
    char text_g[9];
    char text_b[9];
    char threshold[9];
    char colourMethod[3];
    char antiA[9];
    char periodic[3];

    // visMethod already defined adequately above
    int intoutput;
    int intformat;
    int intimageW;
    int intimageH;
    float floatframerate;
    float floatduration;
    float floatstart_r;
    float floatstart_theta;
    float floatstart_phi;
    float floatend_r;
    float floatend_theta;
    float floatend_phi;
    bool boolfly;
    bool boolflip;
    int intelev_there;
    bool boolfull;
    int intelev_back;
    float floatx_tran;
    float floaty_tran;
    float floatz_tran;
    float floatbg_r;
    float floatbg_g;
    float floatbg_b;
    int inttext;
    float floattext_r;
    float floattext_g;
    float floattext_b;
    float floatthreshold;
    bool boolcolourMethod;
    int intantiA;
    bool boolperiodic;

    char delim[1];
    char junk[100];

  public:
    Input();
    char getvisMethod();
    int getoutput();
    int getformat();
    int getimageW();
    int getimageH();
    float getframerate();
    float getduration();
    float getstart_r();
    float getstart_phi();
    float getstart_theta();
    float getend_r();
    float getend_phi();
    float getend_theta();
    bool getfly();
    bool getflip();
    int getelev_there();
    bool getfull();
    int getelev_back();
    float getx_tran();
    float gety_tran();
    float getz_tran();
    float getbg_r();
    float getbg_g();
    float getbg_b();
    int gettext();
    float gettext_r();
    float gettext_g();
    float gettext_b();
    float getthreshold();
    bool getcolourMethod();
    int validateInput();
    int getantiA();
    bool getperiodic();
};
