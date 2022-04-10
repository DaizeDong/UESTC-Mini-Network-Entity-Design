//3 NETWORK Layer(�����)
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cmath>
#include<queue>
#include<winsock2.h>
#include<windows.h>
#include"CfgFileParms.h"
#include"BasicFunction.h"
//#pragma comment(lib,"ws2_32.lib")
using namespace std;

CCfgFileParms Link_file;               //��Ԫ��Ϣ
const int MAX_BUFFER_SIZE=65000;       //����ռ�(bit)
int SOCKADDR_SIZE=sizeof(SOCKADDR_IN); //�׽��ֵ�ַ����(�ֽ�)

WSADATA     Local_data;    //�����ʼ����Ϣ
SOCKET      Local_sock;    //�����׽���
SOCKADDR_IN Local_addr;    //�����׽��ֵ�ַ
SOCKADDR_IN Upper_addr;    //�ϲ��׽��ֵ�ַ(Ӧ�ò�)
SOCKADDR_IN Lower_addr[8]; //�²��׽��ֵ�ַ(��·��)
SOCKADDR_IN Cmd_addr;      //������׽��ֵ�ַ

int Top=0;             //�Ƿ�Ϊ���㣬���Ƿ�Ϊ·����
int Sub_net=0;         //��������������
int Local_ip;          //������ʵIP
int Local_port;        //������ʵ�˿�
int Sub_net_num=1;     //�������� (Ĭ��1)(���512)
int Sub_net_ip[512];   //������Ӧ��ʵIP (Ĭ��Local_ip)
int Sub_net_port[512]; //������Ӧ��ʵ�˿� (Ĭ��-1)
int Sub_net_next[512]; //�����Ӧ�����ĳ��� (Ĭ��-1)(-1���������ڵ�����)(�Ǹ��������²�ӿں�)'
int Router[512];       //���Ӧ�����ĳ����������ǲ���·���� (Ĭ��0)

int Source_subnet;     //��ʱ�洢���յ���֡����Դ����
int Source_ip;         //��ʱ�洢���յ���֡����Դ��ʵIP
int Source_port;       //��ʱ�洢���յ���֡����Դ��ʵ�˿�
int Destination_subnet;//��ʱ�洢���յ���֡����Ŀ������
int Destination_ip;    //��ʱ�洢���յ���֡����Ŀ����ʵIP
int Destination_port;  //��ʱ�洢���յ���֡����Ŀ����ʵ�˿�
int Sequence;          //��ʱ�洢���յ���֡�������

int Print_mode=0;   //�����շ���Ϣ��ӡģʽ��0Ϊ����ӡ�շ������ݣ�1Ϊ��ӡbit����2Ϊ��ӡ8���� (Ĭ��1)
int T_Print_mode=0; //������ʾ��Ϣ��ӡģʽ��0Ϊֻ��ӡ��������ʱ����ʾ��Ϣ��1���Ӵ�ӡ�շ�������Ϣ��2���Ӵ�ӡ����������Ϣ (Ĭ��0)
int Upper_mode=1;   //�ϲ�ӿ�ģʽ��0Ϊbit���飬1Ϊ�ֽ����� (Ĭ��1)
int Lower_mode[8]={1,1,1,1,1,1,1,1}; //�²�ӿ�ģʽ��0Ϊbit���飬1Ϊ�ֽ����� (Ĭ��1)
int Lower_number=1; //�²�ʵ������ (Ĭ��1)(���Ϊ8)

int Upper_full=25;         //�ϲ�ӵ��ָ�� (Ĭ��25)
int Lower_full=25;         //�²�ӵ��ָ�� (Ĭ��25)
int Recv_timeout=20;       //���ճ�ʱ(ms) (Ĭ��20)(��Ϊ�����������е�ʱ�䵥λ)(��Ҫ)
int Upper_Send_timeout=20; //�ϲ㷢�ͳ�ʱ(ms) (Ĭ��20)
int Lower_Send_timeout=20; //�²㷢�ͳ�ʱ(ms) (Ĭ��20)

int Recv_buf;           //���ý��ջ���(bit)(���²����)
int Send_buf;           //���÷��ͻ���(bit)(���²㷢��)
char *Tmp_buf;          //��ʱ����(bit/�ֽ�)
queue<char*>Recv_queue; //��������
queue<char*>Send_queue; //��������
queue<INFO>Recv_info;   //����֡����Ϣ
queue<INFO>Send_info;   //����֡����Ϣ
DATA Recv_amount;       //����������
DATA Recv_time;         //���մ���
DATA Send_amount;       //����������
DATA Send_time;         //���ʹ���

char *Send_tmp_data;        //�����·��͵�����
INFO Send_tmp_info;         //�����·��͵�������Ϣ
char *Special_window_frame; //���ⷢ�ʹ��ڵ�����
INFO Special_window_info;   //���ⷢ�ʹ��ڵ�������Ϣ 
char *Recv_tmp_data;        //�����Ϸ��͵�����
INFO Recv_tmp_info;         //�����Ϸ��͵�������Ϣ

