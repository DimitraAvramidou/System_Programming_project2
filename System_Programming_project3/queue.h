//------------For Server--------------//
struct Node {
  int data;
  struct Node *next;
};
struct Queue {
  struct Node *front;
  struct Node *last;
  unsigned int size;
};
void init(struct Queue *q);
int pop(struct Queue *q);
void push(struct Queue *q, int data);
int empty(struct Queue *q);
int front(struct Queue *q);
//------------------------------------//

//------------For Crawler--------------//
struct Node1 {
  char *link;
  struct Node1 *next;
  struct Node1 *prev;
};
struct Queue1 {
  struct Node1 *front;
  struct Node1 *last;
  unsigned int size;
};
void init1(struct Queue1 *q);
char* pop1(struct Queue1 *q);
void push1(struct Queue1 *q, char *link);
int empty1(struct Queue1 *q);
char* front1(struct Queue1 *q);
//------------------------------------//
