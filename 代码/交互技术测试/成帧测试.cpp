//��֡����
#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<windows.h>
using namespace std;

void String2Num(int *ip,int *port,string addr);//��string�͵ĵ�ַתΪ������
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
HEAD::HEAD(){memset(this,0,sizeof(HEAD));}//ͷ�����캯��
HEAD::HEAD(char type,char *sequence,char *source,char *destination){//ͷ�����캯��
	*(this->type)=type;
	for(int i=1;i<=3;++i) *(this->type+i)=*(sequence+i-1);
	for(int i=0;i<=3;++i) *(this->source+i)=*(source+i);
	for(int i=0;i<=3;++i) *(this->destination+i)=*(destination+i);
}
void HEAD::change_type(bool ack,bool nak,int layer){//�޸�type
	memset(&(this->type),0,1);
	if(ack) *(this->type)|=0b10000000;
	if(nak) *(this->type)|=0b01000000;
	switch(layer){
		case 0:{*(this->type)|=0b00000000;break;}//�����
		case 1:{*(this->type)|=0b00000001;break;}//��·��
		case 2:{*(this->type)|=0b00000010;break;}//�����
		case 3:{*(this->type)|=0b00000011;break;}//Ӧ�ò�
	}
}
void HEAD::change_sequence(int sequence){//�޸�sequence
	char *tmp=(char*)&sequence;
	for(int i=1;i<=3;++i)
		*(this->type+i)=*(tmp+i-1);
}
void HEAD::change_source(int ip,int port){//�޸�source
	char *tmp;
	for(int i=0;i<=3;++i){
		tmp=(char*)&ip+i;
		source[i]=*tmp;
	}
	for(int i=4;i<=5;++i){
		tmp=(char*)&port+i-4;
		source[i]=*tmp;
	}
}
void HEAD::change_destination(int ip,int port){//�޸�destination
	char *tmp;
	for(int i=0;i<=3;++i){
		tmp=(char*)&ip+i;
		destination[i]=*tmp;
	}
	for(int i=4;i<=5;++i){
		tmp=(char*)&port+i-4;
		destination[i]=*tmp;
	}
}
bool HEAD::get_ack(){//��ȡACK��ʶ
	return *(this->type)&0b10000000;
}
bool HEAD::get_nak(){//��ȡNAK��ʶ
	return *(this->type)&0b01000000;
}
int HEAD::get_layer(){//��ȡ�������
	return *(this->type)&0b00000011;
}
int HEAD::get_sequence(){//��ȡ֡���
	int sequence=0;
	char *tmp=(char*)&sequence;
	for(int i=1;i<=3;++i)
		*(tmp+i-1)=*(this->type+i);
	return sequence;
}
int HEAD::get_source_ip(){//��ȡ���ͷ�IP
	//����ֵΪһ��int����������ռ��4���ֽڣ�ÿ���ֽڴ���IP��һ������
	int *ip=(int*)&source;
	return *ip;
}
int HEAD::get_source_port(){//��ȡ���ͷ��˿�
	int port=0;
	char *tmp=(char*)&port;
	for(int i=4;i<=5;++i)
		*(tmp+i-4)=*(this->source+i);
	return port;
}
int HEAD::get_destination_ip(){//��ȡ���շ�IP
	//����ֵΪint����ռ��4���ֽڣ�ÿ���ֽڴ���IP��һ������
	int *ip=(int*)&destination;
	return *ip;
}
int HEAD::get_destination_port(){//��ȡ���ͷ��˿�
	int port=0;
	char *tmp=(char*)&port;
	for(int i=4;i<=5;++i)
		*(tmp+i-4)=*(this->destination+i);
	return port;
}
void change_head(HEAD *h,int mode,int layer,int seq,string sour,string dest){
	//mode����ģʽ��0Ϊ���ͣ�1Ϊ����ACK��2Ϊ����NAK
	switch(mode){
		case 0:{h->change_type(0,0,layer);break;}
		case 1:{h->change_type(1,0,layer);break;}
		case 2:{h->change_type(0,1,layer);break;}
	}
	h->change_sequence(seq);
	
	int s_ip=0,s_port=0;
	int d_ip=0,d_port=0;
	
	String2Num(&s_ip,&s_port,sour);
	h->change_source(s_ip,s_port);
	
	String2Num(&d_ip,&d_port,dest);
	h->change_destination(d_ip,d_port);
}
void change_head(HEAD *h,int mode,int layer,int seq,int s_ip,int s_port,int d_ip,int d_port){//�޸�ͷ��
	//mode����ģʽ��0Ϊ���ͣ�1Ϊ����ACK��2Ϊ����NAK
	switch(mode){
		case 0:{h->change_type(0,0,layer);break;}
		case 1:{h->change_type(1,0,layer);break;}
		case 2:{h->change_type(0,1,layer);break;}
	}
	h->change_sequence(seq);
	h->change_source(s_ip,s_port);
	h->change_destination(d_ip,d_port);
}

