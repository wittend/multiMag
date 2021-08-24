//=========================================================================
// cmdline.h
// 
// header file for commandline / configuration management for runMag utility.
// 
// Author:      David Witten, KD0EAG
// Date:        May 30, 2021
// License:     GPL 3.0
//=========================================================================
#ifndef MULTIMAG_CMDLINE_h
#define MULTIMAG_CMDLINE_h

#include "main.h"

//------------------------------------------
// Prototypes
//------------------------------------------
//int setupDefaults(pList *p);
int setLogRollOver(pList *p, char *rollTime);
int getCommandLine(int argc, char** argv, pList *p);


#endif // MULTIMAG_CMDLINE_h