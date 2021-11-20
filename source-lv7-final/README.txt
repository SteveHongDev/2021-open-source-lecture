오픈소스프로젝트 7단계 소스코드입니다.

1. source-lv7-client 부분은 Raspberry Pi에서 구동될 수 있도록 구현하였습니다.

<사용법>
cd source-lv6-client 로 이동하신 후에, 해당 폴더 내에서 make를 하시면, 폴더 내 Makefile이 동작할 것입니다.
그 후에 ./clientPost 를 실행하여 prand n 명령을 할 수 있습니다.

2. source-lv7-server 부분은 MacBook에서 구동될 수 있도록 구현하였습니다.
sem_init 과 같은 함수가 Mac OS 에서는 없어졌기 때문에, stems.c에 맨 마지막 부분에 Mac OS에서 구동될 수 있도록 하는
함수를 따로 구현하였음을 알려드립니다.

<사용법>
cd source-lv6-server 로 이동하신 후에, 해당 폴더 내에서 make를 하시면, 폴더 내 Makefile이 동작할 것입니다.
그 후에 ./server 를 실행하면 되겠습니다.