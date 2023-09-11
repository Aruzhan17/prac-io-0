#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 8080
#define MAX_CLIENTS 5

int main(){
	int server_socket, client_socket[MAX_CLIENTS];
	if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
		perror("server: socket: ");
		return EXIT_FAILURE;
	}

	int opt = 1;
	if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){
		perror("server: setsockopt: ");
		return EXIT_FAILURE;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1){
		perror("server: bind: ");
		return EXIT_FAILURE;
	}

	if (listen(server_socket, MAX_CLIENTS) == -1){
		perror("server: listen: ");
		return EXIT_FAILURE;
	}

	while (1){
		if ((client_socket = accept(server_socket, NULL, 0)) == -1){
			perror("server: accept: ");
			return EXIT_FAILURE;
		}


///
		char request[1024] = "";
		read(client_socket, &request, 1024);
		write(STDOUT_FILENO, request, 1024);

		//PARSING

		int fd = open("index.html", O_RDONLY);
		if (fd == -1){
			perror("server: open: ");
		}
		char resp[1024] = "";
		strcat(resp, "HTTP/1.1 200 OK\n");
		strcat(resp, "Connection: closed\n");
		strcat(resp, "Content-lenght: 138\n");
		strcat(resp, "Content-type: text/html\n\n");
		printf("%s\n", resp);
		int resp_size = strlen(resp);
		write(client_socket, resp, resp_size);
		char file[1024] = "";
		read(fd, file, 138);
		write(client_socket, file, 138);
		close(fd);
///
		shutdown(client_socket, SHUT_RDWR);
		close(client_socket);

	}
	shutdown(server_socket, SHUT_RDWR);
	close(server_socket);
	return EXIT_SUCCESS;
}


fd_set rfds;
while(1){
	FD_ZERO(&rfds);
	FD_SET(server_socket, &rfds);
	int max_fd = server_socket;
	for (int i = 0; i < MAX_CLIENTS; i++){
		if (client_socket[i] > 0)
			FD_SET(client_socket[i], &rfds);
		if (max_fd < client_socket[i])
			max_fd =client_socket[i];
	}
	if (select(max_fd + 1, &rfds, NULL, NULL, NULL) == -1){
		//error
	}
	if (FD_ISSET(server_socket, &rfds)){
		if ((new_socket = accept(server_socket, NULL, 0)) == -1){
			//error
		}
		for (int i = 0 ; i < MAX_CLIENT; i++)
			if (client_socket[i] == 0){
				client_socket[i] = new_socket;
				break;
			}
	}
	for (int i = 0; i < MAX_CLIENT; i++){
		if (FD_ISSET(client_socket[i], &rfds)){
			//get_request
			//send_response
		}
	}
}
