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

int sortOrders(int, int);
int sortTimes(int, int);

typedef union {
	uint16_t type;
	struct _pulse pulse;
	send_order_msg_t send_order_msg;
} recv_buf_t;

int main(int argc, char **argv){

	name_attach_t *attach;
	int rcvid, client_pid;
	int order_num=0;
	struct _msg_info info;
	recv_buf_t msg;
	int client_orders[10][9];

	// connect
	attach=name_attach(NULL, SERVER_NAME, 0);

	if (attach==NULL){
		printf("Name attach failed.\n");
		return EXIT_FAILURE;
	}

	while (1) {
			//Receives message
			rcvid=MsgReceive(attach->chid, &msg, sizeof(msg), &info);
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
				if (rcvid == -1) {
					perror("MsgReceive");
					exit(EXIT_FAILURE);

				} else if (rcvid > 0) {
					switch(msg.type) {
						case SEND_ORDER_MSG_TYPE:
							client_pid = info.pid;
							//Display msg received from client
							printf("Order Info received from client: \n");
							printf("Book Num=%d, Order Date (DDMMYY)=%d%d%d, Class Date (DDMMYY)=%d%d%d, Class Time (HHMM)=%d%d\n", msg.send_order_msg.orderInfo[0][0], msg.send_order_msg.orderInfo[0][1], msg.send_order_msg.orderInfo[0][2], msg.send_order_msg.orderInfo[0][3],  msg.send_order_msg.orderInfo[0][4],  msg.send_order_msg.orderInfo[0][5],  msg.send_order_msg.orderInfo[0][6],  msg.send_order_msg.orderInfo[0][7],  msg.send_order_msg.orderInfo[0][8]);
							printf("Book Num=%d, Order Date (DDMMYY)=%d%d%d, Class Date (DDMMYY)=%d%d%d, Class Time (HHMM)=%d%d\n", msg.send_order_msg.orderInfo[1][0], msg.send_order_msg.orderInfo[1][1], msg.send_order_msg.orderInfo[1][2], msg.send_order_msg.orderInfo[1][3], msg.send_order_msg.orderInfo[1][4], msg.send_order_msg.orderInfo[1][5],  msg.send_order_msg.orderInfo[1][6],  msg.send_order_msg.orderInfo[1][7],  msg.send_order_msg.orderInfo[1][8]);

							//Store order info for client
							for(int i=0; i<2; i++){
								client_orders[order_num][0]=client_pid;
								for(int j=0; j<9; j++){
									//populate client orders 2D array
									client_orders[order_num][j+1]=msg.send_order_msg.orderInfo[i][j];
									printf("client_orders[%d][%d]=%d\n", order_num, j+1, client_orders[order_num][j+1]);
								}
								order_num++;
							}

							//Figure out priorities for orders- sort in order of: class date->class time->order date

							/*for(int i=0; i<order_num-1; i++){
									printf("date1= %d%d%d, date 2= %d%d%d\n",client_orders[i][4], client_orders[i][5], client_orders[i][6], client_orders[i+1][4], client_orders[i+1][5], client_orders[i+1][6]);
									int sortVal=sortOrders(client_orders[i][4], client_orders[i][5], client_orders[i][6], client_orders[i+1][4], client_orders[i+1][5], client_orders[i+1][6]);

									for(int j=0; j<3; j++){
										sorted_orders[i][j]=client_orders[i][i+4];
										sorted_orders[i+1][j]=client_orders[i+1][i+4];
									}
							}
							qsort(sorted_orders, 3, 3*sizeof(int), sortOrders);*/

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

//Sorts orders by date
int sortOrders(int date1, int date2) {
	//To do: Figure out how to split date by DD, MM, YY and set the variables below
	int y1, y2;
	int m1, m2;
	int d1, d2;

	int yearDiff = y1 - y2;
	if (yearDiff) return 0;
	int monthDiff = m1 - m2;
	if (monthDiff) return 0;
	int dayDiff= d1-d2;
	if(dayDiff) return 0;
	return 1;
}

int sortTimes(int time1, int time2){
	//To do: Figure out how to split Hr and Min and set the variables below
	int h1, h2;
	int m1, m2;

	int hourDiff=h1-h2;
	if(hourDiff) return 0;
	int minDiff=m1-m2;
	if (minDiff) return 0;
	return 1;
}
