//=========================================================================
// logFiles.c
// 
//  Log and data files for multiMag magnetometer utility.
//
// Author:      David M. Witten II, KD0EAG
// Date:        May 3, 2021
// License:     GPL 3.0
//=========================================================================
#include "logFiles.h"

//extern char version[MAXVERSIONLEN];

extern char cityState[MAXPATHBUFLEN];
extern char callSign[MAXPATHBUFLEN];
extern char latLonElv[MAXPATHBUFLEN];
extern char freqStd[MAXPATHBUFLEN];
extern char nodeNum[MAXPATHBUFLEN];
extern char sysTem[MAXPATHBUFLEN];
extern char metadata[MAXPATHBUFLEN];
extern char dataTyp[MAXPATHBUFLEN];
extern char gridSqr[GRIDSQRLEN];
extern char rollOverTime[UTCBUFLEN];

extern char sitePrefix[SITEPREFIXLEN];

extern char baseFileFolder[FOLDERNAMELEN];
extern char baseFilePath[MAXPATHBUFLEN];
extern char outFilePath[MAXPATHBUFLEN];
extern char outFileName[MAXPATHBUFLEN];

const char *fdata[][3] =
{
    {"/PSWS/Srawdata/", "CityState.txt", ""},
    {"/PSWS/Sinfo/",    "CallSign.txt",  ""},
    {"/PSWS/Sstat/",    "LatLonElv.txt", ""},
    {"/PSWS/Scmd/",     "FreqStd.txt",   ""},
    {"/PSWS/Scode/",    "NodeNum.txt",   ""},
    {"/PSWS/Stemp/",    "GridSqr.txt",   ""},
    {"/PSWS/Sdata/",    "System.txt",    ""},
    {"/PSWS/Sdata/",    "Metadata.txt",  ""},
    {"/PSWS/Splot/",    "",              ""},
    {NULL,              "",              ""}
};

