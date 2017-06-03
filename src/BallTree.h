#pragma once

#include "BallTreeNode.h"
#include "Page.h"
#include <list>
#include <fstream>
#include <functional>
using namespace std;

class BallTree {
private:
	int size, dimension;
	BallTreeNode* root;

	int leafNum, visitedLeafNum;

	// the page that store tid - <pid, sid> pairs
	Page *indexPage;

	pair<int, float> _mipSearch(BallTreeNode* root, float* query);

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

	void traverse(BallTreeNode *root, function<void(BallTreeNode *node)> func);

	BallTreeNode* restoreNode(int tid);

	void countNode();
};
