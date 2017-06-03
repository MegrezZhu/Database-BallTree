#include "PagePool.h"

PagePool* PagePool::ins = nullptr;

Page* PagePool::create(int itemSize) {
	auto p = Page::create(itemSize);
	mp.insert(make_pair(p->getId(), p));
	return p;
}

Page* PagePool::createFromFile(const string &path, int pid) {
	auto buffer = new char[50];
	sprintf(buffer, "%s/%d.page", path.c_str(), pid);
	auto p = Page::createFromFile(buffer);
	mp.insert(make_pair(p->getId(), p));
	return p;	
}

Page* PagePool::get(int pid) {
	auto res = mp.find(pid);
	if (res == mp.end()) return nullptr;
	else return res->second;
}

void PagePool::writeBack() {
	for (auto &pair : mp) {
		pair.second->writeBack();
	}
}

void PagePool::remove(int pid) {
	auto res = mp.find(pid);
	if (res != mp.end()) {
		delete res->second;
		mp.erase(res);
	}
}