const int MAX_BUFFER_SIZE=65000; //����ռ�
const int HEAD_SIZE=sizeof(HEAD);//ͷ������
int F_Print_mode=1;              //����֡������Ϣ��ӡģʽ��0Ϊ����ӡ֡�Ĵ�����Ϣ��1Ϊ��ӡ  

int num_len(int num){//�����ֳ��� 
	int k=0;
	if(num==0) k=1;
	while(num!=0){
		num/=10;
		k++;
	}
	return k; 
}
void print_void(int num){//��ӡ�ո� 
	for(int i=1;i<=num;++i) printf(" ");
}
void cprintf(string str,WORD color){//��ɫ���� 
	WORD colorOld;
	HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(handle, &csbi);
	colorOld = csbi.wAttributes;
	SetConsoleTextAttribute(handle, color);
	cout << str;
	SetConsoleTextAttribute(handle, colorOld);
}
void cprintf(int num,WORD color){//��ɫ���� 
	WORD colorOld;
	HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(handle, &csbi);
	colorOld = csbi.wAttributes;
	SetConsoleTextAttribute(handle, color);
	cout << num;
	SetConsoleTextAttribute(handle, colorOld);
}
void cprintf_16(int num,WORD color){//��ɫ���� 
	WORD colorOld;
	HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(handle, &csbi);
	colorOld = csbi.wAttributes;
	SetConsoleTextAttribute(handle, color);
	printf("%02x",num);
	SetConsoleTextAttribute(handle, colorOld);
}

