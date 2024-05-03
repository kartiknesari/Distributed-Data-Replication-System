#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <openssl/evp.h>
#include "../headers/utilities.h"

HostInfo *readHostsFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Error opening file");
        return NULL;
    }

    HostInfo *hosts = malloc(MAX_HOSTS * sizeof(HostInfo));
    if (!hosts)
    {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    char line[MAX_HOSTNAME_LENGTH + 10]; // Assuming the ID won't exceed 10 characters
    int count = 0;

    while (fgets(line, sizeof(line), file) && count < MAX_HOSTS)
    {
        // Tokenize the line to separate hostname and ID
        char *temp_hostname = strtok(line, " ");
        size_t i = strcspn(temp_hostname, ".");
        char *hostname = strndup(temp_hostname, i);
        char *id_str = strtok(NULL, " \n");

        if (hostname && id_str)
        {
            // Store hostname
            strcpy(hosts[count].hostname, hostname);
            // Convert ID from string to integer
            hosts[count].id = atoi(id_str);
            count++;
        }
    }

    fclose(file);

    return hosts;
}

int get_host_id(const char *filename)
{
    static char hostname[MAX_HOSTNAME_LENGTH]; // Static buffer to hold the hostname
    if (gethostname(hostname, MAX_HOSTNAME_LENGTH) != 0)
    {
        // Error occurred while getting hostname
        perror("Error getting hostname");
        return -1;
    }
    HostInfo *hosts = readHostsFromFile(filename);
    if (!hosts)
    {
        printf("Failed to read hosts from file.\n");
        return -1;
    }
    // for (int i = 0; i < MAX_HOSTS; i++)
    //     printf("%s : %d\n", hosts[i].hostname, hosts[i].id);
    for (int i = 0; i < MAX_HOSTS; i++)
    {
        if (!strcmp(hosts[i].hostname, hostname))
            return hosts[i].id;
    }
    return -1;
}

int find_length(int local_arr[128])
{
    printf("Find length local array:\n");
    char char_arr[128];
    for (int i = 0; i < 128; i++)
        char_arr[i] = (char)local_arr[i];

    for (int i = 0; i < 128; i++)
        printf("%c ", char_arr[i]);
    printf("\n");
    int i = 1;

    while (char_arr[i] != '\0')
    {
        i++;
    }
    return i;
}

void byte2md5(const char *data, int length, char *md5buf)
{
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_md5();
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len, i;
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, data, length);
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    EVP_MD_CTX_free(mdctx);
    for (i = 0; i < md_len; i++)
    {
        snprintf(&(md5buf[i * 2]), 16 * 2, "%02x", md_value[i]);
    }
}