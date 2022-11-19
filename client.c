#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>

#define MAX_STRING_LEN    256
#define NUMTHREADS      3


void* sendOrder();
void* sortOrders();
void* sortCustInfo();

int main(int argc, char **argv)
{
	//Declare variables needed
	int server_pid, server_chid;
	int coid, sent_id, rec_msg;
    char* msg;
    char* customerInfo[3], customerInfoSorted[3];
    int orders[3][3], sortedOrders[3][3];

    pthread_t tIDs[NUMTHREADS];


	for(int i=0; i<NUMTHREADS; i++) {
		pthread_create(&tIDs[i], NULL, &sendOrder, NULL);
	}

	for(int i=0; i<NUMTHREADS; i++) {
		printf("Enter order date as DD/MM/YY: ");
		scanf("%d/%d/%d",&orders[i][0],&orders[i][1],&orders[i][2]);
		printf("Enter your name and name of book you would like to get: ");
		scanf("%s", &customerInfo[i]);
	}

	sortedOrders = orders;
	qsort(sortedOrders, 3, 3*sizeof(int), sortOrders);
	customerInfoSorted = sortCustInfo();

	int priorityStart = 3;
	for(int i=0; i<NUMTHREADS; i++) {
		pthread_setschedprio(tIDs[i], priorityStart);
		priorityStart+=1;
	}

	for(int i=0; i<NUMTHREADS; i++) {
		pthread_join(tIDs[i], NULL);
	}

	return EXIT_SUCCESS;
}

void* sortOrders(void* date1, void* date2) {
	int* d1 = (int*)date1;
	int* d2 = (int*)date2;
	int yearDiff = d1[2] - d2[2];
	if (yearDiff) return yearDiff;
	int monthDiff = d1[1] - d2[1];
	if (monthDiff) return monthDiff;
	return arr1[0] - arr2[0];
}

char*[] sortCustInfo(char*[] info, int[][] ords, int[][] sortedOrds) {
	char* tempInfo[3];
	for(int i=0; i<3; i++) {
		for (int j = 0; j < 3; j++) {
			if (sortedOrds[i][j] == order[i][j]) {
				tempInfo[i] = info[i];
			}
		}
	}
	return tempInfo;
}

void* sendOrder() {
	sleep(20);
	// set-up to get the current scheduling priority
	pthread_attr_t tAttr;
	struct sched_param tSchedParam;
	pthread_attr_setschedpolicy(&tAttr, SCHED_RR);
	// get the current scheduling priority
	pthread_attr_getschedparam(&tAttr, &tSchedParam);
	int tSchedPriority = tSchedParam.sched_priority;
	// get the current thread ID
	pthread_t tID = pthread_self();
	// print priority of the thread and its ID
	printf( "Thread priority is %d for thread ID: %d\n", tSchedPriority, tID);
	return (NULL);
}
