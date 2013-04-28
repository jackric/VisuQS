/****************************************************************************
Program:        VisuQS.LX / VQS.exe
File:           readInput.cpp / readInput.h
Description:    Reads input file for VQS prog.
Author:         Richard Marriott / Luke Gyngell
Environment:    Borland C++BuilderX-Personal V.1.0.0.1786
Notes:          Provides access of VQSinput to VQS prog.
Revision:       $Revision: 1.2 $
****************************************************************************/

#include "readInput.h"
#include "main.h"
#include <cstdlib>

Input::Input()
{
    ifstream inp;
    ofstream out;
    string inputFileName;

#ifdef _DEBUG
    cout << "\ninput(): $Revision: 1.2 $";
#endif

    inputFileName = "VQSinput.txt";
    inp.open(inputFileName.c_str(), ifstream::in);
    inp.close();

    ifstream fromfile("VQSinput.txt");
    fromfile.getline(strVisMethod,3,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(colourMethod,3,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(periodic,3,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(threshold,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(output,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(format,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(imageW,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(imageH,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(framerate,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(duration,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(antiA,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(start_r,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(start_phi,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(start_theta,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(end_r,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(end_phi,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(end_theta,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(fly,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(flip,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(elev_there,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(full,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(elev_back,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(x_tran,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(y_tran,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(z_tran,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(bg_r,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(bg_g,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(bg_b,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(text,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(text_r,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(text_g,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.getline(text_b,9,'#');
    fromfile.getline((char*)&junk,100,'\n');
    fromfile.close();
}

DrawStyles::Enum Input::getvisMethod()
{
    switch(strVisMethod[0])
    {
    case 'B':
        return DrawStyles::Bubbles;
        break;
    case 'C':
        return DrawStyles::Cubes;
        break;
    case 'F':
        return DrawStyles::Fog;
        break;
    default:
        cout << "\nERROR: Please select an appropriate visualization method";
        quit(1);
    }
}

int Input::getoutput()
{
    sscanf(&output[0],"%d",&intoutput);
    //printf("Value of &output[0]: %s",&output[0]);
    return intoutput;
}

int Input::getformat()
{
    sscanf(&format[0],"%d",&intformat);
    return intformat;
}

int Input::getimageW()
{
    sscanf(&imageW[0],"%d",&intimageW);
    return intimageW;
}

int Input::getimageH()
{
    sscanf(&imageH[0],"%d",&intimageH);
    return intimageH;
}

float Input::getframerate()
{
    sscanf(&framerate[0],"%f",&floatframerate);
    return floatframerate;
}

float Input::getduration()
{
    sscanf(&duration[0],"%f",&floatduration);
    return floatduration;
}

float Input::getstart_r()
{
    sscanf(&start_r[0],"%f",&floatstart_r);
    return floatstart_r;
}

float Input::getstart_phi()
{
    sscanf(&start_phi[0],"%f",&floatstart_phi);
    return floatstart_phi;
}

float Input::getstart_theta()
{
    sscanf(&start_theta[0],"%f",&floatstart_theta);
    return floatstart_theta;
}

float Input::getend_r()
{
    sscanf(&end_r[0],"%f",&floatend_r);
    return floatend_r;
}

float Input::getend_phi()
{
    sscanf(&end_phi[0],"%f",&floatend_phi);
    return floatend_phi;
}

float Input::getend_theta()
{
    sscanf(&end_theta[0],"%f",&floatend_theta);
    return floatend_theta;
}

bool Input::getfly()
{
    boolfly = false;
    if (fly[0] == '1')
        boolfly = true;
    if (fly[0] == '0')
        boolfly = false;

    return boolfly;
}

bool Input::getflip()
{
    boolflip = false;
    if (flip[0] == '1')
        boolflip = true;
    if (flip[0] == '0')
        boolflip = false;

    return boolflip;
}

int Input::getelev_there()
{
    sscanf(&elev_there[0],"%d",&intelev_there);
    return intelev_there;
}

bool Input::getfull()
{
    boolfull = false;
    if (full[0] == '1')
        boolfull = true;
    if (full[0] == '0')
        boolfull = false;

    return boolfull;
}

int Input::getelev_back()
{
    sscanf(&elev_back[0],"%d",&intelev_back);
    return intelev_back;
}

float Input::getx_tran()
{
    sscanf(&x_tran[0],"%f",&floatx_tran);
    return floatx_tran;
}

float Input::gety_tran()
{
    sscanf(&y_tran[0],"%f",&floaty_tran);
    return floaty_tran;
}

float Input::getz_tran()
{
    sscanf(&z_tran[0],"%f",&floaty_tran);
    return floaty_tran;
}

float Input::getbg_r()
{
    sscanf(&bg_r[0],"%f",&floatbg_r);
    return floatbg_r;
}

float Input::getbg_g()
{
    sscanf(&bg_g[0],"%f",&floatbg_g);
    return floatbg_g;
}

float Input::getbg_b()
{
    sscanf(&bg_b[0],"%f",&floatbg_b);
    return floatbg_b;
}

int Input::gettext()
{
    sscanf(&text[0],"%d",&inttext);

    return inttext;

}

float Input::gettext_r()
{
    sscanf(&text_r[0],"%f",&floattext_r);
    return floattext_r;
}

float Input::gettext_g()
{
    sscanf(&text_g[0],"%f",&floattext_g);
    return floattext_g;
}

float Input::gettext_b()
{
    sscanf(&text_b[0],"%f",&floattext_b);
    return floattext_b;
}

float Input::getthreshold()
{
    sscanf(&threshold[0],"%f",&floatthreshold);
    return floatthreshold;
}

bool Input::getcolourMethod()
{
    boolcolourMethod = false;
    if (colourMethod[0] == '1')
        boolcolourMethod = true;
    if (colourMethod[0] == '0')
        boolcolourMethod = false;

    return boolcolourMethod;
}

int Input::getantiA()
{
    sscanf(&antiA[0],"%d",&intantiA);
    return intantiA;
}

bool Input::getperiodic()
{
    boolperiodic = false;
    if (periodic[0] == '1')
        boolperiodic = true;
    if (periodic[0] == '0')
        boolperiodic = false;

    return boolperiodic;
}

int Input::validateInput()
{
    int status = 0;

#ifdef _DEBUG
    cout << "\nvalidateInput(): $Revision: 1.2 $";
#endif

    if (getoutput() > 3 || getoutput() < 0 || (getoutput() == 0 && output[0] != '0'))
    {
        cout << "\nERROR: Please select an appropriate output type";
        status = 1;
    }
    if (getformat() > 1 || getformat() < 0 || (getformat() == 0 && format[0] != '0'))
    {
        cout << "\nERROR: Please select an appropriate still image output format";
        status = 1;
    }
    if (getvisMethod() == DrawStyles::Fog && getoutput() == 1 && getformat() == 1)
    {
        cout << "\nERROR: EPS output cannot be used with the Fog vizualization method";
        status = 1;
    }
    if (getperiodic() == 1 && getoutput() == 1 && getformat() == 1)
    {
        cout << "\nERROR: EPS output cannot be used with periodic plotting";
        status = 1;
    }
    if (getimageW() < 0 || (getimageW() == 0 && imageW[0] != '0'))
    {
        cout << "\nERROR: Please enter a positive integer or '0' for image width";
        status = 1;
    }
    if (getimageH() < 0 || (getimageH() == 0 && imageH[0] != '0'))
    {
        cout << "\nERROR: Please enter a positive integer or '0' for image height";
        status = 1;
    }
    if (getframerate() < 1)
    {
        cout << "\nERROR: Please enter a positive value >=1 for frame rate";
        status = 1;
    }
    if (getduration() < 0.1)
    {
        cout << "\nERROR: Please enter a positive value >=1 for animation duration";
        status = 1;
    }
    if (getstart_r() < 0 || (getstart_r() == 0 && start_r[0] != '0'))
    {
        cout << "\nERROR: Please enter a positive value for start 'r'";
        status = 1;
    }
    if (getstart_phi() < -3600 || getstart_phi() > 3600 || (getstart_phi() == 0 && start_phi[0] != '0'))
    {
        cout << "\nERROR: Please enter a numeric value for start 'phi'";
        status = 1;
    }
    if (getstart_theta() < -3600 || getstart_theta() > 3600 || (getstart_theta() == 0 && start_theta[0] != '0'))
    {
        cout << "\nERROR: Please enter a numeric value for start 'theta'";
        status = 1;
    }
    if (getend_r() < 0 || (getend_r() == 0 && end_r[0] != '0'))
    {
        cout << "\nERROR: Please enter a positive value for end 'r'";
        status = 1;
    }
    if (getend_phi() < -3600 || getend_phi() > 3600 || (getend_phi() == 0 && end_phi[0] != '0'))
    {
        cout << "\nERROR: Please enter a numeric value for end 'phi'";
        status = 1;
    }
    if (getend_theta() < -3600 || getend_theta() > 3600 || (getend_theta() == 0 && end_theta[0] != '0'))
    {
        cout << "\nERROR: Please enter a numeric value for end 'theta'";
        status = 1;
    }
    if (getfly() > 1 || getfly() < 0 || (getfly() == 0 && fly[0] != '0'))
    {
        cout << "\nERROR: Please select either '0' or '1' for fly-through";
        status = 1;
    }
    if (getflip() > 1 || getflip() < 0 || (getflip() == 0 && flip[0] != '0'))
    {
        cout << "\nERROR: Please select either '0' or '1' for flip on fly-through";
        status = 1;
    }
    if (getelev_there() < -3600 || getelev_there() > 3600 || (getelev_there() == 0 && elev_there[0] != '0'))
    {
        cout << "\nERROR: Please enter a numeric value for elevation of rotation towards end point";
        status = 1;
    }
    if (getfull() > 1 || getfull() < 0 || (getfull() == 0 && full[0] != '0'))
    {
        cout << "\nERROR: Please select either '0' or '1' for full rotation";
        status = 1;
    }
    if (getelev_back() < -3600 || getelev_back() > 3600 || (getelev_back() == 0 && elev_back[0] != '0'))
    {
        cout << "\nERROR: Please enter a numeric value for elevation of rotation back from end point";
        status = 1;
    }
    if (getx_tran() == 0 && x_tran[0] != '0')
    {
        cout << "\nERROR: Please enter a numeric value for x translation";
        status = 1;
    }
    if (gety_tran() == 0 && y_tran[0] != '0')
    {
        cout << "\nERROR: Please enter a numeric value for y translation";
        status = 1;
    }
    if (getz_tran() == 0 && z_tran[0] != '0')
    {
        cout << "\nERROR: Please enter a numeric value for z translation";
        status = 1;
    }
    if (getbg_b() > 1 || getbg_b() < 0 || (getbg_b() == 0 && bg_b[0] != '0'))
    {
        cout << "\nERROR: Please enter a value between 0 and 1 for the blue component of the background";
        status = 1;
    }
    if (getbg_g() > 1 || getbg_g() < 0 || (getbg_g() == 0 && bg_g[0] != '0'))
    {
        cout << "\nERROR: Please enter a value between 0 and 1 for the green component of the background";
        status = 1;
    }
    if (getbg_r() > 1 || getbg_r() < 0 || (getbg_r() == 0 && bg_r[0] != '0'))
    {
        cout << "\nERROR: Please enter a value between 0 and 1 for the red component of the background";
        status = 1;
    }
    if (gettext_b() > 1 || gettext_b() < 0 || (gettext_b() == 0 && text_b[0] != '0'))
    {
        cout << "\nERROR: Please enter a value between 0 and 1 for the blue component of the text";
        status = 1;
    }
    if (gettext() > 2 || gettext() < 0 || (gettext() == 0 && text[0] != '0'))
    {
        cout << "\nERROR: Please select either '0' or '1' for on-image text";
        status = 1;
    }
    if (gettext_g() > 1 || gettext_g() < 0 || (gettext_g() == 0 && text_g[0] != '0'))
    {
        cout << "\nERROR: Please enter a value between 0 and 1 for the green component of the textd";
        status = 1;
    }
    if (gettext_r() > 1 || gettext_r() < 0 || (gettext_r() == 0 && text_r[0] != '0'))
    {
        cout << "\nERROR: Please enter a value between 0 and 1 for the red component of the text";
        status = 1;
    }
    if (getthreshold() > 1 || getthreshold() < 0 || (getthreshold() == 0 && threshold[0] != '0'))
    {
        cout << "\nERROR: Please enter a value between 0 and 1 for the probability threshold";
        status = 1;
    }
    if (getcolourMethod() > 1 || getcolourMethod() < 0 || (getcolourMethod() == 0 && colourMethod[0] != '0'))
    {
        cout << "\nERROR: Please select either '0' or '1' for colouring method";
        status = 1;
    }
    if (getthreshold() > 1 || getthreshold() < 0 || (getthreshold() == 0 && threshold[0] != '0'))
    {
        cout << "\nERROR: Please enter a value between 0 and 1 for the probability threshold";
        status = 1;
    }
    if (getcolourMethod() > 1 || getcolourMethod() < 0 || (getcolourMethod() == 0 && colourMethod[0] != '0'))
    {
        cout << "\nERROR: Please select either '0' or '1' for colouring method";
        status = 1;
    }
    if (getantiA() > 15 || getantiA() < 1)
    {
        cout << "\nERROR: Please select an anti-aliasing level between 1 and 15 (larger than 4 is not recommended for large data sets)";
        status = 1;
    }
    if (getperiodic() > 1 || getperiodic() < 0 || (getperiodic() == 0 && periodic[0] != '0'))
    {
        cout << "\nERROR: Please select either '0' or '1' for periodic plotting";
        status = 1;
    }
    if (getperiodic() == 1 && getvisMethod() == DrawStyles::Fog)
    {
        cout << "\nERROR: Please select either cube or bubble visualisation methods for periodic plotting";
        status = 1;
    }
    if (getantiA() > 1 && getvisMethod() == DrawStyles::Fog)
    {
        cout << "\nWARNING: Antialiasing is not necessary for 'fog'-type visualizations";
    }
    if (getantiA() > 1 && getoutput() == 1)
    {
        cout << "\nWARNING: Antialiasing is not necessary for individual stills";
    }
    if (getoutput() == 2 && getformat() == 1)
    {
        cout << "\nWARNING: EPS Sequence-creation can be time-consuming.";
        cout << "\n         TGA format is recommended for this output method.";
    }

    return status;
}

