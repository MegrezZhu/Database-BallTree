#include "BallTree.h"
#include "Utility.h"
#include <algorithm>
#include <ctime>

using namespace std;

int leafCal = 0;
int leafCount = 0;

BallTree::BallTree(): left(nullptr), right(nullptr), data(nullptr), id(nullptr), size(0), dimension(-1) {
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
	node->center = mean(n, d, data);
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

bool BallTree::storeTree(const char* path) {
	// TO-DO
	return true;
}

bool BallTree::restoreTree(const char* path) {
	// TO-DO
	return true;
}

bool BallTree::isLeaf() { return isleaf; }