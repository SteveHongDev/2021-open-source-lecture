오픈소스프로젝트 8단계 소스코드입니다.

MariaDB -> MongoDB : Database를 관계형 DB에서 NoSQL DB로 변경해보았습니다.

1. source-lv8-client 부분은 Raspberry Pi에서 구동될 수 있도록 구현하였습니다.

<사용법>
cd source-lv6-client 로 이동하신 후에, 해당 폴더 내에서 make를 하시면, 폴더 내 Makefile이 동작할 것입니다.
그 후에 ./clientPost와 ./clientGet을 각각 실행하여 새로운 DB인 MongoDB와 소통이 잘 되는지를 알아볼 수 있습니다.

2. source-lv8-server 부분은 MacBook에서 구동될 수 있도록 구현하였습니다.

<사용법>
cd source-lv6-server 로 이동하신 후에, 해당 폴더 내에서 make를 하시면, 폴더 내 Makefile이 동작할 것입니다.
그 후에 ./server 를 실행하면 되겠습니다.