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
	int** orderDates = malloc(2 * sizeof(int));
	char *orderInfo[2][100];

	for (int i=0; i< MAX_BOOKS; i++) {
		orderDates[i] = malloc(8 * sizeof(int));
	}

	printf("Welcome, please see book menu:\n");
	printMenu();
	printf("\n");

	// predetermined to only let a client order 2 books
	while (bookCount < MAX_BOOKS){
		printf("Bookcount= %d\n", bookCount);
		printf("Enter book number and order date (DDMMYY): \n");
		scanf("%d %d", &orderNums[bookCount], orderDates[bookCount]);
		bookCount++;
		printf("updated bookCount=%d\n", bookCount);
		fflush(stdin);
	}


	//SEG FAULTING RN - also orderDates is now int
	for (int i=0; i< MAX_BOOKS; i++) {
		strcpy(orderInfo[i][0], BOOK_MENU[orderNums[i]-1]);
		strcpy(orderInfo[i][1], orderDates[i]);
	}

	printf("%s %s %s %s", orderInfo[0][0], orderInfo[0][1], orderInfo[1][0], orderInfo[1][1]);

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




