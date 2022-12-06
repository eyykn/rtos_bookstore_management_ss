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
#include <ctype.h>

// global variables
int coid;
int currthread;
int printMenu(); // function to print the books available as a menu
void* getClientOrder(); // function to ask clients for their order & info



int main(int argc, char **argv) {
	// main variables
    pthread_t tids[CLIENTNUM];
	get_order_conf_msg_t get_order_conf_msg;
    int ret_status;
    char store_msg[MAX_STRING_LEN];

    // the current thread starts as 0
    currthread = 0;

    // open connection to server
    coid = name_open(SERVER_NAME, ND_LOCAL_NODE);

    // handle error during server connection
    if(coid ==-1){
    	printf("Error connecting to server.\n");
    	return EXIT_FAILURE;
    }

    // if connected create 3 client threads for bookstore
    for (int i = 0; i < CLIENTNUM; i++){
    	// set the current thread number we are creating
    	currthread = i;
    	// print the bookstore menu for the client
		printf("Welcome, please see book menu:\n");
		printMenu();
		printf("\n");
		// create the thread
	    // getClientOrder order called below in the thread creation receives confirmation that order was received (similar to the "thank you for your order" screen in real-life)
		pthread_create(&tids[i], NULL, getClientOrder, NULL);
		// join the thread
		if(pthread_join(tids[i], NULL)!=0){
			perror("Error joining thread.");
		}
    }

    // for all clients ask for order confirmation (similar to querying "where is my order" in real life)
    for (int i = 0; i < CLIENTNUM; i++){
    	// set order confirmation request message properties
    	get_order_conf_msg.type = GET_ORDER_CONF_MSG_TYPE;
    	get_order_conf_msg.thread_num = i; // thread id to get the order confirmation for
    	// send message to server/bookstore asking for order confirmation
    	ret_status = MsgSend(coid, &get_order_conf_msg, sizeof(get_order_conf_msg), &store_msg, sizeof(store_msg));
    	// check for errors in error sending/receiving
		if(ret_status==-1){
			printf("Error sending message to server.\n");
			break;
		}
		// print the response of the server showing our orders and their priorities
		printf("Store has responded to order confirmation request: %s\n\n", store_msg);
	}
    // exit main thread
	pthread_exit(NULL);

	return EXIT_SUCCESS;
}

// function uses the BOOK_MENU defined in server.h to get the books available in the store and prints them in a formatted way
int printMenu() {
	for (int i=0; i< BOOK_COUNT; i++) {
		int count = 0;
		char* from_menu;
		from_menu = malloc(strlen(BOOK_MENU[i]) * sizeof(char));
		strcpy(from_menu, BOOK_MENU[i]);
		// separate the book info in the BOOK_MENU enum by _ char
		char* book_info = strtok(from_menu, "_");
		// go through the separated book info to assign the info to correct variables
		char* book_name;
		char* book_author_f;
		char* book_author_l;
		while(book_info != NULL) {
		      switch (count) {
		      	  case 0:
		      		book_name = book_info;
		      		break;
		      	  case 1:
		      		book_author_f = book_info;
		      		break;
		      	  case 2:
		      		book_author_l = book_info;
		      		break;
		      	  default:
		      		  break;
		      }
		      count++;
		      book_info = strtok(NULL, "_");
		}
		// when desired info about the book is obtained print the book option to the client's terminal
		if (book_name && book_author_f && book_author_l) {
			printf("(%d) %s by %s %s\n", (i+1), book_name, book_author_f, book_author_l);
		}
	}
	return EXIT_SUCCESS;
}

// function to query user for the pre-determined amount of books they will buy (2 books)

/* test inputs for function
 *
 * 1 13/12/12 10/12/12 11:11
 * 3 13/12/12 11/12/12 11:11
 * 4 11/05/12 11/12/12 09:11
 * 5 11/05/12 12/12/12 09:11
 * 1 02/02/12 01/02/12 08:00
 * 2 02/02/12 01/02/12 08:01
 * */


