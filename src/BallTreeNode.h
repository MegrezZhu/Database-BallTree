#pragma once

#include "PagePool.h"

#include <list>
#include <fstream>
#include <functional>
using namespace std;

// #define MINIMAL

#ifdef MINIMAL

#define N0 2

#endif
#ifndef MINIMAL

#define N0 20

#endif

class BallTreeNode {
public:
	int size, dimension;	//数据量大小，纬度
	list<float*> *data;		//节点中的点的具体数据
	list<int> *id;			//节点中每个点的id
	float *center;			//圆心
	float radius;			//半径
	bool isleaf;			//是否叶子节点
	int tid;				//节点本身id
	int leftId, rightId;	//左右子节点id
	static int _tid;		

	BallTreeNode();
	BallTreeNode *left, *right;

	~BallTreeNode();

	//Node Building
	static BallTreeNode* build(
		int n,
		int d,
		float** data,
		int* id);

	static BallTreeNode* buildFromPage(int pid, int sid);

	// optional
	bool insertData(
		float* data);

	// optional
	bool deleteData(
		float* data);

	// optional
	bool buildQuadTree(
		int n,
		int d,
		float** data);

	float getBound(float* query);

	/* 
		序列化，构造写文件时的二进制流
		@return pair<数据头指针，数据长度>
	*/
	pair<char*, int> serialize();	

	/*
		反序列化，从二进制流中恢复一个结点
	*/
	static BallTreeNode* deserialize(const char* buffer);

	bool isLeaf() { return isleaf; }

	const int& getId() { return tid; }
};
