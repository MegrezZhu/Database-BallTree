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
	int size, dimension;
	list<float*> *data;
	list<int> *id;
	float *center;
	float radius;
	bool isleaf;
	int tid;
	int leftId, rightId;
	static int _tid;

	BallTreeNode();
	BallTreeNode *left, *right;

	~BallTreeNode();

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

	pair<char*, int> serialize();	// 构造写文件时的二进制流

	static BallTreeNode* deserialize(const char* buffer);

	bool isLeaf() { return isleaf; }

	const int& getId() { return tid; }
};
