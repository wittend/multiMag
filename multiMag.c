//=========================================================================
// multiMag.c
// 
// An interface for the RM3100 3-axis magnetometer from PNI Sensor Corp.
// 
// Author:      David M. Witten II, KD0EAG
// Date:        Jan 30, 2021
// License:     GPL 3.0
//=========================================================================
#include "main.h"
#include "logFiles.h"
#include "config.h"

int gflag = 1;

////------------------------------------------
//// struct pStruct
////------------------------------------------
//struct pStruct
//{
//    int id;                    /* key */
//    char key[MAXKEYLEN];
//    char val[MAXVALLEN];
//    UT_hash_handle hh;         /* makes this structure hashable */
//};

struct pStruct *jsparams = NULL;

//------------------------------------------
// Global variables 
//------------------------------------------
//char version[MAXVERSIONLEN]                    = MULTIMAG_VERSION;
// variables to hold metadata read from files
char cityState[MAXPATHBUFLEN]       = "";
char callSign[MAXPATHBUFLEN]        = "";
char latLonElv[MAXPATHBUFLEN]       = "";
char freqStd[MAXPATHBUFLEN]         = "";
char nodeNum[MAXPATHBUFLEN]         = "";
char sysTem[MAXPATHBUFLEN];
char metadata[MAXPATHBUFLEN];
char dataTyp[MAXPATHBUFLEN];
char rollOverTime[UTCBUFLEN]        = "00:00";
char sitePrefix[SITEPREFIXLEN]      = "SITEPREFIX";
char gridSqr[GRIDSQRLEN]            = "EM38uw";     //"XXyyzz";

char baseFileFolder[FOLDERNAMELEN]  = "/PSWS";
char baseFilePath[MAXPATHBUFLEN]    = "";
char outFilePath[MAXPATHBUFLEN]     = "";
char outFileName[MAXPATHBUFLEN]     = "";
char configFilePath[MAXPATHBUFLEN]  = "";
char configFileName[MAXPATHBUFLEN]  = "config/config.json";
//volatile int alarm_fired;

#define SIGTERM_MSG "\nSIGTERM received.\n"
#define SIGINT_MSG "\nSIGINT received.\n"

//------------------------------------------
// sig_term_handler()
//------------------------------------------
void sig_term_handler(int signum, siginfo_t *info, void *ptr)
{
    //saveConfigToFile(pList *p);
    write(STDERR_FILENO, SIGTERM_MSG, sizeof(SIGTERM_MSG));
    exit(0);
}

//------------------------------------------
// catch_sigterm()
//------------------------------------------
void catch_sigterm()
{
    static struct sigaction _sigact;

    memset(&_sigact, 0, sizeof(_sigact));
    _sigact.sa_sigaction = sig_term_handler;
    _sigact.sa_flags = SA_SIGINFO;

    sigaction(SIGTERM, &_sigact, NULL);
}

//------------------------------------------
// sig_int_handler()
//------------------------------------------
void sig_int_handler(int signum, siginfo_t *info, void *ptr)
{
    write(STDERR_FILENO, SIGINT_MSG, sizeof(SIGINT_MSG));
    exit(0);
}

//------------------------------------------
// catch_sigint()
//------------------------------------------
void catch_sigint()
{
    static struct sigaction _sigact;

    memset(&_sigact, 0, sizeof(_sigact));
    _sigact.sa_sigaction = sig_int_handler;
    _sigact.sa_flags = SA_SIGINFO;

    sigaction(SIGINT, &_sigact, NULL);
}

//------------------------------------------
// i2cReader()
//------------------------------------------
void *i2cReaderThread(void *thread_id)
{
    int *id = (int *) thread_id;
    char utcStr[UTCBUFLEN] = "";
    struct tm *utcTime = getUTC();
    
    //while(gflag == 1)
    //{
    //    usleep(10);                   
    //}
    while(1) 
    {
        struct timeval tv;

        //catch_sigterm();
        usleep(1000 * 1000);                   // 1 second
  //      sched_yield();
  //      clock_nanosleep()
        gettimeofday(&tv, NULL);
        printf("Task %u: S: %lu, uS: %lu\n", *id, tv.tv_sec, tv.tv_usec);
    }
    pthread_exit(NULL);
}

//------------------------------------------
// main()
// 
//------------------------------------------
int main(int argc, char** argv)
{
    pthread_t tids[MAXTHREADS];
    int ids[MAXTHREADS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int i;
    pList p;
    int rv = 0;

    catch_sigterm();
    catch_sigint();
    // Set default parameters
    setupDefaults(&p);

    getCommandLine(argc, argv, &p);
    readConfig(&p);
    buildOutputFilePath(&p);
    buildOutputfileName(&p);

//    openLogs(&p);
    switch(p.modeOutputFlag)
    {
        case 0:
            break;
        default:
            break;
    }
    if(p.printParamFlg)
    {
        printParams(&p);
    }
    for(i = 0; i < p.numThreads; i++) 
    {
        printf("Creating i2cReader thread %u\n", i);
        rv = pthread_create(&tids[i], NULL, i2cReaderThread, &ids[i]);
        if(rv) 
        {
            printf("unable to create thread! \n");
            exit(-1);
        }
        usleep(p.threadOffsetUS);                   // Default 150 uS
    }
    //gflag = 1;
    for(i = 0; i < p.numThreads; i++) 
    {
        pthread_join(tids[i], NULL);
    }
    //printf("Waiting...\n");
    //usleep(1000000);
    //printf("gflag = 0 ...\n");
    //gflag = 0;
    //printf("pthread_exit(NULL)\n");
    pthread_exit(NULL);
    hashDeleteAll();
    return 0;
}
