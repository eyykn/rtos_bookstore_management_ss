#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include "server.h"
#include <sys/types.h>
#include <unistd.h>


int printMenu();

int main(int argc, char **argv) {


	// int coid, sentid;
	// char *rmsg;
	int bookCount=0;
	int orderNums[2];
    char** orderDates = malloc(2 * sizeof(char*));
    char** classDates = malloc(2 * sizeof(char*));
    char** classTimes = malloc(2 * sizeof(char*));
	char *orderInfo[4][100];

	for (int i=0; i< MAX_BOOKS; i++) {
		orderDates[i] = malloc(9 * sizeof(char));
		classDates[i] = malloc(9 * sizeof(char));
		classTimes[i] = malloc(6 * sizeof(char));
	}

	printf("Welcome, please see book menu:\n");
	printMenu();
	printf("\n");

	// predetermined to only let a client order 2 books
	do {
		printf("Enter book number to order as X, order date as DD/MM/YY, class start as DD/MM/YY, class start time as HH:MM: \n");
		int d1, d2, m1, m2, y1, y2, min, hour;
		scanf("%d %d/%d/%d %d/%d/%d %d:%d", &orderNums[bookCount], &d1, &m1, &y1, &d2, &m2, &y2, &hour, &min);
		sprintf(orderDates[bookCount], "%d/%d/%d", d1, m1, y1);
		sprintf(classDates[bookCount], "%d/%d/%d", d2, m2, y2);
		sprintf(classTimes[bookCount], "%d:%d", hour, min);
		fflush(stdin);
		bookCount++;
	} while (bookCount < MAX_BOOKS);

	for (int i=0; i< MAX_BOOKS; i++) {
		printf("book menu=%s\n",  BOOK_MENU[orderNums[i]-1]);
		strcpy(orderInfo[i][0], BOOK_MENU[orderNums[i]-1]);
		printf("after book menu\n");
		printf("orderDates[i]");
		strcpy(orderInfo[i][1], orderDates[i]);
		printf("after order dates\n");
		strcpy(orderInfo[i][2], classDates[i]);
		printf("after class dates\n");
		strcpy(orderInfo[i][3], classTimes[i]);
		printf("after class times\n");
	}

	printf("%s %s %s %s %s %s %s %s", orderInfo[0][0], orderInfo[0][1], orderInfo[1][0], orderInfo[1][1],  orderInfo[2][0], orderInfo[2][1], orderInfo[3][0], orderInfo[3][1]);

	// coid = name_open("book_order", ND_LOCAL_NODE);
	// sent_id = MsgSend_r(coid, &orderInfo, sizeof(orderInfo), &rmsg, sizeof(rmsg));
	// printf( "Received message from store server: %d\n", rmsg);

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




