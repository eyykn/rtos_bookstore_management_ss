#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <process.h>
#include <string.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#include "server.h" // defines messages between client & server

// function that is passed into qsort to sort orders
int sortOrders(const void*, const void*);
// global var to count number of orders
int order_num=0;
// expected order numbers
int orders_expected_num = CLIENTNUM*MAX_BOOKS;

// define the possible message types that the server may receive
typedef union {
	uint16_t type;
	struct _pulse pulse;
	send_order_msg_t send_order_msg;
	get_order_conf_msg_t get_order_conf_msg;
} recv_buf_t;

int main(int argc, char **argv){
	// main variables
	int rcvid;
	int i_indexes[2];
	int found_count = 0;
	int client_orders[orders_expected_num][10];
	struct _msg_info info;
	char send_msg[MAX_STRING_LEN];
	name_attach_t *attach;
	recv_buf_t msg;

	// create a channel for the bookstore the clients will open
	attach=name_attach(NULL, SERVER_NAME, 0);
	// handle error during channel creation
	if (attach==NULL){
		printf("Name attach failed.\n");
		return EXIT_FAILURE;
	}

	// if channel successfully created
	while (1) {

			// receive message from client
			rcvid = MsgReceive(attach->chid, &msg, sizeof(msg), &info);
			// handle if the received message is a pulse
			 if(rcvid==0) {
				 switch (msg.pulse.code) {
				 	 // if pulse is a disconnect pulse
					 case _PULSE_CODE_DISCONNECT:
						 printf("Received disconnect pulse\n");
						 if (-1 == ConnectDetach(msg.pulse.scoid)) {
							 perror("ConnectDetach");
						 }
						break;
					 // if pulse is a unblock pulse
					 case _PULSE_CODE_UNBLOCK:
						printf("Received unblock pulse\n");
						if (-1 == MsgError(msg.pulse.value.sival_int, -1)) {
							perror("MsgError");
						}
						break;
					// if pulse is any other type/unknown
					 default:
						printf("unknown pulse received, code = %d\n", msg.pulse.code);
				 }
			// handle if the received message is not a pulse
			} else {
				// if MsgReceive returned an error
				if (rcvid == -1) {
					perror("MsgReceive");
					exit(EXIT_FAILURE);

				}
				// if the message was received successfully and is not a pulse  handle different message types (defined in server.h)
				else if (rcvid > 0) {
					switch(msg.type) {
						// if the message received was the message the client sends to bookstore to create the order
						case SEND_ORDER_MSG_TYPE:
							//Print order info received from client
							printf("Order Info received from client number: %d\n", msg.send_order_msg.order_info[0][0]);
							printf("(1) Book Num:%d, Order Date:%d/%d/%d, Class Date:%d/%d/%d, Class Time:%d:%d\n", msg.send_order_msg.order_info[0][1], msg.send_order_msg.order_info[0][2], msg.send_order_msg.order_info[0][3],  msg.send_order_msg.order_info[0][4],  msg.send_order_msg.order_info[0][5],  msg.send_order_msg.order_info[0][6],  msg.send_order_msg.order_info[0][7],  msg.send_order_msg.order_info[0][8], msg.send_order_msg.order_info[0][9]);
							printf("(2) Book Num:%d, Order Date:%d/%d/%d, Class Date:%d/%d/%d, Class Time:%d:%d\n", msg.send_order_msg.order_info[1][1], msg.send_order_msg.order_info[1][2], msg.send_order_msg.order_info[1][3], msg.send_order_msg.order_info[1][4], msg.send_order_msg.order_info[1][5],  msg.send_order_msg.order_info[1][6],  msg.send_order_msg.order_info[1][7],  msg.send_order_msg.order_info[1][8], msg.send_order_msg.order_info[1][9]);

							//Store order info for client in array that will hold all client orders received
							for(int i=0; i<2; i++){
								for(int j=0; j<10; j++){
									client_orders[order_num][j]=msg.send_order_msg.order_info[i][j];
								}
								// increase order numbers received
								order_num++;
							}
							// reset variable that holds the message that will be sent as a response to client before re-populating it
							memset(send_msg, 0, sizeof send_msg);
							// populate message to send with the confirmation that order was received and the client (thread number) that made the order
							sprintf(send_msg, "Order received for: %d", msg.send_order_msg.order_info[0][0]);
							// send the message built above to client
							MsgReply(rcvid, 0, &send_msg, sizeof(send_msg));
							break;
						// if the message received was the message the client sends to bookstore to get order confirmation/information (asking for order priority)
						case GET_ORDER_CONF_MSG_TYPE:
							// reset variables used throughout this switch case that are meant to be per thread
							found_count = 0;
							memset(i_indexes, 0, sizeof i_indexes);

							// sort the client orders in order of highest to lowest priority
							// calls custom sorting function sortOrders
							qsort(client_orders, orders_expected_num, sizeof(client_orders[0]), sortOrders);

							for(int i=0; i<orders_expected_num; i++){
								// find the orders of the calling thread in client orders sorted by priority through qsort
								if (client_orders[i][0] == msg.get_order_conf_msg.thread_num) {
									// if the client thread that made the order in order has the same number as calling thread
									i_indexes[found_count] = i;
									found_count++;
								}
							}
							// resets buffer holding user inputs for the second book request
							memset(send_msg, 0, sizeof send_msg);
							// build message to send back to client containing:
							// the information for the books ordered and the priority of the order from its index in sorted client orders our of all orders recieved
							sprintf(send_msg, "Orders confirmed for: %d -> (1) Book ordered %s with order priority: %d/%d | (2) Book ordered %s with order priority: %d/%d\n", client_orders[i_indexes[0]][0], BOOK_MENU[client_orders[i_indexes[0]][1]], i_indexes[0], orders_expected_num, BOOK_MENU[client_orders[i_indexes[1]][1]], i_indexes[1], orders_expected_num);
							// send reply to client
							MsgReply(rcvid, 0, &send_msg, sizeof(send_msg));
							break;
						// if any other message than expected types is defined return error
						default:
							printf("MsgError\n");
							MsgError(rcvid, -1);
							return EXIT_FAILURE;
							break;
					}
				}

			}
		}
}

