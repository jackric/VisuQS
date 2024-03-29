/****************************************************************************
Program:        VisuQS.LX / VQS.exe
File:           readInput.cpp / readInput.h
Description:    Reads data for visualisation
Author:         Richard Marriott / Luke Gyngell
Environment:    Borland C++BuilderX-Personal V.1.0.0.1786
Revision:       $Revision: 1.3 $
****************************************************************************/

#include "readData.h"
#include "readInput.h"
#include "quickSort.h"
#include <stdio.h>
#include <string.h>
#include "debug.h"

Input commands;

void findDimension(char line[], int dimension_extent[3], int numberLines)
{
    /* Files can be like "x y z phi"
     * Or just "phi" per line, and the program deduces 3d position from order in file
     *
     *
     */

    int dimensionCheck;
    bool invalid = false;

    DEBUG("findDimension()");
    float cubedRoot = pow(numberLines, 1.0/3);
    float squareRoot = (float)sqrt((double)numberLines);

    // exact cube number
    if (cubedRoot == (int)cubedRoot)
    {
        dimensionCheck = 3;
    }
    else if (squareRoot == (int)squareRoot)
    {
        dimensionCheck = 2;
    }
    else
    {
        dimensionCheck = 1;
    }

    // Why?
    int dimensionCount = -1;
    if (line[0] != ' ')
    {
        dimensionCount++;
    }
    int lineSize = strlen(line);
    for (int i = 0; i < lineSize - 1; i++)
    {
        // Amount of space-separations is how many dimensions??
        if (line[i] == ' ' && line[i+1] != ' ')
        {
            dimensionCount++;
        }
    }

    dimension_extent[2] = dimensionCount;
    dimension_extent[0] = dimensionCount;

    switch (dimensionCount)
    {
        // No spaces
    case 0:
        if (dimensionCheck == 3)
        {
            dimension_extent[0] = 3;
            dimension_extent[1] = (int)cubedRoot;
        }
        else if (dimensionCheck == 2)
        {
            dimension_extent[0] = 2;
            dimension_extent[1] = (int)squareRoot;
        }
        else
        {
            dimension_extent[0] = 1;
            dimension_extent[1] = numberLines;
        }
        break;
    case 1:
        dimension_extent[1] = numberLines;
        break;
    case 2:
        if (dimensionCheck == 1)
        {
            invalid = true;
        }
        dimension_extent[1] = (int)squareRoot;
        break;
    case 3:
        if (dimensionCheck != 3)
        {
            invalid = true;
        }
        dimension_extent[1] = (int)cubedRoot;
        break;
    default:
        cout << "\nmemoryCheck(): ERORR in data input file: Unrecognised Format";
        cout << "\nmemoryCheck(): Data with coordinates must be space delimited";
        cout << "\nmemoryCheck(): Different values must be on separate lines";
        exit (2);
        break;
    }

    if (invalid == true)
    {
        cout << "\nmemoryCheck(): Error: Number of data values does not match the coordinate system";
        exit (3);
    }
}


