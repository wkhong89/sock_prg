# 단일 접속 서버 연결
## TCP 소켓 프로그래밍
1. TCP Server
> * 연결 준비 단계
> > + socket 생성 --> 주소 생성 --> bind (socket에 주소 할당) --> listen (socket 등록)
> * 서비스 처리 단계
> > + 연결 접수: accept (blocking)
> > + 자료 송수신: read (socket의 receive buffer에서 읽음)/ write (socket의 send buffer에 씀)
> > + 연결 종료:  close
> * 서버 종료 단계
> > + close

2. TCP client
> * 연결 준비 단계
> > + socket 생성 --> 서버 주소 생성
> * 서비스 처리 단계
> > + 서버 연결: connect
> > + 자료 송수신: read (socket의 receive buffer에서 읽음)/ write (socket의 send buffer에 씀)
> > + 연결 종료: close


## TCP 서버 프로그램의 기능 수행
### 연결 준비 단계
> 1. 소켓 생성(socket): 네트워크 통신을 위해 소켓을 생성; descriptor table에 소켓에 연결될 엔트리의 인덱스 번호를 반환
> > * descriptor table의 0(stdin), 1(stdout), 2(stderr)은 미리 정해져 제외
> 2. 연결할 주소 준비: 클라이언트로부터 서비스 요청을 접수할 서버의 주소 (IP 주소와 포트 번호)를 결정하여 구조체 변수에 저장
> 3. 주소 연결(bind): 소켓에 프로토콜 주소를 할당하여 실제로 연결
> > * 소켓은 듣기 소켓: s[자신의 접속점(IP 주소: 포트번호), 원격지의 접속점(IP 주소: 포트번호)]
> > > - 예) s[*:7000, *.*]: 원격지에서 자신의 모든 네트워크 인터페이스를 통해 7000번 포트를 목적지로 하는 모든 연결 요청 접수
> 4. 개통 요청(listen): 클라이언트의 연결 요청이 오면 접수를 할 수 있도록 함

### 서비스 처리 단계
> 1. 클라이언트 소켓 생성 (socket): 클라이언트에서 네트워크 통신을 위해 소켓 생성
> > 예) client(IP 주소: 163.1.2.3, 포트번호: 200)
> 2. 클라이언트 연결 요청 (connect): 클라이언트에서 서버로 연결 요청
> > 예) c[163.1.2.3:200, 162.5.6.7:7000]
  
> 3. 연결 접수 (accept): 서버는 클라이언트로부터의 연결 요청을 대기하고 있다가 접수
> > 예) s[*:7000, *:*]로 부터 s1[162.5.6.7:7000, 163.1.2.3:200] 소켓 생성
> 4. 자료의 송수신 (read/write)
> > 예) s1[162.5.6.7:7000, 163.1.2.3:200]와 c[163.1.2.3:200, 162.5.6.7:7000] 소켓 자료 송수신
> 5. 연결 종료(close/shutdown): 클라이언트와의 연결 소켓 종료

### 연결 준비 단계
``` c
int s;
struct sockaddr_in s_addr;
// 1. TCP 소켓 생성
s = socket(PF_INET, SOCK_STREAM, 0); 

// 2. 연결할 주소 준비
memset(&s_addr, 0, sizeof(s_addr));
s_addr.sin_addr.s_addr = inet_addr(INADDR_ANY);
s_addr.sin_family =  AF_INET;
s_addr.sin_port = htons(7000);

// 3. 주소에 연결
bind(s, (struct sockaddr *)&s_addr, sizeof(s_addr));

// 4. 개통 요청
listen(s, 0);
```
* INADDR_ANY는 server가 갖고 있는 NIC의 구분없이 모두 수신하겠음을 의미
* 프트번호(s_addr.sin_port)를 0으로 설정하면 시스템이 자동으로 포트를 할당함
* listens(s, 0)의 두번째 인자는 back_log라 함; 
> - back_log는 연결 요청이 들어오는 client들의 queue 크기를 정해 줌
> -  즉, back_log는 클라이언트로부터 연결 접수를 허용하고 대기시켜 줄 수 있는 수를 의미함
> - 연결 요청 중 (SYN_RECEIVED)와 연결 완료 (ESTABLISHED) 상태를 합친 값
> - accept 함수를 호출하면 대기 중인 큐에서 하나의 연결 접수를 가져옴
* netstat 명령어
> - 소켓의 큐 상태(LISTEN, SYN_RECEIVED, ESTABLISHED)를 확인할 수 있음
> > - 예) netstat -a | more
> > - 예) netstat -an | more

