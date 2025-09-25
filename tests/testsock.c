#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

// #define NO_RECVSENDMSG

#define SOCKET_PATH "/tmp/sock1"
#define BUF_SIZE    128

void handle_server(int listen_fd);
void handle_client();

int main() {
    int                listen_fd;
    struct sockaddr_un addr;

    unlink(SOCKET_PATH);

    if ((listen_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) ==
        -1) {
        perror("bind");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

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
        // Child - client
        poll(NULL, 0, 1000); // fancy wait
        handle_client();
        exit(EXIT_SUCCESS);
    } else {
        // Parent - server
        handle_server(listen_fd);
        wait(NULL); // Wait for child
        unlink(SOCKET_PATH);
    }

    return 0;
}

void handle_server(int listen_fd) {
    fprintf(stderr, "Server: doing accept(%d)\n", listen_fd);
    int conn_fd = accept(listen_fd, NULL, NULL);
    if (conn_fd == -1) {
        perror("accept");
        close(listen_fd);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Server: Connection accepted (fd = %d)!\n", conn_fd);
    fflush(stderr);
    fprintf(stderr, "Server: trying to read on socket fd %d\n", conn_fd);
    fflush(stderr);

    // === First exchange using write/read ===
    char buffer[BUF_SIZE];

    // Read message from client
    ssize_t bytes_read = read(conn_fd, buffer, BUF_SIZE - 1);
    if (bytes_read == -1) {
        perror("server read");
    } else {
        buffer[bytes_read] = '\0';
        fprintf(stderr, "Server received (read): %s\n", buffer);
        fflush(stderr);
    }

    // Send message to client
    const char *server_msg = "Hello from server (write)";
    if (write(conn_fd, server_msg, strlen(server_msg)) == -1) {
        perror("server write");
    }
    fflush(stderr);

#ifndef NO_RECVSENDMSG
    // === Second exchange using sendmsg/recvmsg ===
    // Receive from client
    struct msghdr msg = {0};
    struct iovec  iov;
    char          recv_buf[BUF_SIZE];

    iov.iov_base = recv_buf;
    iov.iov_len  = BUF_SIZE;

    msg.msg_iov    = &iov;
    msg.msg_iovlen = 1;

    ssize_t n = recvmsg(conn_fd, &msg, 0);
    if (n == -1) {
        perror("server recvmsg");
    } else {
        recv_buf[n] = '\0';
        fprintf(stderr, "Server received (recvmsg): %s\n", recv_buf);
    }

    // Send using sendmsg
    const char   *msg2   = "Hello from server (sendmsg)";
    struct msghdr sndmsg = {0};
    struct iovec  siov;

    siov.iov_base = (void *)msg2;
    siov.iov_len  = strlen(msg2);

    sndmsg.msg_iov    = &siov;
    sndmsg.msg_iovlen = 1;

    if (sendmsg(conn_fd, &sndmsg, 0) == -1) {
        perror("server sendmsg");
    }
#endif

    close(conn_fd);
    close(listen_fd);
}

void handle_client() {
    int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("client socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sock_fd,
                (struct sockaddr *)&addr,
                sizeof(struct sockaddr_un)) == -1) {
        perror("client connect");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // === First exchange using write/read ===
    const char *client_msg = "Hello from client (write)";
    if (write(sock_fd, client_msg, strlen(client_msg)) == -1) {
        perror("client write");
    }

    char    buffer[BUF_SIZE];
    ssize_t bytes_read = read(sock_fd, buffer, BUF_SIZE - 1);
    if (bytes_read == -1) {
        perror("client read");
    } else {
        buffer[bytes_read] = '\0';
        fprintf(stderr, "Client received (read): %s\n", buffer);
    }

#ifndef NO_RECVSENDMSG
    // === Second exchange using sendmsg/recvmsg ===
    const char   *msg2   = "Hello from client (sendmsg)";
    struct msghdr sndmsg = {0};
    struct iovec  siov;

    siov.iov_base = (void *)msg2;
    siov.iov_len  = strlen(msg2);

    sndmsg.msg_iov    = &siov;
    sndmsg.msg_iovlen = 1;

    if (sendmsg(sock_fd, &sndmsg, 0) == -1) {
        perror("client sendmsg");
    }

    // Receive response using recvmsg
    struct msghdr rcvmsg = {0};
    struct iovec  riov;
    char          recv_buf[BUF_SIZE];

    riov.iov_base = recv_buf;
    riov.iov_len  = BUF_SIZE;

    rcvmsg.msg_iov    = &riov;
    rcvmsg.msg_iovlen = 1;

    ssize_t n = recvmsg(sock_fd, &rcvmsg, 0);
    if (n == -1) {
        perror("client recvmsg");
    } else {
        recv_buf[n] = '\0';
        fprintf(stderr, "Client received (recvmsg): %s\n", recv_buf);
    }
#endif

    close(sock_fd);
}
