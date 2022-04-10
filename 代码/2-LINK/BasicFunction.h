//basic function of all layers
#ifndef BASIC_FUNCTION
#define BASIC_FUNCTION

#include<cstdio>
#include<iostream>
#include<cstring>
#include<winsock2.h>
#include<windows.h>
//#pragma comment (lib,"wsock32.lib")
using namespace std;

struct DATA{//ͳ����Ϣ
	unsigned int Total,Error,Unknown;                    //������Total=���гɹ���Error=����ʧ�ܣ�Unknown=������Դ����
	unsigned int Upper,UData,UCtrl;                      //�ϲ㣬Upper=���У�Data=����֡��Ctrl=����֡
	unsigned int Lower,LData,LCtrl,Re,Wrong,ACK,NAK,Auto;//�²㣬Lower=���У�Data=����֡��Ctrl=����֡��Re=�ش���Wrong=��֡��Auto=�Զ�
	DATA();
};
void print_data(int Recv_timeout,int Upper_Send_timeout,int Lower_Send_timeout,int R_Send_timeout,int Divide_len,\
                int Upper_full,int Lower_full,int Send_buf,int Recv_buf,int Send_window_size,int Send_window_busy_num,int Send_window_stop_num,\
                DATA *Send_amount,DATA *Send_time,DATA *Recv_amount,DATA *Recv_time,int layer,int color);//��ӡͳ����Ϣ

struct HEAD{//ͷ��
	char type[4];        //��1λΪ�������Ϣ ACK NAK��ʶ | ��3λΪ��֡���
	char source[6];      //ǰ4λΪip | ��2λΪ�˿�
	char destination[6]; //ǰ4λΪip | ��2λΪ�˿�
	HEAD();                                                       //ͷ�����캯��
	HEAD(char type,char *sequence,char *source,char *destination);//ͷ�����캯��
	void change_type(bool ack,bool nak,int layer);//�޸�type
	void change_sequence(int sequence);           //�޸�sequence
	void change_source(int ip,int port);          //�޸�source
	void change_destination(int ip,int port);     //�޸�destination
	bool get_ack();            //��ȡACK��ʶ
	bool get_nak();            //��ȡNAK��ʶ
	int get_layer();           //��ȡ�������
	int get_sequence();        //��ȡ֡���
	int get_source_ip();       //��ȡ���ͷ�IP
	int get_source_port();     //��ȡ���ͷ��˿�
	int get_destination_ip();  //��ȡ���շ�IP
	int get_destination_port();//��ȡ���շ��˿�
};
extern HEAD NULL_HEAD;//�սṹ�壬����������;
void change_head(HEAD *h,int mode,int layer,int seq,string sour,string dest);//�޸�ͷ��
void change_head(HEAD *h,int mode,int layer,int seq,int s_ip,int s_port,int d_ip,int d_port);//�޸�ͷ��
void change_frame(char *object,char *source,int len);//ת��֡����

struct INFO{//�շ�֡��Ϣ
	int sequence;     //֡�����
	int len;          //֡��ʵ�ʳ���(bit/�ֽ�)
	int buf_len;      //֡�ڻ����еĳ���(�ֽ�)
	int Lower_number; //�²�ʵ���
	int Mac_number;   //�²�mac���
	int lable;        //Ϊ0�����֡Ϊ����֡��Ϊ1�����֡ΪACK��Ϊ2�����֡ΪNAK��Ϊ3�����֡Ϊ����֡
	int resend;       //�ش�����
	INFO();                                                                          //�շ�֡��Ϣ���캯��
	INFO(int sequence,int len,int buf_len,int Lower_number,int Mac_number,int lable);//�շ�֡��Ϣ���캯��
};
extern INFO NULL_INFO;//�սṹ�壬����������;

struct FRAME{//�յ���֡������Ϣ
	int sequence;
	char *data;
	FRAME();
	FRAME(int sequence,char *data);
	bool operator > (const FRAME &b) const{
        return sequence<b.sequence;
    }
};

extern const int MAX_BUFFER_SIZE;//����ռ�(bit)
extern const int HEAD_SIZE;      //ͷ������(�ֽ�)
extern int F_Print_mode;         //֡������Ϣ��ӡģʽ

