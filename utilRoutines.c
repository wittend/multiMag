//=========================================================================
// utilRoutines.c
// 
//  Utility routines for multiMag magnetometer support.
//
// Author:      David M. Witten II, KD0EAG
// Date:        Jan 31, 2021
// License:     GPL 3.0
//=========================================================================
#include "utilRoutines.h"

extern char outFilePath[MAXPATHBUFLEN];
extern char outFileName[MAXPATHBUFLEN];

//------------------------------------------
// currentTimeMillis()
//------------------------------------------
long currentTimeMillis()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec * 1000 + time.tv_usec / 1000;
}

//------------------------------------------
// getUTC()
//------------------------------------------
struct tm *getUTC()
{
    time_t now = time(&now);
    if(now == -1)
    {
        puts("The time() function failed");
    }
    struct tm *ptm = gmtime(&now);
    if(ptm == NULL)
    {
        puts("The gmtime() function failed");
    }    
    return ptm;
}

//------------------------------------------
// mkdir_p()
// 
// Recursively create path.
//     return -1 if fail.
//------------------------------------------
int mkdir_p(const char *path)
{
    const size_t len = strlen(path);
    char _path[MAXPATHBUFLEN];
    char *p; 

    errno = 0;

    /* Copy string so its mutable */
    if (len > sizeof(_path)-1)
    {
        errno = ENAMETOOLONG;
        return -1; 
    }   
    strcpy(_path, path);
    /* Iterate the string */
    for (p = _path + 1; *p; p++)
    {
        if (*p == '/')
        {
            /* Temporarily truncate */
            *p = '\0';
            if (mkdir(_path, S_IRWXU) != 0)
            {
                if (errno != EEXIST)
                {
                    return -1; 
                }
            }
            *p = '/';
        }
    }   
    if (mkdir(_path, S_IRWXU) != 0)
    {
        if (errno != EEXIST)
        {
            return -1; 
        }
//printf("Create path: [%s]\n", _path);    
    }   
    return 0;
}

