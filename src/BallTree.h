#pragma once

#include "BallTreeNode.h"
#include "Page.h"
#include <list>
#include <fstream>
#include <functional>
using namespace std;

class BallTree {
private:
	int size, dimension;  //数据量大小以及纬度
	BallTreeNode* root;	  //根节点

	int leafNum, visitedLeafNum;	//叶子节点数目以及搜索过程中访问的节点数目

	// the page that store tid - <pid, sid> pairs
	Page *indexPage;	//专门用于存取每个节点的tid对应寻到那个page的那个槽号

	pair<int, float> _mipSearch(BallTreeNode* root, float* query);		//查询函数

public:
	BallTree();
	~BallTree();

	bool buildTree(
		int n,
		int d,
		float** data);

	bool storeTree(const string& indexPath);
	
	bool restoreTree(const string& indexPath);
	
	int mipSearch(
		int d,
		float* query);

	// optional
	bool insertData(
		int d,
		float* data);

	// optional
	bool deleteData(
		int d,
		float* data);

	// optional
	bool buildQuadTree(
		int n,
		int d,
		float** data);

	/*
		遍历整棵Balltree
	*/
	void traverse(BallTreeNode *root, function<void(BallTreeNode *node)> func);

	/*
		从文件中读取一个结点
	*/
	BallTreeNode* restoreNode(int tid);

	// 统计结点数和叶结点数，未用
	void countNode();
};