psiArray readData()
{
    psiArray theStructure;
    DEBUG("readData()");

    //Open the file for reading
    ifstream datafile ("VQSdata.txt");
    if (! datafile.is_open())
    {
        cout << "\nError opening file";
        exit (1);
    }

//Get number of chars in file
    datafile.seekg(0, ios::end);
    int totalChars = datafile.tellg();
    datafile.seekg(0, ios::beg);

    int lineSize = 0;
    int numberLines = 1;

    bool firstLine = true;
    char tempChar;

    for ( int currentChar = 1; currentChar <= totalChars; currentChar++ ) // Find number of lines in file
    {
        datafile.get(tempChar);
        if ( firstLine == true )
        {
            lineSize++;
            if ( tempChar == '\n' )
            {
                firstLine = false;
            }
        }
        if ( (tempChar == '\n') && (currentChar != totalChars) )
        {
            numberLines++;
        }
    }
    cout<<"\nreadData(): Found numberLines"<<flush;
    printf("Found %d lines", numberLines);
    cout << flush;

//Go back to start of file
    datafile.seekg(0, ios::beg);
    char *char_ptr = new char[lineSize];
    int dimension_extent[3];
    datafile.getline(char_ptr, lineSize);
    findDimension(char_ptr, dimension_extent, numberLines);
    delete char_ptr;
    theStructure.dimension = dimension_extent[0];
    theStructure.extent = dimension_extent[1];

    datafile.seekg(0, ios::beg);

    vector<float> probs; // 1D array of psi^2 values
    vector<int> triplet; // (x, y, z)
    vector< vector<int> > coords; // 1D array of 'triplets'
    float inputValue;

    int i, j, k;

    if(commands.getvisMethod() == DrawStyles::Fog) // Creates linear coords array
    {
        for(i=0; i < theStructure.extent; i++)
        {
            if(theStructure.dimension > 1)
            {
                for(j=0; j < theStructure.extent; j++)
                {
                    if(theStructure.dimension == 3)
                    {
                        for(k=0; k < theStructure.extent; k++)
                        {
                            triplet.push_back(i);
                            triplet.push_back(j);
                            triplet.push_back(k);

                            coords.push_back(triplet);
                            triplet.clear();
                        }
                    }
                    else
                    {
                        triplet.push_back(i);
                        triplet.push_back(j);
                        triplet.push_back(0);

                        coords.push_back(triplet);
                        triplet.clear();
                    }
                }
            }
            else
            {
                triplet.push_back(i);
                triplet.push_back(0);
                triplet.push_back(0);

                coords.push_back(triplet);
                triplet.clear();
            }
        }
        cout<<"\nreadData(): Linear coord.s vector created"<<flush;
    }

    //Test to see what format of data we are reading....

    char testStr[256];

    //get the first line
    datafile.getline(testStr,256);
    //reset the file pointer
    datafile.seekg(0, ios::beg);

    if(strchr(testStr,'E') != NULL) //is scientific format
    {

        for ( int currentLine = 0; currentLine < numberLines; currentLine++ ) // Creates the probs array
        {
            switch (dimension_extent[2])
            {
            case 0:
                break;
            case 1: //1D
                datafile >> skipws >> scientific >> inputValue;
                break;
            case 2: //2D
                datafile >> skipws >> scientific >> inputValue;
                datafile >> skipws >> scientific >> inputValue;
                break;
            case 3: //3D
                datafile >> skipws >> scientific >> inputValue;
                datafile >> skipws >> scientific >> inputValue;
                datafile >> skipws >> scientific >> inputValue;
                break;
            }
            datafile >> skipws >> scientific >> inputValue;
            probs.push_back( inputValue*inputValue ); //is the psi squared value
        }
    }
    else
    {


        for ( int currentLine = 0; currentLine < numberLines; currentLine++ ) // Creates the probs array
        {
            switch (dimension_extent[2])
            {
            case 0:
                break;
            case 1: //1D
                datafile >> inputValue;
                break;
            case 2: //2D
                datafile >> inputValue;
                datafile >> inputValue;
                break;
            case 3: //3D
                datafile >> inputValue;
                datafile >> inputValue;
                datafile >> inputValue;
                break;
            }
            datafile >> inputValue;
            probs.push_back( inputValue*inputValue ); //is the psi squared value
        }
    }
    cout<<"\nreadData(): Data extracted"<<flush;

    theStructure.max = *max_element( probs.begin(), probs.end() );
    theStructure.min = *min_element( probs.begin(), probs.end() );

    if(commands.getvisMethod() == DrawStyles::Fog) // Sort for more effective blending
    {
        quickSort(probs, coords, numberLines); // sorts probs (and respective coords) by probability
        cout<<"\nreadData(): Sorted"<<flush;
    }

    for(int i = 0; i<numberLines; i++) // Reshapes probs vector and assigns to structure
    {
        theStructure.matrix1d.push_back(probs[i]);

        if( (theStructure.dimension > 1) && ((i+1)%theStructure.extent == 0) )
        {
            theStructure.matrix2d.push_back(theStructure.matrix1d);
            theStructure.matrix1d.clear();
            if ( (theStructure.dimension > 2) && (((i+1)/theStructure.extent)%theStructure.extent == 0) )
            {
                theStructure.matrix3d.push_back(theStructure.matrix2d);
                theStructure.matrix2d.clear();
            }
        }
    }
    probs.clear();
    cout<<"\nreadData(): prob.s vector reshaped"<<flush;


    vector<vector<int> > temp1d;

    if(commands.getvisMethod() == DrawStyles::Fog) // Reshapes coord.s vector and assigns to structure
    {
        if(theStructure.dimension == 1)
        {
            for(int i = 0; i<theStructure.extent; i++)
            {
                theStructure.coords1d.push_back(coords[i][0]);
            }
        }
        else if(theStructure.dimension == 2)
        {
            for(i = 0; i<theStructure.extent; i++)
            {
                for(j = 0; j<theStructure.extent; j++)
                {
                    triplet.push_back(coords[(i*theStructure.extent)+j][0]);
                    triplet.push_back(coords[(i*theStructure.extent)+j][1]);
                    triplet.push_back(0);
                    temp1d.push_back(triplet);
                    triplet.clear();
                }
                theStructure.coords2d.push_back(temp1d);
                temp1d.clear();
            }
        }
        else if(theStructure.dimension == 3)
        {
            for(i = 0; i<theStructure.extent; i++)
            {
                for(j = 0; j<theStructure.extent; j++)
                {
                    for(k = 0; k<theStructure.extent; k++)
                    {
                        int element = (i*theStructure.extent*theStructure.extent)+(j*theStructure.extent)+k;
                        triplet.push_back(coords[element][0]);
                        triplet.push_back(coords[element][1]);
                        triplet.push_back(coords[element][2]);
                        temp1d.push_back(triplet);
                        triplet.clear();
                    }
                    theStructure.coords2d.push_back(temp1d);
                    temp1d.clear();
                }
                theStructure.coords3d.push_back(theStructure.coords2d);
                theStructure.coords2d.clear();
            }
        }
        cout<<"\nreadData(): coord.s vector reshaped"<<flush;
    }
    coords.clear();


    return theStructure;
}
