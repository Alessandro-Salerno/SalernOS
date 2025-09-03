#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/sock1"

int main() {
    int                listen_fd, conn_fd;
    struct sockaddr_un addr;

    // Remove any existing socket file
    unlink(SOCKET_PATH);

    // Create socket
    if ((listen_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Setup address
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // Bind socket
    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) ==
        -1) {
        perror("bind");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(listen_fd, 5) == -1) {
        perror("listen");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process - client
        // Wait a bit to ensure server is listening
        sleep(1);

        int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (client_fd == -1) {
            perror("client socket");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_un client_addr;
        memset(&client_addr, 0, sizeof(struct sockaddr_un));
        client_addr.sun_family = AF_UNIX;
        strncpy(client_addr.sun_path,
                SOCKET_PATH,
                sizeof(client_addr.sun_path) - 1);

        if (connect(client_fd,
                    (struct sockaddr *)&client_addr,
                    sizeof(struct sockaddr_un)) == -1) {
            perror("connect");
            close(client_fd);
            exit(EXIT_FAILURE);
        }

        // No message sending, just connect
        close(client_fd);
        exit(EXIT_SUCCESS);
    } else {
        // Parent process - server
        conn_fd = accept(listen_fd, NULL, NULL);
        if (conn_fd == -1) {
            perror("accept");
            close(listen_fd);
            unlink(SOCKET_PATH);
            exit(EXIT_FAILURE);
        }

        printf("Server: Connection accepted!\n");

        close(conn_fd);
        close(listen_fd);
        unlink(SOCKET_PATH);

        wait(NULL); // Wait for child
    }

    return 0;
}
