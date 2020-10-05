#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "queueCar.h"

#define ROAD_LEN 35

struct CarStruct {
  float vel;
  float pos;
  float prePos;
  float dist;
};

char gCarPos[ROAD_LEN];
struct queue *gCarQueue = NULL;
pthread_mutex_t gMutexPrint;
pthread_mutex_t gMutexCarRunQueue;
pthread_mutex_t gMutexCarRemoveQueue;

void *TaskPrint(void* data) {
  while (1) {
    printf("***********************************\n");
    pthread_mutex_lock(&gMutexPrint);
    printf("%s\n", gCarPos);
    pthread_mutex_unlock(&gMutexPrint);
    printf("***********************************\n");
    pthread_mutex_lock(&gMutexCarRunQueue);
    printf("CarSize: %d\r\033[1A\033[1A\033[1A",
      gCarQueue->size);
    pthread_mutex_unlock(&gMutexCarRunQueue);

    usleep(1000);
  }

  pthread_exit(NULL); // 離開子執行緒
}

void *TaskCarGen(void *data) {
  struct CarStruct *car = NULL;

  while (1) {
    usleep((rand()%500 + 500) * 1000);
  
    pthread_mutex_lock(&gMutexCarRunQueue);
    if (!getQueueSize(gCarQueue) ||
        ((struct CarStruct *)(gCarQueue->back->data))->pos > 5) {
      car = calloc(1, sizeof(struct CarStruct));
      car->vel = (float)(rand()%10) / 10.0 + 1.0;
      car->pos = 1;
      car->prePos = 1;

      if (getQueueSize(gCarQueue))
        car->dist = ((struct CarStruct *)(gCarQueue->back->data))->pos - 1.0;
      else
        car->dist = ROAD_LEN;
      pushQueue(gCarQueue, (void *)car);
    }
    pthread_mutex_unlock(&gMutexCarRunQueue);
  }

  pthread_exit(NULL); // 離開子執行緒
}

void *TaskCar(void *data) {
  struct CarStruct *car = NULL;
  struct list *node = NULL;
  float frontPos = 100.0;

  while (1) {
    pthread_mutex_lock(&gMutexCarRunQueue);
    if (getQueueSize(gCarQueue)) {
      node = gCarQueue->front;
    }
  
    pthread_mutex_lock(&gMutexPrint);
    while (node) {
      car = (struct CarStruct *)node->data;
      node = node->next;

      gCarPos[(int)car->prePos] = ' ';
      gCarPos[(int)car->pos] = '@';
      car->prePos = car->pos;
      car->dist = ((void *)car == gCarQueue->front->data) ? 
        ROAD_LEN : frontPos - car->pos;
      frontPos = car->pos;

      if (car->dist <= 5.0) {
        car->vel *= (car->dist - 1) / (10.0 - 1.0);
      } else
        car->vel += (float)(rand()%10) / 100.0;

      car->vel = (car->vel >= 0.0) ? car->vel : 0;
      car->pos += car->vel;
      if ((int)car->pos > ROAD_LEN-1) {
        gCarPos[(int)car->prePos] = ' ';
        gCarPos[ROAD_LEN-1] = '*';
        popQueue(gCarQueue);
      }
    }
    pthread_mutex_unlock(&gMutexPrint);
    pthread_mutex_unlock(&gMutexCarRunQueue);
  
    usleep(100 * 1000);
  }

  pthread_exit(NULL); // 離開子執行緒
}

int main(void) {
  pthread_t t1, t2, t3;

  /* Initialization */
  initQueue(&gCarQueue);
  srand(time(NULL));
  pthread_mutex_init(&gMutexPrint, 0);
  pthread_mutex_init(&gMutexCarRunQueue, 0);

  gCarPos[0] = '*';
  for (int i=1; i<ROAD_LEN-1; i++)
    gCarPos[i] = ' ';
  gCarPos[ROAD_LEN-1] = '*';

  /* Create Threads */
  pthread_create(&t1, NULL, TaskPrint, "taskPrint");
  pthread_create(&t2, NULL, TaskCar, "taskCar");
  pthread_create(&t3, NULL, TaskCarGen, "taskCarGen");

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);

  /* Deinitialization */
  pthread_mutex_destroy(&gMutexPrint);
  pthread_mutex_destroy(&gMutexCarRunQueue);

  return 0;
}
