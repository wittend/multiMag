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

//extern char cityState[MAXPATHBUFLEN];
//extern char callSign[MAXPATHBUFLEN];
//extern char latLonElv[MAXPATHBUFLEN];
//extern char freqStd[MAXPATHBUFLEN];
//extern char nodeNum[MAXPATHBUFLEN];
//extern char sysTem[MAXPATHBUFLEN];
//extern char metadata[MAXPATHBUFLEN];
//extern char dataTyp[MAXPATHBUFLEN];
extern char rollOverTime[UTCBUFLEN];
//extern char sitePrefix[SITEPREFIXLEN];
//extern char gridSqr[GRIDSQRLEN];     //"XXyyzz";
//
//extern char baseFileFolder[FOLDERNAMELEN];
//extern char baseFilePath[MAXPATHBUFLEN];
extern char outFilePath[MAXPATHBUFLEN];
//extern char outFileName[MAXPATHBUFLEN];
//extern char configFilePath[MAXPATHBUFLEN];
//extern char configFileName[MAXPATHBUFLEN];

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

//------------------------------------------
// showSettings()
//------------------------------------------
void showSettings(pList *p)
{
    char pathStr[128] = "";
    snprintf(pathStr, sizeof(pathStr), "/dev/i2c-%i", p->i2cBusNumber);

//    fprintf(stdout, "\nVersion = %s\n", version);
    fprintf(stdout, "\nCurrent Parameters:\n\n");
//    if(!p->magRevId)
//    {
//        getMagRev(p);
//    }
    fprintf(stdout, "   Magnetometer revision ID detected:          %i (dec)\n",    p->magRevId);
//    fprintf(stdout, "   Log output path:                            %s\n",          p->outFilePath ? "TRUE" : "FALSE");
    fprintf(stdout, "   Log output:                                 %s\n",          p->logOutput ? "TRUE" : "FALSE");
    //fprintf(stdout, "   Log Rollover time:                          %s\n",          p->logOutputTime);
    fprintf(stdout, "   Log site prefix string:                     %s\n",          p->sitePrefix);
    fprintf(stdout, "   Output file path:                           %s\n",          p->outputFilePath);
#if (USE_PIPES)
    fprintf(stdout, "   Log output to pipes:                        %s\n",          p->useOutputPipe ? "TRUE" : "FALSE");
    fprintf(stdout, "   Input file path:                            %s\n",          p->pipeInPath);
    fprintf(stdout, "   Output file path:                           %s\n",          p->pipeOutPath);
#endif
    fprintf(stdout, "   I2C bus number as integer:                  %i (dec)\n",    p->i2cBusNumber);
    fprintf(stdout, "   I2C bus path as string:                     %s\n",          pathStr);
    fprintf(stdout, "   Built in self test (BIST) value:            %02X (hex)\n",  p->doBistMask);
    fprintf(stdout, "   NOS Register value:                         %02X (hex)\n",  p->NOSRegValue);
//    fprintf(stdout, "   Post DRDY delay:                            %i (dec)\n",    p->DRDYdelay);
    fprintf(stdout, "   Device sampling mode:                       %s\n",          p->samplingMode     ? "CONTINUOUS" : "POLL");
    fprintf(stdout, "   Cycle counts by vector:                     X: %3i (dec), Y: %3i (dec), Z: %3i (dec)\n", p->cc_x, p->cc_y, p->cc_z);
    fprintf(stdout, "   Gain by vector:                             X: %3i (dec), Y: %3i (dec), Z: %3i (dec)\n", p->x_gain, p->y_gain, p->z_gain);
//    fprintf(stdout, "   Read back CC Regs after set:                %s\n",          p->readBackCCRegs   ? "TRUE" : "FALSE");
    fprintf(stdout, "   Software Loop Delay (uSec):                 %i (dec uSec)\n",    p->outDelay);
    fprintf(stdout, "   CMM sample rate:                            %2X (hex)\n",   p->CMMSampleRate);
    fprintf(stdout, "   TMRC reg value:                             %2X (hex)\n",   p->TMRCRate);
//    fprintf(stdout, "   Format output as JSON:                      %s\n",          p->jsonFlag         ? "TRUE" : "FALSE");
//    fprintf(stdout, "   Read local temperature only:                %s\n",          p->localTempOnly    ? "TRUE" : "FALSE");
//    fprintf(stdout, "   Read remote temperature only:               %s\n",          p->remoteTempOnly   ? "TRUE" : "FALSE");
//    fprintf(stdout, "   Read magnetometer only:                     %s\n",          p->magnetometerOnly ? "TRUE" : "FALSE");
    fprintf(stdout, "   Local temperature address:                  %02X (hex)\n",  p->localTempAddr);
    fprintf(stdout, "   Remote temperature address:                 %02X (hex)\n",  p->remoteTempAddr);
    fprintf(stdout, "   Magnetometer address:                       %02X {hex)\n",  p->magnetometerAddr);
    fprintf(stdout, "   Show parameters:                            %s\n",          p->showParameters   ? "TRUE" : "FALSE");
//    fprintf(stdout, "   Quiet mode:                                 %s\n",          p->quietFlag        ? "TRUE" : "FALSE");
//    fprintf(stdout, "   Hide raw measurements:                      %s\n",          p->hideRaw          ? "TRUE" : "FALSE");
    fprintf(stdout, "   Return single magnetometer reading:         %s\n",          p->singleRead       ? "TRUE" : "FALSE");
//    fprintf(stdout, "   Magnetometer configuation:                  %s\n",          (p->boardMode == LOCAL) ? "Local standalone" : "Extended with remote");
    fprintf(stdout, "   Timestamp format:                           %s\n",          p->tsMilliseconds   ? "RAW"  : "UTCSTRING");
//    fprintf(stdout, "   Verbose output:                             %s\n",          p->verboseFlag      ? "TRUE" : "FALSE");
    fprintf(stdout, "   Show total field:                           %s\n",          p->showTotal        ? "TRUE" : "FALSE");
    fprintf(stdout, "\n\n");
}

