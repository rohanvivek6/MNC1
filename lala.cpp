#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>
#define STDIN 0
#define STDOUT 0
using namespace std;
#define MAXDATASIZE 1024
fd_set mastertab;
//Declaring a structure to store the IP address table

struct IP
{
	int serial_no;
	string hostname;
	string ip_address;
	int clientportno;
	int sock;
	struct IP *next;

};
IP *removenode(struct IP *iplist,int sock)
{
	if(iplist==NULL)
	{
		cout<<endl<<"End Of Linked List";
		return NULL;
	}
	if(iplist->sock==sock)
	{
		IP *temp=iplist->next;
		free(iplist);
		return temp;
	}
	iplist->next=removenode(iplist->next,sock);
	return iplist;
}
IP *terminate(struct IP *iplist,int sn)
{
	if(iplist==NULL)
	{
		cout<<endl<<"End Of Linked List";
		return NULL;
	}
	if(iplist->serial_no==sn)
	{
		IP *temp=iplist->next;
		close(iplist->sock);
		FD_CLR(iplist->sock,&mastertab);
		free(iplist);
		return temp;
	}
	iplist->next=terminate(iplist->next,sn);
	return iplist;
}
IP *updatelist(struct IP *iplist)
{
	IP *q;
	q=iplist;
	int sn=1;
	while(q!=NULL)
	{
		q->serial_no=sn;
		q=q->next;
		sn++;
	}
	return iplist;
}
void esendll(struct IP *iplist,int sock,int no)
{
	char msg[1024]="";
	char t[100];
	cout<<"Number of connections"<<no<<endl;
	sprintf(t,"%d",no);
	strcat(msg,t);
	strcat(msg," ");
	for(int i=0;i<no;i++)
	{
		sprintf(t,"%d",iplist->serial_no);
		strcat(msg,t);
		strcat(msg," ");
		strcat(msg,(iplist->hostname).c_str());
		strcat(msg," ");
		strcat(msg,(iplist->ip_address).c_str());
		strcat(msg," ");
		sprintf(t,"%d",iplist->clientportno);
		strcat(msg,t);
		strcat(msg," ");
		iplist=iplist->next;
	}
	cout<<msg;
	char *str=msg;
	int len=strlen(str);
	int sen;
	if(sen=send(sock,(const char*)str, len,0)==-1);
		perror("send:");
}
string getIP(/*const char* resultIP*/)
{
	sockaddr_in result;
	socklen_t reslen=sizeof(result);
	int udpsock,udpstatus;
	struct addrinfo udp,*udpservinfo;
	memset(&udp,0,sizeof udp);
	udp.ai_family=AF_INET;
	udp.ai_socktype=SOCK_DGRAM;
	udp.ai_flags=AI_PASSIVE;
	if ((udpstatus = getaddrinfo("www.google.com","", &udp, &udpservinfo)) != 0)
		{
			fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(udpstatus));
		}

	udpsock=socket(udpservinfo->ai_family,udpservinfo->ai_socktype,udpservinfo->ai_protocol);
	connect(udpsock,udpservinfo->ai_addr,udpservinfo->ai_addrlen);

	getsockname(udpsock,(struct sockaddr *)&result,&reslen);

	char buff[50];
	string IP=inet_ntop(AF_INET,&result.sin_addr,buff,sizeof(result));
	return IP;

}
void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
void sendport(int data,int sockfd)
{
	char str[MAXDATASIZE];
	sprintf(str, "%d", data);
	char *a=str;
	int len=strlen(a);
	send(sockfd,(const char*)a,len,0);

	//cout <<endl<<"SENT:"<<sen<<endl<<"length"<<len<<endl;
}
int Register(string ip,string portno,int listenport)
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(ip.c_str(),portno.c_str(), &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	}

	// loop through all the results and connect to the first we can
	//CHECK THIS FOR LOOP OUT ERROR??
	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
		{
			perror("client:Register: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("client:Register: connect");
			continue;
		}
		else
		{
			sendport(listenport,sockfd);
		}
		break;
	}

	if (p == NULL)
	{
		fprintf(stderr, "client: failed to connect\n");
		return -200;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure
	return sockfd;

}

