// OneTouch2Close.cpp : ���ļ����� "main" ����������ִ�н��ڴ˴���ʼ��������

#include <iostream>
#include <string>
#include <fstream>
#include <atlstr.h>
#include <vector>
#include <windows.h>
#include <Ws2tcpip.h>  //InetNtop ��InetPton �������ļ�

#pragma comment(lib,"Ws2_32.lib")

using namespace std;

//����ʱ�ر�
void shutdown() {
	for (int i = 5; i >= 1; --i) {
		cout << "������������" << i << "s���Զ��ر�\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
		Sleep(1000);
	}
}

//ֻ����ASCII�����32���ַ���32Ϊ�ո����µĶ��ǿ����ַ�
void myStrcpy(string& dst, string& src) {
	size_t i;
	const char* cpSrc;
	dst.clear();
	if (src.empty())return dst.clear();
	cpSrc = src.c_str();
	for (i = 0; i < strlen(cpSrc); i++)
		if (cpSrc[i] > 32 || cpSrc[i] < 0)
			dst.append(1, cpSrc[i]);
}

//�ӽ�ȡ��ʵ�������ִ���@��ȡ��IP��ַ�����û��@����Ĭ��Ϊ�ػ���ַ
string getAddr(string* str) {
	string addr;
	int retval = (int)str->find('@');
	if (retval == -1) addr = "127.0.0.1";//ʹ��Ĭ�ϲ���
	else addr = str->substr(retval + 1);//ȡ@���IP��ַ
	return addr;
}

//��dev����κţ�ʵ�������UDP�˿ںţ���ʽΪʮ���ƣ�1 dev1 layer1 enty2
string getPort(string* dev, int iLayer, string* ent) {
	string strTmp;
	string strLayer;
	
	//���豸�ţ���Σ�ʵ�������ΨһUDP�˿ں�
	strTmp = "1";
	strTmp += dev->c_str();
	strLayer = std::to_string(iLayer);//��κ�;
	strTmp += strLayer;
	if (ent->length() == 1)strTmp += "0";//ֻ��1λ������0
	strTmp += ent->c_str();
	return strTmp;
}

//�жϱ����Ƿ�Ϊ�ָ��У�������������----
bool isSplitLine(string* pstr) {
	int retval;
	retval = (int)pstr->find("--------");
	if (retval < 0) return 0;
	return 1;
}

//�жϱ����Ƿ��ǲ����У������ǿ�ͷ��#
bool isParmsLine(string* pstr) {
	if (pstr->at(0) == '#' || isSplitLine(pstr)) return 0;
	return 1;
}

// �������ִ�����ȡ�豸�ţ�����ִ��Կո�ʼ������ȡ������
string getDev(string* pstr) {
	int retval;
	retval = (int)pstr->find_first_of(" ");
	if (retval <= 0) return string();
	return pstr->substr(0, retval);
}

//�ӽ�ȡ�õ�ʵ�������ֶ��У���ȡ��κź�ʵ���
int getLayerAndEnt(string* str, string& strLayer, string& strEnt) {
	int retval;
	int i;
	
	retval = (int)str->find("@");//Ҫ��str���ҳ�ʵ����ֶ�
	if (retval == -1) strEnt = str->c_str();//û��IP��ַ�����1�������ַ�������
	else strEnt = str->substr(0, retval);//ȡ@֮ǰ

	for (i = (int)strEnt.length() - 1; i >= 0; i--)
		if (strEnt.at(i) < '0' || strEnt.at(i) > '9')
			break;

	strEnt = strEnt.substr(i + 1, strEnt.length() - i - 1);
	strLayer = str->substr(0, i + 1);

	return 0;
}

//����ڽṹ�ִ��У�ָ��layer�Ĳ�κţ��ڽṹ�ִ��У�ÿ��ÿ��ֻ��һ��ʵ�塣ͬ����ʵ��Ҫ���б�ʾ
//���εı��Ҳ���ǻ�ò��λ�ã����֮���Կո�����û���豸�ţ�Ҫ�Կո�ʼ
//����ֵ1��ʾ��Ͳ㣬-1��ʾû�ҵ�
int getLayerNo(string* pStr, string* lay) {
	string strTmp;
	int begin;
	int end;
	int i;

	//�ж��豸�����,-2 ��ʾ�豸�Ų�ƥ�䣬û���豸����ɼ���
	begin = (int)pStr->find(' ');

	//�����ַ���û�пո񣬸�ʽ����,����
	if (begin == -1)return -1;

	for (i = 1; begin < (int)pStr->length() && begin >= 0; i++) {
		//��begin��ʼ�ҵ�һ����0���ֶ�,������һ�׶οո�
		begin = (int)pStr->find_first_not_of(' ', begin);
		//�����ո�֮��û����
		if (begin == -1)break;
		//��ȡ�Ӳ���
		strTmp = pStr->substr(begin, 3);
		//�ҵ������Լ��Ĳ����
		if (strTmp.find(lay->c_str(), 0, 3) != -1) break;

		end = (int)pStr->find_last_of(strTmp.c_str(), begin, 3);
		begin = (int)pStr->find_first_of(' ', end);
	}

	//�����Ĳ���������Ҳ���
	if (begin < 0) return -1;

	return i;
}

