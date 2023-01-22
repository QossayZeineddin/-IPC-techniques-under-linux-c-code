#include "local.h"

#define TAKE_FROM_TANK 25
#define TANK_AC_CAPACITY 100

int alarmFlag = 0; /* Global alarm flag */
void alarmHandler(); /* Forward declaration of alarm handler */
int flag;
int s_pid[8];
int queue_IDs[9];
int timerA2B = 0;
int timerC2D = 0;

int main(int argc, char * argv[])
{


    char * shmptr[7];
    static struct MEMORY * memptr;   //Tank A capacity
    static struct MEMORY2 * memptr2; //Tank C capacity
    static struct WIN * memptr3; //winner
    static struct MEMORY3 * memptr4; // bag p11 caapcity
    static struct MEMORY4 * memptr5; //bag 21 capacity
    static struct TEAM1 * tm1; //team 1
    static struct TEAM2 * tm2; //team2

    static struct REFF ref;

    int shmid[7];
    int semid[4];
    FILE * fptr_queue_IDs;
    int n;
    char m;
    void semaphore_wait(int, int);
    void semaphore_signal(int, int);
    void semaphore_wait2(int, int);
    void semaphore_signal2(int, int);
    void semaphore_wait3(int, int);
    void semaphore_signal3(int, int);
    void semaphore_wait4(int, int);
    void semaphore_signal4(int, int);
    void sigset_catcher2();
    void stopAttack();
    int isAnnoyed;
    //char e ="%%";
    //printf("\n\n\n\n  %s  \n\n\n\n\n", e);

    sigset(SIGUSR1, sigset_catcher2); //user defind signal that execute sigset_catcher2 fuction
    sigset(SIGUSR2, stopAttack);
    pid_t ppid = getppid();

    signal(SIGALRM, alarmHandler); /* Install signal handler */

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<read the queue IDs>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    if ((fptr_queue_IDs = fopen(QUEUE_IDs_FILE, "r")) == NULL)
    {
        perror("QUEUE_IDs_FILE");
        exit(-7);
    }

    for (int f = 0; f < 9; f++)
    {
        fscanf(fptr_queue_IDs, "%d\n", & queue_IDs[f]);
    }

    fclose(fptr_queue_IDs);

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< attach to shared memory >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // no ipc-create: I just want to connect to it
    if ((shmid[0] = shmget((int) ppid, 0, 0)) != -1)
    {
        if ((shmptr[0] = (char * ) shmat(shmid[0], (char * ) 0, 0)) == (char * ) - 1)
        {
            perror("shmat -- team -- attach");
            exit(1);
        }
        memptr = (struct MEMORY * ) shmptr[0];
        memptr -> tankACapacity = TANK_AC_CAPACITY;

    }
    else
    {
        perror("shmget -- team-- access");
        exit(2);
    }

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< attach to shared memory2 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // no ipc-create: I just want to connect to it
    if ((shmid[1] = shmget((int) ppid + 1, 0, 0)) != -1)
    {
        if ((shmptr[1] = (char * ) shmat(shmid[1], (char * ) 0, 0)) == (char * ) - 1)
        {
            perror("shmat2 -- team -- attach");
            exit(1);
        }
        memptr2 = (struct MEMORY2 * ) shmptr[1];
        memptr2 -> tankCCapacity = TANK_AC_CAPACITY;


    }
    else
    {
        perror("shmget2 -- team-- access");
        exit(2);
    }
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< attach to shared memory3 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // no ipc-create: I just want to connect to it
    if ((shmid[2] = shmget((int) ppid + 2, 0, 0)) != -1)
    {
        if ((shmptr[2] = (char * ) shmat(shmid[2], (char * ) 0, 0)) == (char * ) - 1)
        {
            perror("shmat3 -- team -- attach");
            exit(1);
        }
        memptr3 = (struct WIN * ) shmptr[2];
        memptr3 -> flagW = 0;

    }
    else
    {
        perror("shmget3 -- team-- access");
        exit(2);
    }

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< attach to shared memory4 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // no ipc-create: I just want to connect to it
    if ((shmid[3] = shmget((int) ppid + 3, 0, 0)) != -1)
    {
        if ((shmptr[3] = (char * ) shmat(shmid[3], (char * ) 0, 0)) == (char * ) - 1)
        {
            perror("shmat4 -- team -- attach");
            exit(1);
        }
        memptr4 = (struct MEMORY3 * ) shmptr[3];
        memptr4 -> bagCapacity = 0;
    }
    else
    {
        perror("shmget4 -- team-- access");
        exit(2);
    }
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< attach to shared memory5 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // no ipc-create: I just want to connect to it
    if ((shmid[4] = shmget((int) ppid + 4, 0, 0)) != -1)
    {
        if ((shmptr[4] = (char * ) shmat(shmid[4], (char * ) 0, 0)) == (char * ) - 1)
        {
            perror("shmat5 -- team -- attach");
            exit(1);
        }
        memptr5 = (struct MEMORY4 * ) shmptr[4];
        memptr5 -> bag21Capacity = 0;

    }
    else
    {
        perror("shmget5 -- team-- access");
        exit(2);
    }

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< attach to shared memory6 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // no ipc-create: I just want to connect to it
    if ((shmid[5] = shmget((int) ppid + 5, 0, 0)) != -1)
    {
        if ((shmptr[5] = (char * ) shmat(shmid[5], (char * ) 0, 0)) == (char * ) - 1)
        {
            perror("shmat5 -- team -- attach");
            exit(1);
        }
        tm1 = (struct TEAM1 * ) shmptr[5];
        tm1 -> tankBCapacity = 0;
        //bag of processes that annoy
        tm1 -> bagOFannoyed1.waterCapacity = 5;
        tm1 -> bagOFannoyed1.sandCapacity = 5;
        tm1 -> bagOFannoyed2.waterCapacity = 5;
        tm1 -> bagOFannoyed2.sandCapacity = 5;


    }
    else
    {
        perror("shmget6 -- team-- access");
        exit(2);
    }

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< attach to shared memory7 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // no ipc-create: I just want to connect to it
    if ((shmid[6] = shmget((int) ppid + 6, 0, 0)) != -1)
    {
        if ((shmptr[6] = (char * ) shmat(shmid[6], (char * ) 0, 0)) == (char * ) - 1)
        {
            perror("shmat7 -- team -- attach");
            exit(1);
        }
        tm2 = (struct TEAM2 * ) shmptr[6];

        tm2->tankDCapacity = 0;


        //bag of processes that annoy
        tm2->bagOFannoyed1.waterCapacity = 5;
        tm2->bagOFannoyed1.sandCapacity = 5;
        tm2->bagOFannoyed2.waterCapacity = 5;
        tm2->bagOFannoyed2.sandCapacity = 5;

    }
    else
    {
        perror("shmget7 -- team-- access");
        exit(2);
    }
    /*
     * Access the semaphore set
     */
    //
    for (int k = 0; k <= 3; k++)
    {
        if ((semid[k] = semget((int) ppid + k, 1, 0)) == -1)
        {
            perror("semget -- producer -- access");
            exit(3);
        }
    }

    /* read the pid of the children from a file and store it*/
    FILE * f;
    int ii = 0;
    if ((f = fopen("PID.txt", "r")) == NULL)
        exit(10);

    for (ii = 0; ii < 9; ii++)
    {
        if (fscanf(f, "%d", & s_pid[ii]) != 1)
        {
            exit(1);
        }
    }
    fclose(f);

    if (strcmp(argv[0], "8") == 0)
    {

        for (int j = 0; j < 9; j++)
        {
            // sleep(2);
            kill(s_pid[j], SIGUSR1);
        }
    }

    if (flag == 0)
        sigpause(SIGUSR1);

    srand(time(0));

    if(strcmp(argv[1], "R") == 0)
        alarm(25); /* Schedule an alarm signal in three seconds */

    while (1)
    {
        sleep(1);

        if (memptr3 -> flagW == 1)
        {
            break;
        }

        if (strcmp(argv[1], "P11") == 0)
        {


            if( timerA2B ==1)
            {
                blue();
                printf("[A]  P11  -  -  -  -   [B]\n");
                normals();
                fflush(stdout);
            }
            else if ( timerA2B == 2)
            {
                blue();
                printf("[A]  -  P11  -  -  -   [B]\n");
                normals();
                fflush(stdout);
            }
            else if ( timerA2B == 3 )
            {
                blue();
                printf("[A]  -  -   P11  -  -   [B]\n");
                normals();
                fflush(stdout);
            }
            else if (timerA2B ==4 )
            {
                blue();
                printf("[A]  -  -  -   P11   -   [B]\n");
                normals();
                fflush(stdout);
            }
            else if ( timerA2B ==5 )
            {
                blue();
                printf("[A]  -  -  -  -  P11  [B]\n");
                normals();
                fflush(stdout);
            }

            if (timerA2B == 0)    //check the timer
            {
                yellow();
                printf("P11 is Filling its Bag Now\n");
                normals();
                fflush(stdout);
                //sleep(0.2);
                semaphore_wait(semid[0], 0);
                if (memptr -> tankACapacity != 0)   //check if tank A is not empty
                {
                    memptr -> tankACapacity = (memptr -> tankACapacity) - TAKE_FROM_TANK; //take from tank 25 %
                    if ( memptr -> tankACapacity <=0)  // if it becomes zero or less , make tank A empty
                    {
                        memptr -> tankACapacity =0;
                    }

                    yellow();
                    printf("P11 Tank [A] new Capacity is {%d %%}  \n", memptr -> tankACapacity);
                    normals();
                    fflush(stdout);

                }
                else
                {
                    red();
                    printf("tank [A] is Empty\n");
                    normals();
                    fflush(stdout);
                    kill(s_pid[8], SIGUSR1); //if tsnk A is empty send signal to the refree
                }
                semaphore_signal(semid[0], 0);
                semaphore_wait2(semid[1], 0);
                memptr4 -> bagCapacity = memptr4 -> bagCapacity + TAKE_FROM_TANK;  //fill the bag capacity of p11 with amount taken from Tank A
                // printf("\033[4;36m");
                //printf("\n\n\n\t\t\t********************************** TEAM 1 STARTING MOVING TO TANK 'B' **********************************\n\n");
                //normals();
                //fflush(stdout);
                cyan();
                printf("P11 Bag Capacity after filling its bag {%d %%} \n", memptr4 -> bagCapacity);
                normals();
                fflush(stdout);
                semaphore_signal2(semid[1], 0);
                timerA2B++;

            }
            else if (timerA2B == 5)
            {

                semaphore_wait2(semid[1], 0);
                if(memptr4 -> bagCapacity != 0)
                {
                    yellow();
                    printf("P11 Reached Tank [B] with bag Capacity {%d %%} \n", memptr4 -> bagCapacity);
                    normals();
                    fflush(stdout);
                }

                if( tm1 ->tankBCapacity + memptr4 -> bagCapacity > 100)
                {
                    tm1 ->tankBCapacity  = 100;
                }
                else
                {
                    tm1 ->tankBCapacity = tm1 ->tankBCapacity + memptr4 -> bagCapacity; //fill tank B with bag capacity
                }
                memptr4 -> bagCapacity = 0; //make bag empty

                if (tm1 ->tankBCapacity != 0)
                {
                    green();
                    printf("Tank [B] Capacity is Now {%d %%}\n P11 Go back to Tank [A]\n", tm1 ->tankBCapacity);
                    normals();
                    fflush(stdout);
                }
                semaphore_signal2(semid[1], 0);
                sleep(2);
                timerA2B = 0;

            }
            else
            {
                timerA2B++;
                srand(time(0));
                int pAstate = (int)(1 + (rand() % (10 - 1)));
                semaphore_wait2(semid[1], 0);
                if (pAstate == 1 && memptr4 -> bagCapacity != 0)   //if P11 fell down it loses all water
                {
                    memptr4 -> bagCapacity = 0;
                    yellow();
                    printf("P11 Fell down And Dropped all water in its Bag Go back to Tank [A]\n");
                    normals();
                    fflush(stdout);
                    sleep(1); //sleep according to current timer
                    timerA2B = 0;
                    semaphore_signal2(semid[1], 0);

                }
                else if (pAstate == 2 && memptr4 -> bagCapacity != 0)     //p11 lose porrtion of water while walking
                {

                    srand(time(0));
                    int pfall = (int)(1 + (rand() % (4 - 1))) ;
                    memptr4 -> bagCapacity = memptr4 -> bagCapacity - pfall;
                    Purple();
                    printf("P11 Fell while walking and lost {%d %%} of its Bag {%d %%}\n", pfall, memptr4 -> bagCapacity);
                    normals();
                    fflush(stdout);

                    if (memptr4 -> bagCapacity <= 0)
                    {
                        memptr4 -> bagCapacity = 0;
                        yellow();
                        printf("P11 Fell while walking and lost all water in its Bag Go back to Tank [A]\n");
                        normals();
                        fflush(stdout);
                        sleep(1); //sleep according to current timer
                        timerA2B = 0;
                    }
                    semaphore_signal2(semid[1], 0);

                }
                else
                {
                    semaphore_signal2(semid[1], 0);
                }

            }


        }
        else if (strcmp(argv[1], "P21") == 0)
        {
            if( timerC2D ==1)
            {
                blue();
                printf("[C]  P21  -  -  -  -   [D]\n");
                normals();
                fflush(stdout);
            }
            else if ( timerC2D == 2)
            {
                blue();
                printf("[C]  -  P21   -  -  -   [D]\n");
                normals();
                fflush(stdout);
            }
            else if ( timerC2D == 3 )
            {
                blue();
                printf("[C]  -  -   P21  -  -   [D]\n");
                normals();
                fflush(stdout);
            }
            else if (timerC2D ==4 )
            {
                blue();
                printf("[C]  -  -  -   P21   -  [D]\n");
                normals();
                fflush(stdout);
            }
            else if (timerC2D ==5 )
            {
                if(memptr5 -> bag21Capacity != 0)
                {
                    blue();
                    printf("[C]  -  -  -  -  P21  [D]\n");
                    normals();
                    fflush(stdout);
                }
            }
            if (timerC2D == 0)
            {
                yellow();
                printf("P21 is Filling its Bag Now\n");
                normals();
                fflush(stdout);
                semaphore_wait3(semid[2], 0);
                if (memptr2 -> tankCCapacity != 0)
                {
                    memptr2 -> tankCCapacity = memptr2 -> tankCCapacity - TAKE_FROM_TANK; //Take from tank C
                    if ( memptr2 -> tankCCapacity <=0)
                    {
                        memptr2 -> tankCCapacity =0;
                    }
                    yellow();
                    printf("P21 Tank [C] new Capacity is now {%d %%} \n", memptr2 -> tankCCapacity);
                    normals();
                    fflush(stdout);
                    semaphore_signal3(semid[2], 0);
                }
                else
                {
                    red();
                    printf("tank [C] in Empty\n");
                    normals();
                    fflush(stdout);
                    semaphore_signal3(semid[2], 0); //if tank C is empty send to the refree
                    kill(s_pid[8], SIGUSR1);
                }

                semaphore_wait4(semid[3], 0);
                memptr5 -> bag21Capacity = memptr5 -> bag21Capacity + TAKE_FROM_TANK; //fill bag p21 with amount taken from tank c
                // printf("\033[4;36m");
                // printf("\n\n\n\t\t\t********************************** TEAM 2 STARTING MOVING TO TANK 'C' **********************************\n\n");
                //normals();
                //fflush(stdout);
                cyan();
                printf("P21 bag Capacity after filling its bag {%d %%}\n", memptr5 -> bag21Capacity);
                normals();
                fflush(stdout);
                semaphore_signal4(semid[3], 0);
                timerC2D++;

            }
            else if (timerC2D == 5)     //if it reached location D
            {

                semaphore_wait4(semid[3], 0);
                if(memptr5 -> bag21Capacity != 0)
                {
                    yellow();
                    printf("P21 Reached Tank [D] with bag Capacity {%d %%}\n", memptr5 -> bag21Capacity);
                    normals();
                    fflush(stdout);
                }
                if(tm2->tankDCapacity + memptr5 -> bag21Capacity > 100)
                {
                    tm2->tankDCapacity = 100;
                }
                else
                {
                    tm2->tankDCapacity =tm2->tankDCapacity + memptr5 -> bag21Capacity; //fill tank D with water in bag
                }
                memptr5 -> bag21Capacity = 0; //empty the bag
                if( tm2 ->tankDCapacity != 0)
                {
                    green();
                    printf("Tank [D] Capacity is Now {%d %%}\nP21 Go back to Tank [C]\n\n", tm2 ->tankDCapacity);
                    normals();
                    fflush(stdout);
                }
                semaphore_signal4(semid[3], 0);
                sleep(2);
                timerC2D = 0;


            }
            else
            {
                timerC2D++;
                int pAstate = (int)(1 + (rand() % (20 - 1)));
                semaphore_wait4(semid[3], 0);
                if (pAstate == 1 && memptr5 -> bag21Capacity != 0)   //if p21 fell down
                {
                    memptr5 -> bag21Capacity = 0; //loses all water in its bag
                    semaphore_signal4(semid[3], 0);
                    yellow();
                    printf("P21 Felt And Dropped its Bag Go back to Tank [C]\n\n");
                    normals();
                    fflush(stdout);
                    timerC2D = 0;
                    sleep(1); //sleep according to current timer

                }
                else if (pAstate == 2 && memptr5 -> bag21Capacity != 0)     //if p21 loses some water whaile walking
                {
                    //semaphore_signal3(semid[3], 0);
                    srand(time(0));
                    int pfall = (int)(1 + (rand() % (4 - 1))) ;
                    memptr5 -> bag21Capacity = memptr5 -> bag21Capacity- pfall; //bag21 lost water what p21 is walking
                    Purple();
                    printf("P21 Felt while walking and lost {%d %%} of its Bag {%d %%} Go back to Tank [A]\n",pfall, memptr5 -> bag21Capacity);
                    normals();
                    fflush(stdout);

                    if (memptr5 -> bag21Capacity <= 0)
                    {
                        memptr5 -> bag21Capacity = 0;
                        yellow();
                        printf("P21 Felt  while walking and lost all of its Bag Go back to Tank [C]\n");
                        normals();
                        fflush(stdout);
                        sleep(1); //sleep according to current timer
                        timerC2D = 0;
                    }
                    semaphore_signal4(semid[3], 0);

                }
                else
                {
                    semaphore_signal4(semid[3], 0);
                }
            }



        }
        else if (strcmp(argv[1], "P22") == 0)
        {
            //sleep(2);
            /* make the child  sensitive to SIGUSR2 signal*/
            if (signal(SIGUSR2, stopAttack) == SIG_ERR) //signal handling, returns -1 (SIG_ERR) on failure
            {
                perror("Signal can not set SIGUSR2");
                exit(SIGUSR2);
            }
            if (memptr4 -> bagCapacity != 0)   //if bag p11 isn't empty
            {
                isAnnoyed = (int)((rand() % (1)));
                // printf("annoyed %d\n", isAnnoyed);
                //fflush(stdout);

                if (isAnnoyed != 0)
                {
                    srand(time(0));

                    int annoy = (int)(1 + (rand() % (tm1 ->bagOFannoyed1.waterCapacity - 1))) ; //randomize how much to steal from bag according to p22 bag capacity
                    semaphore_wait2(semid[1], 0);
                    if (memptr4 -> bagCapacity > 0)
                    {
                        // cyan3();
                        // printf(" The enemy teams (2) P22 start annoying the player P11 and Steal his water\n");
                        // normals();
                        // fflush(stdout);
                        // value of bag that process annoys
                        if (memptr4 -> bagCapacity - annoy <= 0)

                        {
                            memptr4 -> bagCapacity = 0;
                            blue();
                            printf("P22 Annoyed P11 and Took {%d %%} From P11's bag until its zero \n", annoy);
                            printf("P11 bo back to Tank [A]\n");
                            fflush(stdout);
                        }
                        else
                        {
                            memptr4 -> bagCapacity = memptr4 -> bagCapacity - annoy; //p22 steals from p11 bag
                            blue();
                            printf("P22 Annoyed P11 and Took {%d %%} From P11's bag\n", annoy);
                            normals();
                            fflush(stdout);
                        }
                        semaphore_signal2(semid[1], 0);

                        // value of the water in the bag of the annoying process
                        tm1 ->bagOFannoyed1.waterCapacity = annoy;

                    }
                    else
                    {
                        semaphore_signal2(semid[1], 0);
                    }
                    semaphore_wait(semid[0], 0);
                    if (memptr -> tankACapacity != 0 && memptr -> tankACapacity <= (TANK_AC_CAPACITY - annoy))
                    {
                        memptr -> tankACapacity = memptr -> tankACapacity + annoy; //p22 put the amount it stole in tank A
                        blue();
                        printf("P22 Returned {%d %%} to Tank [A], Tank [A] new Capacity is {%d %%}\n", annoy, memptr -> tankACapacity);
                        normals();
                        fflush(stdout);
                        semaphore_signal(semid[0], 0);

                    }
                    else if (memptr -> tankACapacity == 0)
                    {
                        semaphore_signal(semid[0], 0);
                        red();
                        printf("Tank [A] is Empty\n");
                        normals();
                        fflush(stdout);
                        kill(s_pid[8], SIGUSR1);
                    }
                    else
                    {
                        semaphore_signal(semid[0], 0);
                    }

                }
            } //else{
            //semaphore_signal2(semid[1], 0);
            //}
        }
        else if (strcmp(argv[1], "P23") == 0)
        {
            //sleep(2);
            /* make the child  sensitive to SIGUSR2 signal*/
            if (signal(SIGUSR2, stopAttack) == SIG_ERR) //signal handling, returns -1 (SIG_ERR) on failure
            {
                perror("Signal can not set SIGUSR2");
                exit(SIGUSR2);
            }
            // semaphore_wait2(semid[1], 0);
            if (memptr4 -> bagCapacity != 0)
            {
                //semaphore_signal2(semid[1], 0);
                isAnnoyed = (int)((rand() % (1)));

                if (isAnnoyed != 0)
                {


                    srand(time(0));

                    int annoy = (int)(1 + (rand() % (tm1 ->bagOFannoyed2.waterCapacity - 1))) ; //randomize how much to steal from p11
                    semaphore_wait2(semid[1], 0);
                    if (memptr4 -> bagCapacity > 0)
                    {
                        // cyan3();
                        // printf("----------------- The enemy teams (2) P23 start annoying the player P11 and Steal his water -----------------\n");
                        // normals();
                        // value of bag that process annoys
                        if (memptr4 -> bagCapacity - annoy <= 0)

                        {
                            memptr4 -> bagCapacity = 0;
                            blue();
                            printf("P23 Annoyed P11 and Took {%d %%} From P11's bag\n", annoy);
                            printf("P11 bo back to Tank [A]\n");
                            fflush(stdout);
                        }
                        else
                        {
                            memptr4 -> bagCapacity = memptr4 -> bagCapacity - annoy; //p23 steals from p11
                            blue();
                            printf("P22 Annoyed P11 and Took {%d %%} From P11's bag\n", annoy);
                            normals();
                            fflush(stdout);
                        }
                        semaphore_signal2(semid[1], 0);
                        // value of the water in the bag of the annoying process
                        tm1 ->bagOFannoyed2.waterCapacity = annoy;

                    }
                    else
                    {
                        semaphore_signal2(semid[1], 0);
                    }

                    semaphore_wait(semid[0], 0);
                    if (memptr -> tankACapacity != 0 && memptr -> tankACapacity <= (TANK_AC_CAPACITY - annoy))
                    {
                        memptr -> tankACapacity = memptr -> tankACapacity + annoy; //p23 returns the amount that stole in tank A
                        blue();
                        printf("P23 Returned {%d %%} to Tank [A], Tank [A] new Capacity is {%d %%}\n", annoy, memptr -> tankACapacity);
                        normals();
                        fflush(stdout);
                        semaphore_signal(semid[0], 0);

                    }
                    else if (memptr -> tankACapacity == 0)
                    {
                        semaphore_signal(semid[0], 0);
                        red();
                        printf("Tank [A] is Empty\n");
                        normals();
                        fflush(stdout);
                        kill(s_pid[8], SIGUSR1);
                    }
                    else
                    {
                        semaphore_signal(semid[0], 0);
                    }


                }
            }
            //else{
            //semaphore_signal2(semid[1], 0);
            //}

        }
        else if (strcmp(argv[1], "P12") == 0)
        {
            //sleep(2);
            //semaphore_wait4(semid[3], 0);
            if (signal(SIGUSR2, stopAttack) == SIG_ERR) //signal handling, returns -1 (SIG_ERR) on failure
            {
                perror("Signal can not set SIGUSR2");
                exit(SIGUSR2);
            }
            if (memptr5 -> bag21Capacity != 0)
            {
                semaphore_signal4(semid[3], 0);
                isAnnoyed = (int)((rand() % (2)));

                if (isAnnoyed > 0)
                {

                    srand(time(0));

                    int annoy = (int)(1 + (rand() % (tm2->bagOFannoyed1.waterCapacity - 1))) ;//randomize the amount of stealing

                    semaphore_wait4(semid[3], 0);
                    if (memptr5 -> bag21Capacity > 0)
                    {
                        //cyan3();
                        // printf("********The enemy teams (1) P12 start annoying the player P21 and Steal his water *****************\n");
                        //normals();

                        // value of bag that process annoys
                        if (memptr5 -> bag21Capacity - annoy <= 0)

                        {
                            memptr5 -> bag21Capacity = 0;
                            blue();
                            printf("P12 Annoyed P21 and Took {%d %%} From P21's bag\n", annoy);
                            printf("P21 bo back to Tank [C]\n");
                            normals();
                            fflush(stdout);
                        }
                        else
                        {
                            memptr5 -> bag21Capacity = memptr5 -> bag21Capacity - annoy; //p12 steals from p22
                            blue();
                            printf("P12 Annoyed P21 and Took {%d %%} From P21's bag\n", annoy);
                            normals();
                            fflush(stdout);
                        }

                        semaphore_signal4(semid[3], 0);
                        // value of the water in the bag of the annoying process
                        tm2->bagOFannoyed1.waterCapacity = annoy;


                    }
                    else
                    {
                        semaphore_signal4(semid[3], 0);
                    }

                    semaphore_wait3(semid[2], 0);
                    if (memptr2 -> tankCCapacity != 0 && memptr2 -> tankCCapacity <= (TANK_AC_CAPACITY - annoy))
                    {
                        memptr2 -> tankCCapacity = memptr2 -> tankCCapacity + annoy; //p12 fill tank C with what it stole
                        blue();
                        printf("P12 Returned {%d %%} to Tank [C], Tank [C] new Capacity is {%d %%}\n",  annoy, memptr2 -> tankCCapacity);
                        normals();
                        fflush(stdout);
                        semaphore_signal3(semid[2], 0);

                    }
                    else if (memptr2 -> tankCCapacity == 0)
                    {
                        semaphore_signal3(semid[2], 0);
                        red();
                        printf("Tank [C] is Empty\n");
                        normals();
                        fflush(stdout);
                        kill(s_pid[8], SIGUSR1);
                    }
                    else
                    {
                        semaphore_signal3(semid[2], 0);
                    }

                }
            }
            // else{
            //semaphore_signal4(semid[3], 0);
            //}
        }
        else if (strcmp(argv[1], "P13") == 0)
        {
            //sleep(2);
            //semaphore_wait4(semid[3], 0);
            if (signal(SIGUSR2, stopAttack) == SIG_ERR) //signal handling, returns -1 (SIG_ERR) on failure
            {
                perror("Signal can not set SIGUSR2");
                exit(SIGUSR2);
            }
            if (memptr5 -> bag21Capacity != 0)
            {
                //semaphore_signal4(semid[3], 0);
                isAnnoyed = (int)((rand() % (2)));

                if (isAnnoyed > 0)
                {

                    srand(time(0));
                    int annoy = (int)(1 + (rand() % (tm2->bagOFannoyed2.waterCapacity - 1))) ; //value of how much to steal
                    semaphore_wait4(semid[3], 0);
                    if (memptr5 -> bag21Capacity > 0)
                    {
                        //cyan3();
                        //printf("\t***************** The enemy teams (1) P13 start annoying the player P21 and Steal his water ***************** \n\n");
                        //normals();
                        //fflush(stdout);
                        // value of bag that process annoys
                        if (memptr5 -> bag21Capacity - annoy <= 0)

                        {
                            memptr5 -> bag21Capacity = 0;
                            blue();
                            printf("P13 Annoyed P21 and Took {%d %%} From P21's bag\n", annoy);
                            normals();
                            fflush(stdout);
                        }
                        else
                        {
                            memptr5 -> bag21Capacity = memptr5 -> bag21Capacity - annoy; //p13 steals from p21
                            blue();
                            printf("P13 Annoyed P21 and Took {%d %%} From P21's bag\n", annoy);
                            normals();
                            fflush(stdout);
                        }
                        semaphore_signal4(semid[3], 0);
                        // value of the water in the bag of the annoying process
                        tm2->bagOFannoyed2.waterCapacity = annoy;

                    }
                    else
                    {
                        semaphore_signal4(semid[3], 0);
                    }
                    semaphore_wait3(semid[2], 0);
                    if (memptr2 -> tankCCapacity != 0 && memptr2 -> tankCCapacity <= (TANK_AC_CAPACITY - annoy))
                    {
                        memptr2 -> tankCCapacity = memptr2 -> tankCCapacity + annoy; //p13 return the amount it stole to tank C
                        blue();
                        printf("P13 Returned {%d %%} to Tank [C], Tank [C] new Capacity is {%d %%}\n", annoy, memptr2 -> tankCCapacity);
                        fflush(stdout);
                        normals();
                        semaphore_signal3(semid[2], 0);
                    }
                    else if (memptr2 -> tankCCapacity == 0)
                    {
                        semaphore_signal3(semid[2], 0);
                        red();
                        printf("Tank [C] is Empty\n");
                        normals();
                        fflush(stdout);
                        kill(s_pid[8], SIGUSR1);

                    }
                    else
                    {
                        semaphore_signal3(semid[2], 0);
                    }

                }
            }
            // else{
            // semaphore_signal4(semid[3], 0);
            //}

        }
        else if (strcmp(argv[1], "P14") == 0)
        {
            //sleep(2);
            srand(time(0));
            int victim = (int)(1 + (rand() % (3 - 1))); //choose the process tu push one at a time

            if (victim == 1)   //p22
            {
                int isAnnoyed = (int)((rand() % (2)));
                if (isAnnoyed > 0)
                {

                    kill(s_pid[6], SIGUSR2); //send signal to the other process that want to push back p14
                    sigpause(SIGUSR1); //free the pause
                    /* recieve message to queue */
                    struct message message;
                    if (msgrcv(queue_IDs[6], (char * ) & message, sizeof(message), 0, 0) != -1)
                    {
                        if (strcmp(message.mtext, "1") == 0)   //if message in queue is 1
                        {
                            yellow();
                            printf("----------------- P23 PUSHED P14-----------------\n");
                            normals();
                            fflush(stdout);
                        }
                        else
                        {
                            yellow();
                            printf("----------------- P23 FAILED TO PUSHED P14 -----------------\n");
                            normals();
                            fflush(stdout);
                            //if it ailed to push back p14 , now p14 can put sand in p22
                            int  sand = (1 + (rand() % (int)( abs ( tm1 ->bagOFannoyed1.waterCapacity - tm1 ->bagOFannoyed1.sandCapacity) - 1))); ///randomize the value of sand
                            tm1 ->bagOFannoyed1.sandCapacity = sand; //put sand in the bag
                            //now it can steal water in relation to how much sand exists
                            green();
                            printf("p14 put sand in p22 bag and its sand capacity now {%d %%}\n", tm1 ->bagOFannoyed1.sandCapacity);
                            normals();
                            fflush(stdout);
                            tm1 ->bagOFannoyed1.waterCapacity =  tm1 ->bagOFannoyed1.waterCapacity -  tm1 ->bagOFannoyed1.sandCapacity;
                            if ( tm1 ->bagOFannoyed1.waterCapacity  <=0)
                            {
                                tm1 ->bagOFannoyed1.waterCapacity =5;
                            }
                            green();
                            printf("p22 water bag capacity after putting sand is {%d %%}\n",  tm1 ->bagOFannoyed1.waterCapacity);
                            normals();
                            fflush(stdout);
                        }
                    }
                }

            }
            else     //p23
            {
                int isAnnoyed = (int)((rand() % (2)));
                if (isAnnoyed > 0)
                {
                    kill(s_pid[5], SIGUSR2); //send signal to p22
                    sigpause(SIGUSR1);
                    /* recieve message to queue */
                    struct message message;
                    if (msgrcv(queue_IDs[5], (char * ) & message, sizeof(message), 0, 0) != -1)
                    {
                        if (strcmp(message.mtext, "1") == 0)
                        {
                            yellow();
                            printf("----------------- P22 PUSHED P14 -----------------\n");
                            normals();
                            fflush(stdout);
                        }
                        else
                        {
                            yellow();
                            printf("----------------- P22 FAILED TO PUSHED P14 -----------------\n");
                            normals();
                            fflush(stdout);
                            // p22 failed to push p14 , p14 puts sand in p23 bag
                            int sand = (int)(1 + (rand() % (int)(abs (tm1 ->bagOFannoyed2.waterCapacity - tm1 ->bagOFannoyed2.sandCapacity) - 1))); ///randomize the value of sand
                            tm1 ->bagOFannoyed2.sandCapacity = sand; //put sand in the bag
                            //now it can steal water in relation to how much sande exists
                            Purple();
                            printf("p14 put sand in p23 bag and its sand capacity now {%d %%}\n", tm1 ->bagOFannoyed2.sandCapacity);
                            normals();
                            fflush(stdout);
                            tm1 ->bagOFannoyed2.waterCapacity = tm1 ->bagOFannoyed2.waterCapacity - tm1 ->bagOFannoyed2.sandCapacity;
                            if ( tm1 ->bagOFannoyed2.waterCapacity  <=0)
                            {
                                tm1 ->bagOFannoyed2.waterCapacity =5;
                            }
                            blue();
                            printf("p23 water bag capacity after putting sand is {%d %%}\n", tm1 ->bagOFannoyed2.waterCapacity);
                            normals();
                            fflush(stdout);
                        }

                    }

                }
            }
        }

        else if (strcmp(argv[1], "P24") == 0)
        {
            //sleep(2);
            srand(time(0));
            int victim = (int)(1 + (rand() % (3 - 1)));

            if (victim == 1)   //p12
            {
                int isAnnoyed = (int)((rand() % (2)));
                if (isAnnoyed > 0)
                {

                    kill(s_pid[2], SIGUSR2); // send signal to p13
                    sigpause(SIGUSR1);
                    /* send message to queue */
                    struct message message;
                    if (msgrcv(queue_IDs[2], (char * ) & message, sizeof(message), 0, 0) != -1)
                    {
                        if (strcmp(message.mtext, "1") == 0)
                        {
                            yellow();
                            printf("----------------- P13 PUSHED P24 ----------------- \n");
                            normals();
                            fflush(stdout);
                        }
                        else
                        {
                            yellow();
                            printf("----------------- P13 FAILED TO PUSHED P24-----------------\n");
                            normals();
                            fflush(stdout);
                            //p13 failed to push p24 , now p24 puts sand in p12
                            int sand = (1 + (rand() % (int)(abs (tm2->bagOFannoyed1.waterCapacity - tm2->bagOFannoyed1.sandCapacity) - 1))); ///randomize the value of sand
                            tm2->bagOFannoyed1.sandCapacity = sand; //put sand in the bag
                            //now it can steal water in relation to how much sande exists
                            green();
                            printf("p24 put sand in p12 bag and its sand capacity now {%d %%}\n", tm2->bagOFannoyed1.sandCapacity);
                            normals();
                            fflush(stdout);
                            tm2->bagOFannoyed1.waterCapacity = tm2->bagOFannoyed1.waterCapacity - tm2->bagOFannoyed1.sandCapacity;
                            if ( tm2 ->bagOFannoyed1.waterCapacity  <=0)
                            {
                                tm2 ->bagOFannoyed1.waterCapacity =5;
                            }
                            green();
                            printf("p12 water bag capacity after putting sand is {%d %%}\n", tm2->bagOFannoyed1.waterCapacity);
                            normals();
                            fflush(stdout);
                        }
                    }
                }

            }
            else     //p13
            {
                int isAnnoyed = (int)((rand() % (2)));
                if (isAnnoyed > 0)
                {
                    kill(s_pid[1], SIGUSR2); //send signal to p12
                    sigpause(SIGUSR1);
                    /* send message to queue */
                    struct message message;
                    if (msgrcv(queue_IDs[1], (char * ) & message, sizeof(message), 0, 0) != -1)
                    {
                        if (strcmp(message.mtext, "1") == 0)
                        {
                            yellow();
                            printf("----------------- P12 PUSHED P24 -----------------\n");
                            normals();
                            fflush(stdout);
                        }
                        else
                        {
                            yellow();
                            printf("----------------- P12 FAILED TO PUSHED P24 -----------------\n");
                            normals();
                            fflush(stdout);
                            // p12 failed to push p24 , p24 puts sand in p13 bag
                            int sand = (int)(1 + (rand() % (int)(abs (tm2->bagOFannoyed2.waterCapacity  - tm2->bagOFannoyed2.sandCapacity) - 1))); ///randomize the value of sand
                            tm2->bagOFannoyed2.sandCapacity = sand; //put sand in the bag
                            //now it can steal water in relation to how much sande exists
                            Purple();
                            printf("p24 put sand in p13 bag and its sand capacity now {%d %%}\n", tm2->bagOFannoyed2.sandCapacity);
                            normals();
                            fflush(stdout);
                            tm2->bagOFannoyed2.waterCapacity = tm2->bagOFannoyed2.waterCapacity - tm2->bagOFannoyed2.sandCapacity;
                            if ( tm2 ->bagOFannoyed2.waterCapacity  <=0)
                            {
                                tm2 ->bagOFannoyed2.waterCapacity =5;
                            }
                            blue();
                            printf("p13 water bag capacity after putting sand is {%d %%}\n", tm2->bagOFannoyed2.waterCapacity);
                            normals();
                            fflush(stdout);
                        }

                    }

                }
            }

        }
        else if (strcmp(argv[1], "R") == 0)
        {

            sigpause(SIGUSR1);

            ref.winner[0] = ref.winner[1]; //shift the value of the winnig round to detect 3 round in row
            ref.winner[1] = ref.winner[2];
            if ( tm1 ->tankBCapacity > tm2->tankDCapacity)   //check the capacity of two tanks to see the winner
            {
                ref.winner[2] = 1; //team 1
                Purple2();
                printf("The Winner of This Round is Team [%d], Team[1] Tank {%d %%}  While Team[2] Tank {%d %%}\n", ref.winner[2], tm1 ->tankBCapacity, tm2->tankDCapacity);
                printf("------------------------------------------------New Round------------------------------------------\n\n");
                normals();
                fflush(stdout);
            }
            else if (tm1 ->tankBCapacity < tm2->tankDCapacity)
            {
                ref.winner[2] = 2; //team 2
                Purple2();
                printf("The Winner of This Round is Team [%d], Team[1] Tank {%d %%} While Team[2] Tank {%d %%}\n", ref.winner[2], tm1 ->tankBCapacity,tm2->tankDCapacity);
                printf("------------------------------------------------New Round------------------------------------------\n\n");
                normals();
                fflush(stdout);
            }
            else
            {
                ref.winner[2] = 0;
                Purple2();
                printf("It Was A Draw Between The Two Teams, Team[1] Tank {%d %%} While Team[2] Tank {%d %%} \n\n", tm1 ->tankBCapacity, tm2->tankDCapacity);
                printf("------------------------------------------------New Round------------------------------------------\n\n");
                normals();
                fflush(stdout);
            }

            if (ref.winner[0] == 1 && ref.winner[1] == 1 && ref.winner[2] == 1)
            {
                memptr3 -> flagW = 1;
                red3();
                printf("The Winner of This Game is {Team 1} \n");
                normals();
                fflush(stdout);
                break;

            }
            else if (ref.winner[0] == 2 && ref.winner[1] == 2 && ref.winner[2] == 2)
            {
                red2();
                printf("The Winner of This Game is {Team 2} \n");
                normals();
                fflush(stdout);
                memptr3 -> flagW = 1;
                break;
            }

            if (memptr3 -> flagW == 0)   //for the next round re-initialize the values of tanks and bags
            {
                timerC2D = 0;
                semaphore_wait3(semid[2], 0);
                memptr2 -> tankCCapacity = TANK_AC_CAPACITY;
                semaphore_signal3(semid[2], 0);
                tm2->tankDCapacity = 0;

                semaphore_wait4(semid[3], 0);
                memptr5 -> bag21Capacity = 0;
                semaphore_signal4(semid[3], 0);
                tm2->bagOFannoyed1.waterCapacity = 5;
                tm2->bagOFannoyed1.sandCapacity = 5;
                tm2->bagOFannoyed2.waterCapacity = 5;
                tm2->bagOFannoyed2.sandCapacity = 5;

                timerA2B = 0;
                semaphore_wait(semid[0], 0);
                memptr -> tankACapacity = TANK_AC_CAPACITY;
                semaphore_signal(semid[0], 0);
                tm1 ->tankBCapacity = 0;

                semaphore_wait2(semid[1], 0);
                memptr4 -> bagCapacity = 0;
                semaphore_signal2(semid[1], 0);
                tm1 ->bagOFannoyed1.waterCapacity = 5;
                tm1 ->bagOFannoyed1.sandCapacity = 5;
                tm1 ->bagOFannoyed2.waterCapacity = 5;
                tm1 ->bagOFannoyed2.sandCapacity = 5;

                alarm(25);
            }
        }

    }
    return 0;

}


