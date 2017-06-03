#pragma once

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
private:
	BallTreeNode *left, *right;
	int size, dimension;
	list<float*> *data;
	list<int> *id;
	float *center;
	float radius;
	bool isleaf;
	int tid;
	static int _tid;

	pair<int, float> _mipSearch(float* query);

	BallTreeNode();
public:
	~BallTreeNode();

	static BallTreeNode* build(
		int n,
		int d,
		float** data,
		int* id);

	int mipSearch(
		float* query);

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

	static BallTreeNode* deserialize(void* buffer);

	bool isLeaf() { return isleaf; }

	void traverse(function<void(BallTreeNode *node)> func);

	const int& getId() { return tid; }
};
