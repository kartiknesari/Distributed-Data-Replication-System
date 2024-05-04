CC=gcc
CFLAGS=-g -Wall

receiver: recv_udp.o
	$(CC) -o receiver recv_udp.o ./utilities/*.c -pthread -lcrypto -lssl

send_msg: send_udp.o
	$(CC) -o send_msg send_udp.o ./utilities/*.c -lcrypto -lssl

all: receiver send_msg

clean:
	rm -f *.o receiver send_msg 
