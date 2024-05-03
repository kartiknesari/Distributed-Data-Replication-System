#ifndef SERVER_UTILITIES
#define SERVER_UTILITIES

#include "utilities.h"

int server_write_resolve(int socket_fd, int local_data[128], struct msg_packet msg, HostInfo hosts, int host_id);

#endif