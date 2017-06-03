#include "BallTreeNode.h"
#include "Utility.h"
#include "Page.h"
#include <algorithm>
#include <ctime>

using namespace std;

int BallTreeNode::_tid = 0;
int leafCal = 0;
int leafCount = 0;

BallTreeNode::BallTreeNode() : left(nullptr), right(nullptr), data(nullptr), id(nullptr), size(0), dimension(-1) {
	tid = ++_tid;
}

BallTreeNode::~BallTreeNode() {
	if (left) delete left;
	if (right) delete right;

	// self-release
	delete[] center;
	delete data;
	delete id;
}

BallTreeNode* BallTreeNode::build(int n, int d, float** data, int* id) {
	auto node = new BallTreeNode();
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

int BallTreeNode::mipSearch(float* query) {
	leafCal = 0;
	auto startTime = clock();
	auto tmp = _mipSearch(query).first;
	printf("done in %d ms. leaf counted: %d. Total leaves: %d\n", clock() - startTime, leafCal, leafCount);
	return tmp;
}

pair<int, float> BallTreeNode::_mipSearch(float* query) {
	if (data) {
		// leaf
		leafCal++;
		float maxProd = innerProduct(query, *data->begin(), dimension);
		int maxi = *id->begin();
		auto itData = ++data->begin();
		auto itId = ++id->begin();
		while (itData != data->end()) {
			float prod = innerProduct(query, *itData, dimension);
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
		float leftBound = left->getBound(query), rightBound = right->getBound(query);
		if (leftBound > rightBound) {
			auto leftRes = left->_mipSearch(query);
			if (leftRes.second >= rightBound)
				return leftRes;
			auto rightRes = right->_mipSearch(query);
			return leftRes.second > rightRes.second ? leftRes : rightRes;
		}
		else {
			auto rightRes = right->_mipSearch(query);
			if (rightRes.second >= leftBound)
				return rightRes;
			auto leftRes = left->_mipSearch(query);
			return leftRes.second > rightRes.second ? leftRes : rightRes;
		}
	}
}

float BallTreeNode::getBound(float* query) {
	return innerProduct(query, center, dimension) + radius * norm(query, dimension);
}

void BallTreeNode::traverse(function<void(BallTreeNode *node)> func) {
	func(this);
	if (left) left->traverse(func);
	if (right) right->traverse(func);
}

pair<char*, int> BallTreeNode::serialize() {
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

BallTreeNode* BallTreeNode::deserialize(void* buffer) {
	return NULL;
}
