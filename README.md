# 2021_ENE4019_12447

## Socket programming

-	소켓 프로그래밍의 전반적인 로직은 아래 그림과 같다. 서버와 client는 각각 연결을 위한 socket을 만든다. server에서는 bind와 listen을 통해 대기열을 만들고, client에서 connect 요청이 들어오게 되면 accept를 통해서 연결을 하게 된다. 이후 서버-클라이언트 연결이 성사되고, read()/write()를 통해서 서로 데이터를 주고받을 수 있게 된다. Close()를 통해 연결을 종료할 수 있다.
-	하지만 이는 client 하나만 연결되었을 때의 상황이고, 실생활에서는 하나의 server의 여러 client가 접속하게 된다. 이때는 멀티 스레드 방식, 혹은 멀티 프로세스 방식을 사용해야한다. 이번 과제에는 멀티 프로세스를 통한 서버-클라이언트 connection을 구현하였다.
-	Makefile을 이용해 컴파일 했으며, make 명령을 입력하면, client와 server 총 두개의 실행파일이 만들어진다.

### server.c
1)	Listen socket을 만든다. (IP와 PORT는 127.0.0.1:8080으로 설정하였다.)
2)	bind()와 listen()을 통해 대기열을 만든다. 이때 queue의 size는 5로 설정하였다.
3)	Client와 연결할 connect socket을 만든다. accept()를 통해서 client의 연결요청이 올 때까지 대기한다.
4)	연결요청이 들어와 성공적으로 연결되었으면 fork()를 통해 자식프로세스를 만들어, 해당 자식프로세스에서 client와 계속 통신을 한다. 하나의 자식프로세스가 하나의 client이다.
5)	자식프로세스 내부에서는 read()를 통해 client로부터 사용자의 이름을 입력 받는다. 이후 client에서 보낸 message를 읽어서 출력한다. 이때, 해당 사용자의 이름이 메시지 앞에 나타나게 된다. client에서 quit라는 단어가 들어오면 연결을 종료한다. 이때 signal()을 통해 부모프로세스는 자식프로세스가 종료되었다는 것을 알 수 있다.
6)	부모프로세스는 다시 while문의 시작으로 돌아와서 새로운 client의 연결을 받기 위해 대기한다. 연결 요청이 들어오면 4) 로 이동한다.

### client.c
과제 specification에 클라이언트는 총 3개가 있어야한다고 했다. 처음에는 3개 분할 터미널에서 하나씩 client 실행파일을 실행하는 방법으로 하려고 하였으나, 테스트를 하기 비효율적이라고 생각해 c의 pthread를 활용하였다. 총 3개의 thread를 만들어주었고, 스레드가 동작하는 함수는 serv_connect이다. 전역변수로 pthread_cond_t 와 pthread_mutex_t 를 통해 한 client가 메시지를 보낼 때에는 다른 client들이 메시지를 보내지 못하도록 했다.

### main 함수 동작 과정
1)	Pthread_create를 통해 3개의 스레드가 serv_connect 함수를 실행하도록 했다.
2)	Pthread_join()을 통해 모든 스레드의 작업이 끝날 때까지 대기한다.
serv_connect 함수 동작 과정
1)	Server와 연결하기 위한 connect socket을 만든다.
2)	Connect() 함수를 통해 server에 연결을 요청한다.
3)	연결이 정상적으로 완료되었으면, 나의 이름을 server로 전송하고, quit가 입력되기 전까지 내가 보내고 싶은 메시지를 서버에 계속해서 보낸다.
4)	Quit가 입력되면 연결을 종료한다.

### 실행 화면
1.	Server 연결 대기
 
2.	첫번째 Client 연결시도 -> Server 연결 성공
 



3.	첫번째 client 이름 입력, 메시지 하나 전송. -> 두번째 client 연결 성공.
 	
4.	두번째 client 이름과 메시지 입력 -> 세번째 client도 연결 성공 후 이름 메시지 입력
 
5.	메시지 계속 입력하다가 quit 한 번 입력. 







6.	남은 client도 quit 입력 후 모두 연결 종료.

 
