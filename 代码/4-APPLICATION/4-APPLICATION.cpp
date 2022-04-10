//4 APPLICATION Layer(Ӧ�ò�)
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<cmath>
#include<queue>
#include<winsock2.h>
#include<windows.h>
#include<conio.h>
#include"CfgFileParms.h"
#include"BasicFunction.h"
//#pragma comment(lib,"ws2_32.lib")
using namespace std;

CCfgFileParms Link_file;               //��Ԫ��Ϣ
const int MAX_BUFFER_SIZE=65000;       //����ռ�(bit)
int SOCKADDR_SIZE=sizeof(SOCKADDR_IN); //�׽��ֵ�ַ����(�ֽ�)

WSADATA     Local_data; //�����ʼ����Ϣ
SOCKET      Local_sock; //�����׽���
SOCKADDR_IN Local_addr; //�����׽��ֵ�ַ
SOCKADDR_IN Lower_addr; //�²��׽��ֵ�ַ(�����)
SOCKADDR_IN Cmd_addr;   //������׽��ֵ�ַ

int Sub_net=0;  //��������������
int Local_ip;   //������ʵIP
int Local_port; //������ʵ�˿�

string Destination_ip;     //Ŀ��IP��ַ (Ĭ��Ϊ�����㲥��ַ)
int Destination_ip_subnet; //Ŀ��IP������ (Ĭ��Sub_net)
int Destination_ip_port;   //Ŀ��IP����ʵ�˿� (Ĭ��65535)

int Source_ip_num=0;                  //��ԴIP������
int Source_ip_subnet[128];            //��ԴIP������
int Source_ip_port[128];              //��ԴIP����ʵ�˿�
bool Source_ip_seq[128][4194304];     //��ԴIP��֡���ͳ��
int Source_ip_max_seq[128];           //��ԴIP��֡��Ž���ֵ(���ֵ)
priority_queue<FRAME*>Recv_data[512]; //�յ�������

int Print_mode=0;   //�����շ���Ϣ��ӡģʽ��0Ϊ����ӡ�շ������ݣ�1Ϊ��ӡbit����2Ϊ��ӡ8���� (Ĭ��1)
int F_Print_mode=0; //����������Ϣ��ӡģʽ��0Ϊ����ӡ������Ϣ��1Ϊ��ӡ (Ĭ��0)
int T_Print_mode=0; //������ʾ��Ϣ��ӡģʽ��0Ϊֻ��ӡ��������ʱ����ʾ��Ϣ��1���Ӵ�ӡ�շ�������Ϣ��2���Ӵ�ӡ����������Ϣ (Ĭ��0)
int Lower_mode=1;   //�²�ӿ�ģʽ��0Ϊbit���飬1Ϊ�ֽ����� (Ĭ��1)
int Lower_number=1; //�²�ʵ������ (Ĭ��1)(���Ϊ8)

int Auto_full=25;          //�Զ�����ӵ��ָ�� (Ĭ��25)
int Lower_full=25;         //�²�ӵ��ָ�� (Ĭ��25)
int Recv_timeout=20;       //���ճ�ʱ(ms) (Ĭ��20)(��Ϊ�����������е�ʱ�䵥λ)(��Ҫ)
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

bool Auto_send=0;      //�Ƿ��Զ�����
int Auto_send_mode=0;  //�Զ�����ģʽ��0Ϊ���ӿ�����Ϣ��ʼ�����Զ��������ʷ��ͣ�1Ϊ���ݿ�����Ϣʵʱ�������ʣ����ܻ��ʵ�����
int Auto_send_time=20; //�Զ����ͳ�ʱ�����Զ�����ģʽΪ0ʱʹ��
int Auto_send_len=15;  //�Զ����ͳ��� (�ֽ���)(Ĭ��15)(���65000)
char *Auto_send_data;  //�Զ����͵����� (�ֽ���)(Ϊ01010101��ѭ��)

int Send_buf_high_thershold=0.4*MAX_BUFFER_SIZE; //���ͻ�������ж���ֵ
int Send_buf_low_thershold=0.25*MAX_BUFFER_SIZE; //���ͻ���Ͽ��ж���ֵ
int Auto_full_thershold=25;  //�Զ�����ӵ��ָ���ϴ��ж���ֵ
int Lower_full_thershold=25; //�²�ӵ��ָ���ϴ��ж���ֵ

int Auto_send_stop=0;     //�Զ�������ͣ������
int Lower_send_stop=0;    //���·�����ͣ������
bool Lower_send_data=0;   //�Ƿ����²㷢����������Ϣ
bool Auto_recv_data=0;    //�Ƿ��Զ�������������Ϣ
bool Lower_recv_data=0;   //�Ƿ���²������������Ϣ
bool Send_buf_no_empty=0; //�ϸ����ڷ��ͻ����Ƿ񲻿�

int Recv_data_seq;           //���յ����������֡��� (����Ϊ131072)
char Recv_data_time[131072]; //���յ�������֡��Ŵ���ͳ��

const int HEAD_SIZE=0; //��֤�������ͨ�������ñ������� 

void shutdown(){//�˳�
	WSACleanup();
	if(Tmp_buf) delete [] Tmp_buf;
	if(Special_window_frame) delete [] Special_window_frame;
	if(Send_tmp_data) delete [] Send_tmp_data;
	if(Auto_send_data) delete [] Auto_send_data;
}
void tip(int type,int num){//��ʾ
	if(type==0&&num==0){
		cprintf("#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#",46);
		cprintf("        Ӧ        ��        ��        ",46);
		cprintf("#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#\n",46);
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
				case 10:{cprintf("*�ѿ����Զ����ͣ�",3);
						print_void(102);printf("\n");break;}
				case 11:{cprintf("*�ѹر��Զ����ͣ�",3);
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
			s2="APPLICATION";
			s3=argv[3];
			break;
		}
		default:tip(1,1);
	}
	CCfgFileParms File(s1,s2,s3);
	Link_file=File;
	Link_file.getValueInt(Sub_net,(char*)"Sub_net");//������
	Link_file.getValueInt(Print_mode,(char*)"Print_mode");    //�շ���Ϣ��ӡģʽ
	Link_file.getValueInt(T_Print_mode,(char*)"T_Print_mode");//��ʾ��Ϣ��ӡģʽ
	Link_file.getValueInt(Recv_timeout,(char*)"Recv_timeout");//���ճ�ʱ
	Link_file.getValueInt(Lower_Send_timeout,(char*)"Lower_Send_timeout");//�²㷢�ͳ�ʱ
	Link_file.getValueInt(Auto_send_time,(char*)"Auto_send_time");//�Զ����ͳ�ʱ
	Link_file.getValueInt(Auto_send_mode,(char*)"Auto_send_mode");//�Զ�����ģʽ
	Link_file.getValueInt(Auto_send_len,(char*)"Auto_send_len");  //�Զ����ͳ���
	string Destination_ip=Link_file.getValueStr("Destination_ip");//Ŀ�ĵ�ַ
	int num_tmp[2]={Sub_net,65535};
	if(Destination_ip.length()>0) Many_String2Num(Destination_ip,num_tmp,2);
	Destination_ip_subnet=num_tmp[0];
	Destination_ip_port=num_tmp[1];
	//��ȡ����IP��˿���Ϣ
	string info=Link_file.getUDPAddrString(CCfgFileParms::LOCAL,0);//����
	String2Num(&Local_ip,&Local_port,info);
	tip(0,1);
	
	//����
	Tmp_buf=new char[MAX_BUFFER_SIZE];//��ʱ����
	Auto_send_data=new char[MAX_BUFFER_SIZE];//�Զ����͵�����
	if(Auto_send_data) memset(Auto_send_data,85,MAX_BUFFER_SIZE);
	tip((!Tmp_buf)||(!Auto_send_data),2);
	
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
		//�²��׽���
		Lower_addr=Link_file.getUDPAddr(CCfgFileParms::LOWER,0);//�Զ�����
		Link_file.getValueInt(Lower_mode,(char*)"Lower_mode");//�ӿ�ģʽ
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
	Link_file.print(10);
	cprintf("���س��� ����/�ر� �Զ����ͣ���\"`\"������Ŀ��IP��ַ\n",10);
}
bool check_end_frame(char* data){//�ж�һ��֡�Ƿ�Ϊ����֡
	//����ֵ��1Ϊ�ǣ�0Ϊ��
	
	
	return 0;
}
void Send_Lower(int i){//���²㷢�����ݣ�����Ϊ��������
	//ע�⣺���ں�Ϊ-1ʱ����ʹ�����ⴰ��(ר�����շ�������Ϣ)��Ϊ1ʱ�����Զ�����
	char *Send_data;
	int buf_len;
	
	switch(i){
		case -1:{//������Ϣ
			Send_data=Special_window_frame;
			  buf_len=16;
			  break;
		}
		case 1:{//�Զ�����
			Send_data=Auto_send_data;
			  buf_len=Auto_send_len*8;
			  break;
		}
		default:{//��ͨ����
			Send_data=Send_tmp_data;
			  buf_len=Send_tmp_info.buf_len;
			  break;
		}
	}
	
	int Send_len,len=buf_len/8+(buf_len>0&&buf_len%8!=0);
	char byte[len+3];
	if(i!=1){//���Զ����� 
		//�Ƚ�����֡תΪ�ֽ��ͣ��ٷ���
		Bit2Byte(Send_data,buf_len,byte+3,len);
		set_destination_ip_info(Destination_ip_subnet,Destination_ip_port,byte);
		len+=3;
		Send_len=sendto(Local_sock,byte,len,0,(SOCKADDR*)&Lower_addr,SOCKADDR_SIZE);
	}
	else{//�Զ����� 
		len=Auto_send_len+3;
		set_destination_ip_info(Destination_ip_subnet,Destination_ip_port,Auto_send_data);
		Send_len=sendto(Local_sock,Auto_send_data,len,0,(SOCKADDR*)&Lower_addr,SOCKADDR_SIZE);
	}
//	cout<<Destination_ip_subnet<<" * "<<Destination_ip_port<<endl;
	
	if(Send_len<=0){//����ʧ�ܣ���׼���ش�
		Send_amount.Error+=buf_len; Send_time.Error++;
		if(i!=-1&&i!=1){//ֻ�ش�����֡�����ش�����֡���Զ����͵�֡
			Send_buf+=buf_len;             //���ӻ��� 
			Send_queue.push(Send_tmp_data);//���֡������ĩ
			Send_info.push(Send_tmp_info); //���֡��Ϣ������ĩ
		}
		
		if(Print_mode){//�����Ϣ
			if(i==-1){
				if(Print_mode==1) print_bit(Send_data,buf_len,"���²㷢�Ϳ�����Ϣʧ��...��bit����",0,10);
				else print_byte(byte,len,"���²㷢�Ϳ�����Ϣʧ��...���ֽ�����",1,10);
			}
			else{
				if(Print_mode==1){
					if(i!=1) print_bit(Send_data,buf_len,"���²㷢������ʧ��...����bit����",0,10);
					else print_bit(Send_data,len,"���²㷢������ʧ��...����bit����",1,10);
				}
				else{
					if(i!=1) print_byte(byte,len,"���²㷢������ʧ��...�����ֽ�����",1,10);
					else print_byte(Send_data,len,"���²㷢������ʧ��...�����ֽ�����",1,10);
				}
			}
		}
		if(i==-1) delete[] Special_window_frame;
	}
	else{//���ͳɹ�
		Send_amount.Total+=buf_len; Send_time.Total++;
		Send_amount.Lower+=buf_len; Send_time.Lower++;
		switch(i){
			case -1: {Send_amount.LCtrl+=buf_len; Send_time.LCtrl++;break;}//������Ϣ
			case  1: {Send_amount. Auto+=buf_len; Send_time. Auto++; Lower_send_data=1;break;}//�Զ�����
			default: {Send_amount.LData+=buf_len; Send_time.LData++; Lower_send_data=1;break;}//��ͨ����
		}
		
		if(Print_mode){//�����Ϣ
			if(i==-1){
				if(Print_mode==1) print_bit(Send_data,buf_len,"���²㷢�Ϳ�����Ϣ�ɹ�����bit����",0,10);
				else print_byte(byte,len,"���²㷢�Ϳ�����Ϣ�ɹ������ֽ�����",1,10);
			}
			else{
				if(Print_mode==1){
					if(i!=1) print_bit(Send_data,buf_len,"���²㷢�����ݳɹ�������bit����",0,10);
					else print_bit(Send_data,len,"���²㷢�����ݳɹ�������bit����",1,10);
				}
				else{
					if(i!=1) print_byte(byte,len,"���²㷢�����ݳɹ��������ֽ�����",1,10);
					else print_byte(Send_data,len,"���²㷢�����ݳɹ��������ֽ�����",1,10);
				}
			}
		}
		if(i==-1) delete[] Special_window_frame;
		else if(i!=1) delete[] Send_tmp_data;
	}
}
int Send_Cmd(char *buf,int len){//�����㷢�����ݣ����ط��ͳ���
	return sendto(Local_sock,buf,len,0,(SOCKADDR*)&Cmd_addr,SOCKADDR_SIZE);
}
void Recv_Lower(int len){//���²��յ�
	if(len==2&&(unsigned char)*Tmp_buf==0xff){//����ǿ�����Ϣ�������ͬ��ӵ��ָ��
		Recv_amount.Total+=16; Recv_time.Total++;
		Recv_amount.Lower+=16; Recv_time.Lower++;
		Recv_amount.LCtrl+=16; Recv_time.LCtrl++;
		if(!(Auto_send&&(Auto_send_mode==0))){//������ǲ��� ģʽ0���Զ����� ���з��ͣ������
			char n=*(Tmp_buf+1);
			if(n==0) Lower_full=0;
			else if(n>0) Lower_full+=n;
		}
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
		
		int ip_subnet=0,ip_port=0,seq=0;
		get_source_ip_info(&ip_subnet,&ip_port,&seq,Tmp_buf,len);
		char *data=Tmp_buf+6;
		
		
		//����Ϊ�ļ�����������֣�����ʱ�����ޣ���δ��ɣ���˼·�Ѿ���Ϊ����
		if(seq==-1){//�ǹ㲥
			
		}
		else{
			if(seq==0){//����ʼ֡
				//������У��ж��ļ����ͣ���¼�ļ�����
				
			}
			else{
				if(check_end_frame(data)){//�ǽ���֡
					//���ñ�־��������Ų���������ÿ���յ�֡�ж��Ƿ�Ҫ����������ļ�
					
				}
				else{//������֡
					//Ѱ�Ҷ�Ӧ���У������
					
				}
			}
		}
		
/*		char *tmp_frame;
		try{tmp_frame=new char[8*len];}catch(bad_alloc){tip(2,9);return;}
		Byte2Bit(tmp_frame,8*len,Tmp_buf,len);//��ת��Ϊbit�ͣ������
		
		Recv_buf+=8*len;           //������ռ�ô�С
		Recv_queue.push(tmp_frame);//��������ն���
		
		INFO tmp_info(0,len,8*len,0,0);
		Recv_info.push(tmp_info);  //��ӽ�����Ϣ������
*/	}
}
int main(int argc,char *argv[]){
	//��ʼ��
	tip(0,0);
	initial(argc,argv);
	
	//ѭ�����ա���������
	SOCKADDR_IN Remote_addr;                  //���ڴ洢���յ����׽���
	register bool event_happen=1;             //�Ƿ����¼�����(�����ж��Ƿ��ӡ������Ϣ)
	register int tmp;                         //�洢���׽��ֽ��յ������ݳ���
	register unsigned short Auto_send_wait=0; //�Զ����͵ĵȴ�ʱ��
	register unsigned short Auto_recv_wait=0; //�Զ����յĵȴ�ʱ��
	register unsigned short Lower_wait=0;     //���²㷢�͵ĵȴ�ʱ��
	register unsigned long long cycle=0;      //ѭ��������
	while(++cycle){
//		printf(" $%d$ ",cycle);
		//���������룬����/�ر��Զ�����
		if(kbhit()){
			char tmp=getch();
			if(tmp==13){
				event_happen=1;
				Auto_send=!Auto_send;
				if(Auto_send==0) tip(2,11);
				else tip(2,10);
			}
			if(tmp=='`'){
				event_happen=1;
				cprintf("�������µ�Ŀ��IP��ַ��",10);
				HANDLE gh_std_out=GetStdHandle(STD_OUTPUT_HANDLE);
				CONSOLE_SCREEN_BUFFER_INFO bInfo;
				GetConsoleScreenBufferInfo(gh_std_out,&bInfo);//��ȡ���λ�� 
				print_void(97);printf("\n");
				goto_xy(bInfo.dwCursorPosition.X,bInfo.dwCursorPosition.Y);//��귵�� 
				cin>>Destination_ip;
				String2Num(&Destination_ip_subnet,&Destination_ip_port,Destination_ip);
//				cout<<Destination_ip<<"  "<<Destination_ip_subnet<<"  "<<Destination_ip_port;
			}
		}
		
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
				if(compare_port(Remote_addr,Lower_addr)){//�²�
					unknown=0;
					Recv_Lower(tmp);
				}
				if(unknown){//��Դδ֪
					Recv_amount.Unknown+=tmp; Recv_time.Unknown++;
				}
			}
		}
		
		//�������·�������
		if(Auto_send){//����������Զ����ͣ�������Զ����Ͳ���
			Auto_send_wait+=Recv_timeout;
			if(Auto_send_mode==0){//ģʽ0���ٶȺ㶨
				while(Auto_send_wait>=Auto_send_time){
					event_happen=1;
					Auto_send_wait-=Auto_send_time;
					Send_Lower(1);
				}
			}
			else{//ģʽ1���ٶȶ�̬����
				Auto_recv_wait+=Recv_timeout;
				if(Auto_send_stop) Auto_send_stop--;
				else{
					while(Auto_recv_wait>=Auto_send_time){
						event_happen=1;
						Auto_recv_data=1;
						Auto_recv_wait-=Auto_send_time;
						Send_buf+=8*Auto_send_len;
						if(Send_buf>Send_buf_high_thershold){
							char control[16],byte[2];
							set_control(control,Send_buf,Send_buf_high_thershold);
							Bit2Byte(control,16,byte,2);
							if(byte[1]==0) Auto_full=0;
							else if(byte[1]>0) Auto_full+=byte[1];
						}
					}
				}
				while(Auto_send_wait>=Lower_Send_timeout&&Send_buf>0){
					event_happen=1;
					Auto_send_wait-=Lower_Send_timeout;
					Send_buf-=8*Auto_send_len;
					Send_Lower(1);
				}
			}
		}
		else{//���ļ����룬Ȼ����
			if(Lower_send_stop) Lower_send_stop--;
			else{
				Lower_wait+=Recv_timeout;
				while(Lower_wait>=Lower_Send_timeout){
					Lower_wait-=Lower_Send_timeout;
					if(Send_buf){
						if(Send_queue.empty()||Send_info.empty()){//�������󣬻��治Ϊ�գ�������Ϊ��
/*							tip(2,5);
							Send_buf=0;                                 //���û���
							while(!Send_queue.empty()) Send_queue.pop();//��ն���
							while(!Send_info.empty()) Send_info.pop();  //��ն���
*/						}
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
/*					else if((!Send_queue.empty())||(!Send_info.empty())){//�������󣬻���Ϊ�գ������в�Ϊ��
						tip(2,5);
						Send_buf=0;                                 //���û���
						while(!Send_queue.empty()) Send_queue.pop();//��ն���
						while(!Send_info.empty()) Send_info.pop();  //��ն���
					}*/
				}
			}
		}
		
		//��������������Ϣ
		if(!(Auto_send&&(Auto_send_mode==0))){//������ǲ��� ģʽ0���Զ����� ���з��ͣ������
			//1.ӵ��ָ��
			if(Lower_send_data&&(Send_buf>Send_buf_low_thershold)&&(Send_buf<=Send_buf_high_thershold)){//�²�ӵ��ָ��
				event_happen=1;
				Lower_full--;
			}
			if(Auto_recv_data&&(Send_buf>Send_buf_high_thershold)){//�²�ӵ��ָ��
				event_happen=1;
				Lower_full--;
			}
			if(Auto_send&&Send_buf<Send_buf_high_thershold){//�Զ�����ӵ��ָ��
				event_happen=1;
				Auto_full--;
			}
			//2.�Զ��������
			if(Auto_full>=100){//�Զ����ͳ�ʱ
				event_happen=1;
				double k=log(100-Auto_full_thershold)/log(2);
				int time=ceil((double)Recv_timeout*0.05*k);
				if(Auto_send_time+time<=1000) Auto_send_time+=time;
				Auto_full=3;
				Auto_send_stop=3;
			}
			else if(Auto_full>Auto_full_thershold){
				event_happen=1;
				double k=log(Auto_full-Auto_full_thershold)/log(2);
				int time=ceil((double)Recv_timeout*0.05*k);
				if(Auto_send_time+time<=1000) Auto_send_time+=time;
				Auto_full=3;
			}
			else if(Auto_full<=0){
				event_happen=1;
				Auto_full=Auto_full_thershold*0.5;
				int time=ceil((double)Auto_send_time*0.1);
				if(Auto_send_time-time>=5) Auto_send_time-=time;
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
			if(Send_buf_no_empty&&(Send_buf==0)){//���ͻ�������ת�գ��������������Ϣ
				for(int i=0;i<=Lower_number-1;++i){
					char *control;
					try{control=new char[16];}catch(bad_alloc){tip(2,16);}
					set_control(control,-1,0);
					Special_window_frame=control;
					Send_Lower(-1);
				}
			}
			Auto_recv_data=0;          //�����Զ����������ж�ֵ
			Lower_send_data=0;         //�������·��������ж�ֵ
			Lower_recv_data=0;         //���ô��½��������ж�ֵ
			Send_buf_no_empty=Send_buf;//���·��ͻ���״̬�ж�ֵ
		}
		
		//��ӡͳ����Ϣ
		if(event_happen){//ע��˴���Auto_send_timeд����Send_timeout��λ�ã���Auto_fullд����Upper_full��λ��
			print_data(Recv_timeout,Auto_send_time,Lower_Send_timeout,0,0,\
			           Auto_full,Lower_full,Send_buf,Recv_buf,0,0,0,\
			           &Send_amount,&Send_time,&Recv_amount,&Recv_time,3,10);
			event_happen=0;
		}
	}
}