void code(unsigned long x,char A[],int length){//������x��length���ȵ�λ��������A��
	unsigned long test=1<<(length-1);//��λ��ǰ
	for(int i=0;i<=length-1;++i){//���㷨��������λ������"��"���㣬��λ���x��ÿһλ��0����1
		if(test&x) A[i]=1;
		else A[i]=0;
		test>>=1; 
	}
}
unsigned long decode(char A[],int length){//��A����Ϊ����length������x
	unsigned long x=0;
	for(int i=0;i<length;++i){
		x<<=1;
		if(A[i]) x|=1;
	}
	return x;
}
void String2Num(int *ip,int *port,string addr){//��string�͵ĵ�ַתΪ������
	int length=addr.length();
	
	char s[30],*tmp=NULL;
	int ip_num[4];
	
	char num_tmp[5][10];
	int num_cnt=0,cnt=0;
	
	memset(s,0,sizeof(s));
	memset(ip_num,0,sizeof(ip_num));
	memset(num_tmp,0,sizeof(num_tmp));
	
	strcpy(s,addr.c_str());//stringתchar
	for(int i=0;i<=length-1;++i){
		if(s[i]=='.'||s[i]==':'){
			ip_num[num_cnt]=atoi(num_tmp[num_cnt]);
			num_cnt++;
			cnt=0;
		}
		else num_tmp[num_cnt][cnt++]=s[i];
	}
	for(int i=0;i<=3;++i){
		tmp=(char*)ip+i;
		*tmp=(char)ip_num[i];
	}
	*port=atoi(num_tmp[num_cnt]);
	return;
}
int Byte2Bit(char* bit,int bitlength,char* byte,int bytelength){//���ֽ�����תΪbit���飬*bitΪ���������ֵΪ���С
	int length=min(bytelength,bitlength/8);
	for(int i=0;i<=length-1;++i)//ÿ�α���8λ
		code(byte[i],&(bit[i*8]),8);
	return length*8;
}
int Bit2Byte(char* bit,int bitlength,char* byte,int bytelength){//��bit����תΪ�ֽ�����
	//*byteΪ���������ֵΪ���С
	
	int length=min(bytelength*8,bitlength);
	int retlength;
	if(bitlength>bytelength*8)//�ض�ת��
		retlength=bytelength;
	else if(bitlength%8!=0)   //bit����С
		retlength=bitlength/8+1;
	else                      //ǡ�����
		retlength=bitlength/8;
	
	int cnt=0;
	memset(byte,0,retlength);
	for(int i=0;i<=length-1;++i){
		if(i%8==0&&i!=0) cnt++;
		if(*(bit+i)){
			int mod=i%8;
			switch(mod){
				case 0:{byte[cnt]|=0b10000000;break;}
				case 1:{byte[cnt]|=0b01000000;break;}
				case 2:{byte[cnt]|=0b00100000;break;}
				case 3:{byte[cnt]|=0b00010000;break;}
				case 4:{byte[cnt]|=0b00001000;break;}
				case 5:{byte[cnt]|=0b00000100;break;}
				case 6:{byte[cnt]|=0b00000010;break;}
				case 7:{byte[cnt]|=0b00000001;break;}
			}
		}
	}
	return retlength;
}
void print_bit(char* A,int length,string word,int mode,int color){//��bit��ʽ��ӡ���ݻ���������
	//mode:ԭʼ���ݸ�ʽ��0Ϊbit���飬1Ϊ�ֽ�����
	int LineCount=0;
	
	if(color>8) cprintf(word,16*color-114);
	else if(color<8) cprintf(word,16*color+128);
	else cprintf(word,142);
	if(mode==0){
		printf("(���� %d bit)",length);
		print_void(108-word.length()-num_len(length));
	}
	else{
		printf("(���� %d bit)",length*8);
		print_void(108-word.length()-num_len(length*8));
	}
	printf("\n");
	
	if(mode==0){
		for(int i=0;i<=length-1;++i){
			if(LineCount>=64){
				LineCount=0;
				printf("\n");
			}
			if(LineCount%8==0){
				printf("   ");
				if(LineCount){
					cprintf(" ",255);
					printf("   ");
				}
			}
			cprintf(A[i],color);
			LineCount++;
		}
	}
	else{
		char B[8];
		for(int i=0;i<=length-1;++i){
			code(A[i],B,8);
			if(LineCount>=64){
				LineCount=0;
				printf("\n");
			}
			if(LineCount%8==0){
				printf("   ");
				if(LineCount){
					cprintf(" ",255);
					printf("   ");
				}
			}
			for(int j=0;j<=7;++j){
				cprintf(B[j],color);
				LineCount++;
			}
		}
	}
	print_void(116-15*(LineCount/8)-LineCount%8);
	printf("\n");
}
void print_byte(char* A,int length,string word,int mode,int color){//��16�����ֽ���ʽ��ӡ���ݻ���������
	//mode:ԭʼ���ݸ�ʽ��0Ϊbit���飬1Ϊ�ֽ�����
	int LineCount=0;
	if(mode==0) length=Bit2Byte(A,length,A,length);
	
	if(color>8) cprintf(word,16*color-114);
	else if(color<8) cprintf(word,16*color+128);
	else cprintf(word,142);
	if(mode==0){
		printf("(���� %d )",length/16);
		print_void(111-word.length()-num_len(length/16));
	}
	else{
		printf("(���� %d )",length/2);
		print_void(111-word.length()-num_len(length/2));
	}
	printf("\n");
	
	for(int i=0;i<=length-1;++i){
		if(LineCount>=64){
			LineCount=0;
			printf("\n");
		}
		cprintf_16(A[i],color);
		if(LineCount%4==0){
			printf(" ");
			if(LineCount){
				cprintf(" ",255);
				printf(" ");
			}
		}
		LineCount++;
	}
	printf("\n");
}

