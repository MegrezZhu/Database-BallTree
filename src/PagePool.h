#pragma once

#include "Page.h"

class PagePool {
	static PagePool* ins;
	std::map<int, Page*> mp;
	std::string base;

	PagePool() {}
public:
	static PagePool* getInstance() {
		if (!ins) ins = new PagePool();
		return ins;
	}

	Page* createFromFile(const string &path, int pid); 
	Page* createFromFile(int pid) { return createFromFile(base, pid); }
	Page* create(int itemSize);
	Page* get(int pid);
	void remove(int pid);

	void setBaseDir(const string& path) { base = path; }
	void writeBack();
};

#define pagePool PagePool::getInstance()