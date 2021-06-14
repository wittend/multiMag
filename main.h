//=========================================================================
// main.h
// 
// An interface for the RM3100 3-axis magnetometer from PNI Sensor Corp.
// Derived in part from several sources:
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
#ifdef ODROID_N2
    #include <wiringPi.h>
#endif
#ifdef USE_BCM2835
    #include <bcm2835.h>
#endif
#include "rm3100.h"
#include "mcp9808.h"
//#include "logFiles.h"
#include "utilRoutines.h"
#include "uthash/uthash.h"

#define MAXKEYLEN   64
#define MAXVALLEN   64

#define MAXVERSIONLEN       30
#define MULTIMAG_VERSION   "0.0.0"

#define MAXTHREADS          10
#define UTCBUFLEN           64
#define MAXPATHBUFLEN       1500
#define FOLDERNAMELEN       1500
#define SITEPREFIXLEN       32
#define GRIDSQRLEN          7

//------------------------------------------
// struct pStruct
//------------------------------------------
struct pStruct
{
    int id;                    /* key */
    char key[MAXKEYLEN];
    char val[MAXVALLEN];
    UT_hash_handle hh;         /* makes this structure hashable */
};

//------------------------------------------
// types
//------------------------------------------
typedef struct tag_pList
{
    int printParamFlg;
    int numThreads;
    int threadOffsetUS;
    int i2cBusNumber;
    int i2c_fd;
    int modeOutputFlag;
    char *baseFilePath;
    char *outputFilePath;
    char *outputFileName;
    char *gridSqr;
    char *sitePrefix;
    char *logOutputTime;
    struct pStruct **jsParams;
} pList;

//------------------------------------------
// prototypes
//------------------------------------------
int main(int argc, char** argv);
void *i2cReader(void *thread_id);
int getCommandLine(int argc, char** argv, pList *p);
int setupDefaults(pList *p);

#endif  // MULTIMAGMAIN_h
