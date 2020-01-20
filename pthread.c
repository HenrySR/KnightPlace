//Henry Rosalsky

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
int n = 2;//set an N but real input is from the user
int tasksDone = 0;
int c1TasksDone = 0;
int c2TasksDone = 0;
int c1Done = 0, c2Done = 0;

typedef struct {
	int data[2];
	int isFull,isEmpty;
	long head, tail;
	pthread_mutex_t *lock;
	pthread_cond_t *notFull, *notEmpty;
} taskQueue;

typedef struct {
	int threadNum;
	taskQueue *tq;
} cInput;

cInput *initConsumer() {
	cInput *tq;
	tq = (cInput *)malloc (sizeof (cInput)); // allocates memory based on need size
	if (tq == NULL) return (NULL);
	return tq;
}
void addToTaskQ (taskQueue *tq, int in) {
	tq->data[tq->tail] = in;
	tq->tail++;
	if (tq->tail == 2) {
		tq->tail = 0;}
	if (tq->tail == tq->head) {
		tq->isFull = 1;}
	tq->isEmpty = 0;
	return;
}

void removeTaskQ (taskQueue *tq) {
	tq->head++;
	if (tq->head == 2) {
		tq->head = 0;
	}

	if (tq->head == tq->tail) {
		tq->isEmpty = 1;
	}
	tq->isFull = 0;
	return;
}


void deleteTaskQ (taskQueue *tq) {
	pthread_mutex_destroy (tq->lock);
	free(tq->lock);
	pthread_cond_destroy (tq->notFull);
	free(tq->notFull);
	pthread_cond_destroy (tq->notEmpty);
	free(tq->notEmpty);
	free(tq);
}

taskQueue *initTaskQ() {
	taskQueue *tq;
	tq = (taskQueue *)malloc (sizeof (taskQueue));// allocates memory based on need size
	if (tq == NULL) return (NULL);
	tq->isEmpty = 1;
	tq->isFull = 0;
	tq->head = 0;
	tq->tail = 0;
	tq->lock = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
	pthread_mutex_init (tq->lock, NULL), tq->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (tq->notFull, NULL), tq->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (tq->notEmpty, NULL);
	return tq; //returns the queue
}

void *produce(void * tq) {
	taskQueue *taskQ = (taskQueue *)tq;
	int curTask;
	for(curTask = 1; curTask <= n; curTask++) {
		pthread_mutex_lock (taskQ->lock);
		while(taskQ->isFull) {
			pthread_cond_wait(taskQ->notFull, taskQ->lock);}
		addToTaskQ(taskQ, curTask);
		printf("Producer: added task %i to the queue.\n", curTask);
		pthread_cond_signal(taskQ->notEmpty);
		pthread_mutex_unlock(taskQ->lock);
	}

	return NULL;
}

// runs the consumer
void *consume(void * ci) {
	cInput *input = (cInput *)ci;
	taskQueue *taskQ = input->tq;
	int threadNum = input->threadNum; 
	while(tasksDone < n) {
		pthread_mutex_lock(taskQ->lock);
		while(taskQ->isEmpty) {
			pthread_cond_wait(taskQ->notEmpty, taskQ->lock);}
		if(tasksDone < n) {
			switch(threadNum) {
			case 1: c1TasksDone++; break;
			case 2: c2TasksDone++; break;}
			removeTaskQ(taskQ);
			printf("Consumer%i: took task %i from the queue.\n", threadNum, ++tasksDone);
			pthread_cond_signal(taskQ->notFull);}
		pthread_mutex_unlock(taskQ->lock);
	}
	if(threadNum == 1) {
		c1Done = 1;}
	else {
		c2Done = 1;}
	while((c1Done & c2Done) == 0) {
		taskQ->isEmpty = 0;
		pthread_cond_broadcast(taskQ->notEmpty);
	}
	return NULL;
}
void FreeConsumerMemory (cInput *ci) {
	free(ci);
}

int main(int argc, char** argv) {
	taskQueue *taskQ; 
	cInput *input1, *input2;
  pthread_t consumer1, consumer2; // consumer created threads
	pthread_t producer; // producer created thread
  input1 = initConsumer();
	input2 = initConsumer();
	taskQ = initTaskQ();
	input1->threadNum = 1;
	input1->tq = taskQ;
	input2->threadNum = 2;
	input2->tq = taskQ;
  pthread_create(&producer, NULL, produce, taskQ);
	pthread_create(&consumer1, NULL, consume, input1);
	pthread_create(&consumer2, NULL, consume, input2);
	if(argc == 2) {
		n = atoi(argv[1]);}

	pthread_join (producer, NULL);
	pthread_join (consumer1, NULL);
	pthread_join (consumer2, NULL);


	printf("Tasks completed by Consumer 1: %i\n", c1TasksDone);
	printf("Tasks completed by Consumer 2: %i\n", c2TasksDone);

	deleteTaskQ(taskQ);

	return 0;
}
