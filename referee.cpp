// TODO: READY,GO
// write gui

#include <iostream> // cout
#include <cstring>	// memset
#include <cstdio>	// BUFSIZ
#include <sys/types.h>
#include <string>
#include <sys/socket.h> // socket
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h> // struct timeval
#include <sys/ioctl.h>
#include <unistd.h>
#include <netdb.h>
#include <algorithm> // transform
#include <cstdlib>	// exit
#include <errno.h>	// errno

using namespace std;

void e_error(const char *m);
void ee_error(const char *m, int errcode);
int gameLogic(char player1[], char player2[]);

int main(int argc, char* argv[]) {
	
	int choiceNum = 1;
	int server_sfd, client_sfd;
	struct sockaddr server_addr, client_addr;
	socklen_t server_len, client_len;
	struct addrinfo hints, *res;

	fd_set readfds, testfds;
	static char buffer[BUFSIZ];
	static char buffer2[BUFSIZ];
	int buflen = sizeof(buffer);
	int result;
	string input;
	char name[1024];
	char port[1024];
	string c1, c2[2];
	string finalScore;
	int p1Score = 0;
	int p2Score = 0;
	// getaddrinfo to ger local socket address
	// use port 0 for dynamic assignment
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	// if ((result = getaddrinfo(NULL, "0", &hints, &res)) == -1)
	// e_error("bind");
	getaddrinfo(NULL, "0", &hints, &res);
	// Make a socket
	if ((server_sfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1)
		e_error("socket");
	cout << "Referee is using port ";

	// name the socket
	if ((result = bind(server_sfd, res->ai_addr, res->ai_addrlen)) == -1)
		e_error("socket");

	// Destermine assigned Port Number
	server_len = sizeof(server_addr);
	if ((result = getsockname(server_sfd, &server_addr, &server_len)) == -1)
		e_error("getsockname");

	int flags = NI_NUMERICHOST | NI_NUMERICSERV;
	if ((result = getnameinfo(&server_addr, sizeof(server_addr), name,
	                          sizeof(name), port, sizeof(port), flags)) == -1)
		ee_error("getnameinfo", result);

	cout << port << endl;
	freeaddrinfo(res);

	// Create a connection queue and initialize readfds to handle
	// input from server socket fd
	listen(server_sfd, 1); // limits to two clients
	FD_ZERO(&readfds);
	FD_SET(server_sfd, &readfds);

	//  Now wait for clients and requests.
	int numClients = 3; // STDIN, STDOUT, STDERR
	while (1) {

		int fd; // generic file descriptor
		int nread = 0; // number of read in bytes

		testfds = readfds;
		if (numClients == 3)
			cout << "Referee is waiting for players" << endl;

		if ((result = select(numClients + 1, &testfds, (fd_set *)0,
		                     (fd_set *)0, (struct timeval *)0)) < 1) {
			cerr << "Referee: Error in select call" << endl;
			exit(1);
		}
		// once we have activity, find which descriptor
		// on by checking FD_ISSET
		for (fd = 0; fd < numClients + 1; fd++) {

			if (FD_ISSET(fd, &testfds)) {
				// if server activity
				if (fd == server_sfd) {
					client_len = sizeof(client_addr);
					// accept connection
					client_sfd = accept(server_sfd, &client_addr, &client_len);
					// add fd to set
					FD_SET(client_sfd, &readfds);
					// increment the file descriptor position by 1
					cout << "Player " << client_sfd - 3 << " has connected" <<  endl;
					char num[2];
					sprintf(num, "%d", client_sfd - 3);
					write(client_sfd, num, 2);

					numClients++;
				}
				// Client activity
				else {
					// clear input buffers
					if (numClients == 5) {

						while (true) {
							while (true) {
								nread = read(fd, buffer, BUFSIZ);
								buffer[nread] = 0;
								
								if ((strcmp(buffer, "READY") == 0)) {
									write(fd, "GO", 3);
									break;
								}
							}
							nread = read(fd, buffer, buflen);
							buffer[nread + 1] = 0;
							c1 = buffer;

							if (nread == 0) {
								close(fd);
								FD_CLR(fd, &readfds);
								cout << "Player " << fd - 3 << " has disconnected" << endl;
								numClients--;
								break;
							}
							else if (buffer[nread - 1] == '\0') {
								if(c1 == "STOP")
									c1 = "Exit";
								c2[choiceNum - 1] = c1;
								break;

							}



						}
						choiceNum++;
						if (choiceNum == 3) {
							cout << "P1 choice: " << c2[0] << endl;
							cout << "P2 choice: " << c2[1] << endl;

							int result = gameLogic((char*)c2[0].c_str(), (char*)c2[1].c_str());
							if (result == 0) {
								cout << "It is a Draw" << endl;
								strcpy(buffer, "It is a Draw");
								strcpy(buffer2, "It is a Draw");
								write(4, buffer, strlen(buffer) + 1);
								write(5, buffer2, strlen(buffer2) + 1);
								buffer[0] = 0;
								buffer2[0] = 0;
							}
							else if (result == 1) {
								cout << "P1 wins" << endl;
								strcpy(buffer, "You Win!");
								strcpy(buffer2, "Sorry, you lose");
								p1Score++;
								write(4, buffer, strlen(buffer) + 1);
								write(5, buffer2, strlen(buffer2) + 1);
								buffer[0] = 0;
								buffer2[0] = 0;
							}
							else if (result == 2) {
								cout << "P2 wins" << endl;
								strcpy(buffer, "Sorry, you lose");
								strcpy(buffer2, "You Win!");
								p2Score++;
								write(4, buffer, strlen(buffer) + 1);
								write(5, buffer2, strlen(buffer2) + 1);
								buffer[0] = 0;
								buffer2[0] = 0;

							}
							else if (result == -1) {
								finalScore.append("STOP Game has ended\n");
								cout << "Game has ended" << endl;
								finalScore.append("Final Score\n");
								string p1Out = "Player 1: " + to_string(p1Score) + "\n";
								string p2Out = "Player 2: " + to_string(p2Score) + "\n";
								finalScore.append(p1Out);
								finalScore.append(p2Out);
								write(4, finalScore.c_str(), finalScore.length() + 1);
								write(5, finalScore.c_str(), finalScore.length() + 1);

								close(4); close(5);
								FD_CLR(4, &readfds);
								FD_CLR(5, &readfds);
								numClients = 3;
								finalScore.clear();
								c1.clear(); c2[0].clear(); c2[1].clear();
								choiceNum = 1;
								p1Score = 0;
								p2Score = 0;
								break;
							}
							choiceNum = 1;
							c1.clear(); c2[0].clear(); c2[1].clear();
						}
					}

				}

			}
		}
	}
}

// error functions to make things a bit clearer
void e_error(const char *m)
{
	// general error message
	cerr << m << ": " << strerror(errno) << endl;
	exit(errno);
}

void ee_error(const char *m, int errcode)
{
	// get addrinfo error message
	cerr << m << ": " << gai_strerror(errcode) << endl;
	exit(errcode);
}

int gameLogic(char player1[], char player2[]) {
	string p1(player1);
	string p2(player2);
	int result;
	// Draw
	if ((p1 == "Rock" && p2 == "Rock") || (p1 == "Paper" && p2 == "Paper")
	        || (p1 == "Scissors" && p2 == "Scissors")) {
		result = 0;
	}
	// Player 1 wins
	else if ((p1 == "Rock" && p2 == "Scissors") || (p1 == "Paper" && p2 == "Rock")
	         || (p1 == "Scissors" && p2 == "Paper")) {
		result = 1;
	}
	else if ((p1 == "Scissors" && p2 == "Rock") || (p1 == "Rock" && p2 == "Paper")
	         || (p1 == "Paper" && p2 == "Scissors")) {
		result = 2;
	}
	else if (p1 == "Exit" || p2 == "Exit") {
		result = -1;
	}

	return  result;
}