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
}

bool BallTree::buildTree(int n, int d, float** data, int* id) {
	if (!id) {
		leafCount = 0;
		id = new int[n];
		for (int i = 0; i < n; i++) id[i] = i + 1;
	}

	center = mean(n, d, data);
	radius = ::radius(center, n, d, data);
	size = n;
	if (n <= N0) {
		this->data = new list<float*>(data, data + n);
		this->id = new list<int>(id, id + n);
		leafCount++;
	}
	else {
		auto split = getSplitCenter(n, d, data);
		int numA = makeSplit(n, d, data, id, split);

		left = new BallTree();
		left->buildTree(numA, d, data, id);
		right = new BallTree();
		right->buildTree(n - numA, d, data + numA, id + numA);
	}
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