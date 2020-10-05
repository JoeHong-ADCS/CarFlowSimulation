#include <stdio.h>
#include <stdlib.h>
#include "queueCar.h"

void initQueue(struct queue **q) {
  *q = calloc(1, sizeof(struct queue));
  (*q)->size = 0;
  (*q)->front = NULL;
  (*q)->back = NULL;
}

int getQueueSize(struct queue *q) {
  return q->size;
}

void popQueue(struct queue *q) {
  struct list *tmp = NULL;

  if (getQueueSize(q)) {
    tmp = q->front;
    q->front = tmp->next;
    free(tmp->data);
    free(tmp);
    q->size--;

    if (!q->front)
      q->back = NULL;
  }
}

void pushQueue(struct queue *q, void *data) {
  struct list *tmp = calloc(1, sizeof(struct list));

  tmp->next = NULL;
  tmp->data = data;

  q->size++;
  if (!q->front)
    q->front = tmp;
  else
    q->back->next = tmp;
  
  q->back = tmp;
}
