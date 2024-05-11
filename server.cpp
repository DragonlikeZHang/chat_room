#include <iostream>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <thread>
#include <cstdio>

void* working (void* arg);
void pinjie(char buf[],char prefix[]);
//所有客户端信息，fd=-1代表无人  地址和端口号   文件描述符
struct SockInfo{
	struct sockaddr_in addr;
	int fd;
};
char buff[100];

SockInfo info[10];

int main(){
	//设置cocket套接字
	int fd = socket(AF_INET,SOCK_STREAM,0);
	if(fd==-1){std::cout<<"socket erro!!<<std::endl";return -1;}
	//bind绑定ip地址和端口号
	struct sockaddr_in addr;//构建端口号ip地址存储的结构体
	addr.sin_family = AF_INET;
	uint16_t hport = 10000;
	addr.sin_port = htons(hport);
	addr.sin_addr.s_addr = INADDR_ANY;
	int bin = bind(fd,(struct sockaddr*)&addr,sizeof(addr));
	if(bin==-1){std::cout<<"bin erro!!<<std::endl";return -1;}
	//设置监听
	int lis = listen(fd,128);
	if(lis==-1){std::cout<<"lis erro!!<<std::endl";return -1;}
	
	int number = sizeof(info)/sizeof(info[0]);

	for(int i=0;i<number;i++){
		bzero(&info[i],sizeof(info[i]));
		info[i].fd = -1;
	}

	//阻塞等待接收客户端信息
	uint32_t len_cliaddr = sizeof(struct sockaddr_in);//量sockaddr_in的长度

	while(1){
		struct SockInfo* pinfo;
		for(int i=0;i<number;i++){

			if(info[i].fd==-1){pinfo = &info[i];break;}
		}
		//accept连接客户端
		int acc = accept(fd,(struct sockaddr*)&pinfo->addr,&len_cliaddr);
		
		if(acc==-1){std::cout<<"accept erro!!<<std::endl";return -1;}
		pinfo->fd = acc;
		std::thread t(working,pinfo);
		t.detach();
	}

	close(fd);
	
	return 0;
}
//子线程需要两个参数，一个是accept的文件描述符 另一个是客户端的信息
void* working (void* arg){
	//pinfo存储ip、端口号和文件描述符
	struct SockInfo* pinfo = (struct SockInfo*)arg;
	char ip[32];
	//大小端转换 打印连接客户端的ip地址和端口号
	inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip));
	uint32_t cport = ntohs(pinfo->addr.sin_port);
	std::cout<<"已连接客户端,IP地址为"<<ip<<"端口号"<<cport<<std::endl;

	char message[100];
	sprintf(message,"客户端%s说:",ip);

	while(1){
		//清除buff内容，将读缓冲区的报文放到buff中
		memset(buff, 0, sizeof(buff));
		int re_len = read(pinfo->fd,buff,sizeof(buff));//读消息
		pinjie(buff,message);
		if(re_len>0)
		{
			std::cout<<message<<std::endl;
			//给所有客户端发送信息
			for(int i = 0;i<10;i++){
				if(info[i].fd!=-1){
					write(info[i].fd,buff,re_len);
				}
			}
			
		}
		else if(re_len==0){std::cout<<"客户端断开连接"<<std::endl;break;}
		else{std::cout<<"read erro!!<<std::endl";break;}
	}
	std::cout<<"已断开客户端"<<ip<<std::endl;
	close(pinfo->fd);
	pinfo->fd=-1;
	return NULL;
}

void pinjie(char buf[],char prefix[]){
    size_t length = strlen(buf);//计算buf的长度
    std::cout<<length<<std::endl;
    memmove(buf + strlen(prefix), buf, length + 1);
    memcpy(buf, prefix, strlen(prefix));
}

//该函数使用fd文件描述符接收buf字符串。buf字符串由count个字节
int readn(int fd ,const string* buf,int count)//fd文件描述符  buf读取数据存储的地址 count为读取多少个字节
{	
	int no_read_count = count;//没读到的数量
	int read_count = 0;//读到的数量
	string *p = buf;
	while(no_read_count>0){
		read_count = read(fd,p,no_read_count);
		if(read_count ==-1){cout<<"read error";return -1;}
		else if(read_count == 0){return count - no_read_count;}
		p+=read_count;
		no_read_count -= read_count;
	}
}
//该函数使用fd文件描述符发送buf字符串。buf字符串由count个字节
int writen(int fd ,string* buf,int count)//fd文件描述符  buf发送数据存储的地址 count为读取多少个字节
{	
	int no_write_count = count;//没读到的数量
	int writed_count = 0;//读到的数量
	string *p = buf;
	while(no_write_count>0){
		writed_count = write(fd,p,no_write_count);
		if(writed_count ==-1){cout<<"write error";return -1;}
		else if(writed_count == 0){continue;}
		p+=writed_count;
		no_write_count -= writed_count;
	}
}