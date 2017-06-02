#include "BallTree.h"
#include "Utility.h"
#include "Page.h"
#include <algorithm>
#include <ctime>

using namespace std;

int BallTree::_tid = 0;
int leafCal = 0;
int leafCount = 0;

BallTree::BallTree(): left(nullptr), right(nullptr), data(nullptr), id(nullptr), size(0), dimension(-1) {
	tid = ++_tid;
}

BallTree::~BallTree() {
	if (left) delete left;
	if (right) delete right;
	
	// self-release
	delete[] center;
	delete data;
	delete id;
}

BallTree* BallTree::build(int n, int d, float** data, int* id) {
	auto node = new BallTree();
	node->dimension = d;
	node->center = ::center(n, d, data);
	node->radius = ::radius(node->center, n, d, data);
	node->size = n;
	if (n <= N0) {
		node->data = new list<float*>(data, data + n);
		node->id = new list<int>(id, id + n);
		leafCount++;
		node->isleaf = true;
	}
	else {
		node->isleaf = false;
		auto split = getSplitCenter(n, d, data);
		int numA = makeSplit(n, d, data, id, split);

		node->left = build(numA, d, data, id);
		node->right = build(n - numA, d, data + numA, id + numA);
	}
	return node;
}

bool BallTree::buildTree(int n, int d, float** data) {
	leafCount = 0;
	int* id = new int[n];
	for (int i = 0; i < n; i++) id[i] = i + 1;

	*this = *build(n, d, data, id);

	delete[] id;
	return true;
}

int BallTree::mipSearch(int d, float* query) {
	leafCal = 0;
	auto startTime = clock();
	auto tmp = _mipSearch(d, query).first;
	printf("done in %d ms. leaf counted: %d. Total leaves: %d\n", clock() - startTime, leafCal, leafCount);
	return tmp;
}

pair<int, float> BallTree::_mipSearch(int d, float* query) {
	if (data) {
		// leaf
		leafCal++;
		float maxProd = innerProduct(query, *data->begin(), d);
		int maxi = *id->begin();
		auto itData = ++data->begin();
		auto itId = ++id->begin();
		while (itData != data->end()) {
			float prod = innerProduct(query, *itData, d);
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
		float leftBound = left->getBound(query, d), rightBound = right->getBound(query, d);
		if (leftBound > rightBound) {
			auto leftRes = left->_mipSearch(d, query);
			if (leftRes.second >= rightBound)
				return leftRes;
			auto rightRes = right->_mipSearch(d, query);
			return leftRes.second > rightRes.second ? leftRes : rightRes;
		}
		else {
			auto rightRes = right->_mipSearch(d, query);
			if (rightRes.second >= leftBound)
				return rightRes;
			auto leftRes = left->_mipSearch(d, query);
			return leftRes.second > rightRes.second ? leftRes : rightRes;
		}
	}
}

float BallTree::getBound(float* query, int d) {
	return innerProduct(query, center, d) + radius * norm(query, d);
}

bool BallTree::storeTree(const string& indexPath) {
	auto indexPage = Page::create(9); // isLeaf: 1; pid, sid: 4
	auto nonLeafPage = Page::create(20 + dimension * 4);
	auto leafPage = Page::create(16 + N0 * (4 + dimension * 4) + dimension * 4);
	int leafCount = 0, nonLeafCount = 0;
	char* buffer = new char[255];
	traverse([this, indexPage, buffer, &leafCount, &leafPage, &nonLeafCount, &nonLeafPage, indexPath](BallTree *node) {
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
	// TO-DO
	return true;
}

bool BallTree::isLeaf() { return isleaf; }

void BallTree::traverse(function<void(BallTree *node)> func) {
	func(this);
	if (left) left->traverse(func);
	if (right) right->traverse(func);
}

pair<char*, int> BallTree::serialize() {
	char* data;
	if (isLeaf()) {
		int len = 16 + N0 * (4 + dimension * 4) + dimension * 4; // tid, isLeaf, radius, size: 4 each; center: d*4; ids: size*4; vectors: size*d*4
		data = new char[len];
		memcpy(data, (char*)&tid, 4);
		int _isleaf = isLeaf();
		memcpy(data + 4, (char*)&_isleaf, 4);
		memcpy(data + 8, (char*)center, dimension * 4);
		memcpy(data + 12, (char*)&radius, 4);
		memcpy(data + 16, (char*)&size, 4);
		int i = 0;
		for (auto _id : *this->id) {
			memcpy(data + 20 + i * 4, &id, 4);
			i++;
		}
		i = 0;
		for (auto vec : *this->data) {
			memcpy(data + 20 + dimension * 4 + i * dimension * 4, vec, dimension * 4);
			i++;
		}

		return make_pair(data, len);
	}
	else {
		int len = 20 + dimension * 4; // tid, isLeaf, leftId, rightId, radius: 4 each; center: dimension*4
		data = new char[len];
		memcpy(data, &tid, 4);
		int _isleaf = isLeaf();
		memcpy(data + 4, &_isleaf, 4);
		memcpy(data + 8, &left->tid, 4);
		memcpy(data + 12, &right->tid, 4);
		memcpy(data + 16, &radius, 4);
		memcpy(data + 20, center, dimension * 4);

		return make_pair(data, len);
	}
}

int BallTree::countLeaf() {
	if (isLeaf()) return 1;
	else return left->countLeaf() + right->countLeaf();
}

int BallTree::countNode() {
	if (isLeaf()) return 1;
	else return left->countLeaf() + right->countLeaf() + 1;
}