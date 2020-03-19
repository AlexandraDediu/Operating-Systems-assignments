#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "a2_helper.h"
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>



sem_t T21Started; 
sem_t T21Ended;
sem_t T24Started; 
sem_t T24Ended;
sem_t T5s;
sem_t *T71;
sem_t *T72;
sem_t *T22;



void *thread_function_1( void *param)
{ 
 int th_id = *((int*) param);
 //printf("%d\n", th_id);
 if(th_id==2){
   //sem_post(T22);
   sem_wait(T71);
   sem_wait(T22);
      info(BEGIN,2, th_id);
   info(END, 2, th_id);
   sem_post(T72);

 }
 else if(th_id==1){
   info(BEGIN, 2,th_id);
   sem_post(&T21Started);
   sem_wait(&T24Ended);
   info(END, 2, th_id);
 }

 else if(th_id==4){
   sem_wait(&T21Started);
   info(BEGIN, 2, th_id);
   info(END,2,th_id);
   sem_post(&T24Ended);
 }
 else{
   info(BEGIN,2,th_id);
   info(END,2,th_id);
 }
 return NULL;
}

void* thread_function_2(void* arg)
{ 
  int th_id = *((int*) arg);
  sem_wait(&T5s);
  info(BEGIN,5,th_id);
  info(END, 5, th_id);
  sem_post(&T5s);
  return NULL;
}

void* thread_function_3(void* arg)
{
 int th_id = *((int*) arg);
 if(th_id==1){
   //sem_wait(T22);
   info(BEGIN, 7,th_id);
   sem_post(T71);
   info(END,7, th_id);
   //sem_wait(T22);
   sem_post(T22);
 }
 else if(th_id==2){
   sem_wait(T72);
   info(BEGIN, 7,th_id);
   info(END,7, th_id);
 }

 else{
  info(BEGIN,7,th_id);
  info(END, 7, th_id);
}

return NULL;
}

int main(int argc, char **argv){
  init();
  sem_init(&T21Started, 0, 0);
  sem_init(&T21Ended, 0, 0);
  sem_init(&T24Started, 0, 0);
  sem_init(&T24Ended, 0, 0);
  sem_init(&T5s, 0, 0);


  T71 = sem_open("/T71", O_CREAT, 0600, 0);
  T72 = sem_open("/T72", O_CREAT, 0600, 0);
  T22 = sem_open("/T22", O_CREAT, 0600, 0);


  pthread_t t[4];
  pthread_t t5[40];
  pthread_t t7[5];
  int i;

  info(BEGIN, 1, 0);

  pid_t P2 =fork();
  if(P2==0)
  {

   info(BEGIN, 2, 0);

   // create N competing threads
    int td[4];
    for (i=0; i<4; i++) {
    td[i] = i+1;
      pthread_create(&t[i], NULL, thread_function_1, &td[i]);
    }


    // wait for the created threads to terminate
    for (i=0; i < 4; i++) {
      pthread_join(t[i], NULL);
    }

  pid_t P3=fork();
  if(P3==0)
  {
   info(BEGIN, 3, 0);
   info(END, 3,0);
   exit(3);
 }
 waitpid(P3, NULL, 0);

 info(END, 2, 0);
 exit(2);

}

pid_t P4=fork();
if(P4==0)
{ 
 info(BEGIN, 4, 0);
 pid_t P6=fork();
 if(P6==0)
 {
   info(BEGIN, 6, 0);
   info(END, 6,0);
   exit(6);
 }
 waitpid(P6, NULL, 0);
 info(END, 4, 0);
 exit(4);
}

pid_t P5=fork();
if(P5==0)
{ 
 info(BEGIN, 5, 0);
   // create an unnamed, shared (1) semaphore, with initial value 3
 if (sem_init(&T5s, 1, 3) < 0) {
  perror("Error creating the semaphore");
  exit(2);
}
   // create N competing threads
int td[40];
for (i=0; i<40; i++) {
  td[i] = i+1;
  pthread_create(&t5[i], NULL, thread_function_2, &td[i]);
}


    // wait for the created threads to terminate
for (i=0; i < 40; i++) {
  pthread_join(t5[i], NULL);
}

info(END, 5, 0);
exit(5);
}

pid_t P7=fork();
if(P7==0)
{
 info(BEGIN, 7, 0);

   // create N competing threads
 int td[5];
 for (i=0; i<5; i++) {
  td[i] = i+1;
  pthread_create(&t7[i], NULL, thread_function_3, &td[i]);
}


    // wait for the created threads to terminate
for (i=0; i < 5; i++) {
  pthread_join(t7[i], NULL);
}

info(END, 7, 0);
exit(7);
}

waitpid(P2, NULL, 0);
waitpid(P4, NULL, 0);
waitpid(P5, NULL, 0);
waitpid(P7, NULL, 0);

info(END, 1, 0);

sem_destroy(&T21Started); 
sem_destroy(&T21Ended);
sem_destroy(&T24Started); 
sem_destroy(&T24Ended);
sem_destroy(&T5s);

sem_destroy(T71);
sem_destroy(T72);
sem_destroy(T22);

return 0;
}