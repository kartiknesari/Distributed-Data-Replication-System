#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "headers/myudp.h"
#include "headers/udp_procedures.h"
#include "headers/utilities.h"

int main(int argc, char **argv)
{
    int socket_fd;
    struct sockaddr_in dest;
    struct hostent *gethostbyname();

    char local_data[128];

    if (argc != 1)
    {
        printf("correct execution format: send_udp \n");
        exit(0);
    }

    /*
       Set up a datagram (UDP/IP) socket in the Internet domain.
       We will use it as the handle thru which we will send a
       single datagram. Note that, since this no message is ever
       addressed TO this socket, we do not bind an internet address
       to it. It will be assigned a (temporary) address when we send
       a message thru it.
    */

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1)
    {
        perror("send_udp:socket");
        exit(1);
    }

    /*
       The inet socket address data structure "dest" will be used to
       specify the destination of the datagram. We must fill in the
       blanks.
    */

    bzero((char *)&dest, sizeof(dest)); /* They say you must do this */

    // Read host information from the file and get the host ID
    const char *filename = "./process.hosts";
    int host_id = get_host_id(filename);
    // if (host_id < 0 || host_id > 4)
    // {
    //     perror("Host does not exist in process.hosts file\n");
    //     return 1;
    // }

    // strcpy(local_data, "Hi howa reajsnfuvnsiudfodvmioeruvsdfncerv");

    // broadcast_write_request(socket_fd, local_data, hosts, host_id);

    struct msg_packet msg;
    msg.cmd = htons(READ);
    send_message(socket_fd, host_id, "turing39.eecs.csuohio.edu", msg);

    msg.cmd = htons(WRITE);
    msg.seq = htons(0);
    srand(time(NULL));
    for (int i = 0; i < 32; i++)
        msg.data[i] = rand() % 100;
    send_message(socket_fd, host_id, "turing36.eecs.csuohio.edu", msg);

    msg.data[0] = 15;
    send_message(socket_fd, host_id, "turing37.eecs.csuohio.edu", msg);

    msg.seq = htons(2);
    send_message(socket_fd, host_id, "turing39.eecs.csuohio.edu", msg);

    msg.cmd = htons(READ);
    send_message(socket_fd, host_id, "turing38.eecs.csuohio.edu", msg);

    return (0);
}
