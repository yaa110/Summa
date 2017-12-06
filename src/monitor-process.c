/*
 * =====================================================================================
 *
 *       Filename:  monitor-process.c
 *
 *    Description:  Forks and waits for the child process and records the output of
 *                  child process.
 *
 *        Version:  1.0
 *        Created:  2017/12/06 20:58:14
 *       Compiler:  gcc
 *
 *           Test:
 *                  - Monitor ls: `./monitor-process /usr/bin/ls /tmp`
 *
 *        Authors:  Navid (@yaa110), 
 *        License:  MIT
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdbool.h>

#define BUFFER_SIZE 1024

pid_t pid;
bool has_done;
pthread_mutex_t mutex;

void kill_child(int);
void *read_output(void *);

void *read_output(void *arg) {
    int fd = *((int *)arg);
    free(arg);
    
    int ret = 0;

    char buffer[BUFFER_SIZE];
    do {
        ret = read(fd, buffer, BUFFER_SIZE);
        pthread_mutex_lock(&mutex);
        if (has_done) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
        printf("%s", buffer);
    } while(ret >= 0);

    return NULL;
}

void kill_child(int sig) {
    kill(pid, sig);	
}

int main(int argc, char **argv) {
    int status;
    int fds[2];

    if (argc < 2) {
        fprintf(stderr, "usage: ./monitor-process <program> [args]\n");
        exit(EXIT_FAILURE);
    }

    pipe(fds);
    pid = fork();

    if (pid == 0) {
        /* Child Process */
        dup2(fds[1], 1);
        dup2(fds[1], 2);

        execvp(argv[1], argv + 1);

        /* The exec() functions return only if an error has occurred. */
        fprintf(stderr, "error: unable to exec process: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        /* Parent Process */

        /*send signals to child process */
        signal(SIGTERM, (void (*)(int)) kill_child);
        signal(SIGHUP, (void (*)(int)) kill_child);
        signal(SIGUSR1, (void (*)(int)) kill_child);
        signal(SIGINT, (void (*)(int)) kill_child);
        signal(SIGQUIT, (void (*)(int)) kill_child);

        pthread_mutex_init(&mutex, NULL);

        pthread_mutex_lock(&mutex);
        has_done = false;
        pthread_mutex_unlock(&mutex);

        int *fd_arg = malloc(sizeof(*fd_arg));
        *fd_arg = fds[0];
        pthread_t read_thread;
        pthread_create(&read_thread, NULL, read_output, (void *)fd_arg);
        pthread_detach(read_thread);

        if (wait(&status) < 0) {
            fprintf(stderr, "error: unable to wait: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&mutex);
        has_done = true;
        pthread_mutex_unlock(&mutex);

        close(fds[1]);
        close(fds[0]);

        int exit_code;
        if (WIFEXITED(status)) {
            exit_code = WEXITSTATUS(status);
            printf("\nexited with status code of %d\n", exit_code);
        } else if (WIFSIGNALED(status)) {
            exit_code = WTERMSIG(status);
            printf("\nexited with signal of %d\n", exit_code);
        } else {
            printf("\nexited without status code or signal\n");
        }
    } else {
        /* Error */
        fprintf(stderr, "error: unable to fork process: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    pthread_mutex_destroy(&mutex);
    return EXIT_SUCCESS;
}

