//=========================================================================
// utilRoutines.h
// 
//  Headeer file for multiMag magnetometer support.
//
// Author:      David M. Witten II, KD0EAG
// Date:        Jan 31, 2021
// License:     GPL 3.0
//=========================================================================
#ifndef UTILROUTINES_h
#define UTILROUTINES_h

#include "main.h"

int mkdir_p(const char *path);
long currentTimeMillis();
struct tm *getUTC();

#endif  // UTILROUTINES_h