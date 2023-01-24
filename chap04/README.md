# 단일 접속 서버 구현하기
* client와 server에서 소켓을 생성 한 후 TCP/UDP 통신을 통해 메시지 송수신

## 포트와 서비스 
* IP 주소: 회사의 대표 전화
* 포트 번호: 내선 번호
* 서버측 응용 프로그램: 고객 상담원
* 클라이언트측 응용 프로그램: 고객

## 포트 구분
1. 잘 알려진 포트(Well-Known Port): 0 ~1023
> - 검증되고 잘 알려진 서비스에 할당 (IANA 관리)
2. 등록된 포트 (Registered Port): 1024 ~ 49151 
> - 일반 사용자가 사용하는 서비스에 할당 (게임이나 메신저로 사용)
3. 동적 혹은 사적 포트 (Dynamic and/or Private Port): 49152 ~ 65535
> - 서비스 중에 동적으로 생성해서 사용하는 포트

## 포트와 서비스 할당 확인
* Linux 계열: /etc/services
* MS Windows: Wndows\system32\drivers\etc\service

## 포트별 서비스 내역 출력 프로그램 (getservent.c)
* getservent는 /etc/services를 struct servent로 반환하는 함수
``` c
#include <netdb.h>
struct servent *getservent(void);
```
``` c
struct servent {
  char *s_name; // 서비스 공식 명칭
  char **s_aliases; // 별명 명칭
  int s_port;       // 포트 번호
  char *s_proto;    // 사용하는 프로토콜
}
```

## 주소의 구성
* 접속점 주소 = IP 주소 (이진값/big-endian) + 포트 번호 (2바이트 이진값/big-endian)
1. IP 주소 표현 방식
> a) 도메인 이름 문자열: 예) "www.naver.com"   
> b) 점수 10진수 문자열: "202.131.30.81"   
> c) 이진값(big-endian): 0xCBF92703    
2. 포트 번호
> a) 2바이트 이진 값(big-endian): 0x1234

## 네트워크/호스트 바이트 순서 간 자료 변환 (ordering.c, ntoh_hton.c)
* 호스트는 바이트 순서가 big혹은 little endian일 수 있으며, 네트워크로 전송하기 위해서는 반드시 big-endian으로 바꿔줘야 함
``` c
#include <netinet/in.h>

uint32_t htonl(uint32_t hostlong);
uint16_t ntohs(uint16_t netshort);

```

## IP 주소 변환 시스템 콜 (inet_aton.c)
* 10진 문자열 주소 ==> 32비트 이진 주소
``` c
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int inet_aton(const char *cp, struct in_addr *inp);
```

* 32비트 이진 주소 ==> 10진 문자열 주소  
``` c
char *int_ntoa(struct in_addr in);

struct in_addr {
  u_long s_addr;  // IP 주소 (네트워크 바이트 순서의 이진 값)
}
```

## 32비트 이진 IP 주소 <=> 도메인 주소 (gethostbyname.c)
``` c
#include <netdb.h>

struct hostent *gethostbyname(const char *name);
struct hostent *gethostbyaddr(const char *addr, int len, int type);
```

* struct hostent의 h_addr_list의 char 이중 포인터로 되어 있음
  - IP 이진 주소 값 (4바이트)의 배열이며
  - 각 배열은 host의 NIC에 하나씩 할당됨
  - int *h_addr_list라고 하지 않은 이유? 두가지 주소 유형 (IPv4(4바이트)와 IPv6(16바이트)) 모두를 수용하기 위해서임

``` c
struct hostent {
  char *h_name;     // 공식 도메인 이름
  char **h_aliases;   //별명 
  int h_addrtype;     // 주소 유형
  int h_length;       // 주소 길이
  // host에 여러 개의 NIC가 있을 수 있기 때문에 이차원 배열로 함
  char **h_addr_list  // 네트워크 바이트 순서의 이진 값의 IP 주소
}
```

## 프로토콜 주소의 구성
* sockaddr_in은 sockaddr의 IPv4 전용 프로토콜 주소 구조체임
``` c
struct sockaddr {
  u_short sa_family;        // 주소 패밀리 (AF_INET, AF_LOCAL 등)
  char sa_data[14];         // 주소 (총 14바이트)
}
```
``` c
sturct sockaddr_in {
  sa_family_t sin_family;   // 프로토콜 주소 유형 (IPv4인 경우는 AF_INET)
  uint16_t    sin_port;     // 포트 번호 (네트워크 바이트 순서로 된 이진 값)
  struct in_addr  sin_addr; // IP 주소 (네트워크 바이트 순서로 된 이진 값)
  unsigned char   sin_zero[8];  // 사용 안함
}

struct in_addr {
  uint32_t  s_addr;   // IP 주소 (네트워크 바이트 순서로 된 이진 값)
}
```