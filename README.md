Есть устройство с Linux в качестве ОС. Данное устройство имеет Ethernet 
и несколько интерфейсов CAN. Необходимо разработать скрипт или программу, 
позволяющую использовать данное устройство как конвертер UDP-to-CAN, 
где каждому CAN-устройству соответствует определенный приемный UDP-порт, 
данные с которого передаются в соответствующий CAN, и наоборот, данные, 
пришедшие из CAN, передаются на другой, заранее определенный UDP-порт 
по заранее определенному IP-адресу. В качестве API работы
с CAN рекомендуется использовать SocketCAN.

Решение вижу следующее:
пишем два процесса:
udp_worker - получает и отправляет данные по UDP;
can_worker - получает и отправляет данные по CAN;
обмениваются данными эти процессы между собой посредством механизма
разделяемой памяти. Сделал так. Однако очевидно:

Это всё-таки не UNIX-way. Еще больше нужно процессов:
во-первых, слушать UDP порты нужно только одним процессом, только слушать
(т.е. принимать) и больше ничего. И этот процесс, кроме того,
должен форкаться (или на потоки его надо делить) для каждого UDP-порта.
Потому что сейчас пока recivefrom() блокирует сокет одного порта, другие
порты в пролёте и мы данные теряем. И ещё так нам будет легче увеличивать
количество портов (сейчас их два только), количество вшито в код,
значит программа не масштабируемая. Что тоже не UNIX-way(((

Во-вторых отправлять данные в UDP нужно ещё одним процессом, но тут надо
вспоминать как делить ресурсы, нужно почитать Стивенса.

Тоже самое нужно сказать про CAN приём/передачу. Так как формат фрейма
данных CAN таков, что приём и передачу можно осуществлять небольшими
порциями (8 байт каждая). То следует также разделить приём и передачу в 
разных потоках.


