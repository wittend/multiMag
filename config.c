//=========================================================================
// config.c
// 
//  JSON configuration routines for multiMag magnetometer support.
//
// Date:        2021-06-01
// Author:      David M. Witten II, KD0EAG
// License:     GPL 3.0
//=========================================================================
#include "config.h"
#include "uthash/uthash.h"
#include "jsmn/jsmn.h"

#define _DEBUG 1

// remembar to ditch this..
extern char configFileName[MAXPATHBUFLEN];
int id = 1;

static char *keyWords[] = {
    "numThreads",
    "threadOffsetUS",
    "i2cBusNumber",
    "i2c_fd",
    "modeOutputFlag",
    "baseFilePath",
    "outputFilePath",
    "outputFileName",
    "inputPipeName",
    "outputPipeName",
    "rollOverTime",
    "gridSqr",
    "sitePrefix",
    "city",
    "state",
    "country",
    "postalCode",
    "lattitude",
    "longitude",
    "elevation",
    "system",
    NULL
};

extern struct pStruct *jsparams;

//------------------------------------------
// showSettings()
//------------------------------------------
void showSettings(pList *p)
{
    char pathStr[128] = "";
    snprintf(pathStr, sizeof(pathStr), "/dev/i2c-%i", p->i2cBusNumber);

    fprintf(stdout, "\n  Working Parameters:\n");
    fprintf(stdout, "  -------------------  \n");
    fprintf(stdout, "   Software Version                            %s\n",          p->Version);
    fprintf(stdout, "   Magnetometer revision ID detected:          %i (dec)\n",    p->magRevId);
    fprintf(stdout, "   Log output:                                 %s\n",          p->logOutput ? "TRUE" : "FALSE");
    //fprintf(stdout, "   Log Rollover time:                          %s\n",          p->logOutputTime);
    fprintf(stdout, "   Log site prefix string:                     %s\n",          p->sitePrefix);
    fprintf(stdout, "   Output file path:                           %s\n",          p->outputFilePath);
    fprintf(stdout, "   Number of sensor threads:                   %u\n",          p->numThreads);
#if (USE_PIPES)
    fprintf(stdout, "   Log output to pipes:                        %s\n",          p->useOutputPipe ? "TRUE" : "FALSE");
    fprintf(stdout, "   Input file path:                            %s\n",          p->pipeInPath);
    fprintf(stdout, "   Output file path:                           %s\n",          p->pipeOutPath);
#endif
    fprintf(stdout, "   Use GNSS PPS:                               %s\n",          p->usePPS ? "TRUE" : "FALSE");
    fprintf(stdout, "   I2C bus number as integer:                  %i (dec)\n",    p->i2cBusNumber);
    fprintf(stdout, "   I2C bus path as string:                     %s\n",          pathStr);
    fprintf(stdout, "   Use I2C multiplexor:                        %s\n",          p->useI2CMUX ? "TRUE" : "FALSE");
    fprintf(stdout, "   I2C bus path as string:                     %02X (hex)\n",  p->i2cMUXAddr);
    fprintf(stdout, "   Local temperature address:                  %02X (hex)\n",  p->localTempAddr);
    fprintf(stdout, "   Remote temperature address:                 %02X (hex)\n",  p->remoteTempAddr);
    fprintf(stdout, "   Magnetometer address:                       %02X (hex)\n",  p->magnetometerAddr);
    fprintf(stdout, "   Built in self test (BIST) value:            %02X (hex)\n",  p->doBistMask);
    fprintf(stdout, "   NOS Register value:                         %02X (hex)\n",  p->NOSRegValue);
//    fprintf(stdout, "   Post DRDY delay:                            %i (dec)\n",    p->DRDYdelay);
    fprintf(stdout, "   Device sampling mode:                       %s\n",          p->samplingMode     ? "CONTINUOUS" : "POLL");
    fprintf(stdout, "   Cycle counts by vector:                     X: %3i (dec), Y: %3i (dec), Z: %3i (dec)\n", p->cc_x, p->cc_y, p->cc_z);
    fprintf(stdout, "   Gain by vector:                             X: %3i (dec), Y: %3i (dec), Z: %3i (dec)\n", p->x_gain, p->y_gain, p->z_gain);
    fprintf(stdout, "   Software Loop Delay (uSec):                 %2i (dec uSec)\n",    p->outDelay);
    fprintf(stdout, "   CMM sample rate:                            %2X (hex)\n",   p->CMMSampleRate);
    fprintf(stdout, "   TMRC reg value:                             %2X (hex)\n",   p->TMRCRate);
    fprintf(stdout, "   Show parameters:                            %s\n",          p->showParameters   ? "TRUE" : "FALSE");
    fprintf(stdout, "   Return single magnetometer reading:         %s\n",          p->singleRead       ? "TRUE" : "FALSE");
    fprintf(stdout, "   Timestamp format:                           %s\n",          p->tsMilliseconds   ? "RAW"  : "UTCSTRING");
    fprintf(stdout, "   Show total field:                           %s\n",          p->showTotal        ? "TRUE" : "FALSE");

    fprintf(stdout, "   City:                                       %s\n",          p->city);
    fprintf(stdout, "   State:                                      %s\n",          p->state);
    fprintf(stdout, "   Country:                                    %s\n",          p->country);
    fprintf(stdout, "   Postalcode:                                 %s\n",          p->postalcode);
    fprintf(stdout, "   GridSqr:                                    %s\n",          p->gridSqr);
    fprintf(stdout, "   Lattitude:                                  %s\n",          p->lattitude);
    fprintf(stdout, "   Longitude:                                  %s\n",          p->longitude);
    fprintf(stdout, "   Elevation:                                  %s\n",          p->elevation);
    fprintf(stdout, "   LogOutputTime:                              %s\n",          p->logOutputTime);
    fprintf(stdout, "   System:                                     %s\n",          p->system);

    fprintf(stdout, "\n");
}


