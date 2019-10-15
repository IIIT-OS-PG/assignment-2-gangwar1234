#include<unistd.h>
#include<iostream>
#include<bits/stdc++.h>
#include<stdio.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;

struct server_sock
{

 int d; 

};


struct socket
{
	int id;
	string ipc;
	string port;
	int sock;
};

struct user
{
 string ip;
 string username;
 string port;
 string password;
 bool flag;

};

struct file_info
{
  string file_name_g;
  int fsize;
  vector<string>list_of_owners;
  string sha;

};

 struct group
 {
  
  vector<string>list_filenames;
  vector<string>list_userids;
  string owner_userid;
  //string group_id;
  vector<string>pending_request;
  
 };
  
  
 vector<struct user*>users; 
 map<string,struct group*>groups;
 map<string,file_info*>upload_entry;
 //map<string,user**>upload_entry;




 /*void join_group(string id_for_group,string user_idd)
{  if(groups[id_for_group])
    { 
      struct group *rona=new group;
      vector<string>v;
      rona=map[id_for_group];
      v=rona->list_userids;
      v.push_back(user_idd);
      rona->list_userids=v;
      
    } 
} */


void list_request_in()
{


}


void create_users(string usernamee, string passwordd,string ip,string portt,int new_sock)
{
   struct user *us=(struct user*)malloc(sizeof(struct user));
   //us->ip=ip;
   us->username=usernamee;
   us->password=passwordd;
   //us->port=portt;
   us->flag=0;
   cout<<"user "<<us->username<<" pass "<<us->password<<endl;
   users.push_back(us);
   char t[]="user created  successful";
   send(new_sock,t,sizeof(t),0);
   // for(int i=0;i<users.size();i++)
   // 	cout<<users[i].
   //cout<<users[]<<endl;
}




void do_login(string usernamee,string passwordd,int new_sock)
{    char buf[100];
     cout<<"in login of tracker"<<endl;
     struct user *us= new user; 
     
      //char *str=(char*)s.c_str();
      //cout<<str<<endl;
     //char* token = strtok(str, " ");
      // cout<<"token "<<token<<endl;
      // token = strtok(NULL, " ");
      // us->username=token;
      // cout<<"user "<<token<<endl;
      // token = strtok(NULL, " "); 
      // cout<<"pass "<<token<<endl;
      us->username=usernamee; 
      us->password=passwordd;
      int f=0;
      us->flag=1;
       char t[100];
       strcpy(t,"user does not exist");
      recv(new_sock,buf,sizeof(buf),0);
      cout<<"buf in do login "<<buf<<endl;
       char *a=strtok(buf," ");
       char*b=strtok(NULL," ");
       string ip_address=a;
       string port_address=b;
       for(int i=0;i<users.size();i++)
       {
            if(us->username==users[i]->username&&us->password==users[i]->password)
            	{    us->ip=ip_address;
            		 us->port=port_address;
            		  strcpy(t,"login successful");
            		 f=1;
            	}
           

       }

       
       	//send(new_sock,t,sizeof(t),0);
       
      

}




