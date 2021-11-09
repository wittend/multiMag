//=========================================================================
// config.h
// 
//  Headeer file for multiMag magnetometer support.
//
// Author:      David M. Witten II, KD0EAG
// Date:        Jan 31, 2021
// Date:        June 5, 2021
// Date:        June 25, 2021
// License:     GPL 3.0
//=========================================================================
#ifndef CONFIGROUTINES_h
#define CONFIGROUTINES_h

#include "main.h"

#define JSONBUFLEN          4096
#define JSONBUFTOKENCOUNT   1024

void printParams();
void showSettings(pList *p);
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