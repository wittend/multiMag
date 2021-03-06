//=========================================================================
// config.c
// 
//  JSON configuration routines for multiMag magnetometer support.
//
// Author:      David M. Witten II, KD0EAG
// Date:        Jan 31, 2021
// License:     GPL 3.0
//=========================================================================
#include "config.h"
#include "uthash/uthash.h"
#include "jsmn/jsmn.h"

#define _DEBUG 0
#define MAXKEYLEN   64
#define MAXVALLEN   64

//remembar to ditch this..
extern char configFileName[MAXPATHBUFLEN];
int id = 1;

static char *keyWords[] =
{
    "numThreads",
    "threadOffsetUS",
    "i2cBusNumber",
    "i2c_fd",
    "modeOutputFlag",
    "baseFilePath",
    "outputFilePath",
    "outputFileName",
    "gridSqr",
    "sitePrefix",
     NULL
};

//------------------------------------------
// struct pStruct
//------------------------------------------
struct pStruct
{
    int id;                    /* key */
    char key[MAXKEYLEN];
    char val[MAXVALLEN];
    UT_hash_handle hh;         /* makes this structure hashable */
};

struct pStruct *params = NULL;

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
#ifdef _DEBUG    
    else
    {
        printf("readConfigFromFile(p, %s) Success!\n", configFileName);
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
            printf("\nConfig read from file: %s\n\n", cfgFile);
#if _DEBUG
            printf("%s\n", jsonstr);
#endif
        }
        else
        {
            perror("reading config file");
            exit(EXIT_FAILURE);
        }
    }
    jsmn_parser parser;
    jsmntok_t t[JSONBUFTOKENCOUNT]; /* We expect no more than JSONBUFTOKENCOUNT JSON tokens */
    jsmn_init(&parser);
    rv = jsmn_parse(&parser, jsonstr, strlen(jsonstr), t, JSONBUFTOKENCOUNT);
#if _DEBUG
    printf("\n");
    switch(rv)
    {
        case JSMN_ERROR_INVAL:
            printf("njsmn_parse() returns JSMN_ERROR_INVAL.\nBad token, JSON string is corrupted.\n");
            break;
        case JSMN_ERROR_NOMEM:
            printf("jsmn_parse() returns JSMN_ERROR_NOMEM.\nNot enough tokens, JSON string is too large\n");
            break;
        case JSMN_ERROR_PART:
            printf("jsmn_parse() returns JSMN_ERROR_PART.\nJSON string is too short, expecting more JSON data\n");
            break;
        default:
            printf("jsmn_parse() returns %i tokens.\n", rv);
            for(int i =  0; i < rv; i++)
            {
                char js[MAXVALLEN] = "";
                int type = t[i].type;
                
                //printf("t[%i] type: ", i);
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
            printf("\n");
            break;
    }
    printf("\n");
#endif
    return rv;
}

//------------------------------------------
// addParam()
//------------------------------------------
void addParam(int id, char *key, char *val)
{
    struct pStruct *s;

    HASH_FIND_STR(params, key, s);      /* id already in the hash? */
    if(s == NULL)
    {
        s = (struct pStruct*)malloc(sizeof(struct pStruct));
        s->id = id;
        strncpy(s->key, key, MAXKEYLEN - 1);
        strncpy(s->val, val, MAXVALLEN - 1);
        //HASH_ADD_INT(params, id, s);  /* id: name of key field */
        HASH_ADD_STR(params, key, s);
    }
}

//------------------------------------------
// findUser()
//------------------------------------------
struct pStruct *findKeyStr(char *key)
{
    struct pStruct *s;
#if _DEBUG0    
    printf("findKeyStr(\"%s\")\n", key);
#endif
//    HASH_FIND_STR(params, &id, key);  /* s: output pointer */
    HASH_FIND_STR(params, key, s);
#if _DEBUG0
    if(s)
    {
        printf("%s val is %s\n", s->key, s->val);
    }
#endif
#if _DEBUG0    
    else
    {
        printf("Returns NULL.\n");
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
    
    printf("k: %-20s,  v: %20s\n", key, value);
    addParam(id, key, value);
    printf("Added id: %i\n", id);
    id++;
    return rv;
}

//------------------------------------------
// saveConfigToFile()
//------------------------------------------
void printParams()
{
    struct pStruct *s;

#if _DEBUG
    printf("\n\nIn printParams()\n");
#endif
    for(s = params; s != NULL; s = (struct pStruct*)(s->hh.next))
    {
        printf("id %4d: key: \"%s\", val: \"%s\"\n", s->id, s->key, s->val);
    }
#if _DEBUG
    printf("\n\n");
    printf("\nShow KNOWN key:val pairs\n");
    int i = 0;
    while(keyWords[i] != NULL)
    {
        if((s = findKeyStr(keyWords[i++])) != NULL)
        {
            printf("id %4d: key: \"%s\", val: \"%s\"\n", s->id, s->key, s->val);
        }
    }
    printf("\nShow ALL key:val pairs\n");
    {
        for (s = params; s != NULL; s = s->hh.next)
        {
            printf("id %4d: key: \"%s\", val: \"%s\"\n", s->id, s->key, s->val);
        }
    }    
    printf("\n\n");
#endif    
}

//------------------------------------------
// findKeyInt()
//------------------------------------------
struct pStruct *findKeyInt(int id)
{
    struct pStruct *s;

    HASH_FIND_INT(params, &id, s);  /* s: output pointer */
    return s;
}

//------------------------------------------
// hashDeleteAll()
//------------------------------------------
void hashDeleteAll()
{
    struct pStruct *current_param, *tmp;
    
    HASH_ITER(hh, params, current_param, tmp)
    {
        HASH_DEL(params, current_param);  /* delete; users advances to next */
        free(current_param);              /* optional- if you want to free  */
    }
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
    
    sprintf(js, "[\n    {");
    strcat(jsonstr, js);
 
//    sprintf(js, "\n    \"swVersion\": \"%s\",", version);
//    strcat(jsonstr, js);
    
    sprintf(js, "\n      \"numThreads\"        :   %u,", p->numThreads);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"threadOffsetUS\"    :   %u,", p->threadOffsetUS);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"i2cBusNumber\"      :   %u,", p->i2cBusNumber);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"i2c_fd\"            :   %u,", p->i2c_fd);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"modeOutputFlag\"    :   %u,", p->modeOutputFlag);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"baseFilePath\"      :   \"%s\",", p->baseFilePath);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"outputFilePath\"    :   \"%s\",", p->outputFilePath);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"outputFileName\"    :   \"%s\",", p->outputFileName);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"gridSqr\"           :   \"%s\",", p->gridSqr);
    strcat(jsonstr, js);
    sprintf(js, "\n      \"sitePrefix\"        :   \"%s\",", p->sitePrefix);
    strcat(jsonstr, js);
   
    sprintf(js, "\n    }\n]\n");
    strcat(jsonstr, js);

#if _DEBUG
    printf("%s", jsonstr);
#endif

    if((fd = fopen(cfgFile, "w")) != NULL)
    {
        if(fwrite(jsonstr, strlen(jsonstr), 1, fd))
        {
            printf("\nSaved config to file: %s\n\n", cfgFile);
        }
        else
        {
            perror("writing config file");
        }
        fclose(fd);
    }
    return rv;
}

