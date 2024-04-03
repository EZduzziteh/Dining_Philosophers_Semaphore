#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <stdbool.h>
#include <pthread.h>

//Name: Dining Philosophers Problem
//Author: Sasha Greene
//School: University of Lethbridge
//Course: Operating Systems

//Assignment 2

/*
Programming assignment #2 CPSC 3655 | Spring 2024
Dinning Philosophers’ problem
The dinning philosophers’ problem is a classic synchronization problem. Though, it does not
notably represent a real-world problem, it provides a significant learning value, particularly in
process synchronization. It is a simple representation of the need to allocate several
resources among several processes in a deadlock-free and starvation-free manner. There are
n philosophers dinning together at the same table. Each philosopher has their own place at
the table. There is a fork between each plate. The dish served is a kind of spaghetti which
has to be eaten with two forks. Each philosopher can only alternately think and eat.
Moreover, a philosopher can only eat their spaghetti when they have both a left and right
fork. Thus, two forks will only be available when their two nearest neighbors are thinking,
not eating. After an individual philosopher finishes eating, they will put down both forks.
An example where n = 5

Write a C program (phils.c) that solves the dinning philosophers’ problem where each
dinning philosopher is a modeled as a thread. You should use semaphores (not mutexes)
for synchronization. Your program will take two arguments: the number of philosophers at
the table and a number of times to eat for each philosopher.
Care must be taken to prevent a deadlock. To alleviate deadlock, implement the
“asymmetric solution”, that is, an odd philosopher picks up first a left chopstick and
then the right one, while an even philosopher picks up first a right chopstick and
then the left one.

Example of output:
$ ./dphil 7 10 //7 philosophers, each eats 10 times total.
Philosopher 0 is thinking...
Philosopher 1 is eating...
Philosopher 3 is thinking...
Philosopher 4 is thinking...



Programming assignment #2 CPSC 3655 | Spring 2024
Submit a single .c source code file of your solutions. Do not forget to write your name.
I should be able to compile your code as follows:
gcc -o phils phils.c -lpthread
Notes:
- The number of philosophers (therefore chopsticks) and the number of eat times
should be dynamic. A solution for a fix number of philosophers is not acceptable.
- As stated above, you need to use semaphores, not mutexes in your solution.
- Use the sleep(…) function to simulate eating and thinking.*/



//struct that holds the parameters for the philosopher function
struct PhilosopherParameters
{
    int philosopherID;
    int timesToEat;
    int philosopherCount;
    sem_t *forks;
};