// function that essentially assigns priority to client orders through sorting them
// please see comments in the function for explanation of what the orders are sorted by
int sortOrders(const void *d1, const void *d2) {
	int* a = (int*)d1;
	int* b = (int*)d2;
	// if 2 orders being compared have different years for when the class they need the book for starts, return the earliest year
	int classYearDiff = a[4] - b[4];
	if (classYearDiff) return classYearDiff;
	// if 2 orders being compared have same year, compare months for when the class they need the book for starts, if they have different months return the earliest month
	int classMonthDiff = a[3] - b[3];
	if (classMonthDiff) return classMonthDiff;
	// if 2 orders being compared have same year, and same month, compare day for when the class they need the book for starts, if they have different days return the earliest day
	int classDayDiff = a[2] - b[2];
	if (classDayDiff) return classDayDiff;
	// if 2 orders being compared have same year, same month, and same day, compare hour for when the class they need the book for starts, if they have different hours return the earliest hour
	int classHourDiff = a[9] - b[9];
	if (classHourDiff) return classHourDiff;
	// if 2 orders being compared have same year, same month, same day, and same hour, compare minute for when the class they need the book for starts, if they have different minutes return the earliest minute
	int classMinDiff = a[8] - b[8];
	if (classMinDiff) return classMinDiff;
	// if 2 orders being compared have same year, same month, same day, and same hour, and same minutes for when the class they need the book for starts
	// compare year for when they placed the orders, if they have different years return the earliest year
	int orderYearDiff = a[7] - b[7];
	if (orderYearDiff) return orderYearDiff;
	// if 2 orders being compared have same year, same month, same day, and same hour, and same minutes for when the class they need the book for starts
	// if 2 orders being compared, for when they placed the orders have same year, compare months, if they have different months return the earliest month
	int orderMonthDiff = a[6] - b[6];
	if (orderMonthDiff) return orderMonthDiff;
	// if 2 orders being compared have same year, same month, same day, and same hour, and same minutes for when the class they need the book for starts
	// if 2 orders being compared, for when they placed the orders have same year, and same month, compare days, if they have different days return the earliest day
	int orderDayDiff = a[5] - b[5];
	if (orderDayDiff) return orderDayDiff;
	// if all other information is the same between two compared orders return the earliest customer id (when the customer first ordered from the bookstore in context of bookstore's history)
	return a[0] - b[0];
}

