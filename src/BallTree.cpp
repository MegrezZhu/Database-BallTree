#include "BallTree.h"
#include "Utility.h"
#include "Page.h"
#include <algorithm>
#include <ctime>

using namespace std;

BallTree::BallTree(): root(nullptr), size(0), dimension(-1), leafNum(0), indexPage(nullptr) {
}

BallTree::~BallTree() {
	if (root) delete root;
}

bool BallTree::buildTree(int n, int d, float** data) {
	int* id = new int[n];
	for (int i = 0; i < n; i++) id[i] = i + 1;

	size = n;
	dimension = d;
	root = BallTreeNode::build(n, d, data, id);

	countNode();

	delete[] id;
	return true;
}

int BallTree::mipSearch(int d, float* query) {
	// leafCal = 0;
	auto startTime = clock();
	auto tmp = root->mipSearch(query);
	// printf("done in %d ms. leaf counted: %d. Total leaves: %d\n", clock() - startTime, leafCal, leafCount);
	return tmp;
}

bool BallTree::storeTree(const string& indexPath) {
	if (!indexPage) indexPage = Page::create(9); // isLeaf: 1; pid, sid: 4
	auto nonLeafPage = Page::create(20 + dimension * 4);
	auto leafPage = Page::create(16 + N0 * (4 + dimension * 4) + dimension * 4);
	int leafCount = 0, nonLeafCount = 0;
	char* buffer = new char[255];
	traverse([this, buffer, &leafCount, &leafPage, &nonLeafCount, &nonLeafPage, indexPath](BallTreeNode *node) {
		bool leaf = node->isLeaf();
		if (node->isLeaf()) {
			if (leafCount == leafPage->getCapacity()) {
				sprintf(buffer, "%s/%d.page", indexPath.c_str(), leafPage->getId());
				leafPage->writeBack(buffer);
				delete leafPage;
				leafPage = Page::create(16 + N0 * (4 + dimension * 4) + dimension * 4);
				leafCount = 0;
			}
			memcpy(buffer, &leaf, 1);
			memcpy(buffer + 1, &leafPage->getId(), 4);
			memcpy(buffer + 5, &leafCount, 4);
			indexPage->setBySlot(node->getId(), buffer);
			leafPage->setBySlot(leafCount++, node->serialize().first);
		}
		else {
			if (nonLeafCount == nonLeafPage->getCapacity()) {
				sprintf(buffer, "%s/%d.page", indexPath.c_str(), nonLeafPage->getId());
				leafPage->writeBack(buffer);
				delete nonLeafPage;
				nonLeafPage = Page::create(20 + dimension * 4);
				nonLeafCount = 0;
			}
			memcpy(buffer, &leaf, 1);
			memcpy(buffer + 1, &nonLeafPage->getId(), 4);
			memcpy(buffer + 5, &nonLeafCount, 4);
			indexPage->setBySlot(node->getId(), buffer);
			nonLeafPage->setBySlot(nonLeafCount++, node->serialize().first);
		}
	}); 
	sprintf(buffer, "%s/%d.page", indexPath.c_str(), indexPage->getId());
	indexPage->writeBack(buffer);
	delete[] buffer;
	return true;
}

bool BallTree::restoreTree(const string& indexPath) {
	auto indexPage = Page::createFromFile(indexPath + "/0.page");

	return true;
}

void BallTree::traverse(function<void(BallTreeNode *node)> func) {
	root->traverse(func);
}

void BallTree::countNode() {
	int node = 0, leaf = 0;
	traverse([&node, &leaf](BallTreeNode *node) {
		node++;
		if (node->isLeaf()) leaf++;
	});
}