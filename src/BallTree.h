#ifndef BALLTREE
#define BALLTREE

#include "BallTreeNode.h"
#include "Page.h"
#include <list>
#include <fstream>
#include <functional>
using namespace std;

class BallTree {
private:
	int size, dimension;  //数据量大小以及维度
	BallTreeNode* root;	  //根节点

	int leafNum, visitedLeafNum;	//叶子节点数目、搜索过程中访问的节点数目

	Page *indexPage;	//索引页：存储了所有节点在存储页中的位置（所在的page和slot）

	pair<int, float> _mipSearch(BallTreeNode* root, float* query);   //查找时的递归函数

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

#endif