//=========================================================================
// multiMag.c
// 
// An interface for the RM3100 3-axis magnetometer from PNI Sensor Corp.
// 
// Author:      David M. Witten II, KD0EAG
// Date:        Jan 30, 2021
// Date:        Aug 27, 2021
// License:     GPL 3.0
//=========================================================================
#include "main.h"
#include "logFiles.h"
#include "config.h"

//int gflag = 1;
#define USE_SEMAPHORE   0
#define READ_I2C        0

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
char version[]                      = MULTIMAG_VERSION;
char cityState[MAXPATHBUFLEN]       = "Columbia, MO, USA";
char callSign[MAXPATHBUFLEN]        = "KD0EAG";
char latLonElv[MAXPATHBUFLEN]       = "38.92241,-92.29776,228.90m";
char freqStd[MAXPATHBUFLEN]         = "";
char nodeNum[MAXPATHBUFLEN]         = "";
char sysTem[MAXPATHBUFLEN]          = "";
char metadata[MAXPATHBUFLEN]        = "";
char dataTyp[MAXPATHBUFLEN]         = "";
char rollOverTime[UTCBUFLEN]        = "00:00";
char sitePrefix[SITEPREFIXLEN]      = "SITEPREFIX";
char gridSqr[GRIDSQRLEN]            = "EM38uw";

char baseFileFolder[FOLDERNAMELEN]  = "/PSWS";
char baseFilePath[MAXPATHBUFLEN]    = "/home/dave";
char outFilePath[MAXPATHBUFLEN]     = "";
char outFileName[MAXPATHBUFLEN]     = "";
char configFilePath[MAXPATHBUFLEN]  = "./";
char configFileName[MAXPATHBUFLEN]  = "config/config.json";
//volatile int alarm_fired;

char outputPipeName[MAXPATHBUFLEN] = "/home/web/wsroot/pipein.fifo";
char inputPipeName[MAXPATHBUFLEN] = "/home/web/wsroot/pipeout.fifo";

#define SIGTERM_MSG "\nSIGTERM received.\n"
#define SIGINT_MSG "\nSIGINT received.\n"

#if( USE_SEMAPHORE )
// Semaphore mutex
sem_t mutex;
#endif

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
// setupDefaults()
//------------------------------------------
int setupDefaults(pList *p)
{
    int rv = 0;

    if(p != NULL)
    {
        p->numThreads       =   2;
        p->threadOffsetUS   =   150;
        p->threadCadenceUS  =   500;
        p->i2cBusNumber     =   1;
        p->i2c_fd           =   0;
        p->modeOutputFlag   =   0;
        p->baseFilePath     =   "/PSWS";
        p->outputFilePath   =   "/Srawdata";
        p->outputFileName   =   "";
        p->pipeInPath       =   "/tmp/multiMag_pipeout.fifo";
        p->pipeOutPath      =   "/tmp/multiMag_pipein.fifo";
        p->rollOverTime     =   "00:00";
        p->gridSqr          =   "EM38uw";
        p->sitePrefix       =   "N0000023";
        p->city             =   "Columbia";
        p->state            =   "Missouri";
        p->country          =   "USA";
        p->postalcode       =   "65201";
        p->lattitude        =   "38.92241";
        p->longitude        =   "-92.29776";
        p->elevation        =   "228.90m";
        p->system           =   "";
    }
};

//------------------------------------------
// i2cReader()
//------------------------------------------
void *i2cReaderThread(void *thread_id)
{
    int *id = (int *) thread_id;
    char utcStr[UTCBUFLEN] = "";
    struct tm *utcTime = getUTC();
    struct timeval tv;

    while(1)
    {
        //struct timeval tv;
#if( USE_SEMAPHORE )

        // Wait for Semaphore.
        sem_wait(&mutex);

        usleep(1000 * 1000);                   // 1 second
        gettimeofday(&tv, NULL);
        printf("Task %u: S: %lu, uS: %lu\n", *id, tv.tv_sec, tv.tv_usec);

        // Reset Semaphore.
        sem_post(&mutex);

#else
        //catch_sigterm();
        usleep(1000 * 1000);                   // 1 second
  //      sched_yield();
  //      clock_nanosleep()
        gettimeofday(&tv, NULL);
        printf("Task %u: S: %lu, uS: %lu\n", *id, tv.tv_sec, tv.tv_usec);
#endif
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

    // Get default runtime parameters .
    setupDefaults(&p);

    // Get cunfiguration file overrides of runtime parameters .
    readConfig(&p);

    // Get command line overrides of runtime parameters .
    getCommandLine(argc, argv, &p);
    buildOutputFilePath(&p);
    buildOutputfileName(&p);

//    openLogs(&p);
    openUIPipes(&p);
    switch(p.modeOutputFlag)
    {
        case 0:
            break;
        default:
            break;
    }
    // Display running parameters.
    if(p.printParamFlg)
    {
        printParams(&p);
    }

#if( USE_SEMAPHORE )
    // Create Semaphore.
    sem_init(&mutex, 0, 1);
#endif

    // Create Threads.
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

    // 'Join' Threads.
    for(i = 0; i < p.numThreads; i++)
    {
        pthread_join(tids[i], NULL);
    }

#if( USE_SEMAPHORE )

    int id = p.numThreads;
    while(1)
    {
        if(id > 0)
        {
            sem_post(&mutex);
            i--;
            if (i < 1)
            {
                i = p.numThreads;
            }
        }
        usleep(p.threadCadenceUS);
   }

#endif

#if( USE_SEMAPHORE )

    // Destroy Semaphore
    sem_destroy(&mutex);
    printf("Semaphore destroyed. Shutting down...\n");

#endif

    pthread_exit(NULL);
    hashDeleteAll();
    closeLogs(&p);
    closeUIPipes(&p);
    return 0;
}
