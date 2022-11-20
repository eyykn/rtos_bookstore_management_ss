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
							break;
						default:
							break;
					}
				}

			}
		}
}
