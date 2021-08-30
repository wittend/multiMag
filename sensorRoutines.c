//=========================================================================
// sensorRoutines.c
//
// An interface for the RM3100 3-axis magnetometer from PNI Sensor Corp.
// pre-defined calls to request specific data from the PNI rm3100
// magnetometers and the mcp9808 temperature sensors on an I2C bus.
//
// Author:      David M. Witten II, KD0EAG
// Date:        Aug 30, 2021
// License:     GPL 3.0
//=========================================================================
#include "main.h"
#include "sensorRoutines.h"

//------------------------------------------
// openI2CBus()
//------------------------------------------
int openI2CBus(pList *p)
{
    p->i2c_fd = -1;

    char pathStr[64] = "";
    snprintf(pathStr, sizeof(pathStr), "/dev/i2c-%i", p->i2cBusNumber);

    if((p->i2c_fd = open(pathStr, O_RDWR)) < 0)
    {
        perror("Bus open failed\n");
        return -1;
    }
//    else
//    {
//        if(p->verboseFlag)
//        {
//            fprintf(stdout, "Device handle p->i2c_fd:  %d\n", p->i2c_fd);
//            fprintf(stdout, "i2c_init OK!\n");
//            fflush(stdout);
//        }
//    }
    return p->i2c_fd;
}

//--------------------------------------------------------------------
// closeI2CBus()
//
// Close I2C bus
//--------------------------------------------------------------------
void closeI2CBus(int i2c_fd)
{
    close(i2c_fd);
}

//------------------------------------------
// setNOSReg()
//------------------------------------------
int setNOSReg(pList *p)
{
    int rv;
    printf("\nIn setNOSReg():: Setting undocumented NOS register to value: %2X\n", p->NOSRegValue);
    rv = i2c_write(p->i2c_fd, RM3100I2C_NOS, p->NOSRegValue);
    return rv;
}

//------------------------------------------
// setMagSampleRate()
//------------------------------------------
unsigned short setMagSampleRate(pList *p, unsigned short sample_rate)
{
    int i;
    const unsigned short int supported_rates[][2] =
    {
        /* [Hz], register value */
        {   2,  0x0A},   // up to 2Hz
        {   4,  0x09},   // up to 4Hz
        {   8,  0x08},   // up to 8Hz
        {  16,  0x07},   // up to 16Hz
        {  31,  0x06},   // up to 31Hz
        {  62,  0x05},   // up to 62Hz
        { 125,  0x04},   // up to 125Hz
        { 220,  0x03}    // up to 250Hz
    };
//printf("before for loop. I = %i\n", sizeof(supported_rates)/(sizeof(unsigned short int) * 2) - 1);
    for(i = 0; i < sizeof(supported_rates)/(sizeof(unsigned short int) * 2) - 1; i++)
    {
        if(sample_rate <= supported_rates[i][0])
        {
//printf("bbreaking from for loop\n");
            break;
        }
    }
//printf("after for loop\n");
    p->CMMSampleRate = supported_rates[i][0];
    // i2c_write(p->i2c_fd, RM3100I2C_TMRC, p->CMMSampleRate);
    return p->CMMSampleRate;
}

//------------------------------------------
// getMagSampleRate();
// The actual sample rate of the sensor.
//------------------------------------------
unsigned short getMagSampleRate(pList *p)
{
    return p->CMMSampleRate;
}

//------------------------------------------
// getMagRev(pList *p)
//------------------------------------------
int getMagRev(pList *p)
{
    // Set address of the RM3100
    i2c_setAddress(p->i2c_fd,  p->magnetometerAddr);

    // Check Version
    if((p->magRevId = i2c_read(p->i2c_fd, RM3100I2C_REVID)) != (uint8_t)RM3100_VER_EXPECTED)
    {
        // Fail, exit...
        fprintf(stderr, "\nRM3100 REVID NOT CORRECT: ");
        fprintf(stderr, "RM3100 REVID: 0x%X <> EXPECTED: 0x%X.\n\n", p->magRevId, RM3100_VER_EXPECTED);
        fflush(stdout);
        return 0;
    }
//    else
//    {
//        if(p->verboseFlag)
//        {
//             fprintf(stdout,"RM3100 Detected Properly: ");
//             fprintf(stdout,"REVID: %x.\n", p->magRevId);
//        }
//    }
    return p->magRevId;
}

//------------------------------------------
// setup_mag()
//------------------------------------------
int setup_mag(pList *p)
{
    int rv = SensorOK;

    // Set address of the RM3100
    i2c_setAddress(p->i2c_fd,  p->magnetometerAddr);
    // Check Version
    if(!getMagRev(p))
    {
        exit (1);
    }
    // Setup the NOS register
    // setNOSReg(p);
    // Clear out these registers
    i2c_write(p->i2c_fd, RM3100_MAG_POLL, 0);
    i2c_write(p->i2c_fd, RM3100I2C_CMM,  0);
    // Initialize CC settings
    setCycleCountRegs(p);
    // Sleep for 1 second
    usleep(100000);                           // delay to help monitor DRDY pin on eval board
    return rv;
}

