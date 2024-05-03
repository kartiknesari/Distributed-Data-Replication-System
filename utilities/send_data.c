#include <signal.h>
#include <errno.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "../headers/myudp.h"
#include "../headers/udp_procedures.h"
#include "../headers/utilities.h"

int send_message(int socket_fd, int hostid, const char *destination, struct msg_packet mymsg)
{
    struct sockaddr_in dest;
    struct hostent *gethostbyname(), *hostptr;

    /*
       The inet socket address data structure "dest" will be used to
       specify the destination of the datagram. We must fill in the
       blanks.
    */

    bzero((char *)&dest, sizeof(dest)); /* They say you must do this */
    if ((hostptr = gethostbyname(destination)) == NULL)
    {
        fprintf(stderr, "send_udp: invalid host name, %s\n", destination);
        return -1; // Indicate failure
    }
    dest.sin_family = AF_INET;
    bcopy(hostptr->h_addr_list[0], (char *)&dest.sin_addr, hostptr->h_length);
    dest.sin_port = htons((unsigned short)UDP_PORT);

    ssize_t cc = sendto(socket_fd, &mymsg, sizeof(struct msg_packet), 0, (struct sockaddr *)&dest,
                        sizeof(dest));
    if (cc < 0)
    {
        perror("send_udp:sendto");
        return -1; // Indicate failure
    }
    // printf("Message sent to %s \n", destination);

    return 0; // Indicate success
}

void broadcast_message(int socket_fd, HostInfo *hosts, int host_id, struct msg_packet msg)
{
    for (int i = 0; i < MAX_HOSTS; i++)
    {
        if (host_id != hosts[i].id)
        {
            char *temp_hostname = (char *)malloc(strlen(hosts[i].hostname) + strlen(".eecs.csuohio.edu") + 1);
            if (temp_hostname == NULL)
            {
                perror("Memory allocation failed");
                exit(1);
            }
            strcpy(temp_hostname, hosts[i].hostname);
            strcat(temp_hostname, ".eecs.csuohio.edu");
            send_message(socket_fd, host_id, temp_hostname, msg);
            // printf("Sent message to %s\n", temp_hostname);
            free(temp_hostname);
        }
    }
}

int broadcast_write_request(int socket_fd, char local_data[128], HostInfo *hosts, int host_id)
{
    struct msg_packet write_msg;

    write_msg.cmd = htons(WRITE);
    int temp_arr[128];
    // printf("Within write message: \n");
    for (int i = 0; i <= strlen(local_data); i++)
        temp_arr[i] = (int)local_data[i];
    for (int i = 0; i <= strlen(local_data) / data_packet_size; i++)
    {
        write_msg.seq = htons(i);
        memcpy(write_msg.data, &temp_arr[i * 32], 32 * sizeof(int));
        // for (int i = 0; i < 32; i++)
        //     printf("%d:%d ", i, write_msg.data[i]);
        // printf("\n");
        broadcast_message(socket_fd, hosts, host_id, write_msg);
    }
    return 0;
}