int Recv_buf_high_thershold=0.4*MAX_BUFFER_SIZE; //���ջ�������ж���ֵ
int Recv_buf_low_thershold=0.25*MAX_BUFFER_SIZE; //���ջ���Ͽ��ж���ֵ
int Send_buf_high_thershold=0.4*MAX_BUFFER_SIZE; //���ͻ�������ж���ֵ
int Send_buf_low_thershold=0.25*MAX_BUFFER_SIZE; //���ͻ���Ͽ��ж���ֵ
int Upper_full_thershold=25; //�ϲ�ӵ��ָ���ϴ��ж���ֵ
int Lower_full_thershold=25; //�²�ӵ��ָ���ϴ��ж���ֵ

int Upper_send_stop=0;    //���Ϸ�����ͣ������
int Lower_send_stop=0;    //���·�����ͣ������
bool Upper_send_data=0;   //�Ƿ����ϲ㷢����������Ϣ
bool Lower_send_data=0;   //�Ƿ����²㷢����������Ϣ
bool Upper_recv_data=0;   //�Ƿ���ϲ������������Ϣ
bool Lower_recv_data=0;   //�Ƿ���²������������Ϣ
bool Recv_buf_no_empty=0; //�ϸ����ڽ��ջ����Ƿ񲻿�
bool Send_buf_no_empty=0; //�ϸ����ڷ��ͻ����Ƿ񲻿�

int F_Print_mode=0;    //��֤�������ͨ�������ñ������� 
const int HEAD_SIZE=0; //��֤�������ͨ�������ñ������� 

