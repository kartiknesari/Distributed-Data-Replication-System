#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/myudp.h"
#include "../headers/utilities.h"
#include "../headers/udp_procedures.h"
#include "../headers/server_utilities.h"

int server_write_resolve(int socket_fd, int local_data[128], struct msg_packet msg, HostInfo hosts, int host_id)
{
    /* To check the number of elements in msg.data*/
    // int length = find_length(msg.data);

    // memcpy(&local_data[msg.seq * 32], msg.data, length * sizeof(int));
    // printf("\n");

    // length = find_length(local_data);
    // for (int i = 0; i < length; i++)
    //     printf("%d ", local_data[i]);
    printf("\n");
    // if (host_id == 0)
    //     broadcast_message(socket_fd, &hosts, host_id, msg);
}