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

	pair<int, float> _mipSearch(int d, float* query);
	
public:
	BallTree();
	~BallTree();

	bool buildTree(
		int n,
		int d,
		float** data,
		int* id = NULL);

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
};

#endif
