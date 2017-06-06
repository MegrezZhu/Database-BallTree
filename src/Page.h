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
	用于管理一个页文件
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
	static Page* create(int itemSize);			// 比较常用这个构造，根据每项的大小自动计算容量
	~Page();

	void writeBack(const string &path);
	void writeBack();
	const char* getBySlot(int slotNum);			// 根据槽号读
	void setBySlot(int slotNum, char* data);	// 根据槽号写
	bool isDirty();
	int getCapacity() { return itemNum; }
	const int& getId() { return pid; }
};

#endif