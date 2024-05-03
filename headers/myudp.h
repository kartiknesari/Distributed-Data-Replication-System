#ifndef MYUDP
#define MYUDP

#define READ 10
#define READ_REPLY 11
#define WRITE 12
#define WRITE_ACK 13

#define data_packet_size 32
#define UDP_PORT 0x5835

// struct msg_packet
// {
//   unsigned short cmd;
//   unsigned short seq;
//   unsigned int hostid;
//   unsigned int tiebreak;
//   unsigned short vtime[5];
//   unsigned int account_balance;
// };

struct msg_packet
{
  unsigned short cmd;
  unsigned short seq;
  unsigned int data[32];
};

#endif