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
	int size, dimension;  //��������С�Լ�ά��
	BallTreeNode* root;	  //���ڵ�

	int leafNum, visitedLeafNum;	//Ҷ�ӽڵ���Ŀ�����������з��ʵĽڵ���Ŀ

	Page *indexPage;	//����ҳ���洢�����нڵ��ڴ洢ҳ�е�λ�ã����ڵ�page��slot��

	pair<int, float> _mipSearch(BallTreeNode* root, float* query);   //����ʱ�ĵݹ麯��

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
		��������Balltree
	*/
	void traverse(BallTreeNode *root, function<void(BallTreeNode *node)> func);

	/*
		���ļ��ж�ȡһ�����
	*/
	BallTreeNode* restoreNode(int tid);

	// ͳ�ƽ������Ҷ�������δ��
	void countNode();
};

#endif