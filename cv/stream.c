#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <strings.h>

typedef struct {
    char type;
    uint32_t arg;
} Command;

void send_position(int pan, int tilt) {
    int sockfd;
    struct sockaddr_in server;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("0.0.0.0");
    server.sin_port = htons(7997);

    Command pan_cmd = { '-', pan };
    sendto(sockfd, &pan_cmd, sizeof(Command),
            0, (struct sockaddr *) &server, sizeof(server));

    Command tilt_cmd = { '|', tilt };
    sendto(sockfd, &tilt_cmd, sizeof(Command),
            0, (struct sockaddr *) &server, sizeof(server));
}