bool IsPowerOfTwo(int n){//�ж�һ�����Ƿ�Ϊ2���ݣ�0��1��
    int x=n;
	if(n==0) return 0;
	return (x&(x-1))==0;
}
int hamming_length(int len){//���㺣�������볤��
	if(len==0) return 0;
	int ans=1,last=2;
	while(len+ans+1>last){
		ans++;
		last*=2;
	}
	return ans;
}
bool hamming_check(char *buf,int len,bool mode){//��ȷ���д��������£�ʹ�ú��������
	//modeΪ*buf���ݸ�ʽ��0Ϊbit����1Ϊ�ֽ�
	//*bufΪ���������0�������ʧ�ܣ�1����ɹ� 
	
	if(mode==0){//����Ϊbit
		int ham_len=hamming_length(len);//�����볤��
		char old_ham[ham_len];//�ɾ�����
		char new_ham[ham_len];//�¾�����
		memset(new_ham,0,ham_len);//�¾������ʼ��
		register int cnt=0;//���������ʱ�������Ҫ�鿴��λ��
		
		for(int i=0;i<=len-1;++i){
			if(IsPowerOfTwo(i+1)){//����һ��У��λ
				old_ham[cnt]=*(buf+i);//��¼�ɾ�����
				cnt++;//������+1
			}
			else{//��������λ
				if((bool)buf[i]==1){//������ʾ��bitΪ1�����޸��¾������ֵ
					int num_data=i+1;//����λλ��
					int num_che=1;   //У��λλ��
					for(int j=1;j<=cnt;++j){
						if(num_data&1) new_ham[j-1]=((bool)new_ham[j-1])^1;
						num_data>>=1;
						num_che<<=1;
					}
				}
			}
		}
		
		int sum=0;//�¾ɾ��������֮��Ľ��
		int num=1;//��ʱ�洢2��n����
		for(int i=0;i<=ham_len-1;++i){
			if(old_ham[i]!=new_ham[i]) sum+=num;
			num<<=1;
		}
		
		if(F_Print_mode) print_bit(buf,len,"�����������ǰ��ͷ�������ݣ�",0,8);
		if(sum==0||sum>len){//�Ҳ������󣬷���0(����ʧ��)
			if(F_Print_mode) print_bit(buf,len,"��������������ͷ�������ݣ�(�޷�����)",0,12); 
			return 0;
		}
		else{//�ҵ����󣬽��о���
			*(buf+sum-1)=!((bool)*(buf+sum-1));
			if(F_Print_mode){
				cprintf("�����˵�",240);cprintf(sum,240);cprintf("λ\n",240);
				print_bit(buf,len,"��������������ͷ�������ݣ�(�������)",0,7);
			}
			return 1;
		}
	}
	else{//����Ϊ�ֽ�
		
	}
}
int hamming(char *buf,char *h,int len,bool mode){//������ƴ�Ӳ����к�������
	//����*hΪͷ������ָ��
	//modeΪ*buf *h���ݸ�ʽ��0Ϊbit����1Ϊ�ֽ�
	//*bufΪ���������ֵΪ�䳤��
	
	if(mode==0){//����Ϊbit
		//ƴ��ͷ��������
		char new_buf[len+8*HEAD_SIZE];
		len+=8*HEAD_SIZE;
		for(int i=0;i<=8*HEAD_SIZE-1;++i) new_buf[i]=*(h+i);
		for(int i=8*HEAD_SIZE;i<=len-1;++i) new_buf[i]=*(buf+i-8*HEAD_SIZE);
		if(F_Print_mode) print_bit(new_buf,len,"ƴ�Ӻ��ͷ�������ݣ�",0,11);
		
		//���б���
		len+=hamming_length(len);
		register int cnt=0;//���������ʱ�������Ҫ�鿴��λ��
		
		for(int i=0;i<=len-1;++i){//�����µ�����
			if(IsPowerOfTwo(i+1)){//����һ��У��λ
				*(buf+i)=(bool)0;//��ʼ��������λ
				cnt++;//������+1
			}
			else{//��������λ
				int pos=i-cnt;//��Ӧ��new_bufλ��
				*(buf+i)=new_buf[pos];
				if((bool)new_buf[pos]==1){//������ʾ��bitΪ1�����޸Ķ�ӦУ��λ��ֵ
					int num_data=i+1;//����λλ��
					int num_che=1;   //У��λλ��
					for(int j=1;j<=cnt;++j){
						if(num_data&1) *(buf+num_che-1)=((bool)*(buf+num_che-1))^1;
						num_data>>=1;
						num_che<<=1;
					}
				}
			}
		}
		return len;
	}
	else{//����Ϊ�ֽ� 
		
	}
}
int dehamming(char *buf,int len,bool mode){//�����ݽ���
	//modeΪ*buf���ݸ�ʽ��0Ϊbit����1Ϊ�ֽ�
	//*bufΪ���������ֵΪ�䳤��
	
	if(mode==0){//����Ϊbit
		register int last_pos=0;//��һ����Ҫ���滻��λ�� 
		for(int i=0;i<=len-1;++i){
			if(!IsPowerOfTwo(i+1)){//��������λ
				*(buf+last_pos)=*(buf+i);
				last_pos++;
			}
		}
		return len-hamming_length(len);
	}
	else{//����Ϊ�ֽ�
		
	}
}
unsigned int generate_check_sum(char *buf,int len){//����У���(δ�ֶε����ʼֵ)
	//Ĭ��*buf����Ϊ�ֽ�����
	
	int size=len/2;     //��2�ֽ�Ϊ��λ���ֳ�����������
	int left=len%2;     //��2�ֽ�Ϊ��λ�������ʣ�µ��ֽ���
	unsigned int sum=0; //��������Ӻ��ֵ
	
	for(int i=0;i<=size-1;++i) sum+=*((unsigned short*)buf+i); //�����ֱ�����
	if(left){ //�����ֽڵģ�����������
		unsigned short num=*((unsigned short*)buf+size); //����(ʵ���Ͼ�������ȡ��)
		num=num&(0xffff0000);                            //����8λ����
		sum+=num;
	}
	return sum;
}
int add_check_sum(char *buf,int len,bool mode){//������������һ�� 16bit / 2�ֽ� ��У���
	//modeΪ*buf���ݸ�ʽ��0Ϊbit����1Ϊ�ֽ�
	//*bufΪ���������ֵΪ�䳤��
	
	if(mode==0){//����Ϊbit
		int byte_len=len/8+(len%8!=0);
		char byte[byte_len];
		Bit2Byte(buf,len,byte,byte_len);//�Ƚ�*bufתΪ�ֽ�
		
		unsigned int   sum=generate_check_sum(byte,byte_len);//����ֵ
		unsigned short sum_high=sum>>16;                     //��16λ(��λ)
		unsigned short sum_low=sum&(0x0000ffff);             //��16λ
		unsigned int   secend_sum=sum_high+sum_low;          //�ߵ�λ����Կ����н�λ�������ȡһ��ߵ�λ
		unsigned short secend_sum_high=sum>>16;              //�ڶ��εĸ�16λ(��λ)
		unsigned short secend_sum_low=sum&(0x0000ffff);      //�ڶ��εĵ�16λ
		unsigned short check_sum=~(secend_sum_high+secend_sum_low);//У���
		
		char bit[16];
		Byte2Bit(bit,16,(char*)&check_sum,2);//��У���תΪbit
		if(F_Print_mode) print_bit(bit,16,"�������У��ͣ�",0,13);
		
		for(int i=len;i<=len+15;++i) *(buf+i)=bit[i-len];//��У��͸�����ĩβ
		return len+16;
	}
	else{//����Ϊ�ֽ�
		unsigned int   sum=generate_check_sum(buf,len);//����ֵ
		unsigned short sum_high=sum>>16;               //��16λ(��λ)
		unsigned short sum_low=sum&(0x0000ffff);       //��16λ
		unsigned int   secend_sum=sum_high+sum_low;    //�ߵ�λ����Կ����н�λ�������ȡһ��ߵ�λ
		unsigned short secend_sum_high=sum>>16;        //�ڶ��εĸ�16λ(��λ)
		unsigned short secend_sum_low=sum&(0x0000ffff);//�ڶ��εĵ�16λ
		unsigned short check_sum=~(secend_sum_high+secend_sum_low);//У���
		
		*(buf+len)=*((char*)&check_sum);//��У��͸�����ĩβ
		*(buf+len+1)=*((char*)&check_sum+1);//��У��͸�����ĩβ
		return len+2;
	}
}
bool check_check_sum(char *buf,int len,char *check_sum,bool mode){//������ݵ�У���
	//modeΪ*buf���ݸ�ʽ��0Ϊbit����1Ϊ�ֽ�
	//*bufΪ���������ֵΪ�Ƿ���ȷ��0Ϊ��1Ϊ��
	
	if(mode==0){//����Ϊbit
		int byte_len=len/8+(len%8!=0);
		char byte[byte_len];
		Bit2Byte(buf,len,byte,byte_len);//�Ƚ�*bufתΪ�ֽ�
		
		unsigned int   sum=generate_check_sum(byte,byte_len);//������У��͵�����ֵ
		Bit2Byte(check_sum,16,byte,2);//��*check_sumתΪ�ֽ�
		sum+=*((unsigned short*)&byte);//���Ͼ�У���
		
		unsigned short sum_high=sum>>16;                     //��16λ(��λ)
		unsigned short sum_low=sum&(0x0000ffff);             //��16λ
		unsigned int   secend_sum=sum_high+sum_low;          //�ߵ�λ����Կ����н�λ�������ȡһ��ߵ�λ
		unsigned short secend_sum_high=sum>>16;              //�ڶ��εĸ�16λ(��λ)
		unsigned short secend_sum_low=sum&(0x0000ffff);      //�ڶ��εĵ�16λ
		unsigned short new_check_sum=~(secend_sum_high+secend_sum_low);//��У���
		
		if(F_Print_mode){
			char bit[16];
			Byte2Bit(bit,16,(char*)&new_check_sum,2);
			if(new_check_sum) print_bit(bit,16,"�ؼ������У��ͣ�(����)",0,12);
			else print_bit(bit,16,"�ؼ������У��ͣ�(��ȷ)",0,13);
		}
		
		return !(new_check_sum);//������У��ͣ�Ϊ0�򷵻�1�����򷵻�0
	}
	else{//����Ϊ�ֽ�
		unsigned int   sum=generate_check_sum(buf,len);//������У��͵�����ֵ
		
		sum+=*((unsigned short*)&check_sum);//���Ͼ�У���
		
		unsigned short sum_high=sum>>16;               //��16λ(��λ)
		unsigned short sum_low=sum&(0x0000ffff);       //��16λ
		unsigned int   secend_sum=sum_high+sum_low;    //�ߵ�λ����Կ����н�λ�������ȡһ��ߵ�λ
		unsigned short secend_sum_high=sum>>16;        //�ڶ��εĸ�16λ(��λ)
		unsigned short secend_sum_low=sum&(0x0000ffff);//�ڶ��εĵ�16λ
		unsigned short new_check_sum=~(secend_sum_high+secend_sum_low);//��У���
		
		return !(new_check_sum);//������У��ͣ�Ϊ0�򷵻�1�����򷵻�0
	}
}
int frame_bit(char *buf,char *h,int len){//��bit��֡ 
	//����*hΪͷ������(bit)ָ��
	//*bufΪ���������ֵΪ�䳤�ȣ�����-1ʱ�����֡�󳤶ȳ�����������
	
	if(len+8*(4+HEAD_SIZE)+hamming_length(8*HEAD_SIZE+len)\
	   >MAX_BUFFER_SIZE) return -1;//�����ͷ�������������롢У��͡�֡ͷ֡β�󳬳�������-1
	
	//����ͷ�����������룬���У���
	len=hamming(buf,h,len,0);     //���к������룬ͬʱ����ͷ��������
	if(F_Print_mode) print_bit(buf,len,"����������ͷ�������ݣ�",0,6);
	
	len=add_check_sum(buf,len,0); //��buf�������У���
	if(F_Print_mode) print_bit(buf,len,"���У��ͺ��ͷ�������ݣ�",0,13);
	
	char new_buf[(int)(len*1.2+17)]; //�µ�����
	int new_len=0;                   //�µ����ݳ���
	register int cnt=0;              //����
	
	for(int i=0;i<=len-1;++i){//������ݣ��޸�������6��1
		new_buf[new_len++]=*(buf+i);
		if(*(buf+i)) cnt++;
		else cnt=0;
		if(cnt==5){
			new_buf[new_len++]=0;
			cnt=0;
		}
	}
	
	//����֡ͷ֡β����֡
	int final_len=new_len+16;//��֡������ճ���
	if(final_len>MAX_BUFFER_SIZE) return -1;//����������-1
	for(int i=8;i<=7+new_len;++i) *(buf+i)=new_buf[i-8];//��֡����ͷ��+���ݲ���
	for(int i=0;i<=7;++i){//��֡����֡ͷ֡β����
		if(i==0||i==7){
			*(buf+i)=0;
			*(buf+final_len-i-1)=0;
		}
		else{
			*(buf+i)=1;
			*(buf+final_len-i-1)=1;
		}
	}
	if(F_Print_mode) print_bit(buf,final_len,"��֡���ͷ�������ݣ�",0,10);
	return final_len;
}
int frame_byte(char *buf,char *h,int len){//���ֽڳ�֡
	//����*hΪͷ������(�ֽ�)ָ��
	//*bufΪ���������ֵΪ�䳤�ȣ�����-1ʱ�����֡�󳤶ȳ�����������
	
	return len;
}
int frame(char *buf,int len,HEAD *h,bool input_mode,bool output_mode){//��֡
	//����*hΪͷ������(�ֽ�)ָ�룬input/output modeΪ*buf���ݸ�ʽ��0Ϊbit����1Ϊ�ֽ�
	//*bufΪ���������ֵΪ�䳤�ȣ�����-1ʱ�����֡�󳤶ȳ�����������
	
	if(output_mode==0){//���Ϊbit
		char bit[8*HEAD_SIZE];
		Byte2Bit(bit,HEAD_SIZE*8,(char*)h,HEAD_SIZE);//Ҫ�Ƚ�ͷ��ת��Ϊbit
		
		if(F_Print_mode){
			print_bit(bit,8*HEAD_SIZE,"ԭͷ����",0,11);
			print_bit(buf,len,"ԭ���ݣ�",input_mode,11);
		}
		
		if(input_mode==0)//����Ϊbit��ֱ�����bit
			return frame_bit(buf,bit,len);
		else{//����Ϊ�ֽڣ�ת�������bit
			if(len*8>MAX_BUFFER_SIZE) return -1;
			char byte[len];
			for(int i=0;i<=len-1;++i) byte[i]=*(buf+i);
			len=Byte2Bit(buf,len*8,byte,len);//������ת��Ϊbit
			return frame_bit(buf,bit,len);
		}
	}
	else{//���Ϊ�ֽ�
		if(input_mode==1)//����Ϊ�ֽڣ�ֱ������ֽ�
			return frame_byte(buf,(char*)h,len);
		else{//����Ϊbit��ת��������ֽ�
			char bit[len];
			for(int i=0;i<=len-1;++i) bit[i]=*(buf+i);
			len=Bit2Byte(bit,len,buf,len/8+(len>0&&len%8!=0));
			return frame_byte(buf,(char*)h,len);
		}
	}
}
int deframe_bit(char *buf,char *h,int len){//��bit�ָ�
	//����*hΪͷ������(bit)ָ��
	//*buf,*hΪ���������ֵΪ�䳤�ȣ�����-2ʱ����֡�����޷��ָ�
	
	//ȥ��֡ͷ֡β
	char new_buf[len-8];//�µ�����
	int new_len=0;      //�µ����ݳ���
	register int cnt=0; //����
	bool in=0;          //�Ƿ������֡��
	int in_time=0;      //���� ֡ͷ/β �Ĵ���
	for(int i=0;i<=len-1;++i){
		if(*(buf+i)) cnt++;
		else cnt=0;
		if(cnt==6){
			if(*(buf+i+1)==0){
				if(++in_time>2) return -2;//�����������ϵĶ������ֱ�ӷ���-2
				i+=2;
				cnt=0;
				in=!in;
			}
			else return -2;//����������7��1��ֱ�ӷ���-2
		}		
		if(in&&i<=len-1){
			new_buf[new_len++]=*(buf+i);
			if(cnt==5&&*(buf+i+1)==0){
				i++;
				cnt=0;
				continue;
			}
		}
	}
	if(in_time<=1) return -2;//�����������εĶ������ֱ�ӷ���-2
	new_len-=6;//ȥ���ж�֡β���µĶ����ֽ�
	if(new_len<8*HEAD_SIZE+16) return -2;//ȥ���󳤶�С��ͷ����У��ͳ��Ⱥͣ���Ȼ���󣬷���-2
	if(F_Print_mode) print_bit(new_buf,new_len,"��֡���ͷ����������У��ͣ�",0,13);
	
	//�ָ�ͷ�������ݡ�У���
	char check_sum[16];//16bitУ���
	for(int i=new_len-16;i<=new_len-1;++i) check_sum[i-(new_len-16)]=new_buf[i];//�ָ�����У��� 
	new_len-=16;
	
	bool wrong=0;
	if(check_check_sum(new_buf,new_len,check_sum,0)==0){//���У��ͣ�����������о���
		if(hamming_check(new_buf,new_len,0)==0) wrong=1;//ʹ�ú����������ʧ����wrong=1
		if(check_check_sum(new_buf,new_len,check_sum,0)==0) wrong=1;//�ɹ����ٴμ��У��ͣ�������wrong=1
	}
	if(F_Print_mode&&wrong==0) print_bit(new_buf,new_len,"��֡��������ͷ�������ݣ�",0,6);
	
	new_len=dehamming(new_buf,new_len,0);//���뺣������
	if(F_Print_mode&&wrong==0) print_bit(new_buf,new_len,"������ͷ�������ݣ�",0,11); 
	
	for(int i=0;i<=8*HEAD_SIZE-1;++i) *(h+i)=new_buf[i];//�ָ�����ͷ�� 
	if(wrong) return -2;//wrong=1ʱ������-2 (Ŀ�ģ��ָ�ͷ���Գ��Է���NAK) 
	for(int i=8*HEAD_SIZE;i<=new_len-1;++i) *(buf+i-8*HEAD_SIZE)=new_buf[i];//�ָ��������� 
	new_len-=8*HEAD_SIZE; 
	if(F_Print_mode){
		print_bit(h,8*HEAD_SIZE,"���ջָ����ͷ����",0,11);
		print_bit(buf,new_len,"���ջָ�������ݣ�",0,11);
	}
	return new_len;
}
int deframe_byte(char *buf,char *h,int len){//���ֽڻָ�
	//����*hΪͷ������(�ֽ�)ָ��
	//*buf,*hΪ���������ֵΪ�䳤�ȣ�����-1ʱ�����֡�󳤶ȳ�����������
	
	return len;
}
int deframe(char *buf,int len,HEAD *h,bool input_mode,bool output_mode){//�ָ�
	//����*hΪͷ������(�ֽ�)ָ�룬input/output modeΪ*buf���ݸ�ʽ��0Ϊbit����1Ϊ�ֽ�
	//*buf,*hΪ���������ֵΪ�䳤�ȣ�����-1ʱ�����֡�󳤶ȳ����������ޣ�-2ʱ����֡�����޷��ָ�
	
	if(input_mode==0){//����Ϊbit
		if(len>MAX_BUFFER_SIZE) return -1;
		char bit[8*HEAD_SIZE];
		Byte2Bit(bit,8*HEAD_SIZE,(char*)h,HEAD_SIZE);
		len=deframe_bit(buf,bit,len);//�ȷ�֡
		
		Bit2Byte(bit,HEAD_SIZE*8,(char*)h,HEAD_SIZE);//��ͷ��ת��Ϊ�ֽ�
		
		if(output_mode==0)//���Ϊbit����ֱ�����
			return len;
		else{//���Ϊ�ֽڣ�ת���������
			if(len<0) return len;
			char bit[len];
			for(int i=0;i<=len-1;++i) bit[i]=*(buf+i);
			return Bit2Byte(bit,len,buf,len/8+(len>0&&len%8!=0));
		}
	}
	else{//����Ϊ�ֽ�
		if(len*8>MAX_BUFFER_SIZE) return -1;
		len=deframe_byte(buf,(char*)h,len);//�ȷ�֡
		
		if(output_mode==1)//���Ϊ�ֽڣ���ֱ�����
			return len;
		else{//���Ϊbit��ת���������
			if(len<0) return len;
			char byte[len];
			for(int i=0;i<=len-1;++i) byte[i]=*(buf+i);
			return Byte2Bit(buf,len*8,byte,len);
		}
	}
}

int main(){
	HEAD ha,ba; 
	char a[MAX_BUFFER_SIZE]={'1','1','0'};
	memset(a,1,sizeof(a));	
	int len=100;
	
	len=frame(a,len,&ha,0,0);//��֡
	if(len==-1) cprintf("�ռ䲻�㣬��֡ʧ��\n",12);
	else cprintf("��֡�ɹ���\n",10);
	
//	memset(a+10,!(bool)*(a+10),1);//�޸� 
//	memset(a+21,!(bool)*(a+21),1);//�޸� 
//	print_bit(a,len,"�޸ĵ�11λ���֡��",0,9);
	
	len=deframe(a,len,&ba,0,1);//�ָ� 
	if(len==-2) cprintf("֡���Ҿ���ʧ�ܣ��ָ�ʧ��\n",12);
	else cprintf("�ָ��ɹ���\n",11);
	
	system("pause");
	return 0;
}
