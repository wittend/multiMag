//=========================================================================
// main.h
// 
// An interface for the RM3100 3-axis magnetometer from PNI Sensor Corp.
// Derived in part from several sources:
//
//    PSWS_FileNameRequirementsV0_13.pdf
//      2020-07-15 UTC
//      Author: J C Gibbons
// 
// Author:      David M. Witten II, KD0EAG
// Date:        Jan 30, 2021
// License:     GPL 3.0
//=========================================================================
#ifndef MULTIMAGMAIN_h
#define MULTIMAGMAIN_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <pthread.h>

#define MAXTHREADS          10
#define UTCBUFLEN           64
#define MAXPATHBUFLEN       2048
#define SITEPREFIXLEN       32
#define GRIDSQRLEN          7

//------------------------------------------
// types
//------------------------------------------
typedef struct tag_pList
{
    int i2cBusNumber;
    int i2c_fd;
    int numThreads;
    char *outputFilePath;
    char *outputFileName;
    char *gridSqr;
    char *sitePrefix;
} pList;

//------------------------------------------
// prototypes
//------------------------------------------
int main(int argc, char** argv);
void *i2cReader(void *thread_id);
int getCommandLine(int argc, char** argv, pList *p);
long currentTimeMillis();
struct tm *getUTC();


#endif  // MULTIMAGMAIN_h
