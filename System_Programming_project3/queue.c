#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include "queue.h"
#include <stdbool.h>

//----------functions for server's queue-----------//
  void init(struct Queue *q) {
	q->front = NULL;
	q->last = NULL;
	q->size = 0;
}

int front(struct Queue *q) {
	return q->front->data;
}

int pop(struct Queue *q) {
	q->size--;
int c=0;
	struct Node *tmp = q->front;
  c=q->front->data;
	q->front = q->front->next;
	free(tmp);
  return c;
}

void push(struct Queue *q, int data) {
	q->size++;

	if (q->front == NULL) {
		q->front = (struct Node *) malloc(sizeof(struct Node));
		q->front->data = data;
		q->front->next = NULL;
		q->last = q->front;
	} else {
		q->last->next = (struct Node *) malloc(sizeof(struct Node));
		q->last->next->data = data;
		q->last->next->next = NULL;
		q->last = q->last->next;
	}
}
//-----------------------------------------------//

//----------functions for crawler's queue-----------//
  void init1(struct Queue1 *q) {
	q->front = NULL;
	q->last = NULL;
	q->size = 0;
}

char * front1(struct Queue1 *q) {
	return q->front->link;
}

char * pop1(struct Queue1 *q) {
	q->size--;
  char* c;
	struct Node1 *tmp = q->front;
  c=malloc(strlen(q->front->link)+1);
  strcpy(c,q->front->link);
  c[strlen(q->front->link)]='\0';
	q->front = q->front->next;
	free(tmp);
  return c;
}

void push1(struct Queue1 *q, char *link) {
	q->size++;
	if (q->front == NULL) {
		q->front = (struct Node1 *) malloc(sizeof(struct Node1));
    q->front->link=malloc(strlen(link));
		strcpy(q->front->link,link);
		q->front->next = NULL;
		q->last = q->front;
	} else {
		q->last->next = (struct Node1 *) malloc(sizeof(struct Node1));
    q->last->next->link=malloc(strlen(link));
		strcpy(q->last->next->link,link);
		q->last->next->next = NULL;
		q->last = q->last->next;
	}
}
//-----------------------------------------------//