void *Philosopher(void *arg)
{
    struct PhilosopherParameters *params = (struct PhilosopherParameters *)arg;

    //get parameters
    int philosopherID = params->philosopherID;
    int timesToEat = params->timesToEat;
    int philosopherCount = params->philosopherCount;
    sem_t *forks = params->forks;

    // initialize random
    time_t currentTime;
    srand((unsigned)time(&currentTime) * getpid());

    //left fork will be considered our id -1, right will jsut be our id.  
    //ex) philosopher 1(first) will pick up fork index 0 as his left and fork index 1 as his right
    //ex) philosopher 2 will pcik up fork index 1 as left and fork index 2 as right.
    //ex) philosopher 3(last) will pick up fork index 2 as his left and fork index 0 as right

    while (timesToEat > 0)
    {
        printf("Philosopher %d is thinking\n", philosopherID);
        sleep((rand() % 3) + 2); // wait 1-3 seconds

        if (philosopherID % 2 != 0)
        {
            // if our id is odd, pick up left fork first (our id -1, unless we are the first philosopher, then it will be the last fork)

            // pick up left fork
            
               // printf("Philosopher %d is picking up left fork...\n", philosopherID);
                int result = sem_trywait(&forks[philosopherID - 1]);
                if (result == -1)
                {
                    // couldnt get forks
               //     printf("Philosopher %d couldnt find left fork...\n", philosopherID);
                    continue;
                }

            // then pick up right fork (our id), unless we are the last philosopher, then its 0
            if (philosopherID >= philosopherCount)//if we are the last philosopher
            {
                //printf("Philosopher %d is picking up right fork...\n", philosopherID);
                int result = sem_trywait(&forks[0]);
                if (result == -1)
                {
                    // couldnt get right fork
                   // printf("Philosopher %d couldnt find right fork...\n", philosopherID);
                   // printf("Philosopher %d is putting down left fork...\n", philosopherID);
                    sem_post(&forks[philosopherID - 1]);
                    continue;
                }
            }
            else
            {
               // printf("Philosopher %d is picking up right fork...\n", philosopherID);
                int result = sem_trywait(&forks[philosopherID]);
                if (result == -1)
                {
                    // couldnt get right fork
                    // printf("Philosopher %d couldnt find right fork...\n", philosopherID);
                    // drop the left fork
                    // printf("Philosopher %d is putting down left fork...\n", philosopherID);
                    sem_post(&forks[philosopherID - 1]);
                    continue;
                }
            }

            // then eat
            printf("Philosopher %d is Eating \n", philosopherID);
            sleep((rand() % 3) + 2); // wait 1-3 seconds
            timesToEat--;

            // then release right fork
            if (philosopherID >= philosopherCount)
            {
               // printf("Philosopher %d is putting down right fork...\n", philosopherID);
                sem_post(&forks[0]);
            }
            else
            {
                //printf("Philosopher %d is putting down right fork...\n", philosopherID);
                sem_post(&forks[philosopherID]);
            }
            // then release left fork
           // printf("Philosopher %d is putting down left fork...\n", philosopherID);
            sem_post(&forks[philosopherID - 1]);
        }
        else
        {
            // otherwise, pick up right fork first. (our id), unless we are the last philosopher, then its 0)
            if (philosopherID >= philosopherCount)
            {
                //printf("Philosopher %d is picking up right fork...\n", philosopherID);
                int result = sem_trywait(&forks[0]);
                if (result == -1)
                {
                    // couldnt get forks
                    // printf("Philosopher %d couldnt find right fork...\n", philosopherID);
                    continue;
                }
            }
            else
            {
               // printf("Philosopher %d is picking up right fork...\n", philosopherID);
                int result = sem_trywait(&forks[philosopherID]);
                if (result == -1)
                {
                    // couldnt get forks
                   // printf("Philosopher %d couldnt find right fork...\n", philosopherID);
                    continue;
                }
            }

            // then pick up left fork
           
            //printf("Philosopher %d is picking up left fork...\n", philosopherID);
            int result = sem_trywait(&forks[philosopherID - 1]);
            if (result == -1)
            {
                // couldnt find left fork
               // printf("Philosopher %d couldnt find left fork...\n", philosopherID);
                // drop right fork
                if (philosopherID >= philosopherCount)
                {
                  //  printf("Philosopher %d is putting down right fork...\n", philosopherID);
                    sem_post(&forks[0]);
                }
                else
                {
                  //  printf("Philosopher %d is putting down right fork...\n", philosopherID);
                    sem_post(&forks[philosopherID]);
                }
                continue;
            }
            

            // then eat
            printf("Philosopher %d is Eating \n", philosopherID);
            sleep((rand() % 3) + 2); // wait 1-3 seconds
            timesToEat--;
            // then relase left fork
            //printf("Philosopher %d is putting down left fork...\n", philosopherID);
            sem_post(&forks[philosopherID - 1]);
            
            // then release right fork
            if (philosopherID >= philosopherCount)
            {
               // printf("Philosopher %d is putting down right fork...\n", philosopherID);
                sem_post(&forks[0]);
            }
            else
            {
               // printf("Philosopher %d is putting down right fork...\n", philosopherID);
                sem_post(&forks[philosopherID]);
            }
        }
    }
    printf("Philosopher %d is  all done eating! \n", philosopherID);
    return NULL;
}

