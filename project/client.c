#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/netmgr.h>
#include <pthread.h>
#include "server.h"

#define NUMTHREADS      3

int printMenu();
void* getClientOrder();

int main(int argc, char **argv) {
	int coid;
    pthread_t tids[NUMTHREADS];



    // open connection to server
    coid = name_open(SERVER_NAME, ND_LOCAL_NODE);
    printf("coid=%d\n", coid);
    if(coid ==-1){
    	printf("Error connecting to server.\n");
    	return EXIT_FAILURE;
    }

    // print book menu for client
    printf("Welcome, please see book menu:\n");
    printMenu();
    printf("\n");

    //Create client threads
    //SEG FAULTING rn before second thread creation
   	for(int i=0; i<NUMTHREADS; i++) {
   		pthread_create(&tids[i], NULL, getClientOrder(coid), NULL);
   	}

   	/*//Ensures main waits for each thread to terminate before exiting
   	for(int i=0; i<NUMTHREADS; i++){
   		if(pthread_join(tids[i], NULL)!=0){
   			perror("Error joining thread.");
   		}
   	}*/

	return EXIT_SUCCESS;
}

int printMenu() {
	for (int i=0; i< BOOK_COUNT; i++) {
		int count = 0;
		char* fromMenu;
		fromMenu = malloc(strlen(BOOK_MENU[i]) * sizeof(char));
		strcpy(fromMenu, BOOK_MENU[i]);
		char* bookInfo = strtok(fromMenu, "_");
		char* bookName;
		char* bookAuthorF;
		char* bookAuthorL;
		while(bookInfo != NULL) {
		      switch (count) {
		      	  case 0:
		      		bookName = bookInfo;
		      		break;
		      	  case 1:
		      		bookAuthorF = bookInfo;
		      		break;
		      	  case 2:
		      		bookAuthorL = bookInfo;
		      		break;
		      	  default:
		      		  break;
		      }
		      count++;
		      bookInfo = strtok(NULL, "_");
		}
		if (bookName && bookAuthorF && bookAuthorL) {
			printf("(%d) %s by %s %s\n", (i+1), bookName, bookAuthorF, bookAuthorL);
		}
	}
	return EXIT_SUCCESS;
}
void* getClientOrder(int coid){
		printf("HERE\n");
		sleep(5);
	    // order related variables
		int bookCount=0;
		int orderNums[2];
	    int* orderDateDay = malloc(4 * sizeof(int));
	    int* orderDateMon = malloc(4 * sizeof(int));
	    int* orderDateYr = malloc(4 * sizeof(int));
	    int* classDateDay = malloc(4 * sizeof(int));
	    int* classDateMon = malloc(4 * sizeof(int));
	    int* classDateYr = malloc(4 * sizeof(int));
	    int* classTimeHr = malloc(2 * sizeof(int));
	    int* classTimeMin = malloc(2 * sizeof(int));
	    send_order_msg_t send_order_msg;
	    int ret_status;
	    char* store_msg;

		// predetermined to only let a client order 2 books in one session
		do {
			printf("Enter book number to order as X, order date as DD/MM/YY, class start as DD/MM/YY, class start time as HH:MM ->\n");
			char* inputs = malloc(17 * sizeof(char));
			int orderNum, odDay, odMon, odYr, cdDay, cdMon, cdYr, ctHr, ctMin;
			//for later: always reads in 4 (after enter it doesn't count how many fields??)
			printf("PRE scanned inputs %s.\n", inputs);
			int scanned = scanf(" %c %c%c/%c%c/%c%c %c%c/%c%c/%c%c %c%c:%c%c", &inputs[0], &inputs[1], &inputs[2], &inputs[3], &inputs[4], &inputs[5], &inputs[6], &inputs[7], &inputs[8], &inputs[9], &inputs[10], &inputs[11], &inputs[12], &inputs[13], &inputs[14], &inputs[15], &inputs[16]);
			if (scanned!=17) {
				printf("Please enter all fields to complete your order.\n");
				printf("Scanned %d.\n", scanned);
				printf("Scanned inputs %s.\n", inputs);
				fflush(stdin);
			} else {
				printf("in else: Scanned %d.\n", scanned);
				printf("in else: Scanned inputs %s.\n", inputs);
				orderNum = atoi(&inputs[0]);
				odDay = (10 * (inputs[1] - '0' ) ) + inputs[2] - '0';
				odMon = (10 * (inputs[3] - '0' ) ) + inputs[4] - '0';
				odYr = (10 * (inputs[5] - '0' ) ) + inputs[6] - '0';
				cdDay = (10 * (inputs[7] - '0' ) ) + inputs[8] - '0';
				cdMon = (10 * (inputs[9] - '0' ) ) + inputs[10] - '0';
				cdYr = (10 * (inputs[11] - '0' ) ) + inputs[12] - '0';
				ctHr = (10 * (inputs[13] - '0' ) ) + inputs[14] - '0';
				ctMin = (10 * (inputs[15] - '0' ) ) + inputs[16] - '0';
				if (odDay<0 || odDay>31 || odMon <0 || odMon>12 || odYr<0 || odYr>22 || cdDay<0||
					cdDay>31 || cdMon<0|| cdMon>12 || cdYr<0 || cdYr>22 || ctHr<00 || ctHr>24|| ctMin<00|| ctMin>59){
					printf("Please ensure that date/time values are valid.\n");
				}else if(odMon==04 || odMon==06 || odMon==9 || odMon==11){
					if(odDay>30){
						printf("Please ensure that the day value of Order Month is valid.\n");
					}
				}else if(cdMon==04 || cdMon==06 || cdMon==9 || cdMon==11){
					if(cdDay>30){
						printf("Please ensure that the day value of Class Month is valid.\n");
					}
				}else if(cdMon==02 || odMon==02){
					if(cdDay>28 || odDay>28){
						printf("Please ensure that the date values (DD/MM/YY) are valid.");
					}
				}else{
					// correct input path
					orderNums[bookCount] = orderNum;
					orderDateDay[bookCount] = odDay;
					orderDateMon[bookCount] = odMon;
					orderDateYr[bookCount] = odYr;
					classDateDay[bookCount] = cdDay;
					classDateMon[bookCount] = cdMon;
					classDateYr[bookCount] = cdYr;
					classTimeHr[bookCount] = ctHr;
					classTimeMin[bookCount] = ctMin;
					fflush(stdin);
					bookCount++;
				}
			}
			memset(inputs, 0, 17); // resets array
		} while (bookCount < MAX_BOOKS);

		// build message
		send_order_msg.type = SEND_ORDER_MSG_TYPE;
		for (int i=0; i< MAX_BOOKS; i++) {
			send_order_msg.orderInfo[i][0] = orderNums[i]-1;
			send_order_msg.orderInfo[i][1] = orderDateDay[i];
			send_order_msg.orderInfo[i][2] = orderDateMon[i];
			send_order_msg.orderInfo[i][3] = orderDateYr[i];
			send_order_msg.orderInfo[i][4] = classDateDay[i];
			send_order_msg.orderInfo[i][5] = classDateMon[i];
			send_order_msg.orderInfo[i][6] = classDateYr[i];
			send_order_msg.orderInfo[i][7] = classTimeHr[i];
			send_order_msg.orderInfo[i][8] = classTimeMin[i];

		}

		// copy paste for sample input: 1 12/12/12 12/12/12 11:11
		// copy paste for sample input w/ zeros: 1 02/02/02 02/02/02 01:01
		printf("order info for 1st order: %d %d %d %d %d %d %d %d %d\n", send_order_msg.orderInfo[0][0], send_order_msg.orderInfo[0][1], send_order_msg.orderInfo[0][2], send_order_msg.orderInfo[0][3], send_order_msg.orderInfo[0][4], send_order_msg.orderInfo[0][5], send_order_msg.orderInfo[0][6], send_order_msg.orderInfo[0][7], send_order_msg.orderInfo[0][8]);
		printf("order info for 2nd order: %d %d %d %d %d %d %d %d %d\n", send_order_msg.orderInfo[1][0], send_order_msg.orderInfo[1][1], send_order_msg.orderInfo[1][2], send_order_msg.orderInfo[1][3], send_order_msg.orderInfo[1][4], send_order_msg.orderInfo[1][5], send_order_msg.orderInfo[1][6], send_order_msg.orderInfo[1][7], send_order_msg.orderInfo[1][8]);

		printf("coid=%d\n", coid);
	   	ret_status = MsgSend(coid, &send_order_msg, sizeof(send_order_msg), &store_msg, sizeof(store_msg));
	   	if(ret_status==-1){
	   		printf("Error sending message to server.\n");
	   		return (NULL);
	   	}
	   	printf( "Store has responded to order: %s\n", store_msg);


		return (NULL);
}



