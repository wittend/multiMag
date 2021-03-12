//=========================================================================
// multiMag.c
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
#include "main.h"
#include "logFiles.h"
#include "config.h"

int gflag = 1;

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
// getCommandLine()
// Parse the command line options and setup
// the internal option flags
//------------------------------------------
int getCommandLine(int argc, char** argv, pList *p)
{
    int c;

    while((c = getopt(argc, argv, "?hn:")) != -1)
    {
        //int this_option_optind = optind ? optind : 1;
        switch (c)
        {
            case 'n':
                p->numThreads = atoi(optarg);
                if(p->numThreads > MAXTHREADS)
                {
                    fprintf(stdout, "Number of IO threads must be <= %u.", MAXTHREADS);
                    exit(1);
                }
                break;
            case 'h':
            case '?':
                // fprintf(stdout, "\n%s Version = %s\n", argv[0], version);
                fprintf(stdout, "\nParameters:\n\n");
                fprintf(stdout, "   -n <count>             :  Set number of threads.          [ default 2 decimal]\n");
                fprintf(stdout, "   -h or -?               :  Display this help.\n\n");
                return 1;
                break;
            default:
                fprintf(stdout, "\n?? getopt returned character code 0x%2X ??\n", c);
                break;
        }
    }
    if(optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
        {
            printf ("%s ", argv[optind++]);
        }
        printf ("\n");
    }
    return 0;
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
    printParams();
//exit(0);
    buildOutputFilePath(&p);
//exit(0);
    buildOutputfileName(&p);
//exit(0);

//    openLogs(&p);

    switch(p.modeOutputFlag)
    {
        case 0:
            break;
        default:
            break;
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
