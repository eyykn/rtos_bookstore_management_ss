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


int sortOrders(const void*, const void*);

typedef union {
	uint16_t type;
	struct _pulse pulse;
	send_order_msg_t send_order_msg;
	get_order_conf_msg_t get_order_conf_msg;
} recv_buf_t;

int main(int argc, char **argv){

	name_attach_t *attach;
	int rcvid;
	int order_num=0;
	struct _msg_info info;
	recv_buf_t msg;
	int client_orders[10][10];
	char send_msg[MAX_STRING_LEN];
	int foundCount = 0;
	int i_indexes[2];

	// connect
	attach=name_attach(NULL, SERVER_NAME, 0);

	if (attach==NULL){
		printf("Name attach failed.\n");
		return EXIT_FAILURE;
	}

	while (1) {
			//Receives message
			rcvid=MsgReceive(attach->chid, &msg, sizeof(msg), &info);
			printf("Received.\n");
			 if(rcvid==0) {
				//Pulse
				 switch (msg.pulse.code) {
				 case _PULSE_CODE_DISCONNECT:
					 printf("Received disconnect pulse\n");
					 if (-1 == ConnectDetach(msg.pulse.scoid)) {
						 perror("ConnectDetach");
				 	 }
				 	break;
				 case _PULSE_CODE_UNBLOCK:
				 	printf("Received unblock pulse\n");
				 	if (-1 == MsgError(msg.pulse.value.sival_int, -1)) {
				 		perror("MsgError");
				 	}
				 	break;

				 default:
				 	printf("unknown pulse received, code = %d\n", msg.pulse.code);

				 }
			} else {
				//Message
				printf("Received message is a message not pulse.\n");
				if (rcvid == -1) {
					perror("MsgReceive");
					exit(EXIT_FAILURE);

				} else if (rcvid > 0) {
					switch(msg.type) {
						case SEND_ORDER_MSG_TYPE:
							printf("Received message type is SEND_ORDER_MSG_TYPE\n");
							//Display msg received from client
							printf("Order Info received from client: \n");
							printf("Tid %d, Book Num=%d, Order Date (DDMMYY)=%d%d%d, Class Date (DDMMYY)=%d%d%d, Class Time (HHMM)=%d%d\n", msg.send_order_msg.orderInfo[0][0], msg.send_order_msg.orderInfo[0][1], msg.send_order_msg.orderInfo[0][2], msg.send_order_msg.orderInfo[0][3],  msg.send_order_msg.orderInfo[0][4],  msg.send_order_msg.orderInfo[0][5],  msg.send_order_msg.orderInfo[0][6],  msg.send_order_msg.orderInfo[0][7],  msg.send_order_msg.orderInfo[0][8], msg.send_order_msg.orderInfo[0][9]);
							printf("Tid %d, Book Num=%d, Order Date (DDMMYY)=%d%d%d, Class Date (DDMMYY)=%d%d%d, Class Time (HHMM)=%d%d\n", msg.send_order_msg.orderInfo[1][0], msg.send_order_msg.orderInfo[1][1], msg.send_order_msg.orderInfo[1][2], msg.send_order_msg.orderInfo[1][3], msg.send_order_msg.orderInfo[1][4], msg.send_order_msg.orderInfo[1][5],  msg.send_order_msg.orderInfo[1][6],  msg.send_order_msg.orderInfo[1][7],  msg.send_order_msg.orderInfo[1][8], msg.send_order_msg.orderInfo[1][9]);

							//Store order info for client
							for(int i=0; i<2; i++){
								for(int j=0; j<10; j++){
									//populate client orders 2D array
									client_orders[order_num][j]=msg.send_order_msg.orderInfo[i][j];
									printf("SEND_ORDER_MSG_TYPE client_orders[%d][%d]=%d\n", order_num, j, client_orders[order_num][j]);
								}
								order_num++;
							}

							memset(send_msg, 0, sizeof send_msg);
							sprintf(send_msg, "Order received for: %d", msg.send_order_msg.orderInfo[0][0]);

							printf("send_msg=%s\n", send_msg);
							MsgReply(rcvid, 0, &send_msg, sizeof(send_msg));
							printf("After Msg reply.\n");
							break;

						case GET_ORDER_CONF_MSG_TYPE:
							printf("Received message type is GET_ORDER_CONF_MSG_TYPE\n");
							foundCount = 0;
							memset(i_indexes, 0, sizeof i_indexes);


							// print pre-sorted
							printf("pre sort:\n");
							for(int i=0; i<2; i++){
								for(int j=0; j<10; j++){
									printf("GET_ORDER_CONF_MSG_TYPE client_orders[%d][%d]=%d\n", i, j, client_orders[i][j]);
								}
							}


							if (msg.get_order_conf_msg.threadId == 0) {
								qsort(client_orders, 10, sizeof client_orders[0], sortOrders);
							}

							// print sorted
							printf("post sort, sorted orders:\n");
							for(int i=0; i<2; i++){
								for(int j=0; j<10; j++){
									printf("GET_ORDER_CONF_MSG_TYPE sorted client_orders[%d][%d]=%d\n", i, j, client_orders[i][j]);
								}
							}


							for(int i=0; i<10; i++){
								printf("in for client_orders[i][0], is %d and val is: %d\n", i, client_orders[i][0]);
								if (client_orders[i][0] == msg.get_order_conf_msg.threadId) {
									printf("in if\n");
									i_indexes[foundCount] = i;
								}
								foundCount++;
							}

							printf("indices found for %d: %d, %d\n", msg.get_order_conf_msg.threadId,i_indexes[0], i_indexes[1]);

							memset(send_msg, 0, sizeof send_msg);
							sprintf(send_msg, "Orders confirmed for: %d -> (1) Book ordered %s with order priority: %d (2) Book ordered %s with order priority: %d\n", client_orders[i_indexes[0]][0], BOOK_MENU[client_orders[i_indexes[0]][1]], i_indexes[0], BOOK_MENU[client_orders[i_indexes[1]][1]], i_indexes[1]);
							MsgReply(rcvid, 0, &send_msg, sizeof(send_msg));
							break;

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

// https://cboard.cprogramming.com/c-programming/51935-qsort-multi-dimensional-arrays.html
// https://stackoverflow.com/questions/17202178/c-qsort-with-dynamic-n-by-2-multi-dimensional-array
// https://stackoverflow.com/questions/27284185/how-does-the-compare-function-in-qsort-work
int sortOrders(const void *d1, const void *d2) {
	// something about the casting here is broken
	int* a = (int*)d1;
	int* b = (int*)d2;
    printf("in sort\n %d", a[4]);
	int classYearDiff = a[4] - b[4];
	if (classYearDiff) return classYearDiff;
	int classMonthDiff = a[3] - b[3];
	if (classMonthDiff) return classMonthDiff;
	int classDayDiff = a[2] - b[2];
	if (classDayDiff) return classDayDiff;
	int classHourDiff = a[9] - b[9];
	if (classHourDiff) return classHourDiff;
	int classMinDiff = a[8] - b[8];
	if (classMinDiff) return classMinDiff;
	int orderYearDiff = a[7] - b[7];
	if (orderYearDiff) return orderYearDiff;
	int orderMonthDiff = a[6] - b[6];
	if (orderMonthDiff) return orderMonthDiff;
	int orderDayDiff = a[5] - b[5];
	if (orderDayDiff) return orderDayDiff;
	return a[0] - b[0]; // if all is same thread id
}

