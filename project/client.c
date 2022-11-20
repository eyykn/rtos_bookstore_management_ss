#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/netmgr.h>

#include "server.h"

int printMenu();
int getDigitCount();

int main(int argc, char **argv) {

	// order related variables
	int bookCount=0;
	int orderNums[2];
    int* orderDates = malloc(2 * sizeof(int));
    int* classDates = malloc(2 * sizeof(int));
    int* classTimes = malloc(2 * sizeof(int));

    // messaging/channel related variables
	int coid, ret_status;
	char* store_msg;
    send_order_msg_t send_order_msg;

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

	// predetermined to only let a client order 2 books in one session
	do {
		printf("Enter book number to order as X, order date as DDMMYY, class start as DDMMYY, class start time as HHMM: \n");
		int orderNum, orderDate, classDate, classTime;
		// below is error handling for incorrect inputs - incorrect num of inputs & incorrect format
		if(scanf("%d %d %d %d", &orderNum, &orderDate, &classDate, &classTime)!=4){
			printf("Please enter all fields to complete your order.\n");
		// FOR US (DELETE LATER) : Infinite loop if wrong, maybe take a look later lol
		} else if (getDigitCount(orderNum) !=1 ||orderNum > BOOK_COUNT || orderNum < 1) {
			printf("Please ensure book entered is in the menu presented & is in format X (1 digit). \n");
		} else if (getDigitCount(orderDate) !=6) {
			printf("Please enter order date as DDMMYY (6 digits).\n");
		} else if (getDigitCount(classDate) !=6) {
			printf("Please enter class date as DDMMYY (6 digits).\n");
		} else if (getDigitCount(classTime) !=4) {
			printf("Please enter class time as HHMM (4 digits).\n");
		} else{
			// correct input path
			orderNums[bookCount] = orderNum;
			orderDates[bookCount] = orderDate;
			classDates[bookCount] = classDate;
			classTimes[bookCount] = classTime;
			fflush(stdin);
			bookCount++;
		}

	} while (bookCount < MAX_BOOKS);

	// build message
	send_order_msg.type = SEND_ORDER_MSG_TYPE;
	for (int i=0; i< MAX_BOOKS; i++) {
		send_order_msg.orderInfo[i][0] = orderNums[i]-1;
		send_order_msg.orderInfo[i][1] = orderDates[i];
		send_order_msg.orderInfo[i][2] = classDates[i];
		send_order_msg.orderInfo[i][3] = classTimes[i];

	}

	// copy paste for sample input: 1 121212 121212 1111
	printf("order info: %d %d %d %d %d %d %d %d\n", send_order_msg.orderInfo[0][0], send_order_msg.orderInfo[0][1], send_order_msg.orderInfo[0][2], send_order_msg.orderInfo[0][3], send_order_msg.orderInfo[1][0], send_order_msg.orderInfo[1][1], send_order_msg.orderInfo[1][2], send_order_msg.orderInfo[1][3]);


	ret_status = MsgSend(coid, &send_order_msg, sizeof(send_order_msg), &store_msg, sizeof(store_msg));
	if(ret_status==-1){
		printf("Error sending message to server.\n");
		return EXIT_FAILURE;
	}
	printf( "Store has responded to order: %s\n", store_msg);

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

int getDigitCount(int num) {
	int digits=0;
	while(num!=0) {
		num=num/10;
		digits++;
	}
	return digits;
}