### 서비스 처리 단계
``` c
int s, c;
int len;
struct sockaddr_in s_addr;

// 1. 연결 접수 (accept)
len = sizeof(s_addr);
c = accept(s, (struct sockaddr *)s_addr, &len);

// 2. 자료의 송수신 (read/write)
int n;
char rcvBuffer[BUFSIZ];
char sndBuffer[BUFSIZ];

n = read(c, rcvBuffer, BUFSIZ);
n = write(c, sndBuffer, BUFSIZ);

// 3. 연결 종료 단계
close(c);
```

* 연결 접수 (accept)
> - 듣기 소켓은 원격지로부터 연결 요청을 받게 됨
> - 연결 요청은 커널 내부에서 3단계 핸드쉐이킹으로 이루어지며, 3단계 핸드쉐이킹이 완료되면 ESTABLISHED 상태로 큐에 대기
> - accept 함수를 호출하면, ESTABLISHED 상태에 있는 큐에서 대기하고 있는 소켓들 중에 하나의 소켓을 리턴하며 이것을 연결 소켓이라 함

* 자료의 송수신 (read/write)
> - 연결 소켓 내에 송신 버퍼 (Send Buffer)와 수신 버퍼 (Receive Buffer)가 생성됨
> - 송신을 위해 사용자 버퍼에서 연결 소켓의 송신 버퍼로 메시지를 write
> - 마치, 송신 버퍼는 우편함, TCP 혹은 UDP가 배달부의 역할을 하는 것과 같음
> - 수신은 소켓의 수신 버퍼에서 사용자 버퍼로 메시지를 read(copy)

* 블록킹 모드
> + write 시
> > - 모든 데이터가 커널 송신 버퍼에 복사될 때까지 차단 (blocking)
> > - 모든 데이터가 커널 송신 버퍼에 복사된 후 응용 프로그램으로 복귀
> + read 시
> > - 커널 수신 버퍼에 전혀 수신 데이터가 없으면 데이터 도착까지 차단 (blocking)
> > - 데이터가 커널 수신 버퍼에 도착하면 응용 프로그램의 버퍼로 복사한 후 복귀

* 비블록킹 모드
> + write 시
> > - 데이터가 커널 송신 버퍼에 한 바이트도 복사할 수 없으면 응용프로그램으로 복귀 (EWOULDBLOCK)
> > - 일부 데이터라도 커널 버퍼에 복사할 수 있는 경우는 일부만 복사 후 응용프로그램으로 복귀 (복사한 데이터 바이트 수 변환)
> + read 시
> > - 커널 수신 버퍼에 전혀 수신 데이터가 없으면 응용프로그램으로 복귀 (EWOULDBLOCK)
> > - 커널 수신 버퍼에 일부라도 데이터가 있는 경우는 일부만 복사 후 응용프로그램으로 복귀 (복사한 데이터 바이트 수 변환)

* 송신 시 커널 버퍼로 한번에 데이터를 모두 보낼 수 없을 수 있음
> + 예) n = write(s, buf, 7) // n=5
> + 예에서 처럼, 7바이트를 전송하도록 요청했는데 5바이트만 전송되는 경우가 있을 수 있음
> + 따라서, 모든 데이터를 전송 보장하기 위해 다음과 같이 while 문을 사용함
``` c
char sndBuffer[200];
int n, n_send;
int c_socket;
int n_data = 100;

n_left = n_data;
n_send = 0;

while (n_left > 0) {
  if ((n = write(c_socket, &sndBuffer[n_send], n_left) < 0) {
    perror("write error");
    exit (-1);
  }
  n_left = n_left - n;
  n_send = n_send + n;
}
```

* 수신 시 커널 버퍼에 데이터가 도착하지 않아 한번에 데이터가 모두 수신되지 않을 수 있음
> + 예) n = read(c, buf, 7); // n=2
> + 예에서 처럼, 7바이트를 수신하도록 요청했는데 2바이트만 수신하는 경우가 있을 수 있음
``` c
char rcvBuffer[200];
int n, n_left, n_recv;
int c_socket;
int n_send = 100;

n_left = n_send;
n_recv = 0;

while (n_left > 0) {
  if ((n = read(c_socket, &rcvBuffer[n_recv], n_left)) < 0) {
    perror("read error");
    exit (-1);
  }

  n_left = n_left - n;
  n_recv = n_recv + n;
}
```

### 종료 단계
* close는 read/write를 동시에 모두 종료하지만, shutdown은 선택적으로 일부만 종료 가능
``` c
int r, s;

r = shutdown(s, SHUT_RD);   // 읽기만 종료
// SHUT_WR: 쓰기만 종료, SHUT_RDWR: 읽기/쓰기 모두 종료
```

