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

extern char cityState[MAXPATHBUFLEN];
extern char callSign[MAXPATHBUFLEN];
extern char latLonElv[MAXPATHBUFLEN];
extern char freqStd[MAXPATHBUFLEN];
extern char nodeNum[MAXPATHBUFLEN];
extern char sysTem[MAXPATHBUFLEN];
extern char metadata[MAXPATHBUFLEN];
extern char dataTyp[MAXPATHBUFLEN];
extern char rollOverTime[UTCBUFLEN];
extern char sitePrefix[SITEPREFIXLEN];
extern char gridSqr[GRIDSQRLEN];     //"XXyyzz";

extern char baseFileFolder[FOLDERNAMELEN];
extern char baseFilePath[MAXPATHBUFLEN];
extern char outFilePath[MAXPATHBUFLEN];
extern char outFileName[MAXPATHBUFLEN];
extern char configFilePath[MAXPATHBUFLEN];
extern char configFileName[MAXPATHBUFLEN];

//------------------------------------------
// getCommandLine()
// Parse the command line options and setup
// the internal option flags
//------------------------------------------
int getCommandLine(int argc, char** argv, pList *p)
{
    int c;

    while((c = getopt(argc, argv, "?hn:P")) != -1)
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
            case 'P':
                    p->printParamFlg = TRUE;
                    break;
            case 'h':
            case '?':
                // fprintf(stdout, "\n%s Version = %s\n", argv[0], version);
                fprintf(stdout, "\nParameters:\n\n");
                fprintf(stdout, "   -n <count>             :  Set number of threads.          [ default 2 decimal]\n");
                fprintf(stdout, "   -P                     :  Show configuration parameters.\n\n");
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