// semaphores
void semaphore_wait(int sem_id, int sem_num)
{

    acquire.sem_num = tankACapacity;
    if (semop(sem_id, & acquire, 1) == -1)
    {
        perror("semop -- modify -- waiting for consumer to modify number");
        exit(3);
    }
}

void semaphore_signal(int sem_id, int sem_num)
{

    release.sem_num = tankACapacity;
    if (semop(sem_id, & release, 1) == -1)
    {
        perror("semop -- modify -- indicating new number has been modified");
        exit(5);
    }
}

void semaphore_wait2(int sem_id, int sem_num)
{

    acquire2.sem_num = bagCapacity;
    if (semop(sem_id, & acquire2, 1) == -1)
    {
        perror("semop2 -- modify -- waiting for consumer to modify number");
        exit(6);
    }
}

void semaphore_signal2(int sem_id, int sem_num)
{

    release2.sem_num = bagCapacity;
    if (semop(sem_id, & release2, 1) == -1)
    {
        perror("semop2 -- modify -- indicating new number has been modified");
        exit(7);
    }
}

void semaphore_wait3(int sem_id, int sem_num)
{
    acquire3.sem_num = tankCCapacity;
    if (semop(sem_id, & acquire3, 1) == -1)
    {
        perror("semop3 -- modify -- waiting for consumer to modify number");
        exit(3);
    }
}

