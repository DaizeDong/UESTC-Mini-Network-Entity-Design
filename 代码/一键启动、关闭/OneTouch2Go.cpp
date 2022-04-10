//һ������
#include <iostream>
#include <string>
#include <fstream>
#include <atlstr.h>
#include <vector>
#include <windows.h>
using namespace std;

struct functionMap_t {
	string LayerName;
	string funcitonName;
	int len; //��������ַ����ĳ���
}aFunctionMap[7];

//����ʱ�ر�
void shutdown() {
	for (int i = 5; i >= 1; --i) {
		cout << "������������" << i << "s���Զ��ر�\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
		Sleep(1000);
	}
}

string getFunction(string strName) {
	int i;
	for (i = 0; i < 7; i++) {
		if (aFunctionMap[i].LayerName.empty() || strName.empty())
			continue;
		if (strName.compare(aFunctionMap[i].LayerName) == 0)//�ҵ�
			return aFunctionMap[i].funcitonName;
	}
	return "";//û�ҵ� 
}

//ֻ����ASCII�����32���ַ���32Ϊ�ո����µĶ��ǿ����ַ�
void myStrcpy(string& dst, string& src) {
	size_t i;
	const char* cpSrc;
	dst.clear();
	if (src.empty()) return dst.clear();
	cpSrc = src.c_str();
	for (i = 0; i < strlen(cpSrc); i++)
		if (cpSrc[i] > 32 || cpSrc[i] < 0)
			dst.append(1, cpSrc[i]);
}

void readMapFile(ifstream& f) {
	string strTmp;
	string csLeft;
	string csRight;
	string csLayer;
	string csFunc;
	int i = 0;
	
	while (!f.eof()) {
		getline(f, strTmp);
		if (strTmp.empty() || strTmp.find("=") < 0) continue;
		csLeft = strTmp.substr(0, strTmp.find("=") - 1);
		csRight = strTmp.substr(strTmp.find("=") + 1, strTmp.length() - strTmp.find("="));
		myStrcpy(csLayer, csLeft);
		myStrcpy(csFunc, csRight);
		aFunctionMap[i].LayerName = csLayer;
		aFunctionMap[i].funcitonName = csFunc;
		i++;
	}
}

//�жϱ����Ƿ�Ϊ�ָ��У�������������----
bool isSplitLine(string* pstr) {
	int retval;
	retval = (int)pstr->find("-------");
	if (retval < 0) return 0;
	return 1;
}

//�жϱ����Ƿ��ǲ����У������ǿ�ͷ��#
bool isParmsLine(string* pstr) {
	if (pstr->at(0) == '#' || isSplitLine(pstr)) return 0;
	return 1;
}

//�������ִ�����ȡ�豸�ţ�����ִ��Կո�ʼ������ȡ������
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
	
	retval = (int)str->find("@");//��str���ҳ�ʵ����ֶ�
	if (retval == -1) strEnt = str->c_str();//û��IP��ַ�����1�������ַ�������
	else strEnt = str->substr(0, retval);//ȡ@֮ǰ
	
	for (i = (int)strEnt.length() - 1; i >= 0; i--)
		if (strEnt.at(i) < '0' || strEnt.at(i) > '9')
			break;
	
	strEnt = strEnt.substr(i + 1, strEnt.length() - i - 1);
	strLayer = str->substr(0, i + 1);
	return 0;
}

int main() {
	ifstream mapFile("map.txt");//��ӳ���ļ�
	if (!mapFile.is_open()) {
		cout << "ERROR: û���ҵ�map.txt�ļ�" << endl << endl;
		shutdown();
		return 0;
	}
	cout << "��ȡmap.txt�ļ��ɹ�" << endl;
	readMapFile(mapFile);
	mapFile.close();

	ifstream cfgFile("ne.txt");//�������ļ�
	if (!cfgFile.is_open()) {
		cout << endl << "ERROR: û���ҵ�ne.txt�ļ�" << endl << endl;
		shutdown();
		return 0;
	}
	cout << "��ȡne.txt�ļ��ɹ�" << endl;
	
	string* pstrTmp;
	string strTmp;
	vector <string*> archSection;
	while (!cfgFile.eof()) {//���������ļ�
		getline(cfgFile, strTmp);
		if (isSplitLine(&strTmp)) break;
	}
	if (cfgFile.eof()) {//û�ж�����Ч����
		cfgFile.close();
		cout << endl << "ERROR: ne.txt�ļ�ȱʧ��Ч����" << endl << endl;
		shutdown();
		return -2;
	}
	while (!cfgFile.eof()) {//����ܹ��͵�ַ��������
		getline(cfgFile, strTmp);
		if (isSplitLine(&strTmp)) break;
		if (!isParmsLine(&strTmp)) continue;
		pstrTmp = new string(strTmp.c_str());
		archSection.push_back(pstrTmp);
	}
	cfgFile.close();
	if (archSection.size() == 0) {//�ж��ܹ��͵�ַ��������
		cout << endl << "ERROR: ne.txt�ļ�ȱʧ���͵�ַ��������" << endl << endl;
		shutdown();
		return 0;
	}
	cout << "��ȡ���óɹ�" << endl;
	
	int NECount = 0;
	size_t index;
	string strDev;
	string strLayer;
	string strEntity;
	string strCmd;
	string strParm;
	int begin;
	int end;
	
	for (index = 0; index < archSection.size(); index++) {
		pstrTmp = archSection[index];//����һ����ִ��һ��
		begin = 0;//���豸��
		end = (int)pstrTmp->find_first_of(" ");
		if (end < 0) continue;//��һ�в���
		else if (end > 0) strDev = pstrTmp->substr(0, end);//������豸�ţ�����ȡ��������֮ǰ��
		begin = end;
		
		while (begin < (int)pstrTmp->length() && begin >= 0) {
			begin = (int)pstrTmp->find_first_not_of(' ', begin);//�����ո�
			if (begin == -1) break;
			end = (int)pstrTmp->find_first_of(' ', begin);//��ȡ
			if (end == -1) end = (int)pstrTmp->length();//����û����

			strTmp = pstrTmp->substr(begin, end - begin);
			getLayerAndEnt(&strTmp, strLayer, strEntity);
			strCmd = getFunction(strLayer);
			strParm = strDev;
			strParm += " ";
			strParm += strLayer;
			strParm += " ";
			strParm += strEntity;

			ShellExecute(NULL, _T("open"), strCmd.c_str(), strParm.c_str(), NULL, SW_MINIMIZE);
			cout << "������Ԫ " << strDev << " �� " << strLayer << " ��ʵ�� " << strEntity << endl;
			NECount++;
			begin = end;
		}
	}
	cout << "������Ԫ " << NECount << " ��" << endl << endl << "�����ļ��Ѵ�" << endl << endl;
	shutdown();
	return 0;
}