void shutdown(){//�˳�
	WSACleanup();
	if(Tmp_buf) delete [] Tmp_buf;
	if(Special_window_frame) delete [] Special_window_frame;
	if(Recv_tmp_data) delete [] Recv_tmp_data;
	if(Send_tmp_data) delete [] Send_tmp_data;
}
void tip(int type,int num){//��ʾ
	if(type==0&&num==0){
		cprintf("#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#",94);
		cprintf("        ��        ��        ��        ",94);
		cprintf("#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#\n",94);
		return;
	}
	switch(type){
		case 0:{//�ɹ�
			switch(num){
				case 1:{cprintf("*�����Ϣ���óɹ�\n",2);break;}
				case 2:{cprintf("*����ռ����ɹ�\n",2);break;}
				case 3:{cprintf("*�׽��ֻ�����ʼ���ɹ�\n",2);break;}
				case 4:{cprintf("*�׽������ö�ȡ�ɹ�\n",2);break;}
				case 5:{cprintf("*�����׽��ֳ�ʼ�����󶨳ɹ�\n",2);break;}
				case 6:{cprintf("*�������׽��ֳ�ʼ���ɹ�\n",2);break;}
				case 7:{cprintf("*�׽������ó�ʱ�ɹ�\n",2);break;}
				case 8:{cprintf("*�׽������÷������ɹ�\n",2);break;}
			}
			break;
		}
		case 1:{//ʧ��
			switch(num){
				case 1:{cprintf("*�����Ϣ����ʧ��\n",4);break;}
				case 2:{cprintf("*����ռ����ʧ��\n",4);break;}
				case 3:{cprintf("*�׽��ֻ�����ʼ��ʧ��\n",4);break;}
				case 4:{cprintf("*�׽������ö�ȡʧ��\n",4);break;}
				case 5:{cprintf("*�����׽��ְ�ʧ��\n",4);break;}
				case 6:{cprintf("*�²��׽��ֲ�������\n",4);break;}
				case 7:{cprintf("*�׽������ó�ʱʧ��\n",4);break;}
				case 8:{cprintf("*�׽������÷�����ʧ��\n",4);break;}
			}
			shutdown();
			system("@ pause");
			exit(0);
		}
		default:{//�������
			switch(num){
				case 1:{cprintf("*�Է��׽����ѹر�",3);
						print_void(102);printf("\n");break;}
				case 2:{cprintf("*�׽��ַ����쳣",3);
						print_void(104);printf("\n");break;}
				case 3:{cprintf("*���ͣ��յ����ݣ��ѱ��������ͻ���",3);
						print_void(86);printf("\n");break;}//T_Print_mode
				case 4:{cprintf("*���ͣ����ͻ���ﵽ���ޣ��Ѷ����������͵�����",3);
						print_void(74);printf("\n");break;}
				case 5:{cprintf("*���ͣ����Ͷ����뻺��ռ䲻ƥ�䣬�����÷��Ͷ����뻺��",3);
						print_void(66);printf("\n");break;}
				case 6:{cprintf("*���գ��յ����ݣ��ѱ��������ջ���",3);
						print_void(86);printf("\n");break;}//T_Print_mode
				case 7:{cprintf("*���գ����ջ���ﵽ���ޣ��Ѷ������յ�����",3);
						print_void(78);printf("\n");break;}
				case 8:{cprintf("*���գ����ն����뻺��ռ䲻ƥ�䣬�����ý��ն����뻺��",3);
						print_void(66);printf("\n");break;}
				case 9:{cprintf("*�ڴ���䣺ʧ�ܣ�",12);
						print_void(102);printf("\n");break;}
			}
		}
	}
}
void initial(int argc,char *argv[]){//��ʼ�� 
	//�����Ϣ
	string s1,s2,s3;
	switch(argc){
		case 4:{
			s1=argv[1];
			s2=argv[2];
			s3=argv[3];
			break;
		}
		case 3:{
			s1=argv[1];
			s2="NETWORK";
			s3=argv[3];
			break;
		}
		default:tip(1,1);
	}
	CCfgFileParms File(s1,s2,s3);
	Link_file=File;
	//��ȡ����IP��˿���Ϣ
	string info=Link_file.getUDPAddrString(CCfgFileParms::LOCAL,0);//����
	String2Num(&Local_ip,&Local_port,info);
	//��ȡ������Ϣ
	for(int i=0;i<=511;++i){
		Sub_net_ip[i]=Local_ip;
		Sub_net_port[i]=-1;
		Sub_net_next[i]=-1;
	}
	string S_Sub_net_ip,S_Sub_net_port,S_Sub_net_next,S_Router;
	Link_file.getValueInt(Sub_net,(char*)"subnet");//������
	Link_file.getValueInt(Top,(char*)"Top"); //�Ƿ�Ϊ����
	Link_file.getValueInt(Print_mode,(char*)"Print_mode");    //�շ���Ϣ��ӡģʽ
	Link_file.getValueInt(T_Print_mode,(char*)"T_Print_mode");//��ʾ��Ϣ��ӡģʽ
	Link_file.getValueInt(Recv_timeout,(char*)"Recv_timeout");//���ճ�ʱ
	Link_file.getValueInt(Upper_Send_timeout,(char*)"Upper_Send_timeout");//�ϲ㷢�ͳ�ʱ
	Link_file.getValueInt(Lower_Send_timeout,(char*)"Lower_Send_timeout");//�²㷢�ͳ�ʱ
	Link_file.getValueInt(Sub_net_num,(char*)"subnet_num");//��������
	S_Sub_net_ip=Link_file.getValueStr("subnet_ip");    //������Ӧ��ʵIP (int�ͱ�ʾ)
	S_Sub_net_port=Link_file.getValueStr("subnet_port");//������Ӧ��ʵ�˿�
	S_Sub_net_next=Link_file.getValueStr("subnet_next");//��������
	S_Router=Link_file.getValueStr("router");//���������Ƿ�Ϊ·����
	if(S_Sub_net_ip.length()>0) Many_String2Num(S_Sub_net_ip,Sub_net_ip,Sub_net_num);
	if(S_Sub_net_port.length()>0) Many_String2Num(S_Sub_net_port,Sub_net_port,Sub_net_num);
	if(S_Sub_net_next.length()>0) Many_String2Num(S_Sub_net_next,Sub_net_next,Sub_net_num);
	if(S_Router.length()>0) Many_String2Num(S_Router,Router,Sub_net_num);
	//�²�ʵ������
	Lower_number=Link_file.getUDPAddrNumber(CCfgFileParms::LOWER);
	if(Lower_number<0) tip(1,1);
	tip(0,1);
	
	//����
	Tmp_buf=new char[MAX_BUFFER_SIZE]; //��ʱ����
	tip(!Tmp_buf,2);
	
	//�׽��ֻ���
	if(WSAStartup(MAKEWORD(2,2),&Local_data)==0) tip(0,3);//��ʼ��
	else tip(1,3);
	
	//�׽���
	if(Link_file.isConfigExist){
		tip(0,4);
		//�����׽���
		Local_sock=socket(AF_INET,SOCK_DGRAM,0);//���������ӵĴ��䣬UDP
		memset(&Local_addr,0,SOCKADDR_SIZE);//��ʼ��
		Local_addr=Link_file.getUDPAddr(CCfgFileParms::LOCAL,0);//�Զ�����
		Local_addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//����IP��ַ
		if(bind(Local_sock,(SOCKADDR*)&Local_addr,SOCKADDR_SIZE)!=SOCKET_ERROR) tip(0,5);//��
		else tip(1,5);
		//�ϲ��׽���
		Upper_addr=Link_file.getUDPAddr(CCfgFileParms::UPPER,0);//�Զ�����
		Link_file.getValueInt(Upper_mode,(char*)"Upper_mode");//�ӿ�ģʽ
		//�²��׽���
		for(int i=0;i<=Lower_number-1;++i){
			Lower_addr[i]=Link_file.getUDPAddr(CCfgFileParms::LOWER,i);//�Զ�����
			Link_file.getValueInt(Lower_mode[i],(char*)"Lower_mode");//�ӿ�ģʽ
		}
		//����ƽ̨�׽���
		Cmd_addr=Link_file.getUDPAddr(CCfgFileParms::CMDER,0);
		tip(0,6);
	}
	else tip(1,4);
	
	//�׽��ֳ�ʱ
	if(setsockopt(Local_sock,SOL_SOCKET,SO_RCVTIMEO,(char*)&Recv_timeout,sizeof(Recv_timeout))==0) tip(0,7);
	else tip(1,7);
	
/*	//�׽��ַ�����
	unsigned long argp=1;
	if(ioctlsocket(Local_sock,FIONBIO,&argp)==0) tip(0,8);
	else tip(1,8);*/
	
	printf("\n");
	Link_file.print(13);
}
void Send_Upper(int i){//���ϲ㷢�����ݣ�����Ϊ��������
	//ע�⣺���ں�Ϊ-1ʱ����ʹ�����ⴰ��(ר�����շ�������Ϣ)
	char *Recv_data;
	int buf_len;
	
	if(i!=-1){//��ͨ����
		Recv_data=Recv_tmp_data;
		  buf_len=Recv_tmp_info.buf_len;
	}
	else{//������Ϣ
		Recv_data=Special_window_frame;
		  buf_len=16;
	}
	
	//��ϢԤ����
	int len=buf_len/8+(buf_len>0&&buf_len%8!=0);
	char byte[len];
	Bit2Byte(Recv_data,buf_len,byte,len);//������֡תΪ�ֽ���
	
	//����
	int Recv_len=sendto(Local_sock,byte,len,0,(SOCKADDR*)&Upper_addr,SOCKADDR_SIZE);
	if(Recv_len<=0){//����ʧ�ܣ���׼���ش�
		Send_amount.Error+=buf_len; Send_time.Error++;
		if(i!=-1){//ֻ�ش�����֡�����ش�����֡
			Recv_buf+=buf_len;             //���ӻ��� 
			Recv_queue.push(Recv_tmp_data);//���֡������ĩ
			Recv_info.push(Recv_tmp_info); //���֡��Ϣ������ĩ
		}
		
		if(Print_mode){//�����Ϣ
			if(i!=-1){
				if(Print_mode==1) print_bit(Recv_data,buf_len,"���ϲ㷢������ʧ��...����bit����",0,13);
				else print_byte(byte,len,"���ϲ㷢������ʧ��...�����ֽ�����",1,13);
			}
			else{
				if(Print_mode==1) print_bit(Recv_data,buf_len,"���ϲ㷢�Ϳ�����Ϣʧ��...��bit����",0,13);
				else print_byte(byte,len,"���ϲ㷢�Ϳ�����Ϣʧ��...���ֽ�����",1,13);
			}
		}
		if(i==-1) delete[] Special_window_frame;
	}
	else{//���ͳɹ�
		Send_amount.Total+=buf_len; Send_time.Total++;
		Send_amount.Upper+=buf_len; Send_time.Upper++;
		if(i!=-1) {Send_amount.UData+=buf_len; Send_time.UData++; Upper_send_data=1;}
		if(i==-1) {Send_amount.UCtrl+=buf_len; Send_time.UCtrl++;}
		
		if(Print_mode){//�����Ϣ
			if(i!=-1){
				if(Print_mode==1) print_bit(Recv_data,buf_len,"���ϲ㷢�����ݳɹ�������bit����",0,13);
				else print_byte(byte,len,"���ϲ㷢�����ݳɹ��������ֽ�����",1,13);
			}
			else{
				if(Print_mode==1) print_bit(Recv_data,buf_len,"���ϲ㷢�Ϳ�����Ϣ�ɹ�����bit����",0,13);
				else print_byte(byte,len,"���ϲ㷢�Ϳ�����Ϣ�ɹ������ֽ�����",1,13);
			}
		}
		if(i!=-1) delete[] Recv_tmp_data;
		else delete[] Special_window_frame;
	}
}
void Send_Lower(int i){//���²㷢�����ݣ�����Ϊ��������
	//ע�⣺���ں�Ϊ-1ʱ����ʹ�����ⴰ��(ר�����շ�������Ϣ)
	char *Send_data;
	int buf_len,Low_num;
	
	if(i!=-1){//��ͨ����
		Send_data=Send_tmp_data;
		  buf_len=Send_tmp_info.buf_len;
		  Low_num=Send_tmp_info.Lower_number;
	}
	else{//������Ϣ
		Send_data=Special_window_frame;
		  buf_len=16;
		  Low_num=Special_window_info.Lower_number;
	}
	
	//��ϢԤ����
	int len=buf_len/8+(buf_len>0&&buf_len%8!=0);
	char byte[len+6];
	if(i!=-1){//��ͨ����
		Bit2Byte(Send_data,buf_len,byte+6,len);//�Ƚ�����֡תΪ�ֽ���
		
		Destination_subnet=byte[9];
		Destination_ip=Sub_net_ip[Destination_subnet];
		if((Destination_subnet!=Sub_net)&&(Router[Destination_subnet])){//Ҫ��������ݲ��������ڣ����Լ�����һ����·����
			Destination_port=Sub_net_port[Destination_subnet];
		}
		else{//Ҫ�����������������
			if(*((unsigned short*)(byte+10))!=65535) Destination_port=*((unsigned short*)(byte+10))-100;
			else Destination_port=65535;
		}
		
		set_destination_mac_info(Destination_ip,Destination_port,byte);//ת��
//		cout<<Destination_ip<<" # "<<Destination_port<<endl;
		
		len+=6;
	}
	else//������Ϣ
		Bit2Byte(Send_data,buf_len,byte,len);//�Ƚ�����֡תΪ�ֽ���
	
	//����
	int Send_len=sendto(Local_sock,byte,len,0,(SOCKADDR*)&Lower_addr[Low_num],SOCKADDR_SIZE);
	if(Send_len<=0){//����ʧ�ܣ���׼���ش�
		Send_amount.Error+=buf_len; Send_time.Error++;
		if(i!=-1){//ֻ�ش�����֡�����ش�����֡
			Send_buf+=buf_len;             //���ӻ��� 
			Send_queue.push(Send_tmp_data);//���֡������ĩ
			Send_info.push(Send_tmp_info); //���֡��Ϣ������ĩ
		}
		
		if(Print_mode){//�����Ϣ
			if(i!=-1){
				if(Print_mode==1) print_bit(Send_data,buf_len,"���²㷢������ʧ��...����bit����",0,13);
				else print_byte(byte,len,"���²㷢������ʧ��...�����ֽ�����",1,13);
			}
			else{
				if(Print_mode==1) print_bit(Send_data,buf_len,"���²㷢�Ϳ�����Ϣʧ��...��bit����",0,13);
				else print_byte(byte,len,"���²㷢�Ϳ�����Ϣʧ��...���ֽ�����",1,13);
			}
		}
		if(i==-1) delete[] Special_window_frame;
	}
	else{//���ͳɹ�
		Send_amount.Total+=buf_len; Send_time.Total++;
		Send_amount.Lower+=buf_len; Send_time.Lower++;
		if(i!=-1) {Send_amount.LData+=buf_len; Send_time.LData++; Lower_send_data=1;}
		if(i==-1) {Send_amount.LCtrl+=buf_len; Send_time.LCtrl++;}
		
		if(Print_mode){//�����Ϣ
			if(i!=-1){
				if(Print_mode==1) print_bit(Send_data,buf_len,"���²㷢�����ݳɹ�������bit����",0,13);
				else print_byte(byte,len,"���²㷢�����ݳɹ��������ֽ�����",1,13);
			}
			else{
				if(Print_mode==1) print_bit(Send_data,buf_len,"���²㷢�Ϳ�����Ϣ�ɹ�����bit����",0,13);
				else print_byte(byte,len,"���²㷢�Ϳ�����Ϣ�ɹ������ֽ�����",1,13);
			}
		}
		if(i!=-1) delete[] Send_tmp_data;
		else delete[] Special_window_frame;
	}
}
int Send_Cmd(char *buf,int len){//�����㷢�����ݣ����ط��ͳ���
	return sendto(Local_sock,buf,len,0,(SOCKADDR*)&Cmd_addr,SOCKADDR_SIZE);
}
void Recv_Upper(int len){//���ϲ��յ�
	if(len==2&&(unsigned char)*Tmp_buf==0xff){//����ǿ�����Ϣ�������ͬ��ӵ��ָ��
		Recv_amount.Total+=16; Recv_time.Total++;
		Recv_amount.Upper+=16; Recv_time.Upper++;
		Recv_amount.UCtrl+=16; Recv_time.UCtrl++;
		char n=*(Tmp_buf+1);
		if(n==0) Upper_full=0;
		else if(n>0) Upper_full+=n;
		return;
	}//������Ϊ��������Ϣ����������
	
	//��ȡ�·���Ŀ��ʵ���
	get_destination_ip_info(&Destination_subnet,&Destination_port,Tmp_buf,len);
	
	int Low_num=Sub_net_next[Destination_subnet];
	if(Low_num==-1) Low_num=0;
	
	//�����ݼ������ͻ���
	if(Send_buf+8*len>MAX_BUFFER_SIZE){
		Recv_amount.Error+=8*len; Recv_time.Error++;
		tip(2,4);
	}
	else{
		Upper_recv_data=1;
		Recv_amount.Total+=8*len; Recv_time.Total++;
		Recv_amount.Upper+=8*len; Recv_time.Upper++;
		Recv_amount.UData+=8*len; Recv_time.UData++;
		if(T_Print_mode) tip(2,3);
		
		char *tmp_frame;
		try{tmp_frame=new char[8*len+48];}catch(bad_alloc){tip(2,9);return;}
		Byte2Bit(tmp_frame,8,(char*)&Sub_net,1);//ͷ����֡
		Byte2Bit(tmp_frame+8,16,(char*)&Local_port,2);//ͷ����֡
		Byte2Bit(tmp_frame+24,8,(char*)&Destination_subnet,1);//ͷ����֡
		Byte2Bit(tmp_frame+32,16,(char*)&Destination_port,2);//ͷ����֡
		Byte2Bit(tmp_frame+48,8*len,Tmp_buf,len);//��ת��Ϊbit�ͣ������
//		cout<<"@@@"<<Sub_net<<" "<<Local_port<<" "<<Destination_subnet<<" "<<Destination_port<<endl;
		len+=6;
		
		Send_buf+=8*len;           //������ռ�ô�С
		Send_queue.push(tmp_frame);//��������Ͷ���
		
		INFO tmp_info(-1,len,8*len,Low_num,-1,0);
		Send_info.push(tmp_info);  //��ӷ�����Ϣ������
	}
	
	//�жϻ���ռ䣬�����Ϳ�����Ϣ
	if(Send_buf>Send_buf_high_thershold){
		char *control;
		try{control=new char[16];}catch(bad_alloc){tip(2,9);return;}
		set_control(control,Send_buf,Send_buf_high_thershold);
		Special_window_frame=control;
		Send_Upper(-1);
	}
}
void Recv_Lower(int len,int i){//���²��յ�
	if(len==2&&(unsigned char)*Tmp_buf==0xff){//����ǿ�����Ϣ�������ͬ��ӵ��ָ��
		Recv_amount.Total+=16; Recv_time.Total++;
		Recv_amount.Lower+=16; Recv_time.Lower++;
		Recv_amount.LCtrl+=16; Recv_time.LCtrl++;
		char n=*(Tmp_buf+1);
		if(n==0) Lower_full=0;
		else if(n>0) Lower_full+=n;
		return;
	}//������Ϊ��������Ϣ����������
	
	//�����ݼ������ջ���
	if(Recv_buf+8*len>MAX_BUFFER_SIZE){
		Recv_amount.Error+=8*len; Recv_time.Error++;
		tip(2,7);
	}
	else{
		Lower_recv_data=1;
		Recv_amount.Total+=8*len; Recv_time.Total++;
		Recv_amount.Lower+=8*len; Recv_time.Lower++;
		Recv_amount.LData+=8*len; Recv_time.LData++;
		if(T_Print_mode) tip(2,6);
		
		int Source_mac_port=0;
		get_source_mac_info(&Source_ip,&Source_mac_port,&Sequence,Tmp_buf,len);
		len-=9;
		
		Source_subnet=*(Tmp_buf+9);
		Source_port=*((unsigned short*)(Tmp_buf+10));
		Destination_subnet=*(Tmp_buf+12);
		Destination_port=*((unsigned short*)(Tmp_buf+13));
//		cout<<Source_ip<<" "<<Source_mac_port<<" "<<Source_subnet<<" @ "<<Destination_port<<" "<<Destination_subnet<<endl;
		
		int Low_num=Sub_net_next[Destination_subnet];
//		cout<<Low_num<<" # "<<Source_mac_port<<endl;
		if((Low_num==-1)||(Source_mac_port==65535)){//�Լ�����Ŀ�����������ϲ㷢
			
			char *tmp_frame;
			try{tmp_frame=new char[8*len];}catch(bad_alloc){tip(2,9);return;}
			
			set_source_ip_info(Source_subnet,Source_port,-1,Tmp_buf+9);
			Byte2Bit(tmp_frame,8*len,Tmp_buf+9,len);//��ת��Ϊbit�ͣ������
			
			Recv_buf+=8*len;           //������ռ�ô�С
			Recv_queue.push(tmp_frame);//��������ն���
			
			INFO tmp_info(-1,len,8*len,-1,-1,0);
			Recv_info.push(tmp_info);  //��ӽ�����Ϣ������
		}
		else if(Low_num==i)//Ӧ�������ڵ���Ϣ�����ô������⣬ֱ�ӷ���
			return;
		else{//����һ���ӿ�ת��
			char *tmp_frame;
			try{tmp_frame=new char[8*len];}catch(bad_alloc){tip(2,9);return;}
			Byte2Bit(tmp_frame,8*len,Tmp_buf+9,len);//��ת��Ϊbit�ͣ������
			
			Send_buf+=8*len;           //������ռ�ô�С
			Send_queue.push(tmp_frame);//��������Ͷ���
			
			INFO tmp_info(-1,len,8*len,Low_num,-1,0);
			Send_info.push(tmp_info);  //��ӷ�����Ϣ������
		}
	}
	
	//�жϻ���ռ䣬�����Ϳ�����Ϣ
	if(Recv_buf>Recv_buf_high_thershold){
		char *control;
		try{control=new char[16];}catch(bad_alloc){tip(2,9);return;}
		set_control(control,Recv_buf,Recv_buf_high_thershold);
		Special_window_frame=control;
		INFO tmp_info(-1,-1,-1,i,-1,3);
		Special_window_info=tmp_info;
		Send_Lower(-1);
	}
}
int main(int argc,char *argv[]){
	//��ʼ��
	tip(0,0);
	initial(argc,argv);
	
	//ѭ�����ա���������
	SOCKADDR_IN Remote_addr;              //���ڴ洢���յ����׽���
	register bool event_happen=1;         //�Ƿ����¼�����(�����ж��Ƿ��ӡ������Ϣ)
	register int tmp;                     //�洢���׽��ֽ��յ������ݳ���
	register unsigned short Upper_wait=0; //���ϲ㷢�͵ĵȴ�ʱ��
	register unsigned short Lower_wait=0; //���²㷢�͵ĵȴ�ʱ��
	register unsigned long long cycle=0;  //ѭ��������
	while(++cycle){
//		printf(" $%d$ ",cycle);
		//���Խ�������
		while(1){//���Ͻ��գ�ֱ������ʧ��
			tmp=recvfrom(Local_sock,Tmp_buf,MAX_BUFFER_SIZE,0,(SOCKADDR*)&Remote_addr,&SOCKADDR_SIZE);
			if(tmp==0){//��һ�˹ر������ӣ�����ҲӦ�ر�
				closesocket(Local_sock);
				Local_sock=0;
				tip(2,1);
				Sleep(1000);
				break;
			}
			if(tmp==-1){//�����쳣
				tmp=WSAGetLastError();
				if(tmp!=10060){//����ԭ���ǳ�ʱ
					closesocket(Local_sock);
					Local_sock=0;
					cout<<tmp<<endl;
					tip(2,2);
					Sleep(1000);
				}
				break;
			}
			else if(tmp>0){//�������յ������ˣ���ʼ�ж�������Դ
				event_happen=1;
				bool unknown=1;
				if(compare_sock(Remote_addr,Cmd_addr)&&(strncmp(Tmp_buf,"exit",5)==0)){//����㷢���˹ر�ָ��
					unknown=0;
					shutdown();
					return 0;
				}
				if(tmp>MAX_BUFFER_SIZE){//������������
					Recv_amount.Error+=tmp; Recv_time.Error++;
					tip(2,3);
				}
				if(!Top&&compare_sock(Remote_addr,Upper_addr)){//��·�������ϲ�
					unknown=0;
					Recv_Upper(tmp);
				}
				for(int i=0;i<=Lower_number-1;++i)//�²�
				if(compare_port(Remote_addr,Lower_addr[i])){
					unknown=0;
					Recv_Lower(tmp,i);
				}
				if(unknown){//��Դδ֪
					Recv_amount.Unknown+=tmp; Recv_time.Unknown++;
				}
			}
		}
		
		//�������·�������
		if(Lower_send_stop) Lower_send_stop--;
		else{
			Lower_wait+=Recv_timeout;
			while(Lower_wait>=Lower_Send_timeout){
				Lower_wait-=Lower_Send_timeout;
				if(Send_buf){
					if(Send_queue.empty()||Send_info.empty()){//�������󣬻��治Ϊ�գ�������Ϊ��
						tip(2,5);
						Send_buf=0;                                 //���û���
						while(!Send_queue.empty()) Send_queue.pop();//��ն���
						while(!Send_info.empty()) Send_info.pop();  //��ն���
					}
					else{//�޴�����������
						event_happen=1;
						
						Send_tmp_data=Send_queue.front();//��ȡ����������
						Send_tmp_info=Send_info.front(); //��ȡ������������Ϣ
						
						Send_buf-=Send_tmp_info.buf_len;//�ͷſռ�
						Send_queue.pop();//����
						Send_info.pop(); //����
						
						Send_Lower(0);//����
					}
				}
				else if((!Send_queue.empty())||(!Send_info.empty())){//�������󣬻���Ϊ�գ������в�Ϊ��
					tip(2,5);
					Send_buf=0;                                 //���û���
					while(!Send_queue.empty()) Send_queue.pop();//��ն���
					while(!Send_info.empty()) Send_info.pop();  //��ն���
				}
			}
		}
		
		//�������Ϸ�������
		if(Upper_send_stop) Upper_send_stop--;
		else if(!Top){//��·����
			Upper_wait+=Recv_timeout;
			while(Upper_wait>=Upper_Send_timeout){
				Upper_wait-=Upper_Send_timeout;
				if(Recv_buf){
					if(Recv_queue.empty()||Recv_info.empty()){//�������󣬻��治Ϊ�գ�������Ϊ��
						tip(2,8);
						Recv_buf=0;                                 //���û���
						while(!Recv_queue.empty()) Recv_queue.pop();//��ն���
						while(!Recv_info.empty()) Recv_info.pop();  //��ն���
					}
					else{//�޴�����������
						event_happen=1;
						
						Recv_tmp_data=Recv_queue.front();//��ȡ����������
						Recv_tmp_info=Recv_info.front(); //��ȡ������������Ϣ
						
						Recv_buf-=Recv_tmp_info.buf_len;//�ͷſռ�
						Recv_queue.pop();//����
						Recv_info.pop(); //����
						
						Send_Upper(0);
					}
				}
				else if((!Recv_queue.empty())||(!Recv_info.empty())){//�������󣬻���Ϊ�գ������в�Ϊ��
					tip(2,8);
					Recv_buf=0;                                 //���û���
					while(!Recv_queue.empty()) Recv_queue.pop();//��ն���
					while(!Recv_info.empty()) Recv_info.pop();  //��ն���
				}
			}
		}
		
		//��������������Ϣ
		if(!Top){//��·����
			//1.ӵ��ָ��
			if(Upper_send_data&&(Recv_buf>Recv_buf_low_thershold)&&(Recv_buf<=Recv_buf_high_thershold)){//�ϲ�ӵ��ָ��
				event_happen=1;
				Upper_full--;
			}
			if(Lower_send_data&&(Send_buf>Send_buf_low_thershold)&&(Send_buf<=Send_buf_high_thershold)){//�²�ӵ��ָ��
				event_happen=1;
				Lower_full--;
			}
			if(Upper_recv_data&&(Send_buf>Send_buf_high_thershold)){//�²�ӵ��ָ��
				event_happen=1;
				Lower_full--;
			}
			if(Lower_recv_data&&(Recv_buf>Recv_buf_high_thershold)){//�ϲ�ӵ��ָ��
				event_happen=1;
				Upper_full--;
			}
			//2.�ϲ����
			if(Upper_full>=100){//�ϲ㷢�ͳ�ʱ
				event_happen=1;
				double k=log(100-Upper_full_thershold)/log(2);
				int time=ceil((double)Recv_timeout*0.05*k);
				if(Upper_Send_timeout+time<=1000) Upper_Send_timeout+=time;
				Upper_full=3;
				Upper_send_stop=3;
			}
			else if(Upper_full>Upper_full_thershold){
				event_happen=1;
				double k=log(Upper_full-Upper_full_thershold)/log(2);
				int time=ceil((double)Recv_timeout*0.05*k);
				if(Upper_Send_timeout+time<=1000) Upper_Send_timeout+=time;
				Upper_full=3;
			}
			else if(Upper_full<=0){
				event_happen=1;
				Upper_full=Upper_full_thershold*0.5;
				int time=ceil((double)Upper_Send_timeout*0.1);
				if(Upper_Send_timeout-time>=5) Upper_Send_timeout-=time;
			}
			//3.�²����
			if(Lower_full>=100){//�²㷢�ͳ�ʱ
				event_happen=1;
				double k=log(100-Lower_full_thershold)/log(2);
				int time=ceil((double)Recv_timeout*0.05*k);
				if(Lower_Send_timeout+time<=1000) Lower_Send_timeout+=time;
				Lower_full=3;
				Lower_send_stop=3;
			}
			else if(Lower_full>Lower_full_thershold){
				event_happen=1;
				double k=log(Lower_full-Lower_full_thershold)/log(2);
				int time=ceil((double)Recv_timeout*0.05*k);
				if(Lower_Send_timeout+time<=1000) Lower_Send_timeout+=time;
				Lower_full=3;
			}
			else if(Lower_full<=0){
				event_happen=1;
				Lower_full=Lower_full_thershold*0.5;
				int time=ceil((double)Lower_Send_timeout*0.1);
				if(Lower_Send_timeout-time>=5) Lower_Send_timeout-=time;
			}
			//4.�������
			if(Recv_buf_no_empty&&(Recv_buf==0)){//���ջ�������ת�գ��������������Ϣ
				char *control;
				try{control=new char[16];}catch(bad_alloc){tip(2,16);}
				set_control(control,-1,0);
				Special_window_frame=control;
				Send_Upper(-1);
			}
			if(Send_buf_no_empty&&(Send_buf==0)){//���ͻ�������ת�գ��������������Ϣ
				for(int i=0;i<=Lower_number-1;++i){
					char *control;
					try{control=new char[16];}catch(bad_alloc){tip(2,16);}
					set_control(control,-1,0);
					Special_window_frame=control;
					INFO tmp_info(-1,-1,-1,i,-1,-1);
					Special_window_info=tmp_info;
					Send_Lower(-1);
				}
			}
			Upper_send_data=0;         //�������Ϸ��������ж�ֵ
			Lower_send_data=0;         //�������·��������ж�ֵ
			Upper_recv_data=0;         //���ô��Ͻ��������ж�ֵ
			Lower_recv_data=0;         //���ô��½��������ж�ֵ
			Recv_buf_no_empty=Recv_buf;//���½��ջ���״̬�ж�ֵ
			Send_buf_no_empty=Send_buf;//���·��ͻ���״̬�ж�ֵ
		}
		else{//·����
			//1.ӵ��ָ��
			if(Lower_send_data&&(Send_buf>Send_buf_low_thershold)&&(Send_buf<=Send_buf_high_thershold)){//�²�ӵ��ָ��
				event_happen=1;
				Lower_full--;
			}
			if(Lower_recv_data&&(Send_buf>Send_buf_high_thershold)){//�ϲ�ӵ��ָ��
				event_happen=1;
				Lower_full--;
			}
			//2.�²����
			if(Lower_full>=100){//�²㷢�ͳ�ʱ
				event_happen=1;
				double k=log(100-Lower_full_thershold)/log(2);
				int time=ceil((double)Recv_timeout*0.05*k);
				if(Lower_Send_timeout+time<=1000) Lower_Send_timeout+=time;
				Lower_full=3;
				Lower_send_stop=3;
			}
			else if(Lower_full>Lower_full_thershold){
				event_happen=1;
				double k=log(Lower_full-Lower_full_thershold)/log(2);
				int time=ceil((double)Recv_timeout*0.05*k);
				if(Lower_Send_timeout+time<=1000) Lower_Send_timeout+=time;
				Lower_full=3;
			}
			else if(Lower_full<=0){
				event_happen=1;
				Lower_full=Lower_full_thershold*0.5;
				int time=ceil((double)Lower_Send_timeout*0.1);
				if(Lower_Send_timeout-time>=5) Lower_Send_timeout-=time;
			}
			//3.�������
			if(Send_buf_no_empty&&(Send_buf==0)){//���ͻ�������ת�գ��������������Ϣ
				for(int i=0;i<=Lower_number-1;++i){
					char *control;
					try{control=new char[16];}catch(bad_alloc){tip(2,16);}
					set_control(control,-1,0);
					Special_window_frame=control;
					INFO tmp_info(-1,-1,-1,i,-1,-1);
					Special_window_info=tmp_info;
					Send_Lower(-1);
				}
			}
			Lower_send_data=0;         //�������·��������ж�ֵ
			Lower_recv_data=0;         //���ô��½��������ж�ֵ
			Send_buf_no_empty=Send_buf;//���·��ͻ���״̬�ж�ֵ
		}
		
		//��ӡͳ����Ϣ
		if(event_happen){
			print_data(Recv_timeout,Upper_Send_timeout,Lower_Send_timeout,0,0,\
			           Upper_full,Lower_full,Send_buf,Recv_buf,0,0,0,\
			           &Send_amount,&Send_time,&Recv_amount,&Recv_time,2,13);
			event_happen=0;
		}
	}
}
