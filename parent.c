/*
 * The fork() function
 */

#include "local.h"


int counter = 0,statu = 0;

char * pidFile = "PID.txt";
MESSAGE message;
static struct MEMORY memory;
static struct MEMORY2 memory2;
static struct WIN memory3;
static struct MEMORY3 memory4;
static struct MEMORY4 memory5;
static struct TEAM1 memory6;
static struct TEAM2 memory7;


union semun arg, arg1;

void sigset_catcher(int);
void setTimeout(int);
int i, status;
pid_t pid, pid_array[10]; //array to store all pid's of forked children


int main()
{

    int shmid[7];
    int semid[4];
    char * shmptr[7];
    FILE * fptr_queue_IDs;
    pid_t pid = getpid();

    /* set this signal to the handler function sigset_catcher*/
    sigset(SIGUSR1, sigset_catcher);

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< create message queue >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>...
    //  message queue is created
    int mid[9]; // queue for each officer
    char u_char = 'A';
    int key;
    for (int l = 0; l < 9; l++, u_char++)
    {
        key = ftok(".", u_char);
        // create message queue
        if ((mid[l] = msgget(key, IPC_CREAT | 0666)) == -1)
        {
            perror("Queue create");
            exit(-5);
        }
    }
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< write message queue IDs on a file >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

    if ((fptr_queue_IDs = fopen(QUEUE_IDs_FILE, "w")) == NULL)
    {
        perror("QUEUE_IDs_FILE");
        exit(-3);
    }
    for (int i = 0; i < 9; i++)
    {
        fprintf(fptr_queue_IDs, "%d\n", mid[i]);
    }
    fclose(fptr_queue_IDs); //close the file

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< create shared memory & semaphores >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

    if ((shmid[0] = shmget((int) pid, sizeof(memory), IPC_CREAT | 0660)) != -1)
    {
        if ((shmptr[0] = (char * ) shmat(shmid[0], 0, 0)) == (char * ) - 1) // 0: means start from 0
        {
            perror("shmptr -- parent -- attach");
            exit(-3);
        }
        memcpy(shmptr[0], (char * ) & memory, sizeof(memory));
    }
    else
    {
        perror("shmid -- parent -- creation");
        exit(-22);
    }

    if ((shmid[1] = shmget((int) pid + 1, sizeof(memory2), IPC_CREAT | 0660)) != -1)
    {
        if ((shmptr[1] = (char * ) shmat(shmid[1], 0, 0)) == (char * ) - 1) // 0: means start from 0
        {
            perror("shmptr2 -- parent -- attach");
            exit(-4);
        }
        memcpy(shmptr[1], (char * ) & memory2, sizeof(memory2));
    }
    else
    {
        perror("shmid2 -- parent -- creation");
        exit(-23);
    }

    if ((shmid[2] = shmget((int) pid + 2, sizeof(memory3), IPC_CREAT | 0660)) != -1)
    {
        if ((shmptr[2] = (char * ) shmat(shmid[2], 0, 0)) == (char * ) - 1) // 0: means start from 0
        {
            perror("shmptr3 -- parent -- attach");
            exit(-4);
        }
        memcpy(shmptr[2], (char * ) & memory3, sizeof(memory3));
    }
    else
    {
        perror("shmid3 -- parent -- creation");
        exit(-23);
    }

    if ((shmid[3] = shmget((int) pid + 3, sizeof(memory4), IPC_CREAT | 0660)) != -1)
    {
        if ((shmptr[3] = (char * ) shmat(shmid[3], 0, 0)) == (char * ) - 1) // 0: means start from 0
        {
            perror("shmptr4 -- parent -- attach");
            exit(-4);
        }
        memcpy(shmptr[3], (char * ) & memory4, sizeof(memory4));
    }
    else
    {
        perror("shmid4 -- parent -- creation");
        exit(-23);
    }

    if ((shmid[4] = shmget((int) pid + 4, sizeof(memory5), IPC_CREAT | 0660)) != -1)
    {
        if ((shmptr[4] = (char * ) shmat(shmid[4], 0, 0)) == (char * ) - 1) // 0: means start from 0
        {
            perror("shmptr5 -- parent -- attach");
            exit(-4);
        }
        memcpy(shmptr[4], (char * ) & memory5, sizeof(memory5));
    }
    else
    {
        perror("shmid5 -- parent -- creation");
        exit(-23);
    }
    if ((shmid[5] = shmget((int) pid + 5, sizeof(memory6), IPC_CREAT | 0660)) != -1)
    {
        if ((shmptr[5] = (char * ) shmat(shmid[5], 0, 0)) == (char * ) - 1) // 0: means start from 0
        {
            perror("shmptr6 -- parent -- attach");
            exit(-4);
        }
        memcpy(shmptr[5], (char * ) & memory6, sizeof(memory6));
    }
    else
    {
        perror("shmid6 -- parent -- creation");
        exit(-23);
    }
    if ((shmid[6] = shmget((int) pid + 6, sizeof(memory7), IPC_CREAT | 0660)) != -1)
    {
        if ((shmptr[6] = (char * ) shmat(shmid[6], 0, 0)) == (char * ) - 1) // 0: means start from 0
        {
            perror("shmptr7 -- parent -- attach");
            exit(-4);
        }
        memcpy(shmptr[6], (char * ) & memory7, sizeof(memory7));
    }
    else
    {
        perror("shmid6 -- parent -- creation");
        exit(-23);
    }
    /*
     * Create and initialize the semaphores for writing on memory
     */
    static ushort start_val[2] =
    {
        1,
        0
    };

    for (int k = 0; k <= 3; k++)
    {
        if ((semid[k] = semget((int) pid + k, 1, IPC_CREAT | 0666)) != -1)
        {
            arg.array = start_val;

            if (semctl(semid[k], 0, SETALL, arg) == -1)
            {
                perror("semctl -- parent -- initialization");
                exit(-10);
            }
        }
        else
        {
            perror("semget -- parent -- creation");
            exit(-44);
        }
    }

    // Parent creates the pipe to talk to all soldiers
    // if (pipe(f_des) == -1)
    //{
    //perror("Pipe");
    //exit(-1);
    // }
    //this for loop is to  create (fork) 10 children
    printf("\n\t\t\t\t\t\t\t\t\tWaiting for all process to fork successfully\n\n");
    fflush(stdout);
    for (i = 0; i < 9; i++)

    {
        pid = fork(); //create a child
        if (pid == -1)
            exit(-1); //exit if there was a problem when creating
        /*if pid returns 0 then we are in the child process*/
        else if (pid == 0)
        {
            // printf("I am a child with => PID = %d\n", getpid());
            // fflush(stdout);
            /*making children sensitive to the user defind signal SIGUSR1*/
            while (1)
            {
                sigpause(SIGUSR1); /* Wait for USR1 signal */
            }
            exit(-2);
        }
        else
        {

            // counter that stores number of children that were created
            if (counter < 4 )
            {
                yellow();
                printf("\n\n\t\t\t\t\t\t\t\tI am the parent => PID = %d, child ID = %d in team 1 \n", getpid(), pid);
                normals();
                fflush(stdout);
            }
            else if(counter >= 4 && counter <8 )
            {
                Purple();
                printf("\n\n\t\t\t\t\t\t\t\tI am the parent => PID = %d, child ID = %d in team2 \n", getpid(), pid);
                normals();
                fflush(stdout);
            }
            else if (counter == 8)
            {
                cyan();
                printf("\n\n\t\t\t\t\t\t\t\tI am the parent => PID = %d, child ID = %d in R \n", getpid(), pid);
                normals();
                fflush(stdout);
            }
            counter++;


            srand(time(0));

            pid_array[i] = pid; //make the parent store the pid of every forked child
            /* when counter is 10 this means that all 10 children has been forked */
            if (counter == 9)
            {


                // print the  pid of all proccces in file
                FILE * fp2 = fopen(pidFile, "w");
                if (fp2 != NULL)
                {
                    for (int row = 0; row < 9; row++)
                    {
                        fprintf(fp2, "%d\n", pid_array[row]);
                    }
                    fclose(fp2);
                }

                printf( "\n\t\t\t\t\t\t\t\t\tThe Game is starting simulation \n ");
                red();
                printf("\t\t***********************************************************************************************************************************************\n\n\n");
                normals();
                fflush(stdout);
                for (int j = 0; j < 9; j++)
                {
                    sleep(1);
                    kill(pid_array[j], SIGUSR1); //send signal to all children to inform them that all children were created and know they can exec
                    /*this is to make sure that the text file that contains the battlefied array is written and now all children can read it*/

                }

            }
        }
    }

    //kill(pid_array[8], SIGUSR1);
    // while (1) {

    normals();

    if (waitpid(pid_array[8], & status, 0) == pid_array[8])
    {
        Purple();
        printf("\tlana abu hammad 1181219 && ireen hinnawi 1180532 && qossay zeineddin 1180235\n");
        normals();
        fflush(stdout);
    }

    for(int j = 0 ; j < 9 ; j++ )
    {
        kill(pid_array[j],SIGKILL);//signal to kill the remainig members of the enemy team

    }
    for (int k = 0; k <= 6; k++)
    {
        // delete the shared memory
        if (shmdt(shmptr[k]) == -1)
        {
            perror("shmdt:"); // remove
            exit(-5);
        }

        // remove shared memory
        if (shmctl(shmid[k], IPC_RMID, (struct shmid_ds * ) 0))
        {
            perror("shmctl: IPC_RMID"); /* remove shared memory */
            exit(-5);
        }

    }
    printf("\n");
    cyan3();
    printf("\tThe shared memoryes that was created  deleted successfully\n");
    normals();
    for (int k = 0; k <= 3; k++)
    {
        // remove sempahore
        if (semctl(semid[k], 0, IPC_RMID, 0) == -1)
        {
            perror("semctl: IPC_RMID"); /* remove semaphore */
            exit(-6);
        }
    }

    cyan3();
    printf("\tThe sempahores that was created  deleted successfully\n");
    normals();
    // remove message queues
    for (int m = 0; m < 9; m++)
    {
        if (msgctl(mid[m], IPC_RMID, (struct msqid_ds * ) 0) == -1)
        {
            perror("msgctl: IPC_RMID"); // remove message queue
            exit(-6);
        }
    }
    cyan3();
    printf("\tThe message queues that was created  deleted successfully\n");
    normals();

    if(remove("queue_IDs.txt") != -1 &&  remove("PID.txt") != -1 )
    {
        cyan3();
        printf("\tThe files that was created  deleted successfully\n\n");
        normals();
    }

    printf("\n");
    normals();
    fflush(stdout);
    return (0);
}
/*this signal catcher is executed when all the children are created we assumed thet the first 4 pid's are for the first team and the other 4 are for the second team and last for the refree
when we exec  to the teams c file we send the counter of the child with its pids */

void sigset_catcher(int n)
{

    //printf("Beginnin important stuff  %d\n",counter);
    //fflush(stdout);
    char tmp1[5];
    sprintf(tmp1, "%d", counter);
    char tmp2[4];
    if (counter < 4)
    {
        sprintf(tmp2, "P1%d", counter + 1);
        char * arg_list[] =
        {
            "",
            "",
            NULL
        };
        arg_list[0] = tmp1;
        arg_list[1] = tmp2;

        execv("./teams", arg_list);
        perror("execv -- team1");
        exit(-3);
    }
    else if (counter >= 4 && counter != 8)
    {

        sprintf(tmp2, "P2%d", counter - 3);
        char * arg_list[] =
        {
            "",
            "",
            NULL
        };
        arg_list[0] = tmp1;
        arg_list[1] = tmp2;

        execv("./teams", arg_list);
        perror("execv -- team2");
        exit(-4);
    }
    else
    {

        char * arg_list[] =
        {
            "",
            "R",
            NULL
        };
        arg_list[0] = tmp1;


        execv("./teams", arg_list);
        perror("execv -- team2");
        exit(-4);

    };

    // printf("Beginnin important stuff  %d\n",getpid());
    // fflush(stdout);
}
