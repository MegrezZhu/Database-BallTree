#ifndef __BALL_TREE_H
#define __BALL_TREE_H

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

public:
	BallTree();
	~BallTree();

	bool buildTree(
		int n,
		int d,
		float** data);

	static BallTree* buildFromPage(int pid, int slotId);

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

	void traverse(function<void(BallTreeNode *node)> func);

	void countNode();
};

#endif
