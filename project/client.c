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
#define MAX_STRING_LEN    256

int coid;
int printMenu();
void* getClientOrder();



int main(int argc, char **argv) {

    //pthread_t tids[NUMTHREADS];



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



    /*
      pthread_t tid0;
	  pthread_t tid1;
	  pthread_t tid2;

	 pthread_t  tids[] = {&tid0,&tid1,&tid2};
	 */

    pthread_t threads[NUMTHREADS];

	 // pthread_create(&tid0,NULL,getClientOrder,NULL);


	//  pthread_create(&tid1,NULL,getClientOrder,NULL);


	 // pthread_create(&tid2,NULL,getClientOrder,NULL);


	  for (int i = 0; i < 3; i++){
		  printf("Welcome, please see book menu:\n");
		  printMenu();
		  printf("\n");

		  pthread_create(&threads[i],NULL,getClientOrder,
		  	  			NULL);
	  }
	  pthread_exit(NULL);

	  /*
	  for(int i=0; i<2; i++){
	     		if(pthread_join(&tids[i], NULL)!=0){
	     			perror("Error joining thread.");
	     		}
	     	}
	     	*/





    /*
    int check = 0;
    pthread_create(NULL, NULL, getClientOrder(), NULL);
    check++;
    pthread_join(thread_id, NULL);
    pthread_create(NULL, NULL, getClientOrder(), NULL);
    */


    //Create client threads
    //SEG FAULTING rn before second thread creation

    /*
   	for(int i=0; i<NUMTHREADS; i++) {
   		printf("Numthread %d\n", i);
   		int retVal=pthread_create(&tids[i], NULL, getClientOrder(1), NULL);
   		if(retVal!=0){
   			printf("Creation of thread failed.\n");
   		}
   		pthread_join(tids[i], NULL);
   	}

   	//Ensures main waits for each thread to terminate before exiting
   	for(int i=0; i<NUMTHREADS; i++){
   		if(pthread_join(tids[i], NULL)!=0){
   			perror("Error joining thread.");
   		}
   	}
   	*/

	pthread_exit(NULL);

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

void* getClientOrder(){

		printf("In getClientOrder()\n");
		printf("coid=%d\n", coid);
		//printf("tid: %d\n",tid);

	    // order related variables

		int bookCount=0;
		int orderNums[2];
		printf("before malloc\n");
	    int* orderDateDay = malloc(4 * sizeof(int));
	    int* orderDateMon = malloc(4 * sizeof(int));
	    int* orderDateYr = malloc(4 * sizeof(int));
	    int* classDateDay = malloc(4 * sizeof(int));
	    int* classDateMon = malloc(4 * sizeof(int));
	    int* classDateYr = malloc(4 * sizeof(int));
	    int* classTimeHr = malloc(2 * sizeof(int));
	    int* classTimeMin = malloc(2 * sizeof(int));
	    printf("after malloc\n");
	    send_order_msg_t send_order_msg;
	    printf("after send order msg\n");
	    int ret_status;
	    char store_msg[MAX_STRING_LEN];
	    printf("before do\n");
		// predetermined to only let a client order 2 books in one session
		do {
			printf("Enter book number to order as X, order date as DD/MM/YY, class start as DD/MM/YY, class start time as HH:MM ->\n");
			printf("example input: 1 01/01/01 02/02/02 09:25\n");
			char* inputs = malloc(17 * sizeof(char));
			int orderNum, odDay, odMon, odYr, cdDay, cdMon, cdYr, ctHr, ctMin;
			//for later: always reads in 4 (after enter it doesn't count how many fields??)
			int scanned = scanf(" %c %c%c/%c%c/%c%c %c%c/%c%c/%c%c %c%c:%c%c", &inputs[0], &inputs[1], &inputs[2], &inputs[3], &inputs[4], &inputs[5], &inputs[6], &inputs[7], &inputs[8], &inputs[9], &inputs[10], &inputs[11], &inputs[12], &inputs[13], &inputs[14], &inputs[15], &inputs[16]);
			if (scanned!=17) {
				printf("Please enter all fields to complete your order.\n");
				printf("Scanned %d.\n", scanned);
				printf("Scanned inputs %s.\n", inputs);
				fflush(stdin);
			} else {
				printf("book count=%d\n", bookCount);
				orderNum = inputs[0]- '0';
				odDay = (10 * (inputs[1] - '0' ) ) + inputs[2] - '0';
				odMon = (10 * (inputs[3] - '0' ) ) + inputs[4] - '0';
				odYr = (10 * (inputs[5] - '0' ) ) + inputs[6] - '0';
				cdDay = (10 * (inputs[7] - '0' ) ) + inputs[8] - '0';
				cdMon = (10 * (inputs[9] - '0' ) ) + inputs[10] - '0';
				cdYr = (10 * (inputs[11] - '0' ) ) + inputs[12] - '0';
				ctHr = (10 * (inputs[13] - '0' ) ) + inputs[14] - '0';
				ctMin = (10 * (inputs[15] - '0' ) ) + inputs[16] - '0';
				int flag=1;
				if(odDay<0 || odDay>31 || odMon <0 || odMon>12 || odYr<0 || odYr>22 || cdDay<0||
					cdDay>31 || cdMon<0|| cdMon>12 || cdYr<0 || cdYr>22 || ctHr<00 || ctHr>24|| ctMin<00|| ctMin>59){
					printf("Please ensure that date/time values are valid.\n");
					flag=0;
				}
				if(odMon==4 || odMon==6 || odMon==9 || odMon==11){
					if(odDay>30){
						printf("Please ensure that the day value of Order Month is valid.\n");
						flag=0;
					}
				}
				if(cdMon==4 || cdMon==6 || cdMon==9 || cdMon==11){
					if(cdDay>30){
						printf("Please ensure that the day value of Class Month is valid.\n");
						flag=0;
					}
				}if(cdMon==2 || odMon==2){
					if(cdDay>28 || odDay>28){
						printf("Please ensure that the date values (DD/MM/YY) are valid.\n");
						flag=0;
					}
				}
				if(flag){
					printf("in correct path.\n");
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
		//printf("order info for 1st order: %d %d %d %d %d %d %d %d %d\n", send_order_msg.orderInfo[0][0], send_order_msg.orderInfo[0][1], send_order_msg.orderInfo[0][2], send_order_msg.orderInfo[0][3], send_order_msg.orderInfo[0][4], send_order_msg.orderInfo[0][5], send_order_msg.orderInfo[0][6], send_order_msg.orderInfo[0][7], send_order_msg.orderInfo[0][8]);
		//printf("order info for 2nd order: %d %d %d %d %d %d %d %d %d\n", send_order_msg.orderInfo[1][0], send_order_msg.orderInfo[1][1], send_order_msg.orderInfo[1][2], send_order_msg.orderInfo[1][3], send_order_msg.orderInfo[1][4], send_order_msg.orderInfo[1][5], send_order_msg.orderInfo[1][6], send_order_msg.orderInfo[1][7], send_order_msg.orderInfo[1][8]);

		printf("The size of send_order_msg.orderInfo is %zu \n", sizeof(send_order_msg.orderInfo));
		printf("The #elements in send_order_msg.orderInfo is %zu \n",sizeof(send_order_msg.orderInfo)/sizeof(int));
		printf("The #elements in each row of send_order_msg.orderInfo is %zu \n", sizeof(send_order_msg.orderInfo[0])/sizeof(int));
		printf("The #rows of send_order_msg.orderInfo is %zu \n",
		sizeof(send_order_msg.orderInfo)/sizeof(send_order_msg.orderInfo[0]));
		for (int r=0; r<2; r++) {
		 for (int c=0; c<9; c++)
			 printf("%02d ", send_order_msg.orderInfo[r][c]);
		 printf("\n");
		}
		printf("\n");

	   	ret_status = MsgSend(coid, &send_order_msg, sizeof(send_order_msg), &store_msg, sizeof(store_msg));
	   	if(ret_status==-1){
	   		printf("Error sending message to server.\n");
	   		return (NULL);
	   	}
	   	printf( "Store has responded to order: %s\n", store_msg);
	   	//free allocated memory
	   	free(orderDateDay);
	   	free(orderDateMon);
	    free(orderDateYr);
	   	free(classDateDay);
	   	free(classDateMon);
	   	free(classDateYr);
	   	free(classTimeHr);
	   	free(classTimeMin);


		return (NULL);
}


