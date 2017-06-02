#ifndef __BALL_TREE_H
#define __BALL_TREE_H

#include <list>
#include <fstream>
using namespace std;

// #define MINIMAL

#ifdef MINIMAL

#define N0 2

#endif
#ifndef MINIMAL

#define N0 20

#endif


class BallTree {
private:
	BallTree *left, *right;
	int size, dimension;
	list<float*> *data;
	list<int> *id;
	float *center;
	float radius;
	bool isleaf;

	pair<int, float> _mipSearch(int d, float* query);
	
public:
	BallTree();
	~BallTree();

	bool buildTree(
		int n,
		int d,
		float** data);

	static BallTree* build(
		int n,
		int d,
		float** data,
		int* id);

	bool storeTree(
		const char* index_path);

	bool restoreTree(
		const char* index_path);
	
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

	float getBound(float* query, int d);

	void* serialize();	// 构造写文件时的二进制流

	static BallTree* deserialize(void* buffer);

	bool isLeaf();
};

#endif
