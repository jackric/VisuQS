#include "util.h"
#include <sys/stat.h>
#include <string>
using std::string;
//From http://www.techbytes.ca/techbyte103.html
bool FileExists(string strFilename)
{
    struct stat stFileInfo;
    bool blnReturn;
    int intStat;

    // Attempt to get the file attributes
    intStat = stat(strFilename.c_str(),&stFileInfo);
    if(intStat == 0)
    {
        // We were able to get the file attributes
        // so the file obviously exists.
        blnReturn = true;
    }
    else
    {
        // We were not able to get the file attributes.
        // This may mean that we don't have permission to
        // access the folder which contains this file. If you
        // need to do that level of checking, lookup the
        // return values of stat which will give you
        // more details on why stat failed.
        blnReturn = false;
    }

    return(blnReturn);
}

void removeZeros(char address[])
{
    int i;

    for(i=0; address[i] != '\0'; i++) // Find start of trailing zeros
    {
        if(address[i] == '0')
        {
            break;
        }
    }
    if(address[i] == '.') // Prevents 90.0000 from becoming 90. rather than 90.0
        address[i+1] = '\0';
    else
        address[i] = '\0';
}
void truncateExp(char* address)
{
    int expCount = 0;
    char exponent[9]; //holds the exponential part of the value
    char* ep; // points to place in exponent
    char* p;

    for(p = address; *p != '\0'; p++)
    {
        if( (*p == 'E') || (expCount != 0) )
        {
            exponent[expCount] = *p;
            expCount++;
        }
    }
    exponent[expCount] = '#';

    for(p = address; *p != '\0'; p++)
    {
        if( *p != '0' ) // in case of 0.0000
        {
            p = p+4;
            for(ep = exponent; *ep != '#'; ep++)
            {
                *p = *ep;
                p++;
            }
            *p = '\0';
            break;
        }
    }
}
