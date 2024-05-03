CC=gcc
CFLAGS=-g -Wall

all: recv_udp send_udp 

recv_udp: recv_udp.o
	$(CC) -o recv_udp recv_udp.o ./utilities/*.c -pthread -lcrypto -lssl

send_udp: send_udp.o
	$(CC) -o send_udp send_udp.o ./utilities/*.c -lcrypto -lssl

clean:
	rm -f *.o recv_udp send_udp 
