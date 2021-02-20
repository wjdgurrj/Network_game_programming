# Network_game_programming

📲 SOCKET을 사용한 실시간 통신으로, server가 1player의 움직임 정보와 2player의 움직임 정보를 서로 실시간으로 주고 받을 수 있게 각각의 정보를 전송해준다.

## 실행 예시) 

### TCPsever를 켜서 client 접속을 accpet한다.

![client_accept](https://user-images.githubusercontent.com/59694789/108600709-0ba08280-73dc-11eb-9bcb-24b0332136a8.png)

### 2명의 client가 접속을 하면 게임을 시작한다.

👩player1 준비

![player1_start](https://user-images.githubusercontent.com/59694789/108592415-27426380-73b1-11eb-8083-743bac7d1ee0.PNG)

🧑player2 준비

![player2_start](https://user-images.githubusercontent.com/59694789/108592416-2a3d5400-73b1-11eb-9e8e-d57620ba3f73.PNG)

👩player1 동작

![player1_ing](https://user-images.githubusercontent.com/59694789/108592418-2c071780-73b1-11eb-865b-35d9a71b5200.PNG)

🧑player2 동작

![player2_ing](https://user-images.githubusercontent.com/59694789/108592455-47722280-73b1-11eb-8f34-66fd8f5f3a14.PNG)

### 게임이 종료되면 게임 종료창이 뜬다.

![game_end](https://user-images.githubusercontent.com/59694789/108600712-0e9b7300-73dc-11eb-945b-6c8f3b7904cf.png)

### client 접속을 해지하면 server가 이를 인식하고 통신을 종료한다.

![client_end](https://user-images.githubusercontent.com/59694789/108600711-0cd1af80-73dc-11eb-81bf-57e3329bb49f.png)