//------------------------------------------
// logHeader()
// show settings in use (-P)
//------------------------------------------
char *logHeader(int argc, char **argv, pList *p)
{
    char outLine[1024] = "";
    char outStr[4096] = "";
    //char pathStr[128] = "";
    snprintf(outFileName, sizeof(outFileName), "/dev/i2c-%i", p->i2cBusNumber);
    
    strncat(outStr, "\n", strlen("\n")+1);
    snprintf(outLine, 1023, "#=================================================================\n");
    strncat(outStr, outLine, strlen(outLine));
    snprintf(outLine, 1023, "#\n# Current Parameters:\n");
    strncat(outStr, outLine, strlen(outLine));
    snprintf(outLine, 1023, "# \n");
    strncat(outStr, outLine, strlen(outLine));
    snprintf(outLine, 1023, "#   Command line:                     ");
    strncat(outStr, outLine, strlen(outLine));
    snprintf(outLine, 1023, " %s ",  argv[0]); 
    strncat(outStr,  outLine, strlen(outLine));
    for(int i = 1; i < argc; i++)
    {
        snprintf(outLine,1023, " %s", argv[i]); 
        strncat(outStr, outLine, strlen(outLine));
    }
    snprintf(outLine, 1023, "\n");
    //if(!p->magRevId)
    //{
    //    getMagRev(p);
    //}
    snprintf(outLine, 1023, "\n#   Software Version:                  %s\n",        MULTIMAG_VERSION);
    strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Magnetometer rev. ID detected:     %i (dec)\n",    p->magRevId); 
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   I2C bus number as integer:         %i (dec)\n",    p->i2cBusNumber);
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   I2C bus path as string:            %s\n",          pathStr);
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Log output format:                 %s\n",          getOutputMode(p));
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Log file path:                     %s\n",          p->outputFilePath);
    //strncat(outStr, outLine, strlen(outLine));
    ////snprintf(outLine, 1023, "#   Log output path:                   %s\n",          p->buildLogPath ? "TRUE" : "FALSE" );
    ////strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Write Log output:                  %s\n",          p->logOutput ? "TRUE" : "FALSE" );
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Site prefix/NodID string:          %s\n",          p->sitePrefix);
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Timestamp format:                  %s\n",          p->tsMilliseconds   ? "RAW"  : "UTCSTRING");
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Show total field:                  %s\n",          p->showTotal        ? "TRUE" : "FALSE");
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Local temperature address:         %02X (hex)\n",  p->localTempAddr);
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Remote temperature address:        %02X (hex)\n",  p->remoteTempAddr);
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Magnetometer address:              %02X {hex)\n",  p->magnetometerAddr);
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   NOS Register value:                %02X (hex)\n",  p->NOSRegValue);
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Post DRDY delay:                   %i (dec)\n",    p->DRDYdelay);
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Device sampling mode:              %s\n",          p->samplingMode     ? "CONTINUOUS" : "POLL");
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Cycle counts by vector:            X: %3i (dec), Y: %3i (dec), Z: %3i (dec)\n", p->cc_x, p->cc_y, p->cc_z);
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Gain by vector:                    X: %3i (dec), Y: %3i (dec), Z: %3i (dec)\n", p->x_gain, p->y_gain, p->z_gain);
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Read back CC Regs after set:       %s\n",          p->readBackCCRegs   ? "TRUE" : "FALSE" );
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Software Loop Delay (uSec):        %i (dec)\n",    p->outDelay);
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   CMM sample rate:                   %2X (hex)\n",   p->CMMSampleRate);
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   TMRC reg value:                    %2X (hex)\n",   p->TMRCRate);
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Read local temperature only:       %s\n",          p->localTempOnly    ? "TRUE" : "FALSE");
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Read remote temperature only:      %s\n",          p->remoteTempOnly   ? "TRUE" : "FALSE");
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Read magnetometer only:            %s\n",          p->magnetometerOnly ? "TRUE" : "FALSE");
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Show parameters:                   %s\n",          p->showParameters   ? "TRUE" : "FALSE" );
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Hide raw measurements:             %s\n",          p->hideRaw          ? "TRUE" : "FALSE" );
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Return single reading:             %s\n",          p->singleRead       ? "TRUE" : "FALSE");
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#   Axis Translation:                  X -> %s\n",     (p->xTranslate == 0) ? "X"    : ((p->xTranslate == 1) ? "Y" : "Z"));
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#                                      Y -> %s\n",     (p->yTranslate == 0) ? "X"    : ((p->yTranslate == 1) ? "Y" : "Z"));
    //strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#                                      Z -> %s\n",     (p->zTranslate == 0) ? "X"    : ((p->zTranslate == 1) ? "Y" : "Z"));
    //strncat(outStr, outLine, strlen(outLine));
    ////strncat(outStr, outLine, strlen(outLine));
    //snprintf(outLine, 1023, "#\n#=================================================================\n\n");
    //strncat(outStr, outLine, strlen(outLine));
    return outFileName;
}

//------------------------------------------
// openLogs()
//------------------------------------------
int openLogs(pList *p)
{
    int rv = 0;

    if(p->outfp == 0)
    {
        buildOutputFileName(p);
        if((p->outfp = fopen(p->outputFilePath, "a+"))!= NULL)
        {
            printf("Log File: %s\n", p->outputFilePath);
        }
        else
        {
            perror("Log File: ");
        }
        rv = 0;
    }
    return rv;
}

//------------------------------------------
// openLogs()
//------------------------------------------
void closeLogs(pList *p)
{
    if(p->outfp != stdout)
    {
        fclose(p->outfp);
    }
}

//------------------------------------------
//  buildOutputFileName()
//------------------------------------------
void buildOutputFileName(pList *p)
{
    struct tm *utcTime = getUTC();
    char utcStr[UTCBUFLEN] = "";

    char *penv = getenv("HOME");
    strncpy(baseFilePath, "", 1);
    strncpy(baseFilePath, penv, MAXPATHBUFLEN - 1);
    strncat(baseFilePath, baseFileFolder, FOLDERNAMELEN - 1);

    strftime(utcStr, UTCBUFLEN, "%Y-%m-%dT%H%M%S", utcTime);
    strncpy((char *)outFileName, utcStr, UTCBUFLEN);
    strncat((char *)outFileName, "Z_", 3);
    strncat((char *)outFileName, p->sitePrefix, SITEPREFIXLEN);
    strncat((char *)outFileName, "_N_", 4);
    strncat((char *)outFileName, p->gridSqr, 7);
    strncat((char *)outFileName, "_MGT.log", 9);

    strncat((char *)baseFilePath, "/", 2);
    strncat((char *)baseFilePath, outFileName, MAXPATHBUFLEN);

    p->outputFileName = outFileName;

    p->outputFilePath = baseFilePath;
}