int num_len(int num);               //�����ֳ��� 
void goto_xy(int x,int y);          //��λ���λ�õ�ָ������
void print_void(int num);           //��ӡ�ո� 
void cprintf(string str,WORD color);//��ɫ����
void cprintf(int num,WORD color);   //��ɫ����
void cprintf_16(int num,WORD color);//��ɫ����
void cprintf_data(string str,int amount,int time,bool last,int width,int voidth,WORD color);//��ɫ���� 

bool compare_addr(SOCKADDR_IN a,SOCKADDR_IN b);//�Ƚϵ�ַ
bool compare_port(SOCKADDR_IN a,SOCKADDR_IN b);//�Ƚ϶˿�
bool compare_sock(SOCKADDR_IN a,SOCKADDR_IN b);//�Ƚ��׽���

void code(unsigned long x,char A[],int length);//����
unsigned long decode(char A[],int length);     //����

void Many_String2Num(string s,int *n,int num);//�����string�͵�����תΪint
void String2Num(int *ip,int *port,string addr);//��string�͵ĵ�ַתΪ������

int Byte2Bit(char* bit,int bitlength,char* byte,int bytelength);//���ֽ�����תΪbit����
int Bit2Byte(char* bit,int bitlength,char* byte,int bytelength);//��bit����תΪ�ֽ�����

void print_bit(char* A,int length,string word,int mode,int color); //��bit��ʽ��ӡ���ݻ���������
void print_byte(char* A,int length,string word,int mode,int color);//��16�����ֽ���ʽ��ӡ���ݻ���������

bool IsPowerOfTwo(int n);//�ж�һ�����Ƿ�Ϊ2����
int hamming_length(int len);//���㺣�������볤��
bool hamming_check(char *buf,int len,bool mode);//��ȷ���д��������£�ʹ�ú��������
int hamming(char *buf,char *h,int len,bool mode);//������ƴ�Ӳ����к�������
int dehamming(char *buf,int len,bool mode);//��������������ݽ���

unsigned int generate_check_sum(char *buf,int len);//����У���(δ�ֶε����ʼֵ)
int add_check_sum(char *buf,int len,bool mode);//������������һ�� 16bit / 2�ֽ� ��У���
bool check_check_sum(char *buf,int len,char *check_sum,bool mode);//������ݵ�У���

int frame_bit(char *buf,char *h,int len);//��bit��֡ 
int frame_byte(char *buf,char *h,int len);//���ֽڳ�֡
int frame(char *buf,int len,HEAD *h,bool input_mode,bool output_mode);//��֡
int deframe_bit(char *buf,char *h,int len);//��bit�ָ�
int deframe_byte(char *buf,char *h,int len);//���ֽڻָ�
int deframe(char *buf,int len,HEAD *h,bool input_mode,bool output_mode);//�ָ�

void set_control(char *control,int buf,int thershold);//����������Ϣ

void set_source_ip_info(int ip_subnet,int ip_port,int seq,char *buf);//�������ϲ㷢�͵����ݵ�Դ��Ϣ (�������IP��ַ)
void set_destination_ip_info(int ip_subnet,int ip_port,char *buf);//�������²㷢�͵����ݵ�Ŀ����Ϣ (�������IP��ַ)
bool get_source_ip_info(int *ip_subnet,int *ip_port,int *seq,char *buf,int len);//��ȡ���²���յ����ݵ�Դ��Ϣ (�������IP��ַ)
bool get_destination_ip_info(int *ip_subnet,int *ip_port,char *buf,int len);//��ȡ���ϲ���յ����ݵ�Ŀ����Ϣ (�������IP��ַ)

void set_source_mac_info(int mac_ip,int mac_port,int seq,char *buf);//�������ϲ㷢�͵����ݵ�Դ��Ϣ (�������MAC��ַ)
void set_destination_mac_info(int mac_ip,int mac_port,char *buf);//�������²㷢�͵����ݵ�Ŀ����Ϣ (�������MAC��ַ)
bool get_source_mac_info(int *mac_ip,int *mac_port,int *seq,char *buf,int len);//��ȡ���²���յ����ݵ�Դ��Ϣ (�������MAC��ַ)
bool get_destination_mac_info(int *mac_ip,int *mac_port,char *buf,int len);//��ȡ���ϲ���յ����ݵ�Ŀ����Ϣ (�������MAC��ַ)

void print_ip(int ip,bool pvoid,int color);//��ӡip
void print_port(int port,bool pvoid,int color);//��ӡ�˿�

#endif