/*
 * =====================================================================================
 *
 *       Filename:  benchmark-server.c
 *
 *    Description:  Creates a socket to perform download and upload benchmark tests.
 *
 *        Version:  1.0
 *        Created:  2017/12/04 17:30:13
 *       Compiler:  gcc
 *
 *           Test:
 *                  - Run Listener: `./netlink-listener`
 *
 *        Authors:  Navid (@yaa110), 
 *        License:  MIT  
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#define MAX_CLIENTS 3
#define BUFFER_SIZE 16384

void print_error(char *, bool);
void print_usage();
void handle_client(int, bool);
void *handle_download(void *);
void *handle_upload(void *);

void print_error(char *err, bool fatal) {
    fprintf(stderr, err, errno != 0 ? strerror(errno) : NULL);
    if (fatal)
        exit(EXIT_FAILURE);
}

void print_usage() {
    print_error("\
usage:\n\
\tbenchmark <subcommand> <port>\n\
subcommands:\n\
\t-d\tstart download listener\n\
\t-u\tstart upload listener\n", true);
}

void *handle_download(void *arg) {
    int soc = *((int *)arg);
    free(arg);

    const char *res = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
    write(soc, res, strlen(res));

	FILE *fp = fopen("/dev/zero", "rb");
    char buffer[BUFFER_SIZE];
    do {
        fread(buffer, BUFFER_SIZE, 1, fp);
    } while (write(soc, buffer, BUFFER_SIZE) >= 0);

    fclose(fp);
    close(soc);
    return NULL;
}

void *handle_upload(void *arg) {
    int soc = *((int *)arg);
    free(arg);

    const char *res = "HTTP/1.1 200 OK\r\nAccept: */*\r\nAccess-Control-Allow-Origin: *\r\n\r\n";
    write(soc, res, strlen(res));

    char buffer[BUFFER_SIZE];
    while (read(soc, buffer, BUFFER_SIZE) > 0);

    close(soc);
    return NULL;
}

void handle_client(int soc, bool is_download) {
    int *soc_arg = malloc(sizeof(*soc_arg));
    *soc_arg = soc;
    
    pthread_t th;
    pthread_create(&th, NULL, is_download ? handle_download : handle_upload, (void *)soc_arg);
    pthread_detach(th);
}

int main(int argc, char **argv) {
    int soc, cli_soc, ret, addr_len;
    struct sockaddr_in addr, cli_addr;

    if (argc != 3)
        print_usage();

    bool is_download;
    if (strcmp(argv[1], "-d") == 0)
        is_download = true;
    else if (strcmp(argv[1], "-u") == 0)
        is_download = false;
    else
        print_usage();

    soc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc < 0)
        print_error("unable to create socket: %s\n", true);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = INADDR_ANY;
    addr_len = sizeof(struct sockaddr_in);

    ret = bind(soc, (struct sockaddr *)&addr, addr_len);
    if (ret < 0) {
        close(soc);
        print_error("unable to assign address to socket: %s\n", true);
    }

    ret = listen(soc, MAX_CLIENTS);
    if (ret < 0) {
        close(soc);
        print_error("unable to listen for socket connections: %s\n", true);
    }

    while (true) {
        cli_soc = accept(soc, (struct sockaddr *)&cli_addr, (socklen_t *)&addr_len);
        if (cli_soc < 0)
            print_error("unable to accept client: %s\n", false);

        handle_client(cli_soc, is_download);
    }

    close(soc);
    return EXIT_SUCCESS;
}

