#include<bits/stdc++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <openssl/sha.h>
using namespace std;

pthread_mutex_t mutex1;

struct udetails
{
   
   int d;
};

struct chuncksdata
{

  int port;
  int clientport;
};


struct clientss
{
  char *ipaddr;
  int portnum;
  char* filename;
  int chunksum;
  char* newfle;
};

 

string sha256_hash_string (unsigned char hash[SHA256_DIGEST_LENGTH])
{
   stringstream ss;
   for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}

string sha256(const string str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return ss.str();
}




string sha256_of_chunks(FILE* f,int size)
{ cout<<"entered to calculate sha";
  if(!f)
    return NULL;

  string finalSha="";
  unsigned char hashcal[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  const int buf=512;
  unsigned char* buffer=(unsigned char*)malloc(buf+1);
  int n=0;
  if(!buffer)
    return NULL;
  while((n=fread(buffer,sizeof(char),buf,f)) > 0 && size>0)
  {
    SHA256_Update(&sha256,buffer,n);
    SHA256_Final(hashcal,&sha256);
    string outBuf=sha256_hash_string(hashcal);
    string ans=outBuf.substr(0,20);
    finalSha += ans;
    memset(buffer,'\0',512);
    size -= n;
  }
  fclose(f);
  return finalSha;
}




void download_file(int ser_fd)
{
    char buffer[1024];
    FILE *fp=fopen("/home/hp/Desktop/ngj/hello1.txt","wb");
    int size;
    
    recv(ser_fd,&size,sizeof(size),0);
    cout<<"size in client rec"<<size<<endl;
    int ack=1;
    send(ser_fd,&ack,sizeof(ack),0);
     int n1,n;
     cout<<"about to write"<<endl;
     // n1=recv(ser_fd,buffer,1024,0);
     //cout<<"n is "<<n1<<endl;

     while((n=recv(ser_fd,buffer,sizeof(buffer),0))>0 && size>0)
     {  cout<<"entered loop"<<endl;
        cout<<"size is "<<size<<endl;
        cout<<buffer<<endl;
        fwrite(buffer,sizeof(char),n,fp);
        memset(buffer,'\0',1024);
       size-=n;
     } 
}




void* fake_server(void* thrarg)
{ struct udetails *thrStr;
  
  cout<<"in fake "<<endl;
  thrStr=(struct udetails *)thrarg;
  int connectfd=thrStr->d;
  cout<<"accepted in server"<<endl;
  char filename[100];
  int c;
   if(recv(connectfd,&c,sizeof(c),0) < 0)
   {
     perror("error in receiving");
     exit(1);
   }
  int ack;

  
  send(connectfd,&ack,sizeof(ack),0);
  recv(connectfd,filename,sizeof(filename),0);
  cout<<"received filename "<<filename<<endl;
  cout<<"received chunkum "<<c<<endl;
  FILE *fp=fopen(filename,"rb");
  if(fp == NULL){
    cout<<"cant be opened "<<endl;
    exit(1);
  }
 
  rewind(fp);
  fseek(fp,(c-1)*512,SEEK_SET);
   cout<<"after opeining of file descriptor "<<endl;
  int n;
  int size=512;
  char buf[256]={0};
  while((n=fread(buf,sizeof(char),256,fp)) > 0 && size>0)
  { cout<<"in read";
    send(connectfd,buf,n,0);
    cout<<"inn buff"<<buf<<endl;
    memset(buf,'\0',256);
    size=size-n;
  }


  int ack1;
   recv(connectfd,&ack1,sizeof(ack1),0);
  fclose(fp);
  close(connectfd);


}




void *goto_file(void *var)
{ 
  
  struct udetails*u=(struct udetails*)var;
  char buffer[1024];
  int new_sock=u->d;
  FILE *fp=fopen("/home/hp/Desktop/ngj/hello.txt","rb");
   if(fp<0)
   {
    cout<<"error";
    exit(1);
   }
    cout<<"opened"<<endl;
    fseek(fp,0,SEEK_END);
    int size=ftell(fp); 
    fseek(fp,0L,SEEK_SET);
    // size=10;
    cout<<"size in server"<<size<<endl;
    send(new_sock,&size,sizeof(size),0);
    int ack;
    recv(new_sock,&ack,sizeof(ack),0);
    cout<<"ack"<<ack<<endl;
    int n;

    while((n=fread(buffer,sizeof(char),1024,fp))>0&&size>0)
    {  cout<<buffer<<endl;
      send(new_sock,buffer,n,0);
      memset(buffer,'\0',1024);
      size-=n;
    }
    fclose(fp); 
 

}



void* chunck_file(void* clientdetails)
{ cout<<"IN Chunk file download"<<endl;
  struct clientss* cdetails;
  cdetails=(struct clientss*)clientdetails;
  int socketfd=socket(AF_INET,SOCK_STREAM,0);
  
  if(socketfd<0)
  {
    perror("socket failed");
    exit(1);
  }
  struct sockaddr_in serveraddr;
  serveraddr.sin_family=AF_INET;
  serveraddr.sin_port=htons(cdetails->portnum);
  serveraddr.sin_addr.s_addr=inet_addr(cdetails->ipaddr);
  char buf[256]={0};
  int m;
    cout<<"filename is "<<cdetails->filename<<" new file "<<cdetails->newfle<<endl;
    cout<<"port is"<<cdetails->portnum<<endl;
    
  if(connect(socketfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) < 0)
  {
    perror("unable to connect");
    exit(1);
  }

  
  cout<<"connected"<<endl;
  char* filedwn=cdetails->filename;
  cout<<" file name to be downloaded ter creation of client  thre"<<filedwn<<endl;
  int cnum=cdetails->chunksum;
  cout<<"cnum is "<<cnum<<endl;

  if(send(socketfd,&cnum,sizeof(cnum),0)<0)
  { perror("error in sending");
    exit(1);
  }

  int ack;
  recv(socketfd,&ack,sizeof(ack),0);
  send(socketfd,filedwn,sizeof(filedwn),0);
  //cout<<"file name is "<<filedwn<<endl;
  //send(socketfd,&cnum,sizeof(cnum),0);
  pthread_mutex_lock(&mutex1);
  FILE *fp=fopen(cdetails->newfle,"rab+");
  int chunksize=512;
  int n;
  rewind(fp);
  fseek(fp,(cdetails->chunksum-1)*512,SEEK_SET);
  
  
  while(chunksize > 0 && (n=recv(socketfd,buf,256,0)) > 0 )
  {
    fwrite(buf,sizeof(char),n,fp);
    cout<<"Buff"<<buf<<endl;
    memset(buf,'\0',256);

    chunksize=chunksize-n;
    cout<<"chunksize "<<chunksize<<endl;
    
  } 
  fclose(fp);
  int ackk=1;
  send(socketfd,&ackk,sizeof(ackk),0);
  cout<<"releasing lock"<<endl;
  pthread_mutex_unlock(&mutex1);
  cout<<"lock released"<<endl;
  
  close(socketfd);

}





void commands(string s,int ser_fd,string ipaddress,string port_address)
{ 
  cout<<"in commands function "<<endl; 
  cout<<"ser_fd in commands "<<ser_fd<<endl;
  char buffer[1024];
  char *ar=(char*)s.c_str();
  char *br;
  int ack3;
  char cr[1024];
  //send(ser_fd,&ack3,sizeof(ack3),0);
  string h="";
   h=ar;
  br=(char*)h.c_str();
  char* token = strtok(ar, " ");  
  int ack,ackl,alk;
  send(ser_fd,&ackl,sizeof(ackl),0);
  recv(ser_fd,&alk,sizeof(alk),0);
 
  cout<<"br after kata chati "<<br<<endl;   
  send(ser_fd,h.c_str(),h.length(),0);
  //recv(ser_fd,&ack,sizeof(ack),0);
 //After this send my code is not working  
  
  cout<<"token as command "<<token<<endl;
  string username;  
       if(strcmp(token,"login")==0)
        { 
          int i=1;
          int no_in_c=i;
          //token = strtok(NULL, " "); 
          //string ss=token;
          //token = strtok(NULL, " ");
          //string sss=token;
          //string gg=ar;
          //char *ds=(char*)gg.c_str(); 
          //char t[]="login";
          //send(ser_fd,ar,sizeof(ar),0);
          cout<<"full command for tracker to send "<<ar<<endl;
          string ipinfo=ipaddress+" "+port_address;
          char *at=(char*)ipinfo.c_str();
          char atr[1024];
          strcpy(atr,at);
          send(ser_fd,atr,sizeof(atr),0);     
          //send(ser_fd,ds,sizeof(ds),0);
          recv(ser_fd,buffer,1024,0);
          cout<<"buffer after login "<<buffer<<endl;
            int ack2,ack6;
            send(ser_fd,&ack6,sizeof(ack6),0);
            //recv(ser_fd,&ack,sizeof(ack2),0);

          
       }
          else if(strcmp(token,"create_user")==0)
        { 
              cout<<"in create user"<<endl;
          int i=1;
          int no_in_c=i;
          token = strtok(NULL, " "); 
          string ss=token;
          username=ss;
          token = strtok(NULL, " "); 
          string sss=token;
          string gg=ss+sss;
          char *ds=(char*)gg.c_str(); 
          //cout<<"full command for tracker to send "<<ds<<endl;
          // char t[]="create_user";
          // send(ser_fd,t,sizeof(t),0);     
          recv(ser_fd,buffer,1024,0);
          cout<<"buffer "<<buffer<<endl;
          cout<<"buffer after creating user "<<buffer<<endl; 
         
        
       }

       else if(strcmp(token,"download_file")==0)
       {
        cout<<"in download file checking command"<<endl;  
        char Download[1024];
        cout<<"enter filename to download"<<endl;
        cin>>Download;
        cout<<Download<<endl;
        char buf[256]={0};
        //send(ser_fd,Download,sizeof(Download),0);
        int file_size=2669 ;
        //recv(ser_fd,&file_size,sizeof(file_size),0);
        cout<<file_size<<" in peer1"<<endl;
        char newFile[100];
        cout<<"enter filename for new file"<<endl;
        cin>>newFile;
        FILE *fp=fopen(newFile,"wb+");
        int n;
        memset(buf,'\0',256);
        int tempsize;
        tempsize=file_size;
        while(tempsize > 0)
        { if(tempsize>=256)
            fwrite(buf,sizeof(char),256,fp);
           else
            fwrite(buf,sizeof(char),tempsize,fp);
            tempsize=tempsize-256;
        }

        fclose(fp);
        // cout<<"enter filename for new file"<<endl;
        // cin>>newFile;
        int clientnum=3;
        int portnum;
        pthread_t present_client[clientnum];
        int pc[]={6000,7000,8900};
        int ch[]={3,2,1};
        //int pc[]={2050};
        //int ch[]={1};
        int i=0;
        while(clientnum--)
        { 
          struct clientss *cd = (struct clientss*)malloc(sizeof(struct clientss));
          cd->portnum=pc[i];
          cd->chunksum=ch[i];
          string y="127.0.0.1";
          char*u=(char*)y.c_str();
          cd->ipaddr=u;
          cd->filename=Download;
          cd->newfle=newFile;
          pthread_create(&present_client[i],NULL,&chunck_file,(void*)cd);
           i++;
        }
        for(int i=0; i<3; i++)
        pthread_join(present_client[i],NULL);
    
      
       }
       else if(strcmp(token,"upload_file")==0)
       {   char*token1;
           int ack;
           //file name to be uploaded
            token1=strtok(NULL," ");
            FILE* fp=fopen(token1,"rb");
            fseek(fp,0,SEEK_END);
            int size=ftell(fp);
            rewind(fp);
            string sha_received=sha256_of_chunks(fp,size);
            string data_to_send=s+" "+username+" "+to_string(size);
            cout<<"data to send "<<data_to_send<<endl;
            cout<<"data received from sha "<<sha_received<<endl;
            send(ser_fd,(char*)data_to_send.c_str(),data_to_send.length(),0);
            recv(ser_fd,&ack,sizeof(ack),0);
            int n=sha_received.length();
            int i=0;
             while(n>0)
           { cout<<"sha received from "<<i<<" "<<sha_received.substr(0,20)<<endl;
             char *sha_msg=(char*)sha_received.substr(i,i+20).c_str();
             char sha_msg_array[1024];
             strcpy(sha_msg_array,sha_msg);
             send(ser_fd,ar,sizeof(ar),0);
             i++;n--;
           }

        }
        else if(strcmp(token,"create_group")==0)
        {
            int ackl,ack4;
           //send(ser_fd,&ackl,sizeof(ackl),0);
           //recv(ser_fd,&ack4,sizeof(ack4),0);
           //send(ser_fd,ar,1024,0);
           

        }
        else if(strcmp(token,"join_group")==0)
        {

        }

       


}





  
void *servers(void* var)
{  
  cout<<"hey"<<endl;
   struct chuncksdata *cnkdta;
   cnkdta=(struct chuncksdata*)var;
   int ser_fd,new_sock,read_val,opt;
   struct sockaddr_in add;
   int addlen =sizeof(add);
   char buffer[1024]={0};
   const char *msg="hey i am server";
   //socket descriptor
   if((ser_fd=socket(AF_INET,SOCK_STREAM,0))==0)
   {
    perror("socket failed");
    pthread_exit(NULL);
   }
     int portnum=cnkdta->port;
     cout<<"port in server for tracker "<<portnum<<endl;
     add.sin_family=AF_INET;
     add.sin_port=htons(portnum);
     opt=1;
     //socket will bound to all local interfaces
     //
     if (setsockopt(ser_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt)))
     {
      perror("setsocket");
       pthread_exit(NULL);
     }
                                                     
     add.sin_addr.s_addr=INADDR_ANY;  
     if(bind(ser_fd,(struct sockaddr*)&add,sizeof(add))<0)
     {
     perror("error in binding");
      pthread_exit(NULL);
     }

     cout<<"binded"<<endl;
     
     if(listen(ser_fd,3)<0)
     {
     perror("LISTEN");
      pthread_exit(NULL);
     }

     cout<<"listened\n";

   
           
       while((new_sock=accept(ser_fd,(struct sockaddr*)&add,(socklen_t*)&addlen))>0)
     {
       struct udetails f; 
       cout<<"wow you are connected in server"<<endl;
       f.d=new_sock; 
       pthread_t new_bhaiya;
       pthread_create(&new_bhaiya,NULL,fake_server,(void *)&f);
       pthread_detach(new_bhaiya);
     }

   cout<<"accepted"<<endl;
   close(ser_fd);
   close(new_sock);  
}





