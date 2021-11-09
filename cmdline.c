//=========================================================================
// cmdline.c
// 
// An interface for the RM3100 3-axis magnetometer from PNI Sensor Corp.
// Derived in part from several sources:
//
// Author:      David M. Witten II, KD0EAG
// Date:        May 30, 2021
// Date:        June 5, 2021
// License:     GPL 3.0
//=========================================================================
#include "cmdline.h"
#include "sensorRoutines.h"

extern char rollOverTime[UTCBUFLEN];
extern char outFilePath[MAXPATHBUFLEN];

//------------------------------------------
// getCommandLine()
// Parse the command line options and setup
// the internal option flags
//------------------------------------------
int getCommandLine(int argc, char** argv, pList *p)
{
    int c;

    while((c = getopt(argc, argv, "?Ghn:PW")) != -1)
    {
        switch (c)
        {
            case 'G':
                p->usePPS = TRUE;
                break;
            case 'n':
                p->numThreads = atoi(optarg);
                if(p->numThreads > MAXTHREADS)
                {
                    fprintf(stdout, "Number of IO threads must be <= %u.", MAXTHREADS);
                    exit(1);
                }
                break;
            case 'P':
                    p->printParamFlg = TRUE;
                    break;
            case 'h':
            case '?':
                showHelp(p);
                return 1;
                break;
            case 'W':
                p->writeWorkingCFG = TRUE;
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
// setOutputFilePath()
//------------------------------------------
void showHelp(pList *p)
{
    fprintf(stdout, "\nParameters:\n\n");
    fprintf(stdout, "   -G                     :  Use PPS signal.                 [ default 2 decimal]\n");
    fprintf(stdout, "   -n <count>             :  Set number of threads.          [ default 2 decimal]\n");
    fprintf(stdout, "   -P                     :  Show configuration parameters.\n");
    fprintf(stdout, "   -W                     :  Write working parameters to ./config/config.json.\n");
    fprintf(stdout, "   -h or -?               :  Display this help.\n\n");
}


//------------------------------------------
// setOutputFilePath()
//------------------------------------------
int setLogRollOver(pList *p, char *rollTime)
{
    int rv = 0;

    strncpy(rollOverTime, rollTime, strlen(rollTime));
    p->logOutputTime = rollOverTime;
    return rv;
}

//------------------------------------------
// setOutputFilePath()
//------------------------------------------
int setOutputFilePath(pList *p, char *outPath)
{
    int rv = 0;
    
    if(strlen(outPath) > MAXPATHBUFLEN - 1)
    {
        fprintf(stderr, "\nOutput path length exceeds maximum allowed length (%i)\n", MAXPATHBUFLEN - 1);
        rv =  1;
    }
    else
    {
        strncpy(outFilePath, outPath, strlen(outPath));
        p->outputFilePath = outFilePath;
    }
    return rv;
}