//------------------------------------------
// printParams()
//------------------------------------------
void printParams(pList* p)
{
    struct pStruct *s;
    int i = 0;

    fprintf(stdout, "\n  Stored Parameters:\n");
    fprintf(stdout, "  ------------------\n");
    for (s = jsparams; s != NULL; s = s->hh.next)
    {
        fprintf(stdout, "    id %4d: key: \"%s\", val: \"%s\"\n", s->id, s->key, s->val);
    }
}

//------------------------------------------
// readConfig()
//------------------------------------------
int readConfig(pList *p)
{
    int rv = 0;
    if(readConfigFromFile(p, configFileName) == 0)
    {
        rv = 0;
    }
#ifdef _DEBUG0    
    else
    {
        fprintf(stdout, "readConfigFromFile(p, %s) Success!\n", configFileName);
    }
#endif    
    return rv;
}

//------------------------------------------
// readConfigFromFile()
//------------------------------------------
int readConfigFromFile(pList *p, char *cfgFile)
{
    int rv = 0;
    struct stat fs;
    FILE *fd = NULL;
    char jsonstr[JSONBUFLEN] = "";
    int  jsmnStrPairFirst = FALSE;
    int  jsmnStrPair = FALSE;
    char kStr[MAXKEYLEN] = "";
    char kVal[MAXVALLEN] = "";
    
    if(lstat(cfgFile, &fs) == -1)
    {
        char sErr[MAXPATHBUFLEN + 1];
        snprintf(sErr, MAXPATHBUFLEN, "lstat file: %s ", cfgFile);
        perror(sErr);
        exit(EXIT_FAILURE);
    }
    if((fd = fopen(cfgFile, "r")) != NULL)
    {
        if(fread(jsonstr, fs.st_size, 1, fd))
        {
            fprintf(stdout, "\nConfig read from file: %s\n", cfgFile);
#if _DEBUG0
            fprintf(stdout, "%s\n", jsonstr);
#endif
            fclose(fd);
        }
        else
        {
            perror("reading config file");
            exit(EXIT_FAILURE);
        }
    }
    jsmn_parser parser;
    jsmntok_t t[JSONBUFTOKENCOUNT];             // We expect no more than JSONBUFTOKENCOUNT JSON tokens
    jsmn_init(&parser);
    rv = jsmn_parse(&parser, jsonstr, strlen(jsonstr), t, JSONBUFTOKENCOUNT);
    switch(rv)
    {
        case JSMN_ERROR_INVAL:
            fprintf(stdout, "jsmn_parse() returns JSMN_ERROR_INVAL.\nBad token, JSON string is corrupted.\n");
            break;
        case JSMN_ERROR_NOMEM:
            fprintf(stdout, "jsmn_parse() returns JSMN_ERROR_NOMEM.\nNot enough tokens, JSON string is too large\n");
            break;
        case JSMN_ERROR_PART:
            fprintf(stdout, "jsmn_parse() returns JSMN_ERROR_PART.\nJSON string is too short, expecting more JSON data\n");
            break;
        default:
#ifdef DEBUG0            
            fprintf(stdout, "jsmn_parse() returns %i tokens.\n", rv);
#endif            
            for(int i =  0; i < rv; i++)
            {
                char js[MAXVALLEN] = "";
                int type = t[i].type;
                
                switch(type)
                {
                    case JSMN_UNDEFINED:        // = 0,
                        break;
                    case JSMN_OBJECT:           // = 1,
                        break;
                    case JSMN_ARRAY:            // = 2,
                        break;
                    case JSMN_STRING:           // = 3,
                        strncpy(js, jsonstr + t[i].start, t[i].end - t[i].start);
                        if(!jsmnStrPairFirst)
                        {
                            jsmnStrPairFirst = TRUE;
                            strncpy(kStr, js, strlen(js));
                            memset(kVal, 0, MAXVALLEN);
                        }
                        else
                        {
                            jsmnStrPairFirst = FALSE;
                            strncpy(kVal, js, strlen(js));
                            configDecode(p, kStr, kVal);                            
                            memset(kStr, 0, MAXKEYLEN);
                            memset(kVal, 0, MAXVALLEN);
                        }
                        break;
                    case JSMN_PRIMITIVE:        // = 4
                        strncpy(js, jsonstr + t[i].start, t[i].end - t[i].start);
                        if(!jsmnStrPairFirst)
                        {
                            jsmnStrPairFirst = TRUE;
                            strncpy(kStr, js, strlen(js));
                            memset(kVal, 0, MAXVALLEN);
                        }
                        else
                        {
                            jsmnStrPairFirst = FALSE;
                            strncpy(kVal, js, strlen(js));
                            configDecode(p, kStr, kVal);                            
                            memset(kStr, 0, MAXKEYLEN);
                            memset(kVal, 0, MAXVALLEN);
                        }
                        break;
                }
            }
            fprintf(stdout, "\n");
            break;
    }
    return rv;
}

