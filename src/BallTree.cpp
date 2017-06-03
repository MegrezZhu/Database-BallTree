#include "BallTree.h"
#include "Utility.h"
#include "Page.h"
#include "PagePool.h"
#include <algorithm>
#include <ctime>
#include <tuple>

using namespace std;

Page* getPage(const string &indexPath, int pid);
tuple<bool, int, int> readIndex(Page* indexPage, int id);

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
	visitedLeafNum = 0;
	auto startTime = clock();
	auto tmp = _mipSearch(root, query);
	printf("done in %d ms. leaf counted: %d. Total leaves: %d. MIP: %10lf ID: %d\n", clock() - startTime, visitedLeafNum, leafNum, tmp.second, tmp.first);
	return tmp.first;
}

pair<int, float> BallTree::_mipSearch(BallTreeNode* root, float* query) {
	if (root->isLeaf()) {
		// leaf
		visitedLeafNum++;
		float maxProd = innerProduct(query, *root->data->begin(), root->dimension);
		int maxi = *root->id->begin();
		auto itData = ++root->data->begin();
		auto itId = ++root->id->begin();
		while (itData != root->data->end()) {
			float prod = innerProduct(query, *itData, root->dimension);
			if (prod > maxProd) {
				maxProd = prod;
				maxi = *itId;
			}
			itData++;
			itId++;
		}

		return make_pair(maxi, maxProd);
	}
	else {
		if (!root->left) root->left = restoreNode(root->leftId);
		if (!root->right) root->right = restoreNode(root->rightId);
		float leftBound = root->left->getBound(query), rightBound = root->right->getBound(query);
		if (leftBound > rightBound) {
			auto leftRes = _mipSearch(root->left, query);
			if (leftRes.second >= rightBound)
				return leftRes;
			auto rightRes = _mipSearch(root->right, query);
			return leftRes.second > rightRes.second ? leftRes : rightRes;
		}
		else {
			auto rightRes = _mipSearch(root->right, query);
			if (rightRes.second >= leftBound)
				return rightRes;
			auto leftRes = _mipSearch(root->left, query);
			return leftRes.second > rightRes.second ? leftRes : rightRes;
		}
	}
}

bool BallTree::storeTree(const string& indexPath) {
	if (!indexPage) indexPage = pagePool->create(9); // isLeaf: 1; pid, sid: 4
	auto nonLeafPage = pagePool->create(21 + dimension * 4);
	auto leafPage = pagePool->create(17 + N0 * (4 + dimension * 4) + dimension * 4);
	int leafCount = 0, nonLeafCount = 0;
	char* buffer = new char[255];
	traverse(root, [this, buffer, &leafCount, &leafPage, &nonLeafCount, &nonLeafPage, indexPath](BallTreeNode *node) {
		bool leaf = node->isLeaf();
		if (node->isLeaf()) {
			if (leafCount == leafPage->getCapacity()) {
				sprintf(buffer, "%s/%d.page", indexPath.c_str(), leafPage->getId());
				leafPage->writeBack(buffer);
				pagePool->remove(leafPage->getId());
				leafPage = pagePool->create(16 + N0 * (4 + dimension * 4) + dimension * 4);
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
				nonLeafPage->writeBack(buffer);
				pagePool->remove(nonLeafPage->getId());
				nonLeafPage = pagePool->create(20 + dimension * 4);
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
	pagePool->setBaseDir(indexPath);
	indexPage = pagePool->createFromFile(0);
	root = restoreNode(0);
	return true;
}

void BallTree::traverse(BallTreeNode *root, function<void(BallTreeNode *node)> func) {
	if (!root) return;
	func(root);
	traverse(root->left, func);
	traverse(root->right, func);
}

void BallTree::countNode() {
	int nodeNum = 0, leafNum = 0;
	traverse(root, [&nodeNum, &leafNum](BallTreeNode *node) {
		node++;
		if (node->isLeaf()) leafNum++;
	});
	this->leafNum = leafNum;
}

BallTreeNode* BallTree::restoreNode(int tid) {
	auto info = readIndex(indexPage, tid);
	auto page = pagePool->get(get<1>(info));
	if (!page) page = pagePool->createFromFile(get<1>(info));
	auto node = BallTreeNode::deserialize(page->getBySlot(get<2>(info)));
	if (dimension == -1) dimension = node->dimension;
	return node;
}

Page* getPage(const string &indexPath, int pid) {
	auto p = pagePool->get(pid);
	if (p) return p;
	return pagePool->createFromFile(indexPath, pid);
}

tuple<bool, int, int> readIndex(Page* indexPage, int id) {
	auto buffer = indexPage->getBySlot(id);
	bool isLeaf;
	int pid, sid;
	memcpy(&isLeaf, buffer, 1);
	memcpy(&pid, buffer + 1, 4);
	memcpy(&sid, buffer + 5, 4);
	return make_tuple(isLeaf, pid, sid);
}