//------------------------------------------
// runBIST()
// Runs the Built In Self Test.
//------------------------------------------
int runBIST(pList *p)
{
    return 0;
    //return i2c_read(p->i2c_fd, RM3100I2C_TMRC);
}

//------------------------------------------
// startCMM()
// Starts Continuous Measurement Mode
//------------------------------------------
int startCMM(pList *p)
{
    int rv = 0;
    short cmmMode = (CMMMODE_ALL);   // 71 d
    // Start CMM on X, Y, Z
    rv = i2c_write(p->i2c_fd, RM3100I2C_CMM, cmmMode);
    return rv;
}

//------------------------------------------
// setMagSampleRate()
//------------------------------------------
unsigned short getCCGainEquiv(unsigned short CCVal)
{
    int i = 0;
    unsigned short gain = 0;
    const unsigned short int cc_values[][2] =
    {
        /* [Hz], register value */
        {   CC_50,  GAIN_20},   // up to 2Hz
        {  CC_100,  GAIN_38},   // up to 4Hz
        {  CC_200,  GAIN_75},   // up to 8Hz
        {  CC_300, GAIN_113},   // up to 16Hz
        {  CC_400, GAIN_150},   // up to 31Hz
        {  CC_800, GAIN_300}    // up to ? Hz
    };
    // for(i = 0; i < sizeof(cc_values)/(sizeof(unsigned short int) * 2) - 1; i++)
    for(i = 0; i < sizeof(cc_values)/(sizeof(unsigned short int) * 2); i++)
    {
        // printf("Testing (%i <= cc_values[ %i][0])\n", CCVal, i);
        if(CCVal <= cc_values[i][0])
        {
            // printf ("Got it!\n");
            gain = cc_values[i][1];
            break;
        }
    }
    return gain;
}

//------------------------------------------
// setCycleCountRegs()
//------------------------------------------
void setCycleCountRegs(pList *p)
{
    //int i = 0;
    i2c_write(p->i2c_fd, RM3100I2C_CCX_1, (p->cc_x >> 8));
    i2c_write(p->i2c_fd, RM3100I2C_CCX_0, (p->cc_x & 0xff));
    p->x_gain = getCCGainEquiv(p->cc_x);
    i2c_write(p->i2c_fd, RM3100I2C_CCY_1, (p->cc_y >> 8));
    i2c_write(p->i2c_fd, RM3100I2C_CCY_0, (p->cc_y & 0xff));
    p->y_gain = getCCGainEquiv(p->cc_y);
    i2c_write(p->i2c_fd, RM3100I2C_CCZ_1, (p->cc_y >> 8));
    i2c_write(p->i2c_fd, RM3100I2C_CCZ_0, (p->cc_y & 0xff));
    p->z_gain = getCCGainEquiv(p->cc_z);
    // Write NOSRegValue to  register 0A
    i2c_write(p->i2c_fd, RM3100I2C_NOS,   (uint8_t)(p->NOSRegValue));
//    if(p->verboseFlag)
//    {
//        fprintf(stderr, "\nIn setCycleCountRegs():: Setting NOS register to value: %2X\n", p->NOSRegValue);
//        fprintf(stderr, "CycleCounts  - X: %u, Y: %u, Z: %u.\n", p->cc_x, p->cc_y, p->cc_x);
//        fprintf(stderr, "Gains        - X: %u, Y: %u, Z: %u.\n", p->x_gain, p->y_gain, p->z_gain);
//        fprintf(stderr, "NOS Register - %2X.\n", p->NOSRegValue);
//    }
}

//------------------------------------------
// readCycleCountRegs()
//------------------------------------------
void readCycleCountRegs(pList *p)
{
    uint8_t regCC[7]= { 0, 0, 0, 0, 0, 0, 0 };

    i2c_setAddress(p->i2c_fd, p->magnetometerAddr);
    //  Read register settings
    i2c_readbuf(p->i2c_fd, RM3100I2C_CCX_1, regCC, 7);
    fprintf(stdout, "regCC[%i]: 0x%X\n",    0, (uint8_t)regCC[0]);
    fprintf(stdout, "regCC[%i]: 0x%X\n",    1, (uint8_t)regCC[1]);
    fprintf(stdout, "regCC[%i]: 0x%X\n",    2, (uint8_t)regCC[2]);
    fprintf(stdout, "regCC[%i]: 0x%X\n",    3, (uint8_t)regCC[3]);
    fprintf(stdout, "regCC[%i]: 0x%X\n",    4, (uint8_t)regCC[4]);
    fprintf(stdout, "regCC[%i]: 0x%X\n",    5, (uint8_t)regCC[5]);
    fprintf(stdout, "regCC[%i]: 0x%X\n\n",  6, (uint8_t)regCC[6]);
}