int main(int argc, char *argv[])
{
    // example input: ./dphil 7 10 //7 philosophers, each eats 10 times total.

    // cancel out if wrong amount of arguments
    if (argc != 3)
    {
        fprintf(stderr, "Wrong number of arguments!");
        return 1;
    }

    //Get command line parameters
    int philosopherCount = atoi(argv[1]);
    int numTimesToEat = atoi(argv[2]);

    // initialize an array to hold our semaphores based on how many dining forks we should have
    sem_t *forks = (sem_t *)malloc(philosopherCount * sizeof(sem_t));

    // initialize semaphores
    for (int i = 0; i < philosopherCount; i++)
    {
        sem_init(&forks[i], 0, 1);
    }

    // set up the threads to represent our philosophers and allocate memory
    pthread_t *threads = (pthread_t *)malloc(philosopherCount * sizeof(pthread_t));
    struct PhilosopherParameters *params = (struct PhilosopherParameters *)malloc(philosopherCount * sizeof(struct PhilosopherParameters));

    //initialize parameters and create threads for the philosophers
    for (int i = 0; i < philosopherCount; ++i)
    {
        params[i].philosopherID = i + 1;
        params[i].timesToEat = numTimesToEat;
        params[i].philosopherCount = philosopherCount;
        params[i].forks = forks;
        pthread_create(&threads[i], NULL, Philosopher, (void *)&params[i]);
    }

    //join philosophers threads
    for (int i = 0; i < philosopherCount; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    // free all the reources
    free(threads);
    free(params);
    free(forks);

    //clear up semaphores
    for (int i = 0; i < philosopherCount; ++i)
    {
        sem_destroy(&forks[i]);
    }

    exit(0);
}

//Note: I commented out a lot of the unneccesary print statements but they may be useful to understand what exactly is happening! 

//Sample Test output: 
/*

ezduzzit@ezduzzit-virtual-machine:~/Documents/Projects/Examples/Assignment2$ gcc -o phils phils.c -lpthread
ezduzzit@ezduzzit-virtual-machine:~/Documents/Projects/Examples/Assignment2$ ./phils 5 3
Philosopher 1 is thinking
Philosopher 4 is thinking
Philosopher 5 is thinking
Philosopher 3 is thinking
Philosopher 2 is thinking
Philosopher 1 is Eating 
Philosopher 4 is Eating 
Philosopher 5 is thinking
Philosopher 3 is thinking
Philosopher 1 is thinking
Philosopher 2 is Eating 
Philosopher 5 is thinking
Philosopher 4 is thinking
Philosopher 3 is thinking
Philosopher 1 is thinking
Philosopher 2 is thinking
Philosopher 4 is Eating 
Philosopher 5 is thinking
Philosopher 3 is thinking
Philosopher 2 is Eating 
Philosopher 1 is thinking
Philosopher 4 is thinking
Philosopher 3 is thinking
Philosopher 5 is Eating 
Philosopher 1 is thinking
Philosopher 2 is thinking
Philosopher 5 is thinking
Philosopher 3 is Eating 
Philosopher 4 is thinking
Philosopher 2 is thinking
Philosopher 3 is thinking
Philosopher 1 is Eating 
Philosopher 4 is Eating 
Philosopher 2 is thinking
Philosopher 5 is thinking
Philosopher 3 is thinking
Philosopher 1 is thinking
Philosopher 4 is  all done eating! 
Philosopher 2 is Eating 
Philosopher 3 is thinking
Philosopher 2 is  all done eating! 
Philosopher 5 is Eating 
Philosopher 1 is thinking
Philosopher 3 is Eating 
Philosopher 5 is thinking
Philosopher 5 is Eating 
Philosopher 1 is thinking
Philosopher 3 is thinking
Philosopher 5 is  all done eating! 
Philosopher 1 is Eating 
Philosopher 3 is Eating 
Philosopher 1 is  all done eating! 
Philosopher 3 is  all done eating! 
ezduzzit@ezduzzit-virtual-machine:~/Documents/Projects/Examples/Assignment2$ 


*/