//------------------------------------------
// saveConfigToFile()
//------------------------------------------
int saveConfigToFile(pList *p, char *cfgFile)
{
    int rv = 0;
    FILE *fd = NULL;
    char js[256] = "";
    char jsonstr[JSONBUFLEN] = "";

#if _DEBUG0
    fprintf(stdout, "Inside saveConfigToFile() : %s.\n", cfgFile);
    fflush(stdout);
#endif

    sprintf(js, "[\n    {");
    strcat(jsonstr, js);

    sprintf(js, "\n      \"magRevId\"          :   %u,", p->magRevId);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"numThreads\"        :   %u,", p->numThreads);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"threadOffsetUS\"    :   %u,", p->threadOffsetUS);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"threadCadenceUS\"   :   %u,", p->threadCadenceUS);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"threadOffsetUS\"    :   %u,", p->threadOffsetUS);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"i2cBusNumber\"      :   %u,", p->i2cBusNumber);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"i2c_fd\"            :   %u,", p->i2c_fd);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"localTempAddr\"     :   %u,", p->localTempAddr);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"remoteTempAddr\"    :   %u,", p->remoteTempAddr);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"magnetometerAddr\"  :   %u,", p->magnetometerAddr);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"i2cMUXAddr\"        :   %u,", p->i2cMUXAddr);
    strcat(jsonstr, js);

    sprintf(js, "\n      \"showTotal\"         :   %u,", p->showTotal);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"tsMilliseconds\"    :   %u,", p->tsMilliseconds);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"singleRead\"        :   %u,", p->singleRead);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"usePPS\"            :   %u,", p->usePPS);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"showParameters\"    :   %u,", p->showParameters);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"TMRCRate\"          :   %u,", p->TMRCRate);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"CMMSampleRate\"     :   %u,", p->CMMSampleRate);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"samplingMode\"      :   %u,", p->samplingMode);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"outDelay\"          :   %u,", p->outDelay);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"NOSRegValue\"       :   %u,", p->NOSRegValue);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"doBistMask\"        :   %u,", p->doBistMask);
    strcat(jsonstr, js);

