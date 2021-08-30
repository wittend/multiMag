//=========================================================================
// sensorRoutines.h
//
// An interface for the RM3100 3-axis magnetometer from PNI Sensor Corp.
// pre-defined calls to request specific data from the PNI rm3100
// magnetometers and the mcp9808 temperature sensors on an I2C bus.
//
// Author:      David M. Witten II, KD0EAG
// Date:        Aug 30, 2021
// License:     GPL 3.0
//=========================================================================
#ifndef MULTIMAG_SENSORROUTINES_h
#define MULTIMAG_SENSORROUTINES_h

#include "mcp9808.h"
#include "rm3100.h"
#include "i2c.h"

#include "main.h"

//------------------------------------------
// Prototypes
//------------------------------------------
int openI2CBus(pList *p);
void closeI2CBus(int i2c_fd);
int setNOSReg(pList *p);
unsigned short setMagSampleRate(pList *p, unsigned short sample_rate);
unsigned short getMagSampleRate(pList *p);;
unsigned short getCCGainEquiv(unsigned short CCVal);
int startCMM(pList *p);
int getMagRev(pList *p);
int setup_mag(pList *p);
int runBIST(pList *p);
int getCMMReg(pList *p);
void setCMMReg(pList *p);
int getTMRCReg(pList *p);
void setTMRCReg(pList *p);
void setCycleCountRegs(pList *p);
void readCycleCountRegs(pList *p);

#endif // MULTIMAG_SENSORROUTINES_h