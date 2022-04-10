//2 Data Link Layer(��·��)
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
const int HEAD_SIZE=sizeof(HEAD);      //ͷ������(�ֽ�)
int SOCKADDR_SIZE=sizeof(SOCKADDR_IN); //�׽��ֵ�ַ����(�ֽ�)

WSADATA     Local_data;    //�����ʼ����Ϣ
SOCKET      Local_sock;    //�����׽���
SOCKADDR_IN Local_addr;    //�����׽��ֵ�ַ
SOCKADDR_IN Upper_addr;    //�ϲ��׽��ֵ�ַ(�����)
SOCKADDR_IN Lower_addr[8]; //�²��׽��ֵ�ַ(�����)
SOCKADDR_IN Cmd_addr;      //������׽��ֵ�ַ

int Top=0;                  //�Ƿ�Ϊ���㣬���Ƿ�Ϊ������
int Sub_net=0;              //��������������
int Local_ip;               //������ʵIP
int Local_port;             //������ʵ�˿�
int Lower_mac_ip[8][128];   //�²���ԴMAC����ʵIP             (��ַ��)
int Lower_mac_port[8][128]; //�²���ԴMAC����ʵ�˿�           (��ַ��)
int Lower_mac_num[8];       //��ѧϰ���²�MAC��ַ�� (���128) (��ַ�����)

int Print_mode=0;   //�����շ���Ϣ��ӡģʽ��0Ϊ����ӡ�շ������ݣ�1Ϊ��ӡbit����2Ϊ��ӡ8���� (Ĭ��1)
int F_Print_mode=0; //����֡������Ϣ��ӡģʽ��0Ϊ����ӡ֡�Ĵ�����Ϣ��1Ϊ��ӡ (Ĭ��0)
int T_Print_mode=0; //������ʾ��Ϣ��ӡģʽ��0Ϊֻ��ӡ��������ʱ����ʾ��Ϣ��1���Ӵ�ӡ�շ�������Ϣ��2���Ӵ�ӡ����������Ϣ (Ĭ��0)
int A_Print_mode=0; //�����ַ�����Ϣ��ӡģʽ��0Ϊ����ӡ������Ϣ��1Ϊ��ӡ (Ĭ��0)
int Upper_mode=1;   //�ϲ�ӿ�ģʽ��0Ϊbit���飬1Ϊ�ֽ����� (Ĭ��1)
int Lower_mode[8];  //�²�ӿ�ģʽ��0Ϊbit���飬1Ϊ�ֽ����� (Ĭ��0)
int Lower_number=1; //�²�ʵ������ (Ĭ��1)(���Ϊ8)

int Upper_full=25;         //�ϲ�ӵ��ָ�� (Ĭ��25)
int Lower_full=25;         //ͬ��ӵ��ָ�� (Ĭ��25)
int Recv_timeout=20;       //���ճ�ʱ(ms) (Ĭ��20)(��Ϊ�����������е�ʱ�䵥λ)(��Ҫ)
int R_Send_timeout=200;    //�ش���ʱ(ms) (Ĭ��200)
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

int Divide_len=100;        //��֡��֡���� (bit)(Ĭ��100)(���Ϊ1000)
int Divide_up_thershold=5; //ʹ��֡�������ӵģ�δ�������������������ֵ 

int Send_seq[128];            //���͵�֡���
int Send_window_size=10;      //���ʹ��ڴ�С (Ĭ��10)(���Ϊ100)
bool Send_window_use[100];    //���ʹ����Ƿ�ռ��
int Send_window_wait[100];    //���ʹ��ڵĵȴ�ʱ�� 
int Send_window_busy_check=3; //���ʹ��ڵķ�æ�ж� (Ĭ���ش�����Ϊ3�ж�Ϊ��æ)
bool Send_window_busy[100];   //���ʹ����Ƿ�æ 
int Send_window_busy_num;     //���ʹ��ڵķ�æ���� 
int Send_window_stop_check=5; //���ʹ��ڵ���ͣ�ж� (Ĭ���ش�����Ϊ5��ͣ�ش�)
bool Send_window_stop[100];   //���ʹ����Ƿ���ͣ
int Send_window_stop_num;     //���ʹ��ڵ���ͣ���� 

char *Send_window_frame[100]; //���ʹ��ڵ�֡
INFO Send_window_info[100];   //���ʹ��ڵ�֡��Ϣ
char *Special_window_frame;   //���ⷢ�ʹ��ڵ�֡ 
INFO Special_window_info;     //���ⷢ�ʹ��ڵ�֡��Ϣ 
char *Recv_tmp_data;          //�����Ϸ��͵�����
INFO Recv_tmp_info;           //�����Ϸ��͵�������Ϣ

int Recv_buf_high_thershold=0.4*MAX_BUFFER_SIZE; //���ջ�������ж���ֵ
int Recv_buf_low_thershold=0.25*MAX_BUFFER_SIZE; //���ջ���Ͽ��ж���ֵ
int Send_buf_high_thershold=0.4*MAX_BUFFER_SIZE; //���ͻ�������ж���ֵ
int Send_buf_low_thershold=0.25*MAX_BUFFER_SIZE; //���ͻ���Ͽ��ж���ֵ
int Upper_full_thershold=25; //�ϲ�ӵ��ָ���ϴ��ж���ֵ
int Lower_full_thershold=25; //ͬ��ӵ��ָ���ϴ��ж���ֵ

int Upper_send_stop=0;    //���Ϸ�����ͣ������
int Lower_send_stop=0;    //���·�����ͣ������
bool Upper_send_data=0;   //�Ƿ����ϲ㷢����������Ϣ
bool Lower_send_data=0;   //�Ƿ����²㷢����������Ϣ
bool Upper_recv_data=0;   //�Ƿ���ϲ������������Ϣ
bool Lower_recv_data=0;   //�Ƿ���²������������Ϣ
bool Recv_buf_no_empty=0; //�ϸ����ڽ��ջ����Ƿ񲻿�
bool Send_buf_no_empty=0; //�ϸ����ڷ��ͻ����Ƿ񲻿�
bool ACK_add_clear=0;     //�Ƿ���ACK��������������Ϣ

bool Recv_data_time[128][4194304]; //���յ�������֡������ͳ�� (������4194304-1)

