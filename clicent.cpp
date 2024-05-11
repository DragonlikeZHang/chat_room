#include <iostream>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <thread>

void* working(int fd);

struct sockaddr_in seraddr;//构建端口号ip地址存储的结构体

int main (){
    //1、设置套接字
    int fd = socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1){std::cout<<"con erro!!<<std::endl";return -1;}
    //2、connect连接服务器

    seraddr.sin_family = AF_INET;
    uint16_t hport = 10000;
    seraddr.sin_port = htons(hport);
    inet_pton(AF_INET,"172.16.111.223",&seraddr.sin_addr.s_addr);

    //构造连接
    int con = connect(fd,(struct sockaddr*)&seraddr,sizeof(seraddr));
    if(con==-1){std::cout<<"con erro!!<<std::endl";return -1;}
    //构建子线程用来发送数据
    std::thread t(working,fd);
    t.detach();
    //3、写数据read
    while(1){
		char buf[100];
        // std::cin>>buf;
		// int len = write(fd,buf,sizeof(buf));
        //接收数据
        memset(buf, 0, sizeof(buf));
        int re_len = read(fd , buf, sizeof(buf));
		if(re_len>0){std::cout<< buf <<std::endl;}
		else if(re_len==0)
        {
            std::cout<<"服务器断开连接"<<std::endl;
            break;
        }
		else{std::cout<<"read erro!!<<std::endl";break;}
	}
    close(fd);


    return 0;
}


 void* working(int fd){

    char buff [100];
    
    while(1){
    memset(buff, 0, sizeof(buff));
    std::cin>>buff;
    int wri = write(fd,buff,sizeof(buff));
    if(wri==-1){std::cout<<"wri erro!!<<std::endl";break;}
    }
    return NULL;
 }