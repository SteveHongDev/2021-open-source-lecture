이번 오픈소스프로젝트 6단계부터는 한 컴퓨터에서 진행하는 것이 아닌, Raspberry Pi를 Client부분으로, 기존 컴퓨터는 Server부분으로 활용할 것입니다.

그에 따라 각각의 컴퓨터의 역할이 분리되므로, 자연스럽게 필요로 하는 소스코드도 분리될 것입니다.

1. Raspberry Pi 부분(Client)은 이 소스코드들을 활용할 것입니다.

alarm.c
alarmServer.c
clientGet.c
clientPost.c
clientRPI.c
request.c
request.h
stems.c
stems.h

Raspberry Pi로 옮겨지면서 clientPost의 역할은 clientRPI가 하게 되지만, 일단은 clientPost도 남겨두었습니다.

<사용법>
cd source-lv6-client 로 이동하신 후에, 해당 폴더 내에서 make를 하시면, 폴더 내 Makefile이 동작할 것입니다.

2. 기존 컴퓨터 부분(Server)은 이 소스코드들을 활용할 것입니다.

alarmCLient.c
dataGet.c
dataPost.c
request.c
request.h
server.c
stems.c
stems.h

<사용법>
cd source-lv6-server 로 이동하신 후에, 해당 폴더 내에서 make를 하시면, 폴더 내 Makefile이 동작할 것입니다.