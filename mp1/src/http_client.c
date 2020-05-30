/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <arpa/inet.h>

using namespace std;

#define PORT "80" // the port client will be connecting to 

#define MAXDATASIZE 1024*100 // max number of bytes we can get at once 
						//what should we suppose to have this value?

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//to parse ./http-client http://illinois.edu:5078/index.html

vector<string> Parser(string url)
{
	string hostname;
	string port;
	string path_to_file;

	int i1 = url.find_first_of('h');
	url = url.substr(i1+7);

	int i2 = url.find_first_of(':');
	int i3 = url.find_first_of('/');

	if(i2!=string::npos) // have specific port
	{
		hostname = url.substr(0,i2);
		port = url.substr(i2+1, i3-i2-1);
	}
	else
	{
		hostname = url.substr(0,i3);
		port = "80";
	}
	path_to_file = url.substr(i3+1);

	vector<string> URL_Parse;
	URL_Parse.push_back(hostname);
	URL_Parse.push_back(port);
	URL_Parse.push_back(path_to_file);

	return URL_Parse;

}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  //sockfd for identifier
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;  //addrinfo's pointer: hints, servinfo & p are pointer to pointer
	int rv; // is rv means return value?
	char s[INET6_ADDRSTRLEN];

	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints); // initialize all data member to 0
	hints.ai_family = AF_UNSPEC;  //address family is unspecified
	hints.ai_socktype = SOCK_STREAM; //TCP

	string url = argv[1];
	vector<string> URL_Parse = Parser(url); 
	cout<<URL_Parse[0]<<endl;
	cout<<URL_Parse[1]<<endl;
	cout<<URL_Parse[2]<<endl;

	if ((rv = getaddrinfo(URL_Parse[0].c_str(), URL_Parse[1].c_str(), &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	// the for loop to check all the addrinfo linked list
	for(p = servinfo; p != NULL; p = p->ai_next) {

		/*
			Inside for loop, you can use socket() to return a socket descriptor
			and then you use connect to connect this socket to server

		*/
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}
	// suppose to this stage you have completed the connect
	// you break the loop when p is addrinfo*

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	//you use inet_ntop to print readable ip address
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);

	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	//Once you have created the socket and connect successfully, you can read data from server 
	//through recv()

	string req_message = "";
	req_message = "GET /" + URL_Parse[2] + " HTTP/1.1\r\n\r\n";
	//+ "User-Agent: Wget/1.12 (linux-gnu)\r\n" +\
	//+"Host: " + URL_Parse[0] + ":" + URL_Parse[1] + "\r\n" + "Connection: Keep Alive\r\n\r\n";
	int req_message_len = req_message.size();	
	const char* req_c = req_message.c_str();
	cout<<"req_message: "<<req_message<<endl;
	cout<<"req_message_len: "<<req_message_len<<endl;
	cout<<"req_c: "<<req_c<<endl;
	//printf("req_c: %s",req_c);
	//cout<<endl;
	cout<<"size of req_c: "<<sizeof(req_c)<<endl;
	cout<<"size of req_message: "<<sizeof(req_message)<<endl;
	//send(sockfd,req_c,sizeof(req_c),0);
	if ((numbytes = send(sockfd, req_c, sizeof(req_message), 0)) <= 0) {
	    perror("send");
	    exit(1);
	}
	cout<<"numbytes: "<<numbytes<<endl;

	ofstream myfile;
	myfile.open("output");

	while(1)
	{
		numbytes = recv(sockfd,buf,MAXDATASIZE,0);
		if (numbytes==-1)
		{
			perror("recv");
			exit(1);
		}
		else if(numbytes==0)
		{
			break;
		}
		else
		{
			cout<<"hi"<<endl;
			buf[numbytes] = '\0';
			string resp_message = buf;
			while(resp_message.find("\r\n")!=string::npos)
			{
				int idx = resp_message.find("\r\n");
				resp_message = resp_message.substr(idx+2);
			}

			myfile << resp_message;
			continue;
		}
	}
	myfile.close();	

	//

	//printf("client: received '%s'\n",buf);

	//not sure if this is necessary
	
	cout<<"All finished!!!"<<endl;

	close(sockfd);

	return 0;
}

