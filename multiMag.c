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
#include "cmdline.h"
#include "sensorRoutines.h"

//int gflag = 1;
// #define _DEBUG0         1
// #define READ_I2C        0

// Global struct for parsing JSON
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

char baseFileFolder[FOLDERNAMELEN]  = "/PSWS/Srawdata";
char baseFilePath[MAXPATHBUFLEN]    = "/home/dave";
char outFilePath[MAXPATHBUFLEN]     = "";
char outFileName[MAXPATHBUFLEN]     = "";
char configFilePath[MAXPATHBUFLEN]  = "./";
char configFileName[MAXPATHBUFLEN]  = "config/config.json";
//volatile int alarm_fired;

char outputPipeName[MAXPATHBUFLEN] = "/tmp/PSWS-pipein.fifo";
char inputPipeName[MAXPATHBUFLEN]  = "/tmp/PSWS-pipeout.fifo";

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

//    closeI2CBus(&p);

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

    //closeI2CBus(&p);

    sigaction(SIGINT, &_sigact, NULL);
}


//------------------------------------------
// setupDefaults()
// All Default values should be set here.
//------------------------------------------
int setupDefaults(pList *p)
{
    int rv = 0;

    if(p != NULL)
    {
        // from runMag - investigate
        p->magnetometerAddr =   0x20;
        p->localTempAddr    =   0x18;
        p->remoteTempAddr   =   0x19;

        p->magRevId         =   0;
        p->doBistMask       =   0;              // ?
        p->outDelay         =   0;
        p->singleRead       =   FALSE;          // Is this useful?
        p->tsMilliseconds   =   FALSE;          // ?
        p->showTotal        =   FALSE;          // ?
        p->logOutput        =   FALSE;
        p->useOutputPipe    =   FALSE;
        p->usePPS           =   FALSE;
        p->writeWorkingCFG  =   FALSE;
        p->TMRCRate         =   0;
        p->CMMSampleRate    =   0;
        p->samplingMode     =   0;
        p->NOSRegValue      =   0;
        p->cc_x             =   0;
        p->cc_y             =   0;
        p->cc_z             =   0;

        p->x_gain           =   0;
        p->y_gain           =   0;
        p->z_gain           =   0;

        // multiMag from here on...
        p->Version          =   MULTIMAG_VERSION;
        p->printParamFlg    =   FALSE;
        p->numThreads       =   1;
        p->threadCadenceUS  =   1000000;
        p->threadOffsetUS   =   150000;
        p->i2cBusNumber     =   1;
        p->useI2CMUX        =   FALSE;
        p->i2cMUXAddr       =   FALSE;
        p->i2c_fd           =   0;
        p->outfp            =   NULL;
        p->fdPipeIn         =   NOT_USED;
        p->fdPipeOut        =   NOT_USED;
        p->modeOutputFlag   =   0;

        p->logOutputTime    =   "00:00:00";
        p->baseFilePath     =   "/PSWS/Srawdata/";
        p->outputFilePath   =   "";                         //"/Srawdata";
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

#if( USE_SEMAPHORE )
// Semaphore mutex
sem_t mutexp[MAXTHREADS] = {};
#endif

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
        sem_wait(&mutexp[*id]);

        usleep(1000 * 1000);                  // 1 second
        gettimeofday(&tv, NULL);
        printf("Task %u: S: %lu, uS: %lu\n", *id, tv.tv_sec, tv.tv_usec);

        // Reset Semaphore.
        sem_post(&mutexp[*id]);

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
    int idd;
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
    // Open files for logging.
    openLogs(&p);
    // Open pipes for realtime presentation.
    openUIPipes(&p);

    // Write out configuration to ./config/config.json
    if(p.writeWorkingCFG)
    {
        strncat(configFilePath, configFileName, MAXPATHBUFLEN);
        saveConfigToFile(&p, configFilePath);
    }

    // Display running parameters.
    if(p.printParamFlg)
    {
        fprintf(stdout, "#==============================================================\n\n");
        printParams(&p);
        showSettings(&p);
        fprintf(stdout, "#==============================================================\n\n");
    }

    // Open up I2C
    openI2CBus(&p);

    // Create Threads.
    for(i = 0; i < p.numThreads; i++)
    {
#if( USE_SEMAPHORE )
        // Create Semaphore.
        sem_init(&mutexp[i], 0, 1);
#endif
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

    idd = p.numThreads;
    while(1)
    {
        if(idd > 0)
        {
            printf("Posting flag for semaphore %u\n", idd);
            sem_post(&mutexp[idd]);
            i--;
            if (idd < 1)
            {
                idd = p.numThreads;
            }
        }
        usleep(p.threadCadenceUS);
   }

#endif

#if( USE_SEMAPHORE )

    // Destroy Semaphore
    idd = p.numThreads;
    while(idd)
    {
        sem_destroy(&mutexp[idd]);
        idd--;
    }
    printf("Semaphore destroyed. Shutting down...\n");

#endif

    pthread_exit(NULL);
    hashDeleteAll();
    closeLogs(&p);
    closeUIPipes(&p);
    closeI2CBus(&p);
    return 0;
}
