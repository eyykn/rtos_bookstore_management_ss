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
int getDigitCount();
void* getClientOrder();

int main(int argc, char **argv) {
	int coid;
    pthread_t tids[NUMTHREADS];

    // open connection to server
    coid = name_open(SERVER_NAME, ND_LOCAL_NODE);
    if(coid ==-1){
    	printf("Error connecting to server.\n");
    	return EXIT_FAILURE;
    }

    // print book menu for client
    printf("Welcome, please see book menu:\n");
    printMenu();
    printf("\n");

    //Create client threads
   	for(int i=0; i<NUMTHREADS; i++) {
   		pthread_create(&tids[i], NULL, getClientOrder(coid), NULL);
   	}

   	//Ensures main waits for each thread to terminate before exiting
   	for(int i=0; i<NUMTHREADS; i++){
   		if(pthread_join(tids[i], NULL)!=0){
   			perror("Error joining thread.");
   		}
   	}

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
		sleep(20);
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
	    int ret_status;
	    char* store_msg;
	    send_order_msg_t send_order_msg;

		// predetermined to only let a client order 2 books in one session
		do {
			printf("Enter book number to order as X, order date as DD/MM/YY, class start as DD/MM/YY, class start time as HH:MM ->\n");
			int orderNum, odDay, odMon, odYr, cdDay, cdMon, cdYr, ctHr, ctMin;
			//for later: always reads in 4 (after enter it doesn't count how many fields??)
			if(scanf("%d %d/%d/%d %d/%d/%d %d:%d", &orderNum, &odDay, &odMon, &odYr, &cdDay, &cdMon, &cdYr, &ctHr, &ctMin)!=9){
				printf("Please enter all fields to complete your order.\n");
				printf("order num digits= %d\n", getDigitCount(orderNum));
				printf("order date digits= %d|%d|%d\n", getDigitCount(odDay), getDigitCount(odMon), getDigitCount(odYr));
				printf("class date digits= %d|%d|%d\n", getDigitCount(cdDay), getDigitCount(cdMon), getDigitCount(cdYr));
				printf("class time digits= %d|%d\n", getDigitCount(ctHr), getDigitCount(ctMin));
				fflush(stdin);
			} else if (getDigitCount(orderNum) !=1 ||orderNum > BOOK_COUNT || orderNum < 1) {
				printf("Please ensure book entered is in the menu presented & is in format X (1 digit). \n");
			} else if ((getDigitCount(odDay) + getDigitCount(odMon) + getDigitCount(odYr)) !=6) {
				printf("Please enter order date as DD/MM/YY (6 digits).\n");
			} else if ((getDigitCount(cdDay) + getDigitCount(cdMon) + getDigitCount(cdYr))!=6) {
				printf("Please enter class date as DD/MM/YY (6 digits).\n");
			} else if ((getDigitCount(ctHr) + getDigitCount(ctMin)) !=4) {
				printf("Please enter class time as HH:MM (4 digits).\n");
			} else{
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
		printf("order info for 1st order: %d %d %d %d %d %d %d %d %d\n", send_order_msg.orderInfo[0][0], send_order_msg.orderInfo[0][1], send_order_msg.orderInfo[0][2], send_order_msg.orderInfo[0][3], send_order_msg.orderInfo[0][4], send_order_msg.orderInfo[0][5], send_order_msg.orderInfo[0][6], send_order_msg.orderInfo[0][7], send_order_msg.orderInfo[0][8]);
		printf("order info for 2nd order: %d %d %d %d %d %d %d %d %d\n", send_order_msg.orderInfo[1][0], send_order_msg.orderInfo[1][1], send_order_msg.orderInfo[1][2], send_order_msg.orderInfo[1][3], send_order_msg.orderInfo[1][4], send_order_msg.orderInfo[1][5], send_order_msg.orderInfo[1][6], send_order_msg.orderInfo[1][7], send_order_msg.orderInfo[1][8]);


		ret_status = MsgSend(coid, &send_order_msg, sizeof(send_order_msg), &store_msg, sizeof(store_msg));
		if(ret_status==-1){
			printf("Error sending message to server.\n");
			return EXIT_FAILURE;
		}
		printf( "Store has responded to order: %s\n", store_msg);
		return EXIT_SUCCESS;
}


int getDigitCount(int num) {
	int digits=0;
	while(num!=0) {
		num=num/10;
		digits++;
	}
	return digits;
}