void* getClientOrder(){
	    // order related variables
		int book_count=0;
		int order_nums[2];
	    int ret_status;
	    char store_msg[MAX_STRING_LEN];
	    send_order_msg_t send_order_msg;

		// order information expected
	    int* order_date_day = malloc(4 * sizeof(int));
	    int* order_date_mon = malloc(4 * sizeof(int));
	    int* order_date_yr = malloc(4 * sizeof(int));
	    int* class_date_day = malloc(4 * sizeof(int));
	    int* class_date_mon = malloc(4 * sizeof(int));
	    int* class_date_yr = malloc(4 * sizeof(int));
	    int* class_time_hr = malloc(2 * sizeof(int));
	    int* class_time_min = malloc(2 * sizeof(int));


		// loop asks user for 2 books until 2 books are input with correctly formatted information
		do {
			char* inputs = malloc(17 * sizeof(char));
			int orderNum, odDay, odMon, odYr, cdDay, cdMon, cdYr, ctHr, ctMin;
			// user instructions and hints
			printf("Enter book number to order as X, order date as DD/MM/YY, class start as DD/MM/YY, class start time as HH:MM ->\n");
			printf("Ex. input: 1 01/01/01 02/02/02 09:25\n");
			// scan in user information in response to above request
			int scanned = scanf(" %c %c%c/%c%c/%c%c %c%c/%c%c/%c%c %c%c:%c%c", &inputs[0], &inputs[1], &inputs[2], &inputs[3], &inputs[4], &inputs[5], &inputs[6], &inputs[7], &inputs[8], &inputs[9], &inputs[10], &inputs[11], &inputs[12], &inputs[13], &inputs[14], &inputs[15], &inputs[16]);
			// if scanned amount of characters doesn't match the number expected return an error message and ask user for input again
			//Check if scanned values are integers.
			if(!isdigit(inputs[0]) || !isdigit(inputs[1]) || !isdigit(inputs[2]) || !isdigit(inputs[3]) || !isdigit(inputs[4]) || !isdigit(inputs[5]) || !isdigit(inputs[6]) || !isdigit(inputs[7]) || !isdigit(inputs[8]) || !isdigit(inputs[9]) || !isdigit(inputs[10]) || !isdigit(inputs[11]) || !isdigit(inputs[12]) || !isdigit(inputs[13]) || !isdigit(inputs[14]) || !isdigit(inputs[15]) || !isdigit(inputs[16])){
				if(inputs[0]=='/' ||inputs[1]=='/' || inputs[2]=='/' || inputs[3]=='/' || inputs[4]=='/' || inputs[5]=='/' || inputs[6]=='/' || inputs[7]=='/' || inputs[8]=='/' || inputs[9]=='/' || inputs[10]=='/' || inputs[11]=='/' || inputs[12]=='/' || inputs[13]=='/' || inputs[14]=='/' || inputs[15]=='/' || inputs[16]=='/' ||inputs[0]==':' ||inputs[1]==':' || inputs[2]==':' || inputs[3]==':' || inputs[4]==':' || inputs[5]==':' || inputs[6]==':' || inputs[7]==':' || inputs[8]==':' || inputs[9]==':' || inputs[10]==':' || inputs[11]==':' || inputs[12]==':' || inputs[13]==':' || inputs[14]==':' || inputs[15]==':' || inputs[16]==':'){
					printf("Please ensure 2 digits are entered for DD MM YY values.\n");
				}else{
					printf("Please ensure all inputs are digits.\n");
				}
				while (getchar() != '\n');
			}else{
				if (scanned!=17) {
					printf("Please enter all fields to complete your order.\n");
					fflush(stdin);
				} else {
					// if scanned input has correct amount of characters
					// assign char info to the correct variables that will hold the input digits as integers
					orderNum = inputs[0]- '0';
					odDay = (10 * (inputs[1] - '0' ) ) + inputs[2] - '0';
					odMon = (10 * (inputs[3] - '0' ) ) + inputs[4] - '0';
					odYr = (10 * (inputs[5] - '0' ) ) + inputs[6] - '0';
					cdDay = (10 * (inputs[7] - '0' ) ) + inputs[8] - '0';
					cdMon = (10 * (inputs[9] - '0' ) ) + inputs[10] - '0';
					cdYr = (10 * (inputs[11] - '0' ) ) + inputs[12] - '0';
					ctHr = (10 * (inputs[13] - '0' ) ) + inputs[14] - '0';
					ctMin = (10 * (inputs[15] - '0' ) ) + inputs[16] - '0';
					// verify date logic
					int flag=1;
					// check for valid date & time values (a month has 31 days max, a year has 12 months, the input year can't be BC - <0, a month and day doesn't have a 0 val)
					if(odDay<0 || odDay>31 || odMon <0 || odMon>12 || odYr<0 || odYr>22 || cdDay<0||
						cdDay>31 || cdMon<0|| cdMon>12 || cdYr<0 || cdYr>22 || ctHr<00 || ctHr>24|| ctMin<00|| ctMin>59){
						printf("Please ensure that date/time values are valid.\n");
						fflush(stdin);
						flag=0;
					}
					// check that the input showing month order was made, if is during months with 30 days don't have a 31th day
					if(odMon==4 || odMon==6 || odMon==9 || odMon==11){
						if(odDay>30){
							printf("Please ensure that the day value of Order Month is valid.\n");
							fflush(stdin);
							flag=0;
						}
					}
					// check that the input showing month class requiring the book will start, if is during months with 30 days don't have a 31th day
					if(cdMon==4 || cdMon==6 || cdMon==9 || cdMon==11){
						if(cdDay>30){
							printf("Please ensure that the day value of Class Month is valid.\n");
							fflush(stdin);
							flag=0;
						}
					}
					// check that the input showing month order was made & input showing month class requiring the book will start if is during February is correct
					if((cdMon==2 && cdDay>28) || (odMon==2 && odDay>28)){
							printf("Please ensure that the date values (DD/MM/YY) are valid.\n");
							fflush(stdin);
							flag=0;
					}
					// If no logic errors for input are found then add order information to the array that will be sent as a request to the bookstore server
					if(flag){
						order_nums[book_count] = orderNum;
						order_date_day[book_count] = odDay;
						order_date_mon[book_count] = odMon;
						order_date_yr[book_count] = odYr;
						class_date_day[book_count] = cdDay;
						class_date_mon[book_count] = cdMon;
						class_date_yr[book_count] = cdYr;
						class_time_hr[book_count] = ctHr;
						class_time_min[book_count] = ctMin;
						fflush(stdin);
						book_count++;
					}
				}
		}
			 // resets buffer holding user inputs for the second book request
			memset(inputs, 0, 17);
			free(inputs);
		} while (book_count < MAX_BOOKS);

		// build message that will be sent to the bookstore server to send order (send_order_msg)
		send_order_msg.type = SEND_ORDER_MSG_TYPE;
		for (int i=0; i< MAX_BOOKS; i++) {
			// populate the message with order information
			send_order_msg.order_info[i][0] = currthread;
			send_order_msg.order_info[i][1] = order_nums[i]-1;
			send_order_msg.order_info[i][2] = order_date_day[i];
			send_order_msg.order_info[i][3] = order_date_mon[i];
			send_order_msg.order_info[i][4] = order_date_yr[i];
			send_order_msg.order_info[i][5] = class_date_day[i];
			send_order_msg.order_info[i][6] = class_date_mon[i];
			send_order_msg.order_info[i][7] = class_date_yr[i];
			send_order_msg.order_info[i][8] = class_time_hr[i];
			send_order_msg.order_info[i][9] = class_time_min[i];
		}

		// send order to bookstore
	   	ret_status = MsgSend(coid, &send_order_msg, sizeof(send_order_msg), &store_msg, sizeof(store_msg));
	   	// check for errors in error sending/receiving
	   	if(ret_status==-1){
	   		printf("Error sending message to server.\n");
	   		return (NULL);
	   	}
	   	// print response from bookstore to order
	   	printf("Store has responded to order: %s\n\n", store_msg);
	   	//free allocated memory for order information for next thread
	   	free(order_date_day);
	   	free(order_date_mon);
	    free(order_date_yr);
	   	free(class_date_day);
	   	free(class_date_mon);
	   	free(class_date_yr);
	   	free(class_time_hr);
	   	free(class_time_min);

		return (NULL);
}


