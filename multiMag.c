#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <pthread.h>

#define NOTHREADS 100
//#define _POSIX_PRIORITY_SCHEDULING 1
#define UTCBUFLEN           64

//------------------------------------------
// types
//------------------------------------------
typedef struct tag_pList
{
    int numThreads;
} pList;

//------------------------------------------
// prototypes
//------------------------------------------
void * fun1(void *thread_id);
void * fun2(void *thread_id);
int main(int argc, char** argv);
int getCommandLine(int argc, char** argv, pList *p);
long currentTimeMillis();
struct tm *getUTC();

//------------------------------------------
// currentTimeMillis()
//------------------------------------------
long currentTimeMillis()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec * 1000 + time.tv_usec / 1000;
}

//------------------------------------------
// getUTC()
//------------------------------------------
struct tm *getUTC()
{
    time_t now = time(&now);
    if (now == -1)
    {
        puts("The time() function failed");
    }
    struct tm *ptm = gmtime(&now);
    if (ptm == NULL)
    {
        puts("The gmtime() function failed");
    }    
    return ptm;
}

//------------------------------------------
// fun1()
//------------------------------------------
void * fun1(void *thread_id)
{
    int i;
    int *id = (int *) thread_id;
    char utcStr[UTCBUFLEN] = "";
    int currentDay = 0;
    struct tm *utcTime = getUTC();

    while(1) 
    {
        // usleep(1000 * 1000 * 60);    // 1 minute
        // printf("1 minute \n");
        usleep(1000 * 1000);                   // 1 second
        utcTime = getUTC();
        strftime(utcStr, UTCBUFLEN, "%d %b %Y %T", utcTime);                
//        fprintf(outfp, "\"%s\"", utcStr);
        printf("Task %u: 1 second %s\n", *id, utcStr);
  //      sched_yield();
    }
    pthread_exit(NULL);
}

//------------------------------------------
// fun2()
//------------------------------------------
void * fun2(void *thread_id)
{
    int i;
    int *id = (int *) thread_id;

    while(1) 
    {
        // usleep(2000 * 1000 * 60);
        // printf("2 minute \n");
        usleep(1000 * 1000);                   // 1 second
        printf("Task B: 1 second \n");
        sched_yield();
    }
    pthread_exit(NULL);
}

//------------------------------------------
// getCommandLine()
// Parse the command line options and setup
// the internal option flags
//------------------------------------------
int getCommandLine(int argc, char** argv, pList *p)
{
    int c;

    p->numThreads = 2;
    
    if(p != NULL)
    {
        memset(p, 0, sizeof(pList));
    }
    while((c = getopt(argc, argv, "?hn:")) != -1)
    {
        //int this_option_optind = optind ? optind : 1;
        switch (c)
        {
            case 'n':
                // fprintf(stdout, "\nThe -A option is intended to allow setting a value in the NOS register.\n\n");
                p->numThreads = atoi(optarg);
                break;
            case 'h':
            case '?':
                // fprintf(stdout, "\n%s Version = %s\n", argv[0], version);
                fprintf(stdout, "\nParameters:\n\n");
                fprintf(stdout, "   -n <count>             :  Set number of threads.          [ default 2 decimal]\n");
                fprintf(stdout, "   -h or -?               :  Display this help.\n\n");
                return 1;
                break;
            default:
                fprintf(stdout, "\n?? getopt returned character code 0x%2X ??\n", c);
                break;
        }
    }
    if(optind < argc)
    {
        printf ("non-option ARGV-elements: ");
        while (optind < argc)
        {
            printf ("%s ", argv[optind++]);
        }
        printf ("\n");
    }
    return 0;
}
 
//------------------------------------------
// readConfig()
//------------------------------------------
int readConfig(pList *p)
{
    int rv = 0;
    return rv;
}

//------------------------------------------
// openLogs()
//------------------------------------------
int openLogs(pList *p)
{
    int rv = 0;
    return rv;
}

//------------------------------------------
// main()
// 
//------------------------------------------
int main(int argc, char** argv)
{
    pthread_t tids[NOTHREADS];
    int ids[NOTHREADS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    //pthread_t tids[]  //[NOTHREADS];
    //int ids[];        //[NOTHREADS] = {1, 2};
    
    //long t;
    int i;
    pList p;
    int rv = 0;
    //FILE *outfp = stdout;

    getCommandLine(argc, argv, &p);
    readConfig(p)
    openLogs(p)
    for(i = 0; i < p.numThreads; i++) 
    {
        printf("Creating fun1 thread %u\n", i);
        rv = pthread_create(&tids[i], NULL, fun1, &ids[i]);
        if(rv) 
        {
            printf("unable to create thread! \n");
            exit(-1);
        }
    }
    for(i = 0 ; i < p.numThreads; i++) 
    {
        pthread_join(tids[i], NULL);
    }
    pthread_exit(NULL);     
    return 0;
}