//�Ա��IP��ַ�ַ���ת��Ϊ�����ֽ���������ʽ
unsigned long myInetAddr2n(string* pstr) {
	int begin;
	int end;
	unsigned long retval;
	unsigned long temp;
	string strTmp;

	retval = 0;
	begin = (int)pstr->find_first_of(".");
	if (begin < 0)return 0;

	strTmp = pstr->substr(0, begin);
	temp = atoi(strTmp.c_str());
	retval = (temp << 24) & 0xff000000;

	begin = begin + 1;
	end = (int)pstr->find_first_of(".", begin);
	if (end < 0) return 0;

	strTmp = pstr->substr(begin, end - begin);
	temp = atoi(strTmp.c_str());
	retval += (temp << 16) & 0x00ff0000;

	begin = end + 1;
	end = (int)pstr->find_first_of(".", begin);
	if (end < 0) return 0;

	strTmp = pstr->substr(begin, end - begin);
	temp = atoi(strTmp.c_str());
	retval += (temp << 8) & 0x0000ff00;

	begin = end + 1;
	strTmp = pstr->substr(begin);
	temp = atoi(strTmp.c_str());
	retval += temp & 0x000000ff;

	return htonl(retval);
}

int main() {
	WSAData wsa;
	WSAStartup(0x201, &wsa);

	//�������ļ�
	ifstream cfgFile("ne.txt");
	if (!cfgFile.is_open()) {
		cout << "ERROR: û���ҵ�ne.txt�ļ�" << endl << endl;
		shutdown();
		return 0;
	}
	cout << "��ȡne.txt�ļ��ɹ�" << endl;

	//���������ļ�
	//ͨ���豸�ţ����������ʵ��ţ��õ��ĸ�������:basic, lower , upper��peer
	string* pstrTmp;
	string strTmp;
	vector <string*> archSection;
	while (!cfgFile.eof()) {
		getline(cfgFile, strTmp);
		if (isSplitLine(&strTmp)) break;
	}
	if (cfgFile.eof()) {//û�ж�����Ч����
		cfgFile.close();
		cout << endl << "ERROR: ne.txt�ļ�ȱʧ��Ч����" << endl << endl;
		shutdown();
		return -2;
	}
	//����ܹ��͵�ַ��������
	while (!cfgFile.eof()) {
		getline(cfgFile, strTmp);
		if (isSplitLine(&strTmp)) break;
		if (!isParmsLine(&strTmp)) continue;
		pstrTmp = new string(strTmp.c_str());
		archSection.push_back(pstrTmp);
	}
	cfgFile.close();
	//�ж��ܹ��͵�ַ��������
	if (archSection.size() == 0) {
		cout << endl << "ERROR: ne.txt�ļ�ȱʧ���͵�ַ��������" << endl << endl;
		shutdown();
		return 0;
	}
	cout << "��ȡ���óɹ�" << endl;

	//�����ļ�����ʼ����
	int NECount = 0;
	size_t index;
	string strDev;
	string strLayer;
	string strEntity;
	string strCmd;
	string strParm;
	string strAddr;
	string strPort;
	int begin;
	int end;
	SOCKET sock;
	int iLayer;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == SOCKET_ERROR)return 0;

	int retval;
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(20000);
	retval = bind(sock, (sockaddr*)&addr, sizeof(sockaddr_in));

	for (index = 0; index < archSection.size(); index++) {
		//����һ����ִ��һ��
		pstrTmp = archSection[index];
		//���豸��
		begin = 0;
		end = (int)pstrTmp->find_first_of(" ");
		//��һ�в���
		if (end < 0)continue;
		//������豸�ţ�����ȡ���������֮ǰ��
		else if (end > 0) strDev = pstrTmp->substr(0, end);

		begin = end;
		iLayer = 1;
		while (begin < (int)pstrTmp->length() && begin >= 0) {
			//�����ո�
			begin = (int)pstrTmp->find_first_not_of(' ', begin);
			if (begin == -1)break;
			//��ȡ
			end = (int)pstrTmp->find_first_of(' ', begin);
			//����û����
			if (end == -1)end = (int)pstrTmp->length();

			strTmp = pstrTmp->substr(begin, end - begin);
			getLayerAndEnt(&strTmp, strLayer, strEntity);

			strAddr = getAddr(&strTmp);
			strPort = getPort(&strDev, iLayer, &strEntity);

			addr.sin_addr.S_un.S_addr = myInetAddr2n(&strAddr);
			addr.sin_port = htons(atoi(strPort.c_str()));

			//��һ���͹ر�����
			retval = sendto(sock, "exit", 5, 0, (sockaddr*)&(addr), sizeof(sockaddr_in));
			if (retval <= 0) retval = WSAGetLastError();

			cout << "�ر���Ԫ " << strDev << " �� " << strLayer << " ��ʵ�� " << strEntity << " ��ַ��" << addr.sin_addr.S_un.S_addr << " port:" << addr.sin_port << endl;
			NECount++;
			begin = end;
			iLayer++;
		}
	}

	cout << "������Ԫ " << NECount << " ��" << endl << endl << "�����ļ��ѹر�" << endl << endl;
	closesocket(sock);
	WSACleanup();
	shutdown();
	return 0;
}