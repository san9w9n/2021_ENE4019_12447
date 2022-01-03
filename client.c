// 2018008877 이상원 (컴퓨터소프트웨어학부)
#include "client.h"

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void*
serv_connect(void* args)
{
	struct sockaddr_in 		connectSocket;
	char 					sendBuffer[BUFF_SIZE];
	char 					name[100] = {'\0', };

	// Server와 연결할 socket 만들기. (127.0.0.1:8000)
	memset(&connectSocket, 0, sizeof(connectSocket));
	int connectFD = socket(PF_INET, SOCK_STREAM, 0);
	if(connectFD<0) return NULL;
	connectSocket.sin_family = AF_INET;
	connectSocket.sin_addr.s_addr = inet_addr("127.0.0.1");
	connectSocket.sin_port = htons(PORT);

	pthread_mutex_lock(&mut);

	if(connect(connectFD, (sockaddr*)&connectSocket, sizeof(connectSocket)) == -1) {
		// 연결 실패.
		printf("Connection fail!\n");
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mut);
		return NULL;
	}

	else 
	{
		printf("what is your name? : ");
		memset(sendBuffer, '\0', BUFF_SIZE);
		fgets(name, BUFF_SIZE, stdin);
		name[strlen(name) - 1] = '\0';
		write(connectFD, name, sizeof(name));						// server에 자신의 이름을 보낸다.
		while(1) {
			printf("enter : ");
			fgets(sendBuffer,BUFF_SIZE,stdin);
			sendBuffer[BUFF_SIZE-1] = '\0';
			if(!(strcmp(sendBuffer, "quit\n"))) {					// quit 입력 시에 연결종료.
				write(connectFD, sendBuffer, BUFF_SIZE);
				break;
			}
			write(connectFD, sendBuffer, BUFF_SIZE);
			pthread_cond_broadcast(&cond);							// thread는 입력 하나를 하면, 자고 있는 다른 thread를 깨운다.
			pthread_cond_wait(&cond, &mut);							// 자신은 다른 thread가 깨울 때까지 잔다.
		}
	}
	close(connectFD);
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mut);
	return NULL;
}



int main(void)
{
	pthread_t threads[3];											// 한 스레드는 하나의 client를 나타냄.

	// 스레드 생성 -> 1초 뒤 새로운 스레드 생성.
	pthread_create(&threads[0], 0, serv_connect, NULL); sleep(1); 
	pthread_create(&threads[1], 0, serv_connect, NULL); sleep(1);
	pthread_create(&threads[2], 0, serv_connect, NULL);

	for(int i=0; i<3; i++) pthread_join(threads[i], NULL);			// 모든 스레드가 종료될 때 까지 대기한다.

	return 0;
}    
