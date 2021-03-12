//=========================================================================
// config.h
// 
//  Headeer file for multiMag magnetometer support.
//
// Author:      David M. Witten II, KD0EAG
// Date:        Jan 31, 2021
// License:     GPL 3.0
//=========================================================================
#ifndef CONFIGROUTINES_h
#define CONFIGROUTINES_h

#include "main.h"
//#include "uthash/uthash.h"
//#include "jsmn/jsmn.h"

#define JSONBUFLEN          1025
#define JSONBUFTOKENCOUNT   1024

int readConfigFromFile(pList *p, char *cfgFile);
int saveConfigToFile(pList *p, char *cfgFile);
int readConfig(pList *p);
int configDecode(pList *p, char *key, char *value);
void printParams();
struct pStruct *findKeyInt(int id);
struct pStruct *findKeyStr(char *key);
void hashDeleteAll();

#endif // CONFIGROUTINES_h