int ConnectClient(string ip,string portno,int listenport)
{
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(ip.c_str(),portno.c_str(), &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	}

	// loop through all the results and connect to the first we can
	//CHECK THIS FOR LOOP OUT ERROR??
	for(p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)
		{
			perror("client:Connect: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sockfd);
			perror("client:Connectr: connect");
			continue;
		}
		else
		{
			sendport(listenport,sockfd);
		}
		break;
	}

	if (p == NULL)
	{
		fprintf(stderr, "client: failed to connect\n");
		return (-200);
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure
	return sockfd;

}char* getchar(int sockfd)
{
	char *buf=new char[1024];
	int numbytes;
	if ((numbytes=recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
	{
	    perror("recv");
	    exit(1);
	}
	buf[numbytes+1] = '\0';
	return buf;
}
char *getportno(int sockfd)
{
	char* buf=new char[1024];
	bzero(buf,sizeof buf);
	int numbytes;
	//cout<<"\nsize of buf ="<<sizeof buf<<endl;
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
	{
	    perror("recv");
	    exit(1);
	}
	//cout<<"\nbuf"<<buf;

	//cout<<endl<<"recv"<<numbytes<<endl;
	buf[numbytes+1] = '\0';
	return buf;
}
char *getname(char in[])
{
	char host[1024];
	struct sockaddr_in ip;
	inet_pton(AF_INET,in,&ip.sin_addr);
	ip.sin_port=htons(80);
	ip.sin_family=AF_INET;


	getnameinfo((struct sockaddr *)&ip,sizeof ip, host, sizeof host,NULL,NULL,0);
	return host;
}

int main(int argc, char *argv[] )
{

	IP *iplist,*q,*temp,*Serv_Ip_List=new IP;
	//int connectionNo=0;
	//temp= new IP;
	iplist=NULL;
	char program_type;
	//string command;

	string sysip=getIP();
	cout<<sysip<<"\n";
	//MAKE SURE THAT THERE ARE ONLY 2 ARGUMENTS INPUTTED AT THE START OF THE PROGRAM
	if (argc != 3)
	{
		cout << "Error : There must be 2 Arguments";
	}
//CHECKS IF THE PROGRAM SHOULD RUN AS A SERVER OR AS A CLIENT
	if (strcmp(argv[1],"s")==0)
	{
		program_type= 's';
	}

	if (strcmp(argv[1],"c")==0)
	{
		program_type= 'c';
	}


//Declaring the variables
//too be used to set up the
//listening connection.
	int listen_port= atoi(argv[2]);
	int listensock,yes=1;								//Yes is set to 1, to get rid of the address already in use error
	struct addrinfo setup,*servinfo,*p;

//Setting up the setup variable
	memset(&setup,0,sizeof setup);
	setup.ai_family=AF_INET;
	setup.ai_socktype=SOCK_STREAM;
	setup.ai_flags = AI_PASSIVE;

	int status;
	if((status=getaddrinfo(NULL,argv[2],&setup,&servinfo))!=0)
	{
		cout << "GET ADDRINFO ERROR";
		exit(1);
	}
	for(p=servinfo;p!=NULL;p=p->ai_next)
	{
		if((listensock=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1)
		{
			perror("Server : Socket");
			continue;
		}

//Use setsockopt to prevent the
//address already in use error.
        if (setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

		if(bind(listensock,p->ai_addr,p->ai_addrlen)==-1)
		{
			close(listensock);
			perror(" Server : Bind");
			continue;
		}

		break;
	}
	cout<< "Socket Descriptor = "<<listensock<<"\n";
	if(p==NULL)
		cout<<"Problem creating socket\n";

//Setting Up the server
//to listen for incoming
//connections

	if(listen(listensock,5)==-1)
	{
		perror("Listen");
	}
	else
	cout <<"listening\n";

	fd_set readfds;
	int fdmax;					//Keep track of the biggest file descriptor


	FD_ZERO(&mastertab);
	FD_ZERO(&readfds);


	FD_SET(STDIN,&mastertab);

	FD_SET(listensock,&mastertab);
	fdmax=listensock+1;


	cout<<"proj1>>";
	fflush(STDIN);
	//VARIABLES USED FOR THE ACCEPT SETUP HERE
	struct sockaddr_storage remoteaddr;
	socklen_t addrlen;
	int newsockfd;				//NEW SOCKET CREATED WHEN ACCEPTING A CLIENT
	string ip;					//Stores the IP address of the new client before writing it to the Struct IP

	char remoteIP[INET6_ADDRSTRLEN];
	string delimiter = " ";
	string input;
	int connectionNo=0;
	int servfd=-2;
	while(1)
	{
		readfds=mastertab;

		if((select(fdmax,&readfds,NULL,NULL,NULL))==-1)
			perror("Select");

		for(int i=0;i<=fdmax;i++)			//To Scan all the file descriptors to see if they are ready
		{
			if(FD_ISSET(i,&readfds))		//Checking file descriptor 'i'
			{
				if(i==listensock)			//NEED TO KEEP A LIMIT ON THE NUMBER OF CLIENTS ALLOWED TO CONNECT.!!!!NEED TO SEND THE LIST TO CLIENT!!!!
				{
					/*
					 * This means there is an incoming connection on the listening port.
					 */
					if(program_type=='s')
					{
						addrlen=sizeof remoteaddr;
						if((newsockfd=accept(listensock,(struct sockaddr*)&remoteaddr,&addrlen))==-1)
						{
							perror("Accept");
						}
						else
						{
							FD_SET(newsockfd,&mastertab);
							if(newsockfd>=fdmax)
							{
								fdmax=newsockfd+1;
							}


							connectionNo++;
							ip=inet_ntop(remoteaddr.ss_family,get_in_addr((struct sockaddr*)&remoteaddr),remoteIP,INET6_ADDRSTRLEN);

							char *buf=new char[1024];
							buf=getportno(newsockfd);

							//printf("client: received '%s'\n",buf);

							char cstr[ip.length()];
							strcpy(cstr, ip.c_str());
							char *host=getname(cstr);

							//Adding the new client to a linked list
							temp=new IP;
							temp->serial_no=connectionNo;
							temp->ip_address=ip;
							temp->clientportno=atoi(buf);
							temp->hostname=host;
							temp->sock=newsockfd;
							temp->next=NULL;

							if(iplist==NULL)
								iplist=temp;
							else
							{
								q=iplist;
								while(q->next !=NULL)
									q=q->next;
								q->next=temp;
							}


							printf("New Connection From %s on socket %d \n",ip.c_str(),newsockfd);
							printf("\nUpdating Ip list On all Clients\n");
							q=iplist;


							while(q!=NULL)
							{
								esendll(iplist,q->sock,connectionNo);
								q=q->next;
							}

							cout<<"list sent";
							cout<<endl<<"proj1>> ";
						}
					}
					else
					{
						//client in coming
						if(connectionNo<5)
						{
							addrlen=sizeof remoteaddr;
							if((newsockfd=accept(listensock,(struct sockaddr*)&remoteaddr,&addrlen))==-1)
							{
								perror("Accept");
							}
							else
							{

								cout<<endl<<"The Incoming connection had been accepted";
								FD_SET(newsockfd,&mastertab);
								if(newsockfd>=fdmax)
								{
									fdmax=newsockfd+1;
								}
								connectionNo++;
								ip=inet_ntop(remoteaddr.ss_family,get_in_addr((struct sockaddr*)&remoteaddr),remoteIP,INET6_ADDRSTRLEN);

								char *buf=new char[1024];
								buf=getportno(newsockfd);

								printf("client: received '%s'\n",buf);

								char cstr[ip.length()];
								strcpy(cstr, ip.c_str());
								char *host=getname(cstr);

								//Adding the new client to a linked list
								temp=new IP;
								temp->serial_no=connectionNo;
								temp->ip_address=ip;
								temp->clientportno=atoi(buf);
								temp->hostname=host;
								temp->sock=newsockfd;
								temp->next=NULL;

								if(iplist==NULL)
									iplist=temp;
								else
								{
									q=iplist;
									while(q->next !=NULL)
										q=q->next;
									q->next=temp;
								}

							}
						}
						else
							cout<<endl<<"Unable to accept connection.";
					}
					fflush(STDIN);
				}
				if (i==STDIN)
				{
					/*
					 * All commands issued by the user are processed here.
					 */
							string command;
							getline(cin,input);
							char inputc[1024];
							strcpy(inputc, input.c_str());
							char *pch=strtok(inputc," ");
							command=pch;
					    	if(command.compare("MYIP")==0)
							{
								cout<<sysip<<endl;
							}
					    	else if(command.compare("LIST")==0)
							{
								if(iplist==NULL)
								{
									cout<<"The IP-List is empty\n";
								}
								else
								{
									cout<<"No\t"<<"Hostname\t"<<"IP Address\t"<<"Port No\t"<<"Sockfd\n";
									q=iplist;
									while(q!=NULL)
									{
										cout<< q->serial_no <<"\t"<<q->hostname<< "\t" << q->ip_address <<"\t"<<q->clientportno<<"\t"<<q->sock<<"\n";
										q=q->next;

									}
								}
							}
					    	else if(command.compare("MYPORT")==0)
					    	{
					    		cout<<listen_port<<endl;
					    	}
					    	else if((command.compare("REGISTER")==0)&&program_type!='c'&&servfd!=-2)
							{
								cout<<"The Server cannot register";
							}
					    	else if((command.compare("REGISTER")==0)&&program_type=='c'&&servfd==-2)
					    		/*
					    		 * 	Servfd will be -2 only if the client is not registered to the server.
					    		 * If the Client is registered to the served it should not be allowed to register again
					    		 * */
					       	{
					    		string regip,regport;
					    		pch=strtok(NULL," ");
					    		if(pch==NULL)
					    		{
					    			cout<<"Register Needs a Valid IP Address and Port Number";
					    			cout<<endl<<"proj1>>";
					    			fflush(STDIN);
					    			break;
					    		}
					    		regip=pch;
					    		pch=strtok(NULL," ");
					    		if(pch==NULL)
					    		{
					    			cout<<"Register Needs a Valid Port Number";
					    			cout<<endl<<"proj1>>";
					    			fflush(STDIN);
					    			break;
					    		}
					    		regport=pch;
					    		if(strcmp(regip.c_str(),"128.205.36.8")!=0)
					    		{
					    			cout<<"You can only register to the server"<<endl;
					    			cout<<"proj1>>";
					    			fflush(STDIN);
					    			break;
					    		}
					    		else
					    		{
					    			servfd=Register(regip,regport,listen_port);
					    			if(servfd==-200)
					    			{
					    				cout<<"Error Connection Not Established"<<endl;
					    				cout<<"proj1>>";
					    				servfd=-2;
					    				break;
					    			}
					    			else
					    			{
					    				//add servfd to the masterfd to monitor for any updated lists
					    				connectionNo++;
					    				FD_SET(servfd,&mastertab);
					    				//changed

										char cstr[regip.length()];
										strcpy(cstr, regip.c_str());
										char *host=getname(cstr);

										temp=new IP;
										temp->serial_no=connectionNo;
										temp->ip_address=regip;
										temp->clientportno=atoi(regport.c_str());
										temp->hostname=host;
										temp->sock=servfd;
										temp->next=NULL;
										if(iplist==NULL)
											iplist=temp;
										else
										{
											q=iplist;
											while(q->next !=NULL)
												q=q->next;
											q->next=temp;
										}

										if(servfd>=fdmax)
										{
											fdmax=servfd+1;
										}
									}
										//changed
								}

					    	}
					    	else if((command.compare("REGISTER")==0)&&program_type=='c'&&servfd!=-2)
					    	{
					    		cout<<"You are already registered to the server"<<endl;
					    	}
					    	else if ((command.compare("CONNECT")==0)&&(program_type)=='s')
					    	{
					    		cout<<endl<<"The server cannot connect to clients"<<endl;
					    	}
					    	else if((command.compare("CONNECT")==0)&&(program_type)=='c')
					    	{
					    		if(connectionNo>=3)
					    			cout<<endl<<"Maximum Number of connections reached";
					    		else
					    		{
					    			int spresent=0,cpresent=0;
					    			string con_ip,con_port;
						    		pch=strtok(NULL," ");
						    		if(pch==NULL)
						    		{
						    			cout<<"Connect Needs a Valid IP Address and Port Number";
						    			cout<<endl<<"proj1>>";
						    			fflush(STDIN);
						    			break;
						    		}
						    		con_ip=pch;
						    		pch=strtok(NULL," ");
						    		if(pch==NULL)
						    		{
						    			cout<<"Connect Needs a Port Number";
						    			cout<<endl<<"proj1>>";
						    			fflush(STDIN);
						    			break;
						    		}
						    		if(con_ip==sysip)
						    		{
						    			cout<<"Client cannot connect to itself"<<endl;
						    			cout<<"proj1>>";
						    			fflush(STDIN);
						    			break;
						    		}
						    		con_port=pch;
					    			/*
					    			 *Check if con_ip is spresent in the Serv_Ip_List
					    			 *if so allow it to connect otherwise print error
					    			 *message
					    			 */
					    			q=Serv_Ip_List;
					    			while(q!=NULL)
					    			{
					    				if((q->ip_address).compare(con_ip)==0)
					    				{
					    					spresent=1;
					    					break;
					    				}
					    				q=q->next;
					    			}
					    			if(spresent==1)
					    			{
					    				q=iplist;
					    				/*
					    				 * Check if the client is already connected to the IP address provided.
					    				 * If it is print a message.Else connect them.
					    				 */
					    				while(q!=NULL)
					    				{
					    					if((q->ip_address).compare(con_ip)==0)
					    					{
					    						cpresent=1;
					    						break;
					    					}
					    					q=q->next;
					    				}
					    				if(cpresent!=0)
					    				{
					    					cout<<endl<<"There is already an active connection established with "<<con_ip<<endl;
					    				}
					    				else
					    				{
					    					int cfd=ConnectClient(con_ip,con_port,listen_port);
					    					if(cfd==-200)
					    					{
					    						cout<<"Client Failed to connect"<<endl;
					    						cout<<"proj1>>";
					    						fflush(STDIN);
					    						break;
					    					}
					    					else
					    					{
					    						FD_SET(cfd,&mastertab);
												if(cfd>=fdmax)
												{
													fdmax=cfd+1;
												}
												connectionNo++;
												char cstr[con_ip.length()];
												strcpy(cstr, con_ip.c_str());
												char *clientname=getname(cstr);
												IP *temp=new IP;

												temp->serial_no=connectionNo;
												temp->hostname=clientname;
												temp->ip_address=con_ip;
												temp->clientportno=atoi(con_port.c_str());
												temp->sock=cfd;
												temp->next=NULL;
												if(iplist==NULL)
													iplist=temp;
												else
												{
													q=iplist;
													while(q->next !=NULL)
														q=q->next;
													q->next=temp;
												}
					    					}
					    				}
					    			}
					    			else
					    			{
					    				cout<<endl<<"The Ip address entered is not connected to the server."<<endl;
					    			}
					    		}

					    	}
					    	else if(command.compare("TERMINATE")==0)
					    	{
					    		int conn;
					    		pch=strtok(NULL," ");
					    		if(pch==NULL)
					    			{
										cout<<"Terminate Needs A Valid Connection Number To Terminate";
					    				cout<<endl<<"proj1>>";
					    				fflush(STDIN);
					    				break;
					    			}
					    		conn=atoi(pch);
					    		iplist=terminate(iplist,conn);
					    		connectionNo--;


					    		cout<<"Connection "<<conn<<" has been terminated"<<endl;

					    	}
					    	else if(command.compare("EXIT")==0)
					    	{
					    		for(int i=1;i<connectionNo;i++)
					    		{
					    			iplist=terminate(iplist,i);
					    		}
					    		exit(0);
					    	}
					    	else
					    	{
					    		cout<<"UNKNOWN COMMAND\n";
					    	}
					    	cout<<"proj1>>";
					    	fflush(STDIN);

				}
				if(i==servfd)				//Incoming data from the server.
				{
					cout<<"\nIncoming data from the server";
					char *buf=new char[1024];
					int nbytes=recv(i,buf,MAXDATASIZE-1,0);
					if(nbytes==0)
					{
						cout<<"Server has ended the connection";

						close(i);
						FD_CLR(i,&mastertab);
						servfd=-2;
						//removenode(iplist,i);					NEED TO REMOVE FROM IPLIST OF CLIENT
						cout<<endl<<"proj1>> ";
					}
					if(nbytes>0)
					{
						Serv_Ip_List=new IP;
						Serv_Ip_List=NULL;
						char *tok=new char[1024];
						tok=strtok(buf," ");
						int nl=atoi(tok);

						for(int j=0;j<nl;j++)
						{
							temp=new IP;
							tok = strtok (NULL, " ");
							int serial=atoi(tok);
							string h=strtok(NULL," ");
							string i=strtok(NULL," ");
							tok =strtok(NULL," ");
							int pn=atoi(tok);
							temp->serial_no=serial;
							temp->hostname=h;
							temp->ip_address=i;
							temp->clientportno=pn;
							temp->next=NULL;
							if(Serv_Ip_List==NULL)
								Serv_Ip_List=temp;
							else
							{
								q=Serv_Ip_List;
								while(q->next != NULL)
									q=q->next;
								q->next=temp;
							}

						}
						//q=NULL;
						cout<<endl<<"No\t"<<"Hostname\t"<<"IP Address\t"<<"Port No\n";
						q=Serv_Ip_List;
						while(q!=NULL)
						{
							cout<<endl<< q->serial_no <<"\t"<<q->hostname<< "\t" << q->ip_address <<"\t"<<q->clientportno<<"\n";
							q=q->next;
						}
						cout<<endl<<"proj1>> ";
					}
					fflush(STDIN);

				}
				else						//Check for incoming client data
					{
					//cout<<"\nChecking for incoming data";
					char *buf=new char[1024];
					int nbytes=recv(i,buf,MAXDATASIZE -1,0);
					string buffer=buf;
					//cout<<buf;
					//Check if any clients disconnect
					if(nbytes==0)
					{
						/*
						 * When a client disconnects from the server it recv() will return 0.
						 * We must then close that clients socket, update the master fd list,
						 * reduce the number of total connections (connectionNo) by 1,
						 * update the iplist and finally broadcast this updated list to all
						 * connected clients.
						 */
						cout<<endl<<"Client Disconnected";

						close(i);
						FD_CLR(i,&mastertab);
						//NEED TO Remove Client From Linked List And Update

						iplist=removenode(iplist,i);
						iplist=updatelist(iplist);
						connectionNo--;
						q=iplist;
						while(q!=NULL)
						{
							esendll(iplist,q->sock,connectionNo);
							q=q->next;
						}
						cout<<"\nRemoved "<<i<<" from list\n";
						cout<<endl<<"proj1>> ";

					}
					if(nbytes>0)
					{
						cout<<"Data= "<<buffer<<endl;
						cout<<endl<<"proj1>> ";
					}


					fflush(STDIN);

				}


			}
		}
	}
}