void semaphore_signal3(int sem_id, int sem_num)
{

    release3.sem_num = tankCCapacity;
    if (semop(sem_id, & release3, 1) == -1)
    {
        perror("semop3 -- modify -- indicating new number has been modified");
        exit(5);
    }
}
void semaphore_wait4(int sem_id, int sem_num)
{

    acquire4.sem_num = bagCapacity2;
    if (semop(sem_id, & acquire4, 1) == -1)
    {
        perror("semop4 -- modify -- waiting for consumer to modify number");
        exit(6);
    }
}

void semaphore_signal4(int sem_id, int sem_num)
{

    release4.sem_num = bagCapacity2;
    if (semop(sem_id, & release4, 1) == -1)
    {
        perror("semop4 -- modify -- indicating new number has been modified");
        exit(7);
    }
}

/*set flag to 1 to make sure that the process dont pause again*/
void sigset_catcher2()
{
    flag = 1;
}
/// to check if it can push or not
void stopAttack()
{
    srand(time(0));
    int push = (int)((rand() % (2)));

    struct message message;
    message.mtype = 1;
    memset( & (message.mtext), 0, sizeof(message.mtext));
    if (push == 1)   //if the other process can push the process with sand
    {

        (void) strcpy(message.mtext, "1"); //write on their message queues 1 according to the id of the process that wants to push
        if (getpid() == s_pid[5])
        {
            /* send message to queue */
            if (msgsnd(queue_IDs[5], (char * ) & message, sizeof(message), 0) == -1)
            {
                perror("msgsnd");
                exit(11);
            }
            kill(s_pid[3], SIGUSR1);
        }
        else if (getpid() == s_pid[6])
        {
            /* send message to queue */
            if (msgsnd(queue_IDs[6], (char * ) & message, sizeof(message), 0) == -1)
            {
                perror("msgsnd");
                exit(17);
            }
            kill(s_pid[3], SIGUSR1);
        }

        else if (getpid() == s_pid[1])
        {
            /* send message to queue */
            if (msgsnd(queue_IDs[1], (char * ) & message, sizeof(message), 0) == -1)
            {
                perror("msgsnd");
                exit(17);
            }
            kill(s_pid[7], SIGUSR1);
        }

        else if (getpid() == s_pid[2])
        {
            /* send message to queue */
            if (msgsnd(queue_IDs[2], (char * ) & message, sizeof(message), 0) == -1)
            {
                perror("msgsnd");
                exit(17);
            }
            kill(s_pid[7], SIGUSR1);
        }
    }
    else
    {

        (void) strcpy(message.mtext, "0"); // else write zero if can't push

        /* send message to queue */
        if (getpid() == s_pid[5])
        {
            /* send message to queue */
            if (msgsnd(queue_IDs[5], (char * ) & message, sizeof(message), 0) == -1)
            {
                perror("msgsnd");
                exit(11);
            }
            kill(s_pid[3], SIGUSR1);
        }
        else if (getpid() == s_pid[6])
        {
            /* send message to queue */
            if (msgsnd(queue_IDs[6], (char * ) & message, sizeof(message), 0) == -1)
            {
                perror("msgsnd");
                exit(17);
            }
            kill(s_pid[3], SIGUSR1);
        }

        else if (getpid() == s_pid[1])
        {
            /* send message to queue */
            if (msgsnd(queue_IDs[1], (char * ) & message, sizeof(message), 0) == -1)
            {
                perror("msgsnd");
                exit(17);
            }
            kill(s_pid[7], SIGUSR1);
        }

        else if (getpid() == s_pid[2])
        {
            /* send message to queue */
            if (msgsnd(queue_IDs[2], (char * ) & message, sizeof(message), 0) == -1)
            {
                perror("msgsnd");
                exit(17);
            }
            kill(s_pid[7], SIGUSR1);
        }
    }
    //kill(s_pid[3], SIGUSR1);
}

void alarmHandler()
{

    if (getpid() == s_pid[7])
    {
        red2();
        printf("An alarm clock signal was received\n");
        normals();
        fflush(stdout);
        alarm(25);
        kill(s_pid[8], SIGUSR1);

    }

}
