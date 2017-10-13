#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

int login(char *user_input_name, char *user_input_password){
    FILE *file;
    char buf[1000];
    char  id[32], user_name[32], password[32];
    file =fopen("user.txt","r");
    if (!file){
        printf("there is no user file\n");
    return 0;
    }
        
    // get each line in the user file and devided the string by "\t"
    while(fgets(buf,1000, file)!=NULL)
    {
        sscanf(buf, "%s%s%s", id, user_name, password);
        if(strcmp(user_input_name,user_name)==0 && strcmp(user_input_password,password)==0){
            return 1;
        }
    }
return 0;


fclose(file);
}

void error(const char *msg)
{
    perror(msg);
    exit(1);
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

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char user_name[256],password[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;    //inet_addr("ipadress"), htonl(INADDR_LOOPBACK);
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
    
    //begin listening
     listen(sockfd,5);
     printf("Listening on port %d and waiting for user to login\n",portno);
     clilen = sizeof(cli_addr);

     //accept new socket
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");
    //isLogin to check the login is correct or not  
    int isLogin = 0;
    //random key
    int rand_num;
    while(isLogin==0)
    {
        //read the input user name from the server     
        bzero(user_name,256);
        n = read(newsockfd,user_name,255);
        if (n < 0) error("ERROR reading from socket");

        printf("Here is the user_name: %s\n",user_name);
        
        //read the input password from the server     
        bzero(password,256);
        n = read(newsockfd,password,255);
        if (n < 0) error("ERROR reading from socket");

        printf("Here is the password: %s\n",password);
        
        //check if the user can login and return 1 
        isLogin = login(strtok(user_name,"\n"),strtok(password,"\n"));
        // write the isLogin to the user to inform about the login
        n = write(newsockfd,&isLogin,sizeof(int));
        if (n < 0) error("ERROR writing to socket");

        //login the user
        if(isLogin)
        {
            printf("login succeess\n");

            //write a random number for the user
            srand(time(NULL));
            rand_num = rand() % 100;
            n = write(newsockfd,&rand_num,sizeof(int));
            if (n < 0) error("ERROR writing to socket");

            
        }
    }
    //Read the message from the user until key@data

    // Read the message(this message has already checked the syntax)
    char input_message[256]="aaa\n";
    while(checkEndInput(strtok(input_message,"\n"),rand_num)==1){
        bzero(input_message,256);
        n = read(newsockfd,input_message,255);
        if (n < 0) error("ERROR reading from socket");

        printf("Message receive: %s\n",input_message);
    }

    printf("Close server");
     close(newsockfd);
     close(sockfd);
     return 0; 
}