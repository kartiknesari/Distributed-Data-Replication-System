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
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include "headers/myudp.h"
#include "headers/utilities.h"
#include "headers/udp_procedures.h"
#include "headers/queue.h"
#include <stdbool.h>

int socket_fd;
int host_id;
HostInfo *hosts;
JobQueue queue;
int counter = 0;
int local_data[128];
struct sockaddr_in from;
int reply_sequence = 0;
int reply_counter = 0;

// Declaration of thread condition variable
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
// declaring mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// critical region signal
bool in_cr = false;

void printsin(struct sockaddr_in *sin, char *m1, char *m2)
{

    printf("%s %s:\n", m1, m2);
    printf("  family %d, addr %s, port %d\n", sin->sin_family,
           inet_ntoa(sin->sin_addr), ntohs((unsigned short)(sin->sin_port)));
}

void *printer_thread(void *arg)
{
    while (1)
    {
        sleep(3);
        printQueue(&queue);
    }
}

void *resolver_thread(void *arg)
{
    struct msg_packet *mymsg;
    while (1)
    {
        if (queue.front)
        {
            pthread_mutex_lock(&lock);
            if (in_cr)
            {
                pthread_cond_wait(&cond1, &lock);
            }
            else
            {
                in_cr = true;
                mymsg = dequeue(&queue);
                // printf("Dequeued a message! \n");
                // printQueue(&queue);
                in_cr = false;
                pthread_cond_signal(&cond1);
            }
            pthread_mutex_unlock(&lock);

            switch (ntohs(mymsg->cmd))
            {
            case READ:
                // The program stops working without this line, how?
                printf("Read Message\n");
                struct msg_packet reply;
                char temp_arr[128], md5_temp_arr[32];
                int len = 0;
                while (local_data[len] != 0 && len < 32)
                    len++;
                for (int i = 0; i < len; i++)
                    temp_arr[i] = (char)local_data[i];
                temp_arr[strlen(temp_arr)] = '\0';
                byte2md5(temp_arr, strlen(temp_arr), md5_temp_arr);
                printf("md5: %s\n", md5_temp_arr);
                // for (int i = 0; i < 32; i++)
                //     reply.data[i] = (unsigned int)md5_temp_arr[i];
                // for (int i = 0; i < 8; i++)
                // {
                //     sscanf(md5_temp_arr + i * 4, "%4x", &reply.data[i]);
                // }
                memcpy(reply.data, md5_temp_arr, 32);
                printf("\n");
                for (int i = 0; i < 32; i++)
                    printf("%d ", reply.data[i]);
                printf("\n");
                reply.cmd = ntohs(READ_REPLY);
                reply.seq = ntohs(1);
                // printf("Sender address: %s\n", inet_ntoa(from.sin_addr));
                send_message(socket_fd, host_id, inet_ntoa(from.sin_addr), reply);
                break;
            case WRITE:
                printf("Write message\n");
                reply_sequence = ntohs(mymsg->seq);

                /*Copy to local_data*/
                memcpy(&local_data[ntohs(mymsg->seq) * 32], mymsg->data, 32 * sizeof(int));
                // printf("\n");
                // for (int i = 0; i < 128; i++)
                //     printf("%d:%d ", i, local_data[i]);
                // printf("\n");

                /*Send back reply or broadcast message depending on if its primary server*/
                if (host_id == 0)
                    broadcast_message(socket_fd, hosts, host_id, *mymsg);
                else
                {
                    struct msg_packet reply;
                    reply.cmd = htons(WRITE_ACK);
                    reply.seq = mymsg->seq;
                    send_message(socket_fd, host_id, inet_ntoa(from.sin_addr), reply);
                }
                while (host_id == 0 && reply_counter < 3)
                    ;
                reply_counter = 0;
                if (host_id == 0)
                {
                    struct msg_packet reply;
                    reply.cmd = htons(WRITE_ACK);
                    reply.seq = mymsg->seq;
                    send_message(socket_fd, host_id, inet_ntoa(from.sin_addr), reply);
                    printf("Sent back WRITE ACK\n");
                }
                break;
            default:
                printf("Wrong command\n");
            }
        }
    }
}

void *listener_thread(void *arg)
{
    struct msg_packet msg;
    socklen_t fsize;
    int cc;

    initializeQueue(&queue);

    // printf("Thread created\n");
    while (1)
    {
        fsize = sizeof(from);
        cc = recvfrom(socket_fd, &msg, sizeof(struct msg_packet), 0, (struct sockaddr *)&from, &fsize);
        if (cc < 0)
            perror("recv_udp:recvfrom");
        // printsin(&from, "recv_udp: ", "Packet from:");
        //  printf("message received :: sender ID=%d: content=%s\n", ntohl(msg.hostid), msg.message);
        // printf("message received :: command=%d\n", ntohs(msg.cmd));
        if (ntohs(msg.cmd) == WRITE_ACK && ntohs(msg.seq) == reply_sequence)
        {
            reply_counter++;
        }
        else if (host_id == 0 || !strcmp(inet_ntoa(from.sin_addr), "137.148.142.121") || ntohs(msg.cmd) == READ)
        {
            pthread_mutex_lock(&lock);
            if (in_cr)
            {
                pthread_cond_wait(&cond1, &lock);
            }
            else
            {
                in_cr = true;
                enqueue(&queue, msg);
                // printf("Enqueued a message! \n");
                // printQueue(&queue);
                in_cr = false;
                pthread_cond_signal(&cond1);
            }
            pthread_mutex_unlock(&lock);
        }
        else
        {
            printf("Redirecting Message\n");
            send_message(socket_fd, host_id, "turing36.eecs.csuohio.edu", msg);
        }
    }
    return NULL;
}

int main()
{
    struct sockaddr_in s_in;

    pthread_t pid1, pid2;

    // Read host information from the file and get the host ID
    const char *filename = "./process.hosts";
    hosts = readHostsFromFile(filename);
    host_id = get_host_id(filename);
    if (host_id < 0 || host_id > 3)
    {
        perror("Host does not exist in process.hosts file\n");
        return 1;
    }

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd == -1)
    {
        perror("send_udp:socket");
        exit(1);
    }
    /*
       In order to attach a name to the socket created above, first fill
       in the appropriate blanks in an inet socket address data structure
       called "s_in". We blindly pick port number 0x3333. The second step
       is to BIND the address to the socket. If port 0x3333 is in use, the
       bind system call will fail detectably.
    */

    bzero((char *)&s_in, sizeof(s_in)); /* They say you must do this    */

    s_in.sin_family = (short)AF_INET;
    s_in.sin_addr.s_addr = htonl(INADDR_ANY); /* WILDCARD */
    s_in.sin_port = htons((unsigned short)UDP_PORT);
    // printsin(&s_in, "RECV_UDP", "Local socket is:");
    fflush(stdout);

    /*
       bind port 0x3333 on the current host to the socket accessed through
       socket_fd. If port in use, die.
    */
    if (bind(socket_fd, (struct sockaddr *)&s_in, sizeof(s_in)) < 0)
    {
        perror("recv_udp:bind");
        exit(1);
    }

    if (pthread_create(&pid1, NULL, listener_thread, NULL) != 0)
    {
        perror("error creating listener thread: ");
        return 1;
    }
    if (pthread_create(&pid2, NULL, resolver_thread, NULL) != 0)
    {
        perror("error creating resolver thread: ");
        return 1;
    }

    while (1)
        ;

    return 0;
}