//    fprintf(stdout, "   Cycle counts by vector:                     X: %3i (dec), Y: %3i (dec), Z: %3i (dec)\n", p->cc_x, p->cc_y, p->cc_z);
//    fprintf(stdout, "   Gain by vector:                             X: %3i (dec), Y: %3i (dec), Z: %3i (dec)\n", p->x_gain, p->y_gain, p->z_gain);

    sprintf(js, "\n      \"baseFilePath\"      :   \"%s\",", p->baseFilePath);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"outputFilePath\"    :   \"%s\",", p->outputFilePath);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"outputFileName\"    :   \"%s\",", p->outputFileName);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"rollOverTime\"      :   \"%s\",", p->rollOverTime);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"pipeInPath\"        :   \"%s\",", p->pipeInPath);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"pipeOutPath\"       :   \"%s\",", p->pipeOutPath);
    strcat(jsonstr, js);

    sprintf(js, "\n      \"city\"              :   \"%s\",", p->city);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"state\"             :   \"%s\",", p->state);
    strcat(jsonstr, js);

    sprintf(js, "\n      \"country\"           :   \"%s\",", p->country);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"postalcode\"        :   \"%s\",", p->postalcode);
    strcat(jsonstr, js);

    sprintf(js, "\n      \"gridSqr\"           :   \"%s\",", p->gridSqr);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"lattitude\"         :   \"%s\",", p->lattitude);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"longitude\"         :   \"%s\",", p->longitude);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"elevation\"         :   \"%s\",", p->elevation);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"sitePrefix\"        :   \"%s\",", p->sitePrefix);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"logOutputTime\"     :   \"%s\",", p->logOutputTime);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"Version\"           :   \"%s\",", p->Version);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"system\"            :   \"%s\",", p->system);
    strcat(jsonstr, js);

    sprintf(js, "\n    }\n]\n");
    strcat(jsonstr, js);

#if _DEBUG
    fprintf(stdout, "%s", jsonstr);
#endif

#if _DEBUG0
    fprintf(stdout, "Before write in saveConfigToFile() : %s.\n", cfgFile);
    fflush(stdout);
#endif
    if((fd = fopen(cfgFile, "w")) != NULL)
    {
        if(fwrite(jsonstr, strlen(jsonstr), 1, fd))
        {
            fprintf(stdout, "\nSaved config to file: %s\n\n", cfgFile);
        }
        else
        {
            perror("writing config file");
        }
        fclose(fd);
    }
#if _DEBUG0
    fprintf(stdout, "After write in saveConfigToFile() : %s.\n", cfgFile);
    fflush(stdout);
#endif
    return rv;
}

//------------------------------------------
// addParam()
//------------------------------------------
void addParam(int id, char *key, char *val)
{
    struct pStruct *s;

    HASH_FIND_STR(jsparams, key, s);      /* id already in the hash? */
    if(s == NULL)
    {
        s = (struct pStruct*)malloc(sizeof(struct pStruct));
        s->id = id;
        strncpy(s->key, key, MAXKEYLEN - 1);
        strncpy(s->val, val, MAXVALLEN - 1);
        //HASH_ADD_INT(params, id, s);  /* id: name of key field */
        HASH_ADD_STR(jsparams, key, s);
    }
}

//------------------------------------------
// findUser()
//------------------------------------------
struct pStruct *findKeyStr(char *key)
{
    struct pStruct *s;
    
#if _DEBUG0    
    fprintf(stdout, "findKeyStr(\"%s\")\n", key);
#endif
    HASH_FIND_STR(jsparams, key, s);
#if _DEBUG0
    if(s)
    {
        fprintf(stdout, "%s val is %s\n", s->key, s->val);
    }
#endif
#if _DEBUG0
    else
    {
        fprintf(stdout, "Returns NULL.\n");
    }
#endif
    return s;
}

//------------------------------------------
// configDecode()
//------------------------------------------
int configDecode(pList *p, char *key, char *value)
{
    int rv = 0;
    
#ifdef DEBUG0    
    fprintf(stdout, "k: %-20s,  v: %20s\n", key, value);
#endif
    addParam(id, key, value);
#ifdef DEBUG0    
    fprintf(stdout, "Added id: %i\n", id);
#endif
    id++;
    return rv;
}

//------------------------------------------
// findKeyInt()
//------------------------------------------
struct pStruct *findKeyInt(int id)
{
    struct pStruct *s;

    HASH_FIND_INT(jsparams, &id, s);  /* s: output pointer */
    return s;
}

//------------------------------------------
// hashDeleteAll()
//------------------------------------------
void hashDeleteAll()
{
    struct pStruct *current_param, *tmp;
    
    HASH_ITER(hh, jsparams, current_param, tmp)
    {
        HASH_DEL(jsparams, current_param);      // delete; users advances to next
        free(current_param);                    // optional- if you want to free
    }
}

