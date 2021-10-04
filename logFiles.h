//=========================================================================
// logFiles.h
// 
//  Log and data files for multiMag magnetometer utility.
//
// Author:      David M. Witten II, KD0EAG
// Date:        May 30, 2021
// License:     GPL 3.0
//=========================================================================
#ifndef LOGFILES_h
#define LOGFILES_h

#include "main.h"

char *logHeader(int argc, char **argv, pList *p);
int readConfig(pList *p);
int openLogs(pList *p);
void closeLogs(pList *p);
int openUIPipes(pList *p);
void closeUIPipes(pList *p);
void buildOutputFileName(pList *p);
int buildOutputFilePath(pList *p);
int setOutputFileRoot(pList *p, char *outPath);

#endif  // LOGFILES_h
