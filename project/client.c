#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include "server.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/netmgr.h>


int printMenu();

int main(int argc, char **argv) {

	int bookCount=0;
	int orderNums[2];
	int coid, ret_status;
	char* rmsg;
    char** orderDates = malloc(2 * sizeof(char*));
    char** classDates = malloc(2 * sizeof(char*));
    char** classTimes = malloc(2 * sizeof(char*));
    char*** orderInfo = malloc(2 * sizeof(char**));

    //Open connection to server
    coid = name_open(SERVER_NAME, ND_LOCAL_NODE);
    if(coid ==-1){
    	printf("Error connecting to server.\n");
    	return EXIT_FAILURE;
    }

	for (int i=0; i< MAX_BOOKS; i++) {
		orderDates[i] = malloc(9 * sizeof(char));
		classDates[i] = malloc(9 * sizeof(char));
		classTimes[i] = malloc(6 * sizeof(char));
		orderInfo[i] = malloc(4 * sizeof(char*));
	}

	printf("Welcome, please see book menu:\n");
	printMenu();
	printf("\n");

	// predetermined to only let a client order 2 books
	do {
		printf("Enter book number to order as X, order date as DD/MM/YY, class start as DD/MM/YY, class start time as HH:MM: \n");
		int d1, d2, m1, m2, y1, y2, min, hour;
		if(scanf("%d %d/%d/%d %d/%d/%d %d:%d", &orderNums[bookCount], &d1, &m1, &y1, &d2, &m2, &y2, &hour, &min)!=9){
			printf("Please enter all fields to complete your order.\n");
		}else{
			sprintf(orderDates[bookCount], "%d/%d/%d", d1, m1, y1);
			sprintf(classDates[bookCount], "%d/%d/%d", d2, m2, y2);
			sprintf(classTimes[bookCount], "%d:%d", hour, min);
			fflush(stdin);
			bookCount++;
		}

	} while (bookCount < MAX_BOOKS);

	for (int i=0; i< MAX_BOOKS; i++) {
		char* fromMenu;
		fromMenu = malloc(strlen(BOOK_MENU[orderNums[i]-1]) * sizeof(char));
		strcpy(fromMenu, BOOK_MENU[orderNums[i]-1]);
		orderInfo[i][0] = fromMenu;
		orderInfo[i][1] = orderDates[i];
		orderInfo[i][2] = classDates[i];
		orderInfo[i][3] = classTimes[i];

	}
	// copy paste for sample input: 1 12/12/12 12/12/12 11:11
	printf("order info: %s %s %s %s %s %s %s %s\n", &*orderInfo[0][0],&*orderInfo[0][1], &*orderInfo[0][2], &*orderInfo[0][3], &*orderInfo[1][0], &*orderInfo[1][1], &*orderInfo[1][2], &*orderInfo[1][3]);

	ret_status = MsgSend(coid, &orderInfo, sizeof(orderInfo), &rmsg, sizeof(rmsg));
	if(ret_status==-1){
		printf("Error sending message to server.\n");
		return EXIT_FAILURE;
	}
	printf( "%s\n", rmsg);

	return EXIT_SUCCESS;
}

int printMenu() {
	for (int i=0; i< 5; i++) {
		int count = 0;
		char fromMenu[256];
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




