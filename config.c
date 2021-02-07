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

#define _DEBUG 1

//------------------------------------------
// readConfigFromFile()
//------------------------------------------
int readConfigFromFile(pList *p, char *cfgFile)
{
    int rv = 0;
    struct stat fs;
    FILE *fd = NULL;
    char jsonstr[JSONBUFLEN] = "";
    
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
            printf(jsonstr);
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
                char js[65] = "";
                int type = t[i].type;
                
                printf("t[%i] type: ", i);
                switch(type)
                {
                    case JSMN_UNDEFINED:        // = 0,
                        printf("JSMN_UNDEFINED.\n");
                        break;
                    case JSMN_OBJECT:           // = 1,
                        printf("JSMN_OBJECT.\n");
                        break;
                    case JSMN_ARRAY:            // = 2,
                        printf("JSMN_ARRAY.\n");
                        break;
                    case JSMN_STRING:           // = 3,
                        printf("JSMN_STRING.  Value: ");
                        strncpy(js, jsonstr + t[i].start, t[i].end - t[i].start);
//                        js[t[i].end] = 0;
                        printf("%s\n", js);
                        break;
                    case JSMN_PRIMITIVE:        // = 4
                        printf("JSMN_PRIMITIVE. Value: ");
                        strncpy(js, jsonstr + t[i].start, t[i].end - t[i].start);
//                        js[t[i].end] = 0;
                        printf("%s\n", js);
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

