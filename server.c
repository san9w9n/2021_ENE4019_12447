// 2018008877 이상원 (컴퓨터소프트웨어학부)
#include "server.h"

int listenFD;


static void sigintHandler(int sig) { 										// client의 연결이 모두 정상적으로 종료 된 후, 서버를 종료하기 위해 사용.
	printf("SERVER (%d) : server closed.\n", listenFD);
	close(listenFD);
	exit(0);
}

int
main(void)
{
	struct sockaddr_in 		listenSocket;
	struct sockaddr_in 		connectSocket;
	ssize_t 				readbytes;
	ssize_t 				namelen;
	int 					connectFD;
	int 					num = 1;
	pid_t 					pid;
	char					clntname[100];
	char 					readMSG[BUFF_SIZE];
	socklen_t 				conLen;
	
	signal(SIGINT, sigintHandler);
	signal(SIGCHLD, SIG_IGN);   											// 자식 프로세스 종료 확인.

	// listen socket 만들기. (127.0.0.1:8080)
	memset(&listenSocket, 0, sizeof(listenSocket));
	listenFD = socket(PF_INET, SOCK_STREAM, 0);
	if(listenFD<0) return ERROR;
	listenSocket.sin_family = AF_INET;
	listenSocket.sin_addr.s_addr = inet_addr("127.0.0.1");
	listenSocket.sin_port = htons(PORT);
	
	if(bind(listenFD, (sockaddr*)&listenSocket, sizeof(listenSocket)) == -1) {
		printf("SERVER : bind fail! exit(1)\n");
		return ERROR;
	}
	if(listen(listenFD, QUEUE_SIZE) == -1) {
		printf("SERVER : listen fail! exit(1)\n");
		return ERROR;
	}

	printf("Waiting for clients...\n");
	while(1)																// 부모 프로세스는 loop를 계속해서 돌면서 새로운 연결 대기.
	{
		conLen = sizeof(connectSocket);
		connectFD = accept(listenFD, (sockaddr*)&connectSocket, &conLen);  	// listen socket 으로 accept 호출, connect socket으로 client 연결.
		if(connectFD < 0) { 												// 연결에 실패.
			printf("SERVER : Connection fail! exit(1).\n");
			return ERROR;
		}
		printf("SERVER : Client %d connection success!\n", num++);
		pid = fork(); 														// fork를 통해 자식 프로세스 생성
		if(!pid) {															// 자식 프로세스가 할 일. (한 자식프로세스 당 한 Client)
			close(listenFD);
			namelen = read(connectFD, clntname, sizeof(clntname)-1); 		// 채팅방에 접속한 사람의 이름 받기
			if(namelen == -1)  												// read() 실패
				return ERROR;
			clntname[BUFF_SIZE-1] = '\0';
			printf("SERVER : %s has entered the chat room.\n", clntname);
			while((readbytes = read(connectFD, readMSG, BUFF_SIZE)) > 0) {	// Client로 부터 입력을 받는다.            
				readMSG[readbytes-1] = '\0';
				if(!strcmp(readMSG, "quit\n")) {							// quit 입력시 종료
					printf("%s has left the chat room.\n", clntname);
					break;
				}
				if(readMSG[0] == '\0') continue;
				printf("%s : %s", clntname , readMSG);						// 입력받은 메세지 출력.
			}
			printf("SERVER : Client (%d) connection close.\n", connectFD);
			close(connectFD);												// Client 하나 연결 종료.
			
			return SUCCESS;
		}
		else close(connectFD);		
	}
	close(listenFD);
	return SUCCESS;
}