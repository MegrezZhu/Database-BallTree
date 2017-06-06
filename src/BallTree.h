#pragma once

#include "BallTreeNode.h"
#include "Page.h"
#include <list>
#include <fstream>
#include <functional>
using namespace std;

class BallTree {
private:
	int size, dimension;  //��������С�Լ�γ��
	BallTreeNode* root;	  //���ڵ�

	int leafNum, visitedLeafNum;	//Ҷ�ӽڵ���Ŀ�Լ����������з��ʵĽڵ���Ŀ

	// the page that store tid - <pid, sid> pairs
	Page *indexPage;	//ר�����ڴ�ȡÿ���ڵ��tid��ӦѰ���Ǹ�page���Ǹ��ۺ�

	pair<int, float> _mipSearch(BallTreeNode* root, float* query);		//��ѯ����

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
