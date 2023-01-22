#ifndef __LOCAL_H__
#define __LOCAL_H__


/*
 * Common header file for Message Queue Example
 */

#include <stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <wait.h>
#include <time.h>


#define QUEUE_IDs_FILE "./queue_IDs.txt"


union semun
{
    int val;
    struct semid_ds *buf;
    ushort *array;
};

struct sembuf acquire = {0, -1, SEM_UNDO},
release = {0, 1, SEM_UNDO},
acquire2 = {0, -1, SEM_UNDO},
release2 = {0, 1, SEM_UNDO},
acquire3 = {0, -1, SEM_UNDO},
release3 = {0, 1, SEM_UNDO},
acquire4 = {0, -1, SEM_UNDO},
release4 = {0, 1, SEM_UNDO};

enum { READ, MADE };

enum
{
    tankACapacity
};

enum
{
    bagCapacity //p11
};

enum
{
    tankCCapacity

};

enum
{
    bagCapacity2 //p12
};


struct bagWSCapacity //p22 , p23
{
    int waterCapacity;
    int sandCapacity;
};

struct bagWSCapacity2 //p12, p13
{
    int waterCapacity;
    int sandCapacity;
} ;
struct MEMORY
{
    int tankACapacity;


};

struct MEMORY2
{
    int tankCCapacity;

};

struct MEMORY3
{

    int bagCapacity;


};

struct MEMORY4
{

    int bag21Capacity;


};

struct TEAM1
{

    int tankBCapacity; //tank b
    struct bagWSCapacity bagOFannoyed1;
    struct bagWSCapacity bagOFannoyed2;


};



struct TEAM2
{

    int tankDCapacity; //tank D
    struct bagWSCapacity2 bagOFannoyed1;
    struct bagWSCapacity2 bagOFannoyed2;


};




struct WIN
{
    int flagW;

};


struct REFF
{
    int Counter;
    int winner[3];

};
typedef struct
{
    char buffer[BUFSIZ];
} MESSAGE;

/* message structure */
struct message
{
    long mtype;
    char mtext[5];
};




void red (){
    printf("\033[1;31m");
     fflush(stdout);
}
void red2 (){
    printf("\033[7;31m");
     fflush(stdout);
}
void red3 (){
    printf("\033[6;31m");
     fflush(stdout);
}
void green (){
    printf("\033[1;32m");
    fflush(stdout);
}

void yellow(){
    printf("\033[1;33m");
    fflush(stdout);
}
void yellow2(){
    printf("\033[4;33m");
    fflush(stdout);
}
void yellow3(){
    printf("\033[6;33m");
    fflush(stdout);
}

void blue(){
    printf("\033[1;34m");
    fflush(stdout);
}
void blue2(){
    printf("\033[7;34m");
    fflush(stdout);
}


void normals(){
    printf("\033[0;37m");
    fflush(stdout);
}
void cyan(){
    printf("\033[1;36m");
    fflush(stdout);
    
}

void cyan2(){
    printf("\033[0;36m");
    fflush(stdout);
}
void cyan3(){
    printf("\033[6;36m");
    fflush(stdout);
}

void Purple(){
    printf(" \033[1;35m");
    fflush(stdout);
}
void Purple2(){
    printf(" \033[6;35m");
    fflush(stdout);
}


#endif
