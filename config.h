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

void printParams();
int readConfigFromFile(pList *p, char *cfgFile);
int saveConfigToFile(pList *p, char *cfgFile);
int readConfig(pList *p);
int configDecode(pList *p, char *key, char *value);
struct pStruct *findKeyInt(int id);
struct pStruct *findKeyStr(char *key);
void hashDeleteAll();
//int configDecode(pList *p, char *key, char *value);
//struct pStruct *findKeyIntPlist(pList *p, int id);
//struct pStruct *findKeyStrPlist(pList *p, char *key);
//void addParamPlist(pList *p, int id, char *key, char *val);

#endif // CONFIGROUTINES_h