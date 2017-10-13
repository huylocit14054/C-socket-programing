#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

//if the input is key@EOF return false
int checkEndInput(char *string,int key){
    char checkString[8]; 
    //checkString is now key@EOF to check the stop
    sprintf(checkString,"%d@EOF",key);
    if(strcmp(checkString,string)==0)
        return 0;
    return 1;

}

//check if the user input is valid if valid return 1
int checkValidInput(char *string,int key){
    char checkKey[4];
    int i=0;
    //checkKey will now key@
    sprintf(checkKey,"%d@",key);
    for(i=0; i<strlen(checkKey); i++){
        if(string[i]!=checkKey[i])
            return 0;
    }
return 1;
}

int main(int argc, char *argv[]){
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char user_name[256],password[256];
    char message[256];

    if(argc <3){
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0); 
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) 
        error("ERROR opening socket");
    
	server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
	
	printf("Port number %d\n",portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    //isLogin to check the login is correct or not 
    int isLogin = 0;
    //number is the key send by server when loged in
    int number;

    //if the user is login the loop will break
    while(isLogin==0)
    {
        //write user name in client and read in server
        printf("Please enter the user name: ");
        bzero(user_name,256);
        fgets(user_name,255,stdin);
        n = write(sockfd,user_name,strlen(user_name));
        if (n < 0) 
            error("ERROR writing to socket");

        //write user password in client and read in server
        printf("Please enter the password: ");
        bzero(password,256);
        fgets(password,255,stdin);
        
        n = write(sockfd,password,strlen(password));
        if (n < 0) 
            error("ERROR writing to socket");

        //read the login message return from the server
        n = read(sockfd,&isLogin,sizeof(int));

        
        //check if the user is login
        if(isLogin){
            printf("\n\nYou have loged in the system \nHere is your code number: \n");
            //read the rand number return by the server
            n = read(sockfd,&number,sizeof(int));
            if (n < 0) 
                error("ERROR reading from socket");
            printf("%d\n",number);
        }
        else{
            printf("Access denied\nPlease enter the correct user name\n");
        }
    }
    printf("\nEnter the key with the format \"%d@somestring\" to send it to the server\nType \"%d@EOF\" to quit the connection:\n",number,number);
    
    //the input of the user to send to the server(aaa to get the checkEndInput function run because it won't accept :"")
    char input_message[256] ="aaa\n";
    //check the end of the input
    // run until the message is key@EOF (if the input is not EOF it will return 1)
    while(checkEndInput(strtok(input_message,"\n"),number)==1){
        //write the meesage and send to the server
        printf("\nEnter the message: \n");
        bzero(input_message,256);
        fgets(input_message,256,stdin);
        
        // if the user input the right message the message will be write
        if(checkValidInput(strtok(input_message,"\n"),number)){
            n = write(sockfd,input_message,strlen(input_message));
            if (n < 0) error("ERROR writing to socket");
            printf("\nMessage sent to the server\n");
        }
        else 
            printf("\nYou have entered wrong syntax\nPlease Enter again\n");    
    }
    printf("\nEnd of socket Program\n");
    close(sockfd);
    return 0;
}