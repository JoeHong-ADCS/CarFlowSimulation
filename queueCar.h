#ifndef QUEUECAR_H
#define QUEUECAR_H

struct list {
  void *data;
  struct list *next;
};

struct queue {
  int size;
  struct list *front;
  struct list *back;
};

void initQueue(struct queue **q);
int getQueueSize(struct queue *q);
void popQueue(struct queue *q);
void pushQueue(struct queue *q, void *data);

#endif