## TCP 클라이언트 프로그램의 기능 수행
### 연결 준비 단계
* 소켓 생성 (socket): 네트워크 통신을 위해 소켓을 생성
``` c
int c_sock;
c_sock = socket(PF_INET, SOCK_STREAM, 0);
```
* 연결할 서버 주소 준비: 연결할 서버의 프로토콜 주소(IP 주소와 포트 번호)를 구조체 변수에 저장
``` c
struct sockaddr_in c_addr;

memset(&c_addr, 0, sizeof(c_addr));
c_addr.sin_addr.s_addr = inet_addr("10.1.2.3");
c_addr.sin_family = AF_INET;
c_addr.sin_port = htons(7000);
```

### 서비스 처리 단계
* 서버 연결 (connect): connect 함수를 호출하여 소켓을 서버에 연결
> - 비어 있는 임의의 포트를 할당하여 서버와 연결
> - c[자신의 접속점, 원격지의 접속점]
> > + 예) c[163.1.2.3:31523, 10.1.2.3:7000]
> > + IP 163.1.2.3의 포트번호 31523과 서버(10.1.2.3)의 포트 번호 7000이 연결된 소켓을 의미
> > + 서버에서는 듣기 소켓 s[*:7000, *.*]으로 
```c
connect(c_socket, (struct sockaddr *)&c_addr, sizeof(c_addr));
```

* EOF를 이용한 라인 단위의 자료 송수신
``` c
int sock_sd;
int n, r, length;
char c;
char buf[BUFSIZ], *ptr;

length = 0;
ptr = buf;

while ((n = read(sock_sd, ptr, 1)) > 0) {
  if (*ptr == '\r') continue;
  if (*ptr == '\n') break;
  if (length == BUFSIZ) break;
  ptr++; length++;
}
buf[length] = '\0';
```
* 1바이트씩 읽어서 성능이 떨어짐
* 개선한 코드는 다음과 같음
``` c
static int r_count = 0;
static char *r_ptr;
static char r_buffer[BUFSIZ];

int read_line(int sd, char *temp)
{
  if (r_count <= 0) {
    if ((r_count = read(sd, r_buffer, sizeof(r_buffer)) < 0)  {
      return -1;
    } else if (r_count == 0) {
      return 0;     // EOF
    } 
    r_ptr = r_buffer;
  }
  r_count--;
  *temp = *r_ptr++;

  return 1;
}
```
``` c
int sock_sd;
char buf[BUFSIZ];
char *ptr;
int length = 0;

ptr = buf;
while ((n = read_line(sock_sd, ptr)) > 0) {
  if (*ptr == '\r') continue;
  if (*ptr == '\n') break;
  if (length == BUFSIZ) break;
  ptr++; length++;
}
buf[length] = '\0';
```

## TCP 기반 에코 프로그램
> 1) 클라이언트는 서버와 연결한다.
> 2) 클라이언트는 사용자로부터 데이터를 받아 서버로 보낸다.
> 3) 서버는 클라이언트로 부터 받은 동일한 데이터를 클라이언트로 다시 보낸다.
> 4) 사용자가 종료를 의미하는 문자열 'quit'를 입력하면 클라이언트는 서버와의 연결을 종료한다.

### 연결 종료
> * 연결 종료를 위해 서버든 클라이언트든 먼저 close()를 호출한 쪽은 일정 시간 (timeout)이 지난 후에 CLOSED 상태로 가기 때문에 그 사이에는 해당 포트를 다른 응용프로그램에서 일정시간동안 사용할 수 없는 상태가 됨. 따라서, 서버가 close()를 먼저 호출한 경우 일정 시간 동안 서비스를 제공할 수 없는 상황이 발생할 수 있음 (소켓 옵션 11장)
> * 호스트 A가 호스트 B로 close () 호출시 상태 변화
> > 1) A: FIN 패킷을 보내면서 FINWAIT_1 상태가 됨 --> B: FIN 패킷을 수신하면 CLOSE_WAIT 상태가 되며, read함수는 0을 반환
> > 2) B: ACK 패킷을 보냄 --> A: ACK 패킷을 수신하면 FINWAIT_2 상태가 됨
> > 3) B: close()함수 호출로 FIN 패킷을 보내고 LAST_ACK 상태가 됨 --> A: FIN 패킷을 수신하고 TIME_WAIT 상태가 됨
> > 4) A: ACK 패킷을 보내고 일정시간 후 (timeout) CLOSED 상태가 됨 --> B:  ACK 패킷을 보내면 바로 CLOSED 상태가 됨
> * echo_server와 echo_client를 실행 시키고 나서 echo_client를 종료시킨 후, 'netstat -ant'를 이용해 echo_client의 상태가 TIME_WAIT 상태를 한동안 유지하는 것을 확인할 수 있음