void* clients(void * var)
{
   int ya;
  cout<<" enter any number in client";
  cin>>ya;
  struct chuncksdata *chnk;
  chnk=(struct chuncksdata*)var;
  int ser_fd=0,new_sock,read_val;
  struct sockaddr_in add;
  int addlen =sizeof(add);
  char buffer[1024]={0};
  char msg[]="exist";
  //socket descriptor
  if((ser_fd=socket(AF_INET,SOCK_STREAM,0))<0)
  {
    cout<<"error";
  
  }
   int newport=chnk->clientport;
   int uport=chnk->port; 
   cout<<" connecting with tracker "<<uport<<endl;
   add.sin_port=htons(uport);
   //socket will bound to al local interfaces
   add.sin_addr.s_addr=INADDR_ANY;
   add.sin_family=AF_INET;
    if(inet_pton(AF_INET,"127.0.0.1",&add.sin_addr)<=0)
   {
    printf("invalid address");
    
   }
  
   struct sockaddr_in clientadd;
   clientadd.sin_family=AF_INET;
   clientadd.sin_port=htons(chnk->clientport);
   clientadd.sin_addr.s_addr=inet_addr("127.0.0.1");
   int opt=1;
     //socket will bound to all local interfaces
     //
     if (setsockopt(ser_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt)))
     {
       perror("set socket");
       pthread_exit(NULL);
     }
                                                     
      add.sin_addr.s_addr=INADDR_ANY;  
     if(bind(ser_fd,(struct sockaddr*)&clientadd,sizeof(clientadd))<0)
     {
      perror("error in binding");
      pthread_exit(NULL);
     }

     cout<<"binded"<<endl;

   if((connect(ser_fd,(struct sockaddr*)&add,sizeof(add)))<0)
   {
    printf("not connect");
    exit(1);
   }
   cout<<"connect in client"<<endl;
   cout<<"ser_fd in client thread "<<ser_fd<<endl;
   char*ip =inet_ntoa(add.sin_addr);
   string ip_address=ip;
   string port_address=to_string(uport);
    cin.ignore();
   while(1)
   {    
     string str;
     cout<<"enter command"<<endl;
     getline(cin,str);
     cout<<" str -- "<<str<<endl;
     commands(str,ser_fd,ip_address,port_address);
     
    } 

     close(ser_fd);
  
   //  fclose(fp);

}




int main()
{

   pthread_t client ,server;
   struct  chuncksdata forserver;
   cout<<"give port num for server "<<endl;
   cin>>forserver.port;
   struct chuncksdata forclient;
   cout<<"port number for the tracker to connect to the client"<<endl;
   cin>>forclient.port;
   forclient.clientport=forserver.port;
   int x=pthread_mutex_init(&mutex1,NULL);
    if(x!=0)
    cout<<" mutex init failed "<<endl;
    cout<<"creating server thread"<<endl; 
    pthread_create(&server,NULL,servers,&forserver);
    cout<<"after creation of server "<<endl;
    cout<<"before client thread "<<endl;
    pthread_create(&client,NULL,clients,&forclient);
    cout<<"after creation of client  thread"<<endl;
    pthread_join(server,NULL);
    pthread_join(client,NULL);
   
    return 0;


}