void* choices(void*var)
{   
     while(1)
   {
   	int ack4;
   	char ar[1024]={0};
    int a,ack,ackl;
    char buff[1024]={0};
    struct socket *sck=(struct socket*)var;
    cout<<" sck "<<sck->sock<<endl;

     // recv(sck->sock,&ack4,sizeof(ack4),0);
     // send(sck->sock,&ackl,sizeof(ackl),0);
     recv(sck->sock,&ar,1024,0);
     //send(sck->sock,&ack,sizeof(ack),0);
     char *aty;
       
    //strcpy(aty,ar);

    //cout<<"aty "<<aty<<endl;
    cout<<" ar "<<ar<<endl;
    cout<<"command "<<ar<<endl;
	  cout<<"string in choice "<<endl;
    string m=ar;
    char* token=strtok(ar," ");
    cout<<"token is equal to "<<token<<endl;
    cout<<ar<<endl;
    //string usernamee=token;
    string uname;
    if(strcmp(token,"create_user")==0)
    { int ackb1,ackb2;
   	 char exist[1024];
     token=strtok(NULL," ");
     string usernamee=token;
     cout<<" user name "<<usernamee<<endl;
     token=strtok(NULL," ");
     string passwordd=token;
     cout<<" password "<<passwordd<<endl;
     //recv(sck->sock,buff,1024,0);
     cout<<buff<<endl;
     //recv(sck->sock,exist,1024,0);
   	 // int  a=read(new_sock,exist,1024);
     create_users(usernamee,passwordd,sck->ipc,sck->port, sck->sock);
     cout<<"hey in create user "<<endl;
     recv(sck->sock,&ackb1,sizeof(ackb1),0);
     send(sck->sock,&ackb2,sizeof(ackb2),0); 
    }

    else if(strcmp(token,"login")==0)
   {  int ackk;
           
       char exist[1024];
   	   //int  a=read(sck->sock,exist,1024);
   	    string usernamee;
   	    token=strtok(NULL," ");
   	    usernamee=token;
   	    uname=usernamee;
   	    token=strtok(NULL," ");
   	    string passwordd=token;
        do_login(usernamee,passwordd,sck->sock);
        cout<<"after do login function"<<endl;
        recv(sck->sock,&ackk,sizeof(ackk),0);
        //send(sck->sock,&ack,sizeof(ack),0);

   }
   // else if(strcmp(token,"create_group"))
   // {   
   // 	    token=strtok(NULL," ");
   //      string id_for_group=token;
   //      create_group(sck->sock,id_for_group);

   // }

   /*if(strcmp(token,"upload_file"))
   {
      token=strtok(NULL," ");
      string entry=token;
      struct file_info file =(struct*file_info)malloc(struct file_info);
      file->sha
      upload_entry[entry]=file;  
   }*/   	

  
   
   else if(strcmp(token,"create_group")==0)
   {    int ackb2;  
        char*uy=strtok(NULL," ");
         cout<<uy<<endl;
        // string uy="hi";
         string sd=uy; 
         struct group *group_file=new group;
         group_file->list_userids.push_back(sd);
         group_file->owner_userid=sd;
         //groups[sd]=group_file;
         cout<<"successful created";
         send(sck->sock,&ackb2,sizeof(ackb2),0);

   }
   else if(strcmp(token,"join_group")==0)
   {   
   	   char *uy=strtok(NULL," ");
   	   string id_for_group=uy;
       cout<<" uy "<<uy<<endl;  
       if(groups.find(id_for_group)!=groups.end())
      { 
      struct group *rona=new group;
      vector<string>v;
      rona=groups[id_for_group];
      v=rona->list_userids;
      v.push_back(uname);
      rona->list_userids=v;
      
      } 
      int ackb3;
      send(sck->sock,&ackb3,sizeof(ackb3),0);
       

   }
   else if(strcmp(token,"upload_file"))
   {  int ackn,ackn2;
      int sizee;
      char *fname=strtok(NULL," ");
      char *gidd=strtok(NULL," ");
      char ab[1024]={0};
      char ac[1024]={0};
      recv(sck->sock,&sizee,sizeof(sizee),0);
      send(sck->sock,&ackn2,sizeof(ackn2),0);
      recv(sck->sock,&ab,1024,0);
      cout<<"content of ab is "<<ab<<endl;
      send(sck->sock,&ackn,sizeof(ackn),0);
      string str="";
      while(recv(sck->sock,&ac,1024,0)>0)
      {
         memset(ab,'\0',sizeof(ac));    
         recv(sck->sock,&ac,sizeof(ac),0);
         str=str+ac;
          
      }
      string gid_file=fname;
      string gid_files=gidd;
      gid_file=gid_file+gid_files;
      
      if(upload_entry.find(gid_file)!=upload_entry.end())
      {
         struct file_info *files=new file_info;
         files= upload_entry[gid_file];
         files->sha=str;
         files->fsize=sizee; 
         files->file_name_g=fname;
         files->list_of_owners.push_back(uname);

        
      }
      else
      {
         struct file_info *files=new file_info;
         //files= upload_entry[gid_file];
         files->sha=str;
         files->fsize=sizee; 
         files->file_name_g=fname;
         files->list_of_owners.push_back(uname);
         upload_entry[gid_file]=files;

      }
    int abc;
    send(sck->sock,&abc,sizeof(abc),0);
       
   }
      


   



}

}



void *tracker_server(void* var)
{
    

    
	int tracker_id,opt,new_sock;
	struct sockaddr_in add;
	struct sockaddr_in newadd;
	int addlen1=sizeof(newadd);
	int addlen=sizeof(add);
	char buffer[1024]={0};
	char msg[100];
    int port_number;
    struct server_sock *s=(struct server_sock*)var;
    port_number=s->d;
    cout<<"hey in tracker port "<<port_number<<endl;
	if((tracker_id=socket(AF_INET,SOCK_STREAM,0))==0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

    // cout<<"socked ban gya\n";
    // cout<<"give port number ";
    // int p;
    // cin>>p;
	add.sin_family=AF_INET;
	add.sin_port=htons(port_number);
	opt=1;

	if(setsockopt(tracker_id,SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT,&opt,sizeof(opt)))
	{
	  	perror("setsocket");
	  	exit(EXIT_FAILURE);
	}
	
	add.sin_addr.s_addr=INADDR_ANY;
	
	
	if(bind(tracker_id,(struct sockaddr*)&add,sizeof(add))<0)
	{
		perror("error in binding");
	}


    cout<<"bind v ho gya\n";
	if(listen(tracker_id,3)<0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
    cout<<"listen v ho gya"<<endl;
    
   while((new_sock=accept(tracker_id,(struct sockaddr*)&add,(socklen_t*)&addlen))>0)
   {
   	   struct socket f; 
   	   cout<<"new sock in server "<<new_sock<<endl;
       cout<<"wow you are connected in server"<<endl;
       f.id=new_sock;
       f.port=to_string(port_number);
       char*ip =inet_ntoa(newadd.sin_addr); 
       f.ipc=ip;
       f.sock=new_sock;
       cout<<"new sock in server "<<new_sock<<endl;
       pthread_t new_bhaiya;
       pthread_create(&new_bhaiya,NULL,choices,(void *)&f);
       pthread_detach(new_bhaiya);
   }

   cout<<"accept v ho gya bhyii woaah\n";
   char*ip =inet_ntoa(newadd.sin_addr);
   cout<<ip<<endl; 
   close(new_sock);
   close(tracker_id);


     
}






int main()
{
    pthread_t tracker;
    struct server_sock port_for_server;
    cout<<" give tracker port number "<<endl;
    cin>>port_for_server.d;
	cout<<"creating tracker "<<endl; 
    pthread_create(&tracker,NULL,tracker_server,&(port_for_server));
    cout<<"after creation of tracker "<<endl;
	pthread_join(tracker,NULL);
    return 0;


}
