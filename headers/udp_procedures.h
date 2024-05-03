#ifndef UDP_PROCEDURES
#define UDP_PROCEDURES

#include "utilities.h"

int send_message(int socket_fd, int hostid, const char *destination, struct msg_packet msg);
void broadcast_message(int socket_fd, HostInfo *hosts, int host_id, struct msg_packet msg);
int broadcast_write_request(int socket_fd, char local_data[128], HostInfo *hosts, int host_id);

#endif