//------------------------------------------
//  buildGrapeOutputFileName()
//------------------------------------------
void buildGrapeOutputFileName(pList *p)
{
    struct tm *utcTime = getUTC();
    char utcStr[UTCBUFLEN] = "";

    char *penv = getenv("HOME");
    strncpy(baseFilePath, "", 1);
    strncpy(baseFilePath, penv, MAXPATHBUFLEN - 1);
    strncat(baseFilePath, baseFileFolder, FOLDERNAMELEN - 1);

    strftime(utcStr, UTCBUFLEN, "%Y-%m-%dT%H%M%S", utcTime);
    strncpy((char *)outFileName, utcStr, UTCBUFLEN);
    strncat((char *)outFileName, "Z_", 3);
    strncat((char *)outFileName, p->sitePrefix, SITEPREFIXLEN);
    strncat((char *)outFileName, "_N_", 4);
    strncat((char *)outFileName, p->gridSqr, 7);
    strncat((char *)outFileName, "_MGT.log", 9);

    strncat((char *)baseFilePath, "/", 2);
    strncat((char *)baseFilePath, outFileName, MAXPATHBUFLEN);

    p->outputFileName = outFileName;

    p->outputFilePath = baseFilePath;
}

////------------------------------------------
////  buildOutputFilePath()
////------------------------------------------
//int buildOutputFilePath(pList *p)
//{
//    int rv = 0;
//    char *penv = getenv("HOME");
//    strncpy(baseFilePath, "", 1);
//    strncpy(baseFilePath, penv, MAXPATHBUFLEN - 1);
//    strncat(baseFilePath, baseFileFolder, FOLDERNAMELEN - 1);
//    p->baseFilePath = baseFilePath;
//fprintf(stdout, "buildOutputFilePath(): [%s]\n", p->baseFilePath);
//fflush(stdout);
//    return rv;
//}
//
////------------------------------------------
//// setOutputFileRoot()
////------------------------------------------
//int setOutputFileRoot(pList *p, char *outPath)
//{
//    int rv = 0;
//
//    if(strlen(outPath) > MAXPATHBUFLEN - 1)
//    {
//        fprintf(stderr, "\nOutput path length exceeds maximum allowed length (%i)\n", MAXPATHBUFLEN - 1);
//        rv =  1;
//    }
//    else
//    {
//        strncpy(outFilePath, outPath, strlen(outPath));
//        p->outputFilePath = outFilePath;
//    }
//    return rv;
//}

//------------------------------------------
// openUIPipes()
//------------------------------------------
int openUIPipes(pList *p)
{
    int rv = 0;
    if(p->useOutputPipe = TRUE)
    {
        // Note that fdPipeOut and fdPipeIn are intentionally reversed.
        if(!(p->fdPipeOut = open(p->pipeInPath, O_WRONLY | O_CREAT)))
        {
            perror("Open PIPE Out failed: ");
            fprintf(stderr, "%s\n", p->pipeInPath);
            exit(1);
        }
        if(!(p->fdPipeIn = open(p->pipeOutPath, O_RDONLY | O_CREAT)))
        {
            perror("Open PIPE In failed: ");
            fprintf(stderr, "%s\n", p->pipeOutPath);
            exit(1);
        }
    }
    return rv;
}

//------------------------------------------
// closeUIPipes()
//------------------------------------------
void closeUIPipes(pList *p)
{
    // Note that fdPipeOut and fdPipeIn are intentionally reversed.
    if(p->fdPipeOut != 0)
    {
        close(p->fdPipeOut);
        p->fdPipeOut = 0;
    }
    if(p->fdPipeIn != 0)
    {
        close(p->fdPipeIn);
        p->fdPipeIn = 0;
    }
}
