#ifndef PAGE
#define PAGE

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <map>
#include <utility>
#include <iostream>

using namespace std;

const int PAGE_SIZE = 64 * 1024;

/*
	���ڹ���һ��ҳ�ļ�
*/
class Page {
	int pid, itemSize, itemNum;
	bool dirty;
	string filepath;
	char* data;
	bool* bitmap;

	static int _pid;
public:
	static Page* createFromFile(const string &path);
	static Page* create(int itemNum, int itemSize);
	static Page* create(int itemSize);			// �Ƚϳ���������죬����ÿ��Ĵ�С�Զ���������
	~Page();

	void writeBack(const string &path);
	void writeBack();
	const char* getBySlot(int slotNum);			// ���ݲۺŶ�
	void setBySlot(int slotNum, char* data);	// ���ݲۺ�д
	bool isDirty();
	int getCapacity() { return itemNum; }
	const int& getId() { return pid; }
};

#endif