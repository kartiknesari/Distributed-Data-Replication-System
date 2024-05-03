CC=gcc
CFLAGS=-g -Wall

all: recv_udp send_udp 

recv_udp: recv_udp.o
	$(CC) -o recv_udp recv_udp.o ./utilities/*.c ./server_procedures/*.c -pthread

send_udp: send_udp.o
	$(CC) -o send_udp send_udp.o ./utilities/*.c

clean:
	rm -f *.o recv_udp send_udp 
