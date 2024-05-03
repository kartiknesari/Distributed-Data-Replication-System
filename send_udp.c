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
    // if ((hostptr = gethostbyname(argv[1])) == NULL)
    // {
    //     fprintf(stderr, "send_udp: invalid host name, %s\n", argv[1]);
    //     exit(1);
    // }
    // dest.sin_family = AF_INET;
    // bcopy(hostptr->h_addr_list[0], (char *)&dest.sin_addr, hostptr->h_length);
    // dest.sin_port = htons((unsigned short)UDP_PORT);

    // Read host information from the file and get the host ID
    const char *filename = "./process.hosts";
    HostInfo *hosts = readHostsFromFile(filename);
    int host_id = get_host_id(filename);
    if (host_id < 0 || host_id > 4)
    {
        perror("Host does not exist in process.hosts file\n");
        return 1;
    }

    strcpy(local_data, "Hi howa reajsnfuvnsiudfodvmioeruvsdfncerv");

    broadcast_write_request(socket_fd, local_data, hosts, host_id);

    // printf("Local host data: ");
    // for (int i = 0; i < 128; i++)
    //     printf("%d ", local_data[i]);
    // printf("\n");

    return (0);
}
