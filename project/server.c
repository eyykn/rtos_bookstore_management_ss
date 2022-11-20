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

int sortOrders(int*, int*);

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
	int client_orders[10][5];

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
							printf("Book Num=%d, Order Date (DDMMYY)=%d, Class Date (DDMMYY)=%d, Class Time (HHMM)=%d\n", msg.send_order_msg.orderInfo[0][0], msg.send_order_msg.orderInfo[0][1], msg.send_order_msg.orderInfo[0][2], msg.send_order_msg.orderInfo[0][3]);
							printf("Book Num=%d, Order Date (DDMMYY)=%d, Class Date (DDMMYY)=%d, Class Time (HHMM)=%d\n", msg.send_order_msg.orderInfo[1][0], msg.send_order_msg.orderInfo[1][1], msg.send_order_msg.orderInfo[1][2], msg.send_order_msg.orderInfo[1][3]);

							//Store order info for client
							for(int i=0; i<2; i++){
								client_orders[order_num][0]=client_pid;
								for(int j=0; j<4; j++){
									//populate client orders 2D array
									client_orders[order_num][j+1]=msg.send_order_msg.orderInfo[i][j];
									printf("client_orders[%d][%d]=%d\n", order_num, j+1, client_orders[order_num][j+1]);
								}
								order_num++;
							}
							order_num++;

							//Figure out priorities for orders- sort in order of: class date->class time->order date
							//Sort by class date
							for(int i=0; i<order_num; i++){
									printf("date1= %d, date 2= %d\n",client_orders[i][2], client_orders[i+1][2]);
									//Crashing right now because I haven't figured out how to store DD, MM, YY separately
									sortOrders(client_orders[i][2], client_orders[i+1][2]);
							}

							break;
						default:
							break;
					}
				}

			}
		}
}

int sortOrders(int* date1, int* date2) {
	printf("date1= %d, date2=%d\n", date1, date2);
	char* d1=date1;
	char* d2=date2;
	char* y1, y2;
	char* m1, m2;
	char* day1, day2;

	//Get day from date
	sprintf(day1, "%.*s", 2, d1);
	sprintf(day2, "%.*s", 2, d2);
	//Get month from date
	sprintf(m1, "%.*s", 2, d1+2);
	sprintf(m2, "%.*s", 2, d2+2);
	//Get year from date
	sprintf(y1, "%.*s", 2, d1+4);
	sprintf(y2, "%.*s", 2, d2+4);
	printf("d1=%s, d2=%s, m1=%s, m2=%s, y1=%s, y2=%s\n", day1, day2, m1, m2, y1, y2);

	int yearDiff = (int*)y1 - (int*)y2;
	if (yearDiff) return 0;
	int monthDiff = (int*)m1 - (int*)m2;
	if (monthDiff) return 0;
	int dayDiff= (int*)day1-(int*)day2;
	if(dayDiff) return 0;
	return 1;
}
