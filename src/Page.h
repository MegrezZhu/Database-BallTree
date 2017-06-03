#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

const int PAGE_SIZE = 64 * 1024;

class Page {
	int pid, itemSize, itemNum;
	bool dirty;
	string filepath;
	char* data;
	bool* bitmap;

	static int _pid;
public:
	static Page* craeteFromFileWithPid(int pid);
	static Page* createFromFile(const string &path);
	static Page* create(int itemNum, int itemSize);
	static Page* create(int itemSize);
	~Page();

	void writeBack(const string &path);
	void writeBack();
	const char* getBySlot(int slotNum);
	void setBySlot(int slotNum, char* data);
	bool isDirty();
	int getCapacity() { return itemNum; }
	const int& getId() { return pid; }
};