void shutdown(){//�˳�
	WSACleanup();
	if(Tmp_buf) delete [] Tmp_buf;
	if(Special_window_frame) delete [] Special_window_frame;
	if(Recv_tmp_data) delete [] Recv_tmp_data;
	for(int i=0;i<=99;++i){
		if(Send_window_frame[i])
		delete [] Send_window_frame[i];
	}
}
void tip(int type,int num){//��ʾ
	if(type==0&&num==0){
		cprintf("#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#",62);
		cprintf("        ��        ·        ��        ",62);
		cprintf("#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#\n",62);
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
				case 3:{cprintf("*���ͣ�֡���ȳ����������ޣ���֡ʧ��",3);
						print_void(84);printf("\n");break;}
				case 4:{cprintf("*���ͣ��յ����ݣ��ѱ��������ͻ���",3);
						print_void(86);printf("\n");break;}//T_Print_mode
				case 5:{cprintf("*���ͣ����ͻ���ﵽ���ޣ��Ѷ����������͵�֡",3);
						print_void(76);printf("\n");break;}
				case 6:{cprintf("*���ͣ����Ͷ����뻺��ռ䲻ƥ�䣬�����÷��Ͷ����뻺��",3);
						print_void(66);printf("\n");break;}
				case 7:{cprintf("*���գ�֡���ȳ����������ޣ��ָ�ʧ��",3);
						print_void(84);printf("\n");break;}
				case 8:{cprintf("*���գ�֡�����޷���������֡���ȹ��̣��ж�ΪACK��NAK���Ѷ���",3);
						print_void(60);printf("\n");break;}//T_Print_mode
				case 9:{cprintf("*���գ�֡�����޷���������֡���Ƚϳ����ж�Ϊ����֡�������ش�",3);
						print_void(60);printf("\n");break;}//T_Print_mode
				case 10:{cprintf("*���գ��յ�ACK",3);
						print_void(105);printf("\n");break;}//T_Print_mode
				case 11:{cprintf("*���գ��յ�NAK",3);
						print_void(105);printf("\n");break;}//T_Print_mode
				case 12:{cprintf("*���գ��յ�����֡��֡����ظ����Ѷ�������֡�������ACK�����ͻ���",3);
						print_void(55);printf("\n");break;}//T_Print_mode
				case 13:{cprintf("*���գ��յ�����֡���ѱ��������ջ��棬�����ACK�����ͻ���",3);
						print_void(63);printf("\n");break;}//T_Print_mode
				case 14:{cprintf("*���գ����ջ���ﵽ���ޣ��Ѷ������յ�֡",3);
						print_void(80);printf("\n");break;}
				case 15:{cprintf("*���գ����ն����뻺��ռ䲻ƥ�䣬�����ý��ն����뻺��",3);
						print_void(66);printf("\n");break;}
				case 16:{cprintf("*�ڴ���䣺ʧ�ܣ�",12);
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
			s2="LINK";
			s3=argv[3];
			break;
		}
		default:tip(1,1);
	}
	CCfgFileParms File(s1,s2,s3);
	Link_file=File;
	Link_file.getValueInt(Top,(char*)"Top"); //�Ƿ�Ϊ����
	Link_file.getValueInt(Sub_net,(char*)"Sub_net");//������
	Link_file.getValueInt(Print_mode,(char*)"Print_mode");    //�շ���Ϣ��ӡģʽ
	Link_file.getValueInt(F_Print_mode,(char*)"F_Print_mode");//֡������Ϣ��ӡģʽ
	Link_file.getValueInt(T_Print_mode,(char*)"T_Print_mode");//��ʾ��Ϣ��ӡģʽ
	Link_file.getValueInt(A_Print_mode,(char*)"A_Print_mode");//��ַ��Ϣ��ӡģʽ
	Link_file.getValueInt(Recv_timeout,(char*)"Recv_timeout");    //���ճ�ʱ
	Link_file.getValueInt(R_Send_timeout,(char*)"R_Send_timeout");//�ش���ʱ
	Link_file.getValueInt(Upper_Send_timeout,(char*)"Upper_Send_timeout");//�ϲ㷢�ͳ�ʱ
	Link_file.getValueInt(Lower_Send_timeout,(char*)"Lower_Send_timeout");//�²㷢�ͳ�ʱ
	Link_file.getValueInt(Send_window_size,(char*)"Send_window_size");//���ʹ�������
	Link_file.getValueInt(Divide_len,(char*)"Divide_len");            //��֡����
	//�²�ʵ������
	Lower_number=Link_file.getUDPAddrNumber(CCfgFileParms::LOWER);
	if(Lower_number<0) tip(1,1);
	//��ȡ����IP��˿���Ϣ
	HEAD tmp_h=NULL_HEAD;
	string info=Link_file.getUDPAddrString(CCfgFileParms::LOCAL,0);//����
	String2Num(&Local_ip,&Local_port,info);
/*	//��ȡ�²�IP��˿���Ϣ
	for(int i=0;i<=Lower_number-1;++i){
		info=Link_file.getUDPAddrString(CCfgFileParms::LOWER,i);//�²�
		String2Num(&Lower_mac_ip[i],&Lower_mac_port[i],info);
	}*/
	tip(0,1);
	
	//����
	Tmp_buf=new char[MAX_BUFFER_SIZE];//��ʱ����
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
		if(!Top){
			Upper_addr=Link_file.getUDPAddr(CCfgFileParms::UPPER,0);//�Զ�����
			Link_file.getValueInt(Upper_mode,(char*)"Upper_mode");//�ӿ�ģʽ
		}
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
	Link_file.print(11);
}
void Print_addr(){
	cprintf("MAC��ַ��",62);print_void(108);
	cprintf("\n �ӿ� |        IP        |  �˿�  ",11);print_void(80);printf("\n");
	for(int i=0;i<=Lower_number-1;++i){
		for(int j=0;j<=Lower_mac_num[i]-1;++j){
			print_void(2);cprintf(i,11);print_void(4-num_len(i));cprintf("|",11);
			print_void(2);print_ip(Lower_mac_ip[i][j],1,11);print_void(1);cprintf("|",11);
			print_void(2);print_port(Lower_mac_port[i][j],1,11);print_void(86);printf("\n");
		}
	}
}
void learn_lower_number(HEAD *h,int Low_num){//�����ַѧϰ(ֻѧԴ��ַ)
	bool have=0;
	int mac_ip=h->get_source_ip();
	int mac_port=h->get_source_port();
	if(mac_port==65535) return;//��ʾ��ԴΪ�㲥�������߼���ֱ�ӷ���
	for(int i=0;i<=Lower_mac_num[Low_num]-1;++i){
		if(Lower_mac_port[Low_num][i]==mac_port){
			have=1;
			break;
		}
	}
	if(have==0){
		Lower_mac_ip[Low_num][Lower_mac_num[Low_num]]=mac_ip;
		Lower_mac_port[Low_num][Lower_mac_num[Low_num]++]=mac_port;
		if(A_Print_mode){
			cprintf("ѧϰ���µ�ַ��  ��ip��",11);
			print_ip(mac_ip,1,11);
			cprintf("��˿ڣ�",11);
			print_port(mac_port,1,11);
			print_void(69);printf("\n");
			Print_addr();
		}
	}
}
int learn_lower_number(int mac_ip,int mac_port,int Low_num){//���� �� �����ַѧϰ(����)��������ѧϰ���mac���ڵ����
	if(mac_port==65535) return -1;//��ʾ��ԴΪ�㲥(���ϲ㴫��)����ѧϰ
	bool have=0;
	for(int i=0;i<=Lower_mac_num[Low_num]-1;++i){
		if(Lower_mac_port[Low_num][i]==mac_port){
			have=1;
			return i;
		}
	}
	if(have==0){
		Lower_mac_ip[Low_num][Lower_mac_num[Low_num]]=mac_ip;
		Lower_mac_port[Low_num][Lower_mac_num[Low_num]++]=mac_port;
		if(A_Print_mode){
			cprintf("ѧϰ���µ�ַ��  ��ip��",11);
			print_ip(mac_ip,1,11);
			cprintf("��˿ڣ�",11);
			print_port(mac_port,1,11);
			print_void(69);printf("\n");
			Print_addr();
		}
		return Lower_mac_num[Low_num]-1;
	}
}
int get_lower_number(HEAD *h){//���ҵ�ַ���õ��²�ӿں�
	//�ɹ����ؽӿںţ�ʧ�ܻ�㲥����-1
	int Low_num=-1;
	int mac_ip=h->get_destination_ip();
	int mac_port=h->get_destination_port();
	if(mac_port==65535) return -1;
	for(int i=0;i<=Lower_number-1;++i){
		for(int j=0;j<=Lower_mac_num[i]-1;++j){
			if((Lower_mac_port[i][j]==mac_port)&&(Lower_mac_ip[i][j]==mac_ip)){
				Low_num=i;
				break;
			}
		}
		if(Low_num!=-1) break;
	}
	return Low_num;
}
int get_mac_number(int mac_ip,int mac_port,int Low_num){//���Ҷ�Ӧ�ӿڵĵ�ַ���õ�mac���ڵ�ַ���е�λ��
	//�ɹ�����λ�úţ�ʧ�ܷ���-1
	int Mac_num=-1;
	for(int j=0;j<=Lower_mac_num[Low_num]-1;++j){
		if((Lower_mac_port[Low_num][j]==mac_port)&&(Lower_mac_ip[Low_num][j]==mac_ip)){
			Mac_num=j;
			break;
		}
	}
	return Mac_num;
}
int get_send_window(){//�Զ�����һ�����ʹ���
	//�ɹ��򷵻ش��ںţ�ʧ�ܷ���-1
	for(int i=0;i<=Send_window_size-1;++i)
		if(Send_window_use[i]==0){
			if(Print_mode){//�����Ϣ
				cprintf("�����ˣ�",62);cprintf(i,62);cprintf("�Ŵ���",62);
				print_void(103);printf("\n");
			}
			return i;
		}
	if(Print_mode){
		cprintf("��������������ʧ��",62);
		print_void(101);printf("\n");
	}
	return -1;
}
void close_send_window(int window,int mode){//�رղ�����һ�����ʹ���
	//modeΪ0���ͷ�֡��Ӧ���ڴ�ռ䣬Ϊ1���ͷ�
	//ע�⣬���ں�Ϊ-1ʱ����ʹ�õ�Ϊ���ⴰ��(ר�����շ�ACK/NAK) 
	if(window>=0){//��ͨ���� 
		Send_window_use[window]=0;
		Send_window_wait[window]=0;
		if(Send_window_busy[window]){
			Send_window_busy[window]=0;
			Send_window_busy_num--;
		}
		if(Send_window_stop[window]){
			Send_window_stop[window]=0;
			Send_window_stop_num--;
		}
		Send_window_info[window]=NULL_INFO;
		if(mode&&Send_window_frame[window]) delete[] Send_window_frame[window];
		Send_window_frame[window]=NULL;
		if(Print_mode){//�����Ϣ
			cprintf("�����ˣ�",62);cprintf(window,62);cprintf("�Ŵ���",62);
			print_void(105-num_len(window));printf("\n");
		}
	}
	else{//���ⴰ�� 
		Special_window_info=NULL_INFO;
		if(mode&&Special_window_frame) delete[] Special_window_frame;
		Special_window_frame=NULL;
		if(Print_mode){//�����Ϣ
			cprintf("�����ˣ����ⴰ��",62);
			print_void(103);printf("\n");
		}
	}
}
void Send_Upper(int i){//���ϲ㷢�����ݣ�����Ϊ��������
	//ע�⣺���ں�Ϊ-1ʱ����ʹ�����ⴰ��(ר�����շ�������Ϣ)
	char *Recv_data;
	int buf_len,Low_num,Mac_num,seq;
	
	if(i!=-1){//��ͨ����
		Recv_data=Recv_tmp_data;
		  buf_len=Recv_tmp_info.buf_len;
		  Low_num=Recv_tmp_info.Lower_number;
		  Mac_num=Recv_tmp_info.Mac_number;
		      seq=Recv_tmp_info.sequence;
	}
	else{//������Ϣ
		Recv_data=Special_window_frame;
		  buf_len=16;
	}
//	printf(" #%d @%d ",Recv_tmp_info.Mac_number,Mac_num);
	
	//��ϢԤ����
	int len=buf_len/8+(buf_len>0&&buf_len%8!=0);
	char byte[len+9];
	
	if(i!=-1){//��ͨ����
		Bit2Byte(Recv_data,buf_len,byte+9,len);//������֡תΪ�ֽ���
		//���ip���˿ڡ������Ϣ
		if(Mac_num!=-1) set_source_mac_info(Lower_mac_ip[Low_num][Mac_num],Lower_mac_port[Low_num][Mac_num],seq,byte);
		else set_source_mac_info(-1,-1,seq,byte);
		len+=9;
	}
	else//������Ϣ
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
				if(Print_mode==1) print_bit(Recv_data,buf_len,"���ϲ㷢������ʧ��...����bit����",0,11);
				else print_byte(byte,len,"���ϲ㷢������ʧ��...�����ֽ�����",1,11);
			}
			else{
				if(Print_mode==1) print_bit(Recv_data,buf_len,"���ϲ㷢�Ϳ�����Ϣʧ��...��bit����",0,11);
				else print_byte(byte,len,"���ϲ㷢�Ϳ�����Ϣʧ��...���ֽ�����",1,11);
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
				if(Print_mode==1) print_bit(Recv_data,buf_len,"���ϲ㷢�����ݳɹ�������bit����",0,11);
				else print_byte(byte,len,"���ϲ㷢�����ݳɹ��������ֽ�����",1,11);
			}
			else{
				if(Print_mode==1) print_bit(Recv_data,buf_len,"���ϲ㷢�Ϳ�����Ϣ�ɹ�����bit����",0,11);
				else print_byte(byte,len,"���ϲ㷢�Ϳ�����Ϣ�ɹ������ֽ�����",1,11);
			}
		}
		if(i!=-1) delete[] Recv_tmp_data;
		else delete[] Special_window_frame;
	}
}
void Send_Lower(int i){//���²㷢�ͣ�����Ϊ���ڱ��
	//ע�⣬���ں�Ϊ-1ʱ����ʹ�õ�Ϊ���ⴰ��(ר�����շ�ACK/NAK) 
	char *Send_frame;
	int len,buf_len,Low_num,lable,resend;
	
	if(i>=0){//��ͨ���� 
		Send_frame=Send_window_frame[i];
		       len=Send_window_info[i].len;
		   buf_len=Send_window_info[i].buf_len;
		   Low_num=Send_window_info[i].Lower_number;
		     lable=Send_window_info[i].lable;
		    resend=Send_window_info[i].resend;
	}
	else{//���ⴰ�� 
		Send_frame=Special_window_frame;
		       len=Special_window_info.len;
		   buf_len=Special_window_info.buf_len;
		   Low_num=Special_window_info.Lower_number;
		     lable=Special_window_info.lable;
		    resend=Special_window_info.resend;
	}
	
	if(Lower_mode[Low_num]==0){//�²�ӿ�Ϊbit����
		int Send_len=sendto(Local_sock,Send_frame,len,0,(SOCKADDR*)&Lower_addr[Low_num],SOCKADDR_SIZE);
		if(Send_len<=0){//����ʧ�ܣ���׼���ش�
			Send_amount.Error+=len; Send_time.Error++;
			Send_buf+=buf_len;                        //���ӻ��� 
			if(lable==0){
				Send_queue.push(Send_window_frame[i]);//���֡������ĩ
				Send_info.push(Send_window_info[i]);  //���֡��Ϣ������ĩ
			}
			else{
				Send_queue.push(Special_window_frame);//���֡������ĩ
				Send_info.push(Special_window_info);  //���֡��Ϣ������ĩ
			}
			
			if(Print_mode){//�����Ϣ
				if(Print_mode==1){
					if(lable==0) print_bit(Send_frame,len,"���²㷢������ʧ��...����bit����",0,11);
					else print_bit(Send_frame,len,"���²㷢��ACK/NAKʧ��...��bit����",0,11);
				}
				else{
					if(lable==0) print_byte(Send_frame,len,"���²㷢������ʧ��...�����ֽ�����",0,11);
					else print_byte(Send_frame,len,"���²㷢��ACK/NAKʧ��...���ֽ�����",0,11);
				}
			}
			close_send_window(i,0);//�رմ���
		}
		else{//���ͳɹ�
			Send_amount.Total+=len; Send_time.Total++;
			Send_amount.Lower+=len; Send_time.Lower++;
			if(lable==0&&resend==0) {Send_amount.LData+=len; Send_time.LData++; Lower_send_data=1;}
			if(lable==0&&resend!=0) {Send_amount.Re+=len;    Send_time.Re++;    Lower_send_data=1;}
			if(lable==1) {Send_amount.ACK+=len; Send_time.ACK++;}
			if(lable==2) {Send_amount.NAK+=len; Send_time.NAK++;}
			
			if(Print_mode){//�����Ϣ
				if(Print_mode==1){
					if(lable==0) print_bit(Send_frame,len,"���²㷢�����ݳɹ�������bit����",0,11);
					else print_bit(Send_frame,len,"���²㷢��ACK/NAK�ɹ�����bit����",0,11);
				}
				else{
					if(lable==0) print_byte(Send_frame,len,"���²㷢�����ݳɹ��������ֽ�����",0,11);
					else print_byte(Send_frame,len,"���²㷢��ACK/NAK�ɹ������ֽ�����",0,11);
				}
			}
			if(lable||Top) close_send_window(i,1);//�رմ���
		}
	}
	else{//�²�ӿ�Ϊ�ֽ�����
		//�Ƚ�����֡תΪ�ֽ���
		len=buf_len/8+(buf_len%8!=0);
		char byte[len];
		Bit2Byte(Send_frame,buf_len,byte,len);
		//����
		int Send_len=sendto(Local_sock,byte,len,0,(SOCKADDR*)&Lower_addr[Low_num],SOCKADDR_SIZE);
		if(Send_len<=0){//����ʧ�ܣ���׼���ش�
			Send_amount.Error+=buf_len; Send_time.Error++;
			Send_buf+=buf_len;                        //���ӻ��� 
			if(lable==0){
				Send_queue.push(Send_window_frame[i]);//���֡������ĩ
				Send_info.push(Send_window_info[i]);  //���֡��Ϣ������ĩ
			}
			else{
				Send_queue.push(Special_window_frame);//���֡������ĩ
				Send_info.push(Special_window_info);  //���֡��Ϣ������ĩ
			}
			
			if(Print_mode){//�����Ϣ
				if(Print_mode==1){
					if(lable==0) print_bit(Send_frame,buf_len,"���²㷢������ʧ��...����bit����",0,11);
					else print_bit(Send_frame,buf_len,"���²㷢��ACK/NAKʧ��...��bit����",0,11);
				}
				else{
					if(lable==0) print_byte(byte,len,"���²㷢������ʧ��...�����ֽ�����",1,11);
					else print_byte(byte,len,"���²㷢��ACK/NAKʧ��...���ֽ�����",1,11);
				}
			}
			close_send_window(i,0);//�رմ���
		}
		else{//���ͳɹ�
			Send_amount.Total+=buf_len; Send_time.Total++;
			Send_amount.Lower+=buf_len; Send_time.Lower++;
			if(lable==0&&resend==0) {Send_amount.LData+=buf_len; Send_time.LData++; Lower_send_data=1;}
			if(lable==0&&resend!=0) {Send_amount.Re+=buf_len;    Send_time.Re++;    Lower_send_data=1;}
			if(lable==1) {Send_amount.ACK+=buf_len; Send_time.ACK++;}
			if(lable==2) {Send_amount.NAK+=buf_len; Send_time.NAK++;}
			
			if(Print_mode){//�����Ϣ
				if(Print_mode==1){
					if(lable==0) print_bit(Send_frame,buf_len,"���²㷢�����ݳɹ�������bit����",0,11);
					else print_bit(Send_frame,buf_len,"���²㷢��ACK/NAK�ɹ�����bit����",0,11);
				}
				else{
					if(lable==0) print_byte(byte,len,"���²㷢�����ݳɹ��������ֽ�����",1,11);
					else print_byte(byte,len,"���²㷢��ACK/NAK�ɹ������ֽ�����",1,11);
				}
			}
			if(lable||Top) close_send_window(i,1);//�رմ���
		}
	}
}
void broadcast(int i){//���¹㲥������Ϊ���ں�
	char *Send_frame=Send_window_frame[i];
	int          len=Send_window_info[i].len;
	int      buf_len=Send_window_info[i].buf_len;
	int   source_num=Send_window_info[i].Lower_number;
	int        lable=Send_window_info[i].lable;
	int       resend=Send_window_info[i].resend;
	
	for(int Low_num=0;Low_num<=Lower_number-1;++Low_num){
		if(Low_num==source_num) continue;//�·�ʵ�������Դ��ͬ������
//		printf("%d ",Low_num);
		if(Lower_mode[Low_num]==0){//�²�ӿ�Ϊbit����
			int Send_len=sendto(Local_sock,Send_frame,len,0,(SOCKADDR*)&Lower_addr[Low_num],SOCKADDR_SIZE);
			if(Send_len<=0){//����ʧ�ܣ�ֱ�ӷ���
				Send_amount.Error+=len; Send_time.Error++;
							
				if(Print_mode){//�����Ϣ
					if(Print_mode==1)print_bit(Send_frame,len,"���²�㲥����ʧ��...����bit����",0,11);
					else print_byte(Send_frame,len,"���²�㲥����ʧ��...�����ֽ�����",0,11);
				}
			}
			else{//���ͳɹ�
				Send_amount.Total+=len; Send_time.Total++;
				Send_amount.Lower+=len; Send_time.Lower++;
				Send_amount.LData+=len; Send_time.LData++; Lower_send_data=1;
				
				if(Print_mode){//�����Ϣ
					if(Print_mode==1)print_bit(Send_frame,len,"���²�㲥���ݳɹ�������bit����",0,11);
					else print_byte(Send_frame,len,"���²�㲥���ݳɹ��������ֽ�����",0,11);
				}
			}
		}
		else{//�²�ӿ�Ϊ�ֽ�����
			//�Ƚ�����֡תΪ�ֽ���
			len=buf_len/8+(buf_len%8!=0);
			char byte[len];
			Bit2Byte(Send_frame,buf_len,byte,len);
			//����
			int Send_len=sendto(Local_sock,byte,len,0,(SOCKADDR*)&Lower_addr[Low_num],SOCKADDR_SIZE);
			if(Send_len<=0){//����ʧ�ܣ�ֱ�ӷ���
				Send_amount.Error+=buf_len; Send_time.Error++;
				
				if(Print_mode){//�����Ϣ
					if(Print_mode==1)print_bit(Send_frame,buf_len,"���²�㲥����ʧ��...����bit����",0,11);
					else print_byte(byte,len,"���²�㲥����ʧ��...�����ֽ�����",1,11);
				}
			}
			else{//���ͳɹ�
				Send_amount.Total+=buf_len; Send_time.Total++;
				Send_amount.Lower+=buf_len; Send_time.Lower++;
				Send_amount.LData+=buf_len; Send_time.LData++; Lower_send_data=1;
				
				if(Print_mode){//�����Ϣ
					if(Print_mode==1) print_bit(Send_frame,buf_len,"���²�㲥���ݳɹ�������bit����",0,11);
					else print_byte(byte,len,"���²�㲥���ݳɹ��������ֽ�����",1,11);
				}
			}
		}
	}
	close_send_window(i,1);//�رմ���
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
	
	//��ȡ�·���IP���˿ںš�Ŀ��ʵ���(ֻ����0)
	int mac_ip=0,mac_port=0;
	get_destination_mac_info(&mac_ip,&mac_port,Tmp_buf,len);
	
	int Low_num=0;
	int Mac_num=learn_lower_number(mac_ip,mac_port,Low_num);//�����ַѧϰ(����)
	
	//��֡�����������ͻ���
	int pos=6,len2;
	while(pos<=len-1){
		if(pos+Divide_len/8-1<=len-1) len2=Divide_len/8;
		else len2=len-pos;
		
		//����ͷ��
		HEAD h;
		if(mac_port!=65535) change_head(&h,0,1,Send_seq[Mac_num],Local_ip,Local_port,Lower_mac_ip[Low_num][Mac_num],Lower_mac_port[Low_num][Mac_num]);
		else change_head(&h,0,1,0,Local_ip,Local_port,-1,65535);
		
		//��֡������֡�������ͻ���
		int frame_len=frame(Tmp_buf+pos,len2,&h,1,Lower_mode[Low_num]);
		if(frame_len==-1){
			Recv_amount.Error+=8*len2; Recv_time.Error++;
			tip(2,3);
		}
		else{
			if(Lower_mode[Low_num]==0){//֡Ϊbit�ͣ�ֱ�����
				if(Send_buf+frame_len>MAX_BUFFER_SIZE){
					Recv_amount.Error+=8*len2; Recv_time.Error++;
					tip(2,5);
				}
				else{
					Upper_recv_data=1;
					Recv_amount.Total+=8*len2; Recv_time.Total++;
					Recv_amount.Upper+=8*len2; Recv_time.Upper++;
					Recv_amount.UData+=8*len2; Recv_time.UData++;
					if(T_Print_mode) tip(2,4);
					
					char *tmp_frame;
					try{tmp_frame=new char[frame_len];}catch(bad_alloc){tip(2,16);return;}
					change_frame(tmp_frame,Tmp_buf+pos,frame_len);//����
					
					Send_buf+=frame_len;       //������ռ�ô�С
					Send_queue.push(tmp_frame);//���֡�����Ͷ���
					
					if(mac_port!=65535){//�ǹ㲥
						INFO tmp_info(Send_seq[Mac_num],frame_len,frame_len,Low_num,Mac_num,0);
						Send_info.push(tmp_info);  //��ӷ�����Ϣ������
						Send_seq[Mac_num]++;       //�������+1��Ϊ��һ�η�����׼��
					}
					else{//�㲥
						INFO tmp_info(0,frame_len,frame_len,-1,65535,0);
						Send_info.push(tmp_info);  //��ӷ�����Ϣ������
					}
				}
			}
			else{//֡Ϊ�ֽ��ͣ���ת��Ϊbit�ͣ������
				if(Send_buf+8*frame_len>MAX_BUFFER_SIZE){
					Recv_amount.Error+=8*len2; Recv_time.Error++;
					tip(2,5);
				}
				else{
					Upper_recv_data=1;
					Recv_amount.Total+=8*len2; Recv_time.Total++;
					Recv_amount.Upper+=8*len2; Recv_time.Upper++;
					Recv_amount.UData+=8*len2; Recv_time.UData++;
					if(T_Print_mode) tip(2,4);
					
					char *tmp_frame;
					try{tmp_frame=new char[8*frame_len];}catch(bad_alloc){tip(2,16);return;}
					Byte2Bit(tmp_frame,8*frame_len,Tmp_buf+pos,frame_len);//ת��
					
					Send_buf+=8*frame_len;     //������ռ�ô�С
					Send_queue.push(tmp_frame);//��������Ͷ���
					
					if(mac_port!=65535){//�ǹ㲥
						INFO tmp_info(Send_seq[Mac_num],frame_len,8*frame_len,Low_num,Mac_num,0);
						Send_info.push(tmp_info);  //��ӷ�����Ϣ������
						Send_seq[Mac_num]++;       //�������+1��Ϊ��һ�η�����׼��
					}
					else{//�㲥
						INFO tmp_info(0,frame_len,8*frame_len,-1,65535,0);
						Send_info.push(tmp_info);  //��ӷ�����Ϣ������
					}
				}
			}
		}
		pos+=Divide_len/8;
	}
	
	//�жϻ���ռ䣬�����Ϳ�����Ϣ
	if(Send_buf>Send_buf_high_thershold){
		char *control;
		try{control=new char[16];}catch(bad_alloc){tip(2,16);return;}
		set_control(control,Send_buf,Send_buf_high_thershold);
		Special_window_frame=control;
		Send_Upper(-1);
	}
}
void Recv_Lower(int len,int Low_num){//���²��յ�
	int bit_len=(1+7*(Lower_mode[Low_num]==1))*len;
	
	char backup_frame[bit_len];//����֡�����ڽ�����
	if(Top) change_frame(backup_frame,Tmp_buf,len);
	
	HEAD h;
	int frame_len=deframe(Tmp_buf,len,&h,Lower_mode[Low_num],0);//�ָ����ݣ�������ת��Ϊbit��
	
	if(frame_len==-1){//�յ���֡����
		Recv_amount.Error+=8*bit_len; Recv_time.Error++;
		tip(2,7);
	}
	else if(frame_len==-2){//�յ���֡�������޷�����
		Recv_amount.Total+=8*bit_len; Recv_time.Total++;
		Recv_amount.Lower+=8*bit_len; Recv_time.Lower++;
		Recv_amount.Wrong+=8*bit_len; Recv_time.Wrong++;
		
		if(!Top){//������Ƕ���
			if(bit_len<=8*HEAD_SIZE*1.5){//֡���϶̣�ֱ�Ӷ���
				if(T_Print_mode) tip(2,8);
			}
			else{//֡���ϳ���������ȡ��ţ����NAK�����Ͷ���
				int seq=h.get_sequence();
				if(seq<=0||seq>=4194304) return;//��Ų����������ֱ�ӷ���
				
				int d_ip=h.get_destination_ip();
				int d_port=h.get_destination_port();
				if((d_ip!=Local_ip)||(d_port!=Local_port)) return;//Ŀ�ĵز��Ǳ��أ�ֱ�ӷ���
				
				int Mac_num=get_mac_number(h.get_source_ip(),h.get_source_port(),Low_num);
				if(Mac_num==-1) return;//�޷�ȷ����Դ��ַ��ֱ�ӷ���
				
				if(T_Print_mode) tip(2,9);
				change_head(&h,2,1,seq,Local_ip,Local_port,Lower_mac_ip[Low_num][Mac_num],Lower_mac_port[Low_num][Mac_num]);
				frame_len=frame(Tmp_buf,0,&h,0,Lower_mode[Low_num]);
				
				if(Lower_mode[Low_num]==0){//֡Ϊbit�ͣ�ֱ�����·���
					char *tmp_frame;
					try{tmp_frame=new char[frame_len];}catch(bad_alloc){tip(2,16);return;}
					change_frame(tmp_frame,Tmp_buf,frame_len);//����
					
					INFO tmp_info(seq,frame_len,frame_len,Low_num,Mac_num,2);
					
					Special_window_frame=tmp_frame;
					Special_window_info=tmp_info;
					Send_Lower(-1);//����
				}
				else{//֡Ϊ�ֽ��ͣ���ת��Ϊbit��(���ⷢ��ʧ�ܺ���ӵ������У���������)�������·���
					char *tmp_frame;
					try{tmp_frame=new char[8*frame_len];}catch(bad_alloc){tip(2,16);return;}
					Byte2Bit(tmp_frame,8*frame_len,Tmp_buf,frame_len);//ת��
					
					INFO tmp_info(seq,frame_len,8*frame_len,Low_num,Mac_num,2);
					
					Special_window_frame=tmp_frame;
					Special_window_info=tmp_info;
					Send_Lower(-1);//����
				}
			}
		}
	}
	else{//�յ���֡����
		if(Top){//�Ƕ��㣬ת���ֱ�ӷ���
			learn_lower_number(&h,Low_num);//�����ַѧϰ
			int next_lower_num=get_lower_number(&h);
			int Mac_num=-1;
			if(next_lower_num==-1){//û���ҵ�Ŀ���ַ �� ������ǹ㲥֡
				next_lower_num=Low_num;//��һ���ӿ�������ʾ��Դ�ӿ�
				Mac_num=65535;//�㲥��mac��ַ
			}
			else//�ҵ���Ŀ���ַ
				Mac_num=get_mac_number(h.get_destination_ip(),h.get_destination_port(),next_lower_num);
			
			Recv_amount.Total+=bit_len; Recv_time.Total++;
			Recv_amount.Lower+=bit_len; Recv_time.Lower++;
			Recv_amount.LData+=bit_len; Recv_time.LData++;
			
			char *tmp_data;
			try{tmp_data=new char[len];}catch(bad_alloc){tip(2,16);return;}
			change_frame(tmp_data,backup_frame,len);//����
			Recv_buf+=bit_len;        //������ռ�ô�С
			Recv_queue.push(tmp_data);//��������ն���
			
			INFO tmp_info(-1,len,bit_len,next_lower_num,Mac_num,0);
			Recv_info.push(tmp_info); //��ӽ�����Ϣ������
			
			return;
		}
		//���Ƕ���
		int d_ip=h.get_destination_ip();
		int d_port=h.get_destination_port();
		int seq=h.get_sequence();
		int Mac_num;
		
		if((d_port!=65535)&&((d_ip!=Local_ip)||(d_port!=Local_port))){//���ǹ㲥����Ŀ�ĵز��Ǳ��أ�ֱ�ӷ���
			Recv_amount.Unknown+=bit_len;   Recv_time.Unknown++;
			return;
		}
		Mac_num=learn_lower_number(h.get_source_ip(),h.get_source_port(),Low_num);//�����ַѧϰ(����)
//		printf("%d ",Mac_num);
		
		if(h.get_ack()){//�յ�ACK
			Recv_amount.Total+=bit_len; Recv_time.Total++;
			Recv_amount.Lower+=bit_len; Recv_time.Lower++;
			Recv_amount.ACK+=bit_len;   Recv_time.ACK++;
			if(T_Print_mode) tip(2,10);
			
			if(d_port!=65535){//�ǹ㲥�ŵ���
			//�رմ���
				for(int i=0;i<=Send_window_size-1;++i)
				if(Send_window_use[i]){
					if((seq==Send_window_info[i].sequence)&&(Mac_num==Send_window_info[i].Mac_number)){
						close_send_window(i,1);
						break;
					}
				}
			//����ͬ��ӵ��ָ��
				char byte[2];
				Bit2Byte(Tmp_buf,16,byte,2);
				if(byte[1]==0) Lower_full=0;
				else if(byte[1]>0) Lower_full+=byte[1];
			}
		}
		else if(h.get_nak()){//�յ�NAK
			Recv_amount.Total+=bit_len; Recv_time.Total++;
			Recv_amount.Lower+=bit_len; Recv_time.Lower++;
			Recv_amount.NAK+=bit_len;   Recv_time.NAK++;
			if(T_Print_mode) tip(2,11);
			
			if(d_port!=65535)//�ǹ㲥�ŵ���
			for(int i=0;i<=Send_window_size-1;++i){
				if(Send_window_use[i]){
					if((seq==Send_window_info[i].sequence)&&(Mac_num==Send_window_info[i].Mac_number)){
						Send_window_wait[i]=10000000;//�޸ĵȴ�����Ϊ������ʹ�����¸�ʱ�������Զ��ش�
						break;
					}
				}
			}
		}
		else{//�յ���ͨ����
			Recv_amount.Total+=bit_len; Recv_time.Total++;
			Recv_amount.Lower+=bit_len; Recv_time.Lower++;
			Recv_amount.LData+=bit_len; Recv_time.LData++;
			
			if(seq>=4194304) return;//֡��Ź����޷�����
			if((d_port!=65535)&&Recv_data_time[Mac_num][seq]){//����յ��Ĳ��ǹ㲥�����յ�����֡��ֻ����ACK
				
				change_head(&h,1,1,seq,Local_ip,Local_port,Lower_mac_ip[Low_num][Mac_num],Lower_mac_port[Low_num][Mac_num]);
				if(ACK_add_clear==0) set_control(Tmp_buf,Recv_buf,Recv_buf_high_thershold);//���ӿ�����Ϣ
				else set_control(Tmp_buf,-1,0);//���ӿ�����Ϣ
				frame_len=frame(Tmp_buf,16,&h,0,Lower_mode[Low_num]);
				
				if(Lower_mode[Low_num]==0){//֡Ϊbit�ͣ�ֱ�ӷ���
					if(T_Print_mode) tip(2,12);
					char *tmp_frame;
					try{tmp_frame=new char[frame_len];}catch(bad_alloc){tip(2,16);return;}
					change_frame(tmp_frame,Tmp_buf,frame_len);//����
					
					INFO tmp_info(seq,frame_len,frame_len,Low_num,Mac_num,1);
					
					Special_window_frame=tmp_frame;
					Special_window_info=tmp_info;
					Send_Lower(-1);//����
				}
				else{//֡Ϊ�ֽ��ͣ���ת��Ϊbit��(���ⷢ��ʧ�ܺ���ӵ������У���������)���ٷ���
					if(T_Print_mode) tip(2,12);
					char *tmp_frame;
					try{tmp_frame=new char[8*frame_len];}catch(bad_alloc){tip(2,16);return;}
					Byte2Bit(tmp_frame,8*frame_len,Tmp_buf,frame_len);//ת��
					
					INFO tmp_info(seq,frame_len,8*frame_len,Low_num,Mac_num,1);
					
					Special_window_frame=tmp_frame;
					Special_window_info=tmp_info;
					Send_Lower(-1);//����
				}
			}
			else{//δ�յ�����֡����������ݵ�bit��������/���Ͷ��У�����Ӷ�ӦACK�����Ͷ���
				if(Recv_buf+frame_len>MAX_BUFFER_SIZE){
					Recv_amount.Error+=bit_len; Recv_time.Error++;
					tip(2,14);
				} 
				else{
					Recv_data_time[Mac_num][seq]=1;
					Lower_recv_data=1;
					if(T_Print_mode) tip(2,13);
					
					//�������
					char *tmp_data;
					try{tmp_data=new char[frame_len];}catch(bad_alloc){tip(2,16);return;}
					change_frame(tmp_data,Tmp_buf,frame_len);//����
					Recv_buf+=frame_len;      //������ռ�ô�С
					Recv_queue.push(tmp_data);//��������ն���
					
					INFO tmp_info(seq,frame_len,frame_len,Low_num,Mac_num,0);
					Recv_info.push(tmp_info); //��ӽ�����Ϣ������
					
					//����յ��Ĳ��ǹ㲥������ACK
					if(d_port!=65535){
						change_head(&h,1,1,seq,Local_ip,Local_port,Lower_mac_ip[Low_num][Mac_num],Lower_mac_port[Low_num][Mac_num]);
						if(ACK_add_clear==0) set_control(Tmp_buf,Recv_buf,Recv_buf_high_thershold);//���ӿ�����Ϣ
						else set_control(Tmp_buf,-1,0);//���ӿ�����Ϣ
						frame_len=frame(Tmp_buf,16,&h,0,Lower_mode[Low_num]);
						
						if(Lower_mode[Low_num]==0){//�²�Ϊbit�ͣ�ֱ�ӷ���
							char *tmp_frame;
							try{tmp_frame=new char[frame_len];}catch(bad_alloc){tip(2,16);return;}
							change_frame(tmp_frame,Tmp_buf,frame_len);//����
							
							INFO tmp_info2(seq,frame_len,frame_len,Low_num,Mac_num,1);
							
							Special_window_frame=tmp_frame;
							Special_window_info=tmp_info2;
							Send_Lower(-1);//����
						}
						else{//�²�Ϊ�ֽ��ͣ���ת��Ϊbit��(���ⷢ��ʧ�ܺ���ӵ������У���������)���ٷ���
							char *tmp_frame;
							try{tmp_frame=new char[8*frame_len];}catch(bad_alloc){tip(2,16);return;}
							Byte2Bit(tmp_frame,8*frame_len,Tmp_buf,frame_len);//ת��
							
							INFO tmp_info2(seq,frame_len,8*frame_len,Low_num,Mac_num,1);
							
							Special_window_frame=tmp_frame;
							Special_window_info=tmp_info2;
							Send_Lower(-1);//����
						}
					}
				}
			}
		}
	}
}
int main(int argc,char *argv[]){
	//��ʼ��
	tip(0,0);
	initial(argc,argv);
	
	//ѭ�����ա���������
	SOCKADDR_IN Remote_addr;                //���ڴ洢���յ����׽���
	register bool event_happen=1;           //�Ƿ����¼�����(�����ж��Ƿ��ӡ������Ϣ)
	register bool receive_blocked=0;        //�������н����Ƿ�����(�������յ�)
	register unsigned char Error_time=0;    //�������ش��Ĵ���
	register unsigned char No_Error_cycle=0;//δ�������������������	
	register int tmp;                       //�洢���׽��ֽ��յ������ݳ���
	register unsigned short time_tmp;       //��֡ʱ֡���ȼ��ٵĴ����ж����� 
	register unsigned short Upper_wait=0;   //���ϲ㷢�͵ĵȴ�ʱ��
	register unsigned short Lower_wait=0;   //���²㷢�͵ĵȴ�ʱ��
	register unsigned long long cycle=0;    //ѭ��������
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
				receive_blocked=1;
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
				if(!Top&&compare_sock(Remote_addr,Upper_addr)){//�ϲ�
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
		//1.�ش�������
			for(int i=0;i<=Send_window_size-1;++i)
			if(Send_window_use[i]){//���ڱ�ռ��
				if(receive_blocked) Send_window_wait[i]+=Recv_timeout;
				if(Send_window_wait[i]>=R_Send_timeout){//���ڳ����ش���ʱ
					Send_window_wait[i]=0;
					Send_window_info[i].resend++;
					
					if(Send_window_stop[i]==0){//����δ��ͣ
						if(Send_window_info[i].resend>=Send_window_stop_check){//�ط������ﵽ��ͣ�ж���������ͣ�ش�
							Send_window_stop[i]=1;//����Ϊ��ͣ����
							Send_window_stop_num++;
						}
						else{
							if(Send_window_busy[i]==0){//����δ��æ
								if(Send_window_info[i].resend>=Send_window_busy_check){//�ط������ﵽ��æ�ж���������¼��æ
									Send_window_busy[i]=1;//����Ϊ��æ����
									Send_window_busy_num++;
								}
							}
							event_happen=1;
							Error_time++;
							No_Error_cycle=-1;
							Send_Lower(i);//�ش�
						}
					}
					else if(Send_window_info[i].resend%(Send_window_stop_check*Send_window_stop_check)==0){
						event_happen=1;
						Error_time++;
						Send_Lower(i);//�ش�
					}
				}
			}
		//2.����������
			Lower_wait+=Recv_timeout;
			while(Lower_wait>=Lower_Send_timeout){
				Lower_wait-=Lower_Send_timeout;
				if(Send_buf){
					if(Send_queue.empty()||Send_info.empty()){//�������󣬻��治Ϊ�գ�������Ϊ��
						tip(2,6);
						Send_buf=0;                                 //���û���
						while(!Send_queue.empty()) Send_queue.pop();//��ն���
						while(!Send_info.empty()) Send_info.pop();  //��ն���
					}
					else{//�޴�����������
						int window=get_send_window();
						if(window!=-1){//�з��ʹ��ڿ���
							event_happen=1;
							
							Send_window_use[window]=1;                   //ռ�ô���
							Send_window_frame[window]=Send_queue.front();//��ȡ����������
							Send_window_info[window]=Send_info.front();  //��ȡ������������Ϣ
							
							Send_buf-=Send_window_info[window].buf_len;//�ͷſռ�
							Send_queue.pop();//����
							Send_info.pop(); //����
							
							if(Send_window_info[window].Mac_number==65535) broadcast(window);//�㲥
							else Send_Lower(window);//����
						}
					}
				}
				else if((!Send_queue.empty())||(!Send_info.empty())){//�������󣬻���Ϊ�գ������в�Ϊ��
					tip(2,6);
					Send_buf=0;                                 //���û���
					while(!Send_queue.empty()) Send_queue.pop();//��ն���
					while(!Send_info.empty()) Send_info.pop();  //��ն���
				}
			}
		}
		
		//������Ƕ��㣬�������Ϸ�������
		if(!Top){
			if(Upper_send_stop) Upper_send_stop--;
			else{
				Upper_wait+=Recv_timeout;
				while(Upper_wait>=Upper_Send_timeout){
					Upper_wait-=Upper_Send_timeout;
					if(Recv_buf){
						if(Recv_queue.empty()||Recv_info.empty()){//�������󣬻��治Ϊ�գ�������Ϊ��
							tip(2,15);
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
						tip(2,15);
						Recv_buf=0;                                 //���û���
						while(!Recv_queue.empty()) Recv_queue.pop();//��ն���
						while(!Recv_info.empty()) Recv_info.pop();  //��ն���
					}
				}
			}
		}
		//����Ƕ��㣬ת�ƽ��ջ�������ݵ����ͻ�����
		else if(Recv_buf){
			Recv_tmp_data=Recv_queue.front();//��ȡ����������
			Recv_tmp_info=Recv_info.front(); //��ȡ������������Ϣ 
			
			while(Send_buf+Recv_tmp_info.buf_len<=MAX_BUFFER_SIZE){
				event_happen=1;
				
				Recv_buf-=Recv_tmp_info.buf_len;//�ͷſռ�
				Recv_queue.pop();//����
				Recv_info.pop(); //����
				
				Send_buf+=Recv_tmp_info.buf_len;//ռ�ÿռ�
				Send_queue.push(Recv_tmp_data);//���
				Send_info.push(Recv_tmp_info); //���
				
				if(Recv_buf){
					Recv_tmp_data=Recv_queue.front();//��ȡ����������
					Recv_tmp_info=Recv_info.front(); //��ȡ������������Ϣ 
				}
				else break;
			}
		}
		
		//��������������Ϣ
		if(!Top){
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
		//3.ͬ�����
			if(Lower_full>=100){//ͬ�㷢�ͳ�ʱ
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
			time_tmp=ceil((double)(Send_window_size-Send_window_stop_num)/4)*ceil((double)Recv_timeout/Lower_Send_timeout);
			if(Error_time>time_tmp)//����ش��������࣬���ٷ�֡����
				if(Divide_len>=110) Divide_len-=10;
			if(Lower_send_data&&++No_Error_cycle>=7){//�������7����û���ش������ӷ�֡����
				No_Error_cycle=0;
				if(Divide_len<=990){
					if(!Upper_recv_data) Divide_len+=10;
					else if(Divide_len<=tmp-10) Divide_len+=10;
				}
			}
			if(Send_window_busy_num<floor(Send_window_size*0.5)){//�������ռ���٣���ر�δʹ�õĴ��ڣ����ܴ�����������10��
				if(Send_window_size>10&&Send_window_use[Send_window_size-1]==0){
					event_happen=1;
					Send_window_size--;
					if(T_Print_mode){
						cprintf("*�����ձ���У��Ѽ��ٴ�������",3);
						cprintf("Ŀǰ������",3);cprintf(Send_window_size,14);cprintf("����",3);
						print_void(74-num_len(Send_window_size));printf("\n");
					}
				}
			}
			else if(Send_window_busy_num>=ceil(Send_window_size*0.5)+1){//��������ձ鷱æ������������
				if(Send_window_size<=99){
					event_happen=1;
					Send_window_size++;
					if(T_Print_mode){
						cprintf("*�����ձ鷱æ�������Ӵ�������",3);
						cprintf("Ŀǰ������",3);cprintf(Send_window_size,14);cprintf("����",3);
						print_void(74-num_len(Send_window_size));printf("\n");
					}
				}
			}
			if(Recv_buf_no_empty&&(Recv_buf==0)){//���ջ�������ת�գ��������������Ϣ
				char *control;
				try{control=new char[16];}catch(bad_alloc){tip(2,16);}
				set_control(control,-1,0);
				Special_window_frame=control;
				Send_Upper(-1);
			}
			if(Send_buf_no_empty&&(Send_buf==0)){//���ͻ�������ת�գ�ָʾ�´�ACK����������Ϣ
				ACK_add_clear=1;
			}
		}
		Error_time=0;              //�����ش�����
		receive_blocked=0;         //�������ڽ��������ж�ֵ
		Upper_send_data=0;         //�������Ϸ��������ж�ֵ
		Lower_send_data=0;         //�������·��������ж�ֵ
		Upper_recv_data=0;         //���ô��Ͻ��������ж�ֵ
		Lower_recv_data=0;         //���ô��½��������ж�ֵ
		Recv_buf_no_empty=Recv_buf;//���½��ջ���״̬�ж�ֵ
		Send_buf_no_empty=Send_buf;//���·��ͻ���״̬�ж�ֵ
		
		//��ӡͳ����Ϣ
		if(event_happen){
			print_data(Recv_timeout,Upper_Send_timeout,Lower_Send_timeout,R_Send_timeout,Divide_len,\
			           Upper_full,Lower_full,Send_buf,Recv_buf,Send_window_size,Send_window_busy_num,Send_window_stop_num,\
			           &Send_amount,&Send_time,&Recv_amount,&Recv_time,1,11);
			event_happen=0;
		}
	}
}