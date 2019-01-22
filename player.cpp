// Austin Brummett
// CS375 Project 8
// TODO: READY,GO
#include <cstdlib>	// exit
#include <iostream>	// cout 
#include <fstream>	// fstream
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string>
#include <string.h>
#include <streambuf> // istreambuf

using namespace std;

int main(int argc, char *argv[]) {

	int sockfd;
	struct addrinfo *r;
	int nread = 0, result;
	string sent;
	if (argc != 3) {
		cerr << "Syntax: ./player hostname port" << endl;
		exit(1);
	}

	getaddrinfo(argv[1], argv[2], NULL, &r);
	// create a socket for the client
	sockfd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
	// Connect this socket to the server socket
	if ((result = connect(sockfd, r->ai_addr, r->ai_addrlen)) == -1) {
		cerr << "Player: connect failed" << endl;
		exit(1);
	}
	
	char num[2];
	read(sockfd, num, 2);
	cout << "You are player " << num << ".\n" << endl;
	char choice;
	static char buffer[BUFSIZ];
	string out;
	while (true) {

		buffer[0] = 0;

		sent.clear();

		cout << "0: Exit\n" << "1: Rock\n" << "2: Paper\n" << "3: Scissors" << endl;
		cout << "Enter Choice: ";
		cin >>  choice;
		switch (choice) {
		case '1':
			sent = "Rock";
			break;
		case '2':
			sent = "Paper";
			break;
		case '3':
			sent = "Scissors";
			break;
		case '0':
			sent = "STOP";

			break;

		}
		while (true) {
			write(sockfd, "READY", 6);
			nread = read(sockfd, buffer, BUFSIZ);
			if (strcmp(buffer, "GO") == 0) {
				break;
			}
		}
		write(sockfd, sent.c_str(), sent.length() + 1);
		if (sent == "STOP") {
			out.clear();
			while (1) {
				nread = read(sockfd, buffer, BUFSIZ);
				out.append(buffer, nread);

				if (nread == 0)
					break;

			}
			string o = out.substr(5);
			cout << o << endl;
			break;
		}
		else {
			out.clear();
			 
			nread = read(sockfd, buffer, BUFSIZ);
			buffer[nread] = 0;
			out.append(buffer, nread);
			if (strstr(buffer, "STOP") != NULL) {
				cout << out.substr(5) << endl;
				fflush(stdout);
				break;
			}
			cout << out << "\n" << endl;

		}
	}
	close(sockfd);
	exit(0);
}