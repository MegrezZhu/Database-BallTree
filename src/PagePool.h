#pragma once

#include "Page.h"

/*
	用于管理Page
*/
class PagePool {
	static PagePool* ins;
	std::map<int, Page*> mp;
	std::string base;

	PagePool() {}
public:
	/*
		单例
	*/
	static PagePool* getInstance() {
		if (!ins) ins = new PagePool();
		return ins;
	}

	/*
		创建一个Page并读取
	*/
	Page* createFromFile(const string &path, int pid); 

	Page* createFromFile(int pid) { return createFromFile(base, pid); }
	Page* create(int itemSize);
	Page* get(int pid);
	void remove(int pid);

	void setBaseDir(const string& path) { base = path; }
	void writeBack();
};

#define pagePool PagePool::getInstance()