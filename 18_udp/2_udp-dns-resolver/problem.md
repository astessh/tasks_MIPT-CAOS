# posix/sockets/udp-dns-resolver

Программа читает со стандартного потока последовательность лексем - имен хостов.

Необходимо для каждого имени сформировать UDP-запрос к DNS-серверу 8.8.8.8 для того, чтобы получить IP-адрес сервера для
записи типа A. Далее - получить ответ от сервера и вывести IP-адрес на стандартный поток вывода.

Гарантируется, что для каждого запроса существует ровно 1 IP-адрес.

Указание: используйте инструменты dig и wireshark для того, чтобы исследовать формат запросов и ответов.

## Examples

Input

```
ejudge.ru
ejudge.atp-fivt.org
  ```

Output

```
89.108.121.5
87.251.82.74
 ``` 