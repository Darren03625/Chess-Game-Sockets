#include "hw4.h"

int main() {
    int listenfd, connfd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set options to reuse the IP address and IP port if either is already in use
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))");
        return -1;
    }
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))");
        return -1;
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(listenfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(listenfd, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    INFO("Server listening on port %d", PORT);
    // Accept incoming connection
    if ((connfd = accept(listenfd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    INFO("Server accepted connection");
    
    ChessGame *game = malloc(sizeof(ChessGame));
    initialize_game(game);


    while (1) {
        
        memset(buffer, 0, BUFFER_SIZE);
        int nbytes = read(connfd, buffer, BUFFER_SIZE);
        buffer[strlen(buffer)] = '\0';
        printf("buffer: %s\n", buffer);
        if (nbytes <= 0){
            perror("[Server] read() has failed.");
            exit(EXIT_FAILURE);
        }

        printf("[Server] Received from client: %s\n", buffer);

        int value = receive_command(game, buffer, connfd, true);
        
        if (value == COMMAND_FORFEIT){
            printf("[Server] Client chatter quitting...\n");
            close(connfd);
            if (game->currentPlayer == WHITE_PLAYER) 
                game->currentPlayer = BLACK_PLAYER;
            else 
                game->currentPlayer = WHITE_PLAYER;
            break;
        }

        memset(buffer, 0, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strlen(buffer) - 1] = '\0';
        printf("[Server] Message entered: %s\n", buffer);

        value = send_command(game, buffer, connfd, false);

        while (value == COMMAND_ERROR || value == COMMAND_UNKNOWN){
            memset(buffer, 0, BUFFER_SIZE);
            fgets(buffer, BUFFER_SIZE, stdin);
            buffer[strlen(buffer)-1] = '\0';
            printf("[Server] Message entered: %s", buffer);
            value = send_command(game, buffer, connfd, false);
        }


        if (value == COMMAND_FORFEIT){
            printf("[Server] Quitting...\n");
            close(connfd);
            break;
        }
    }

    close(listenfd);
    free(game);
    return 0;
}
