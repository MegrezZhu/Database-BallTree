#include "BallTreeNode.h"
#include "Utility.h"
#include <cstring>

using namespace std;

int BallTreeNode::_tid = 0;
int leafCal = 0;

BallTreeNode::BallTreeNode() : left(nullptr), right(nullptr), data(nullptr), id(nullptr), size(0), dimension(-1), leftId(-1), rightId(-1) {
	tid = _tid++;
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
	node->center = ::center(n, d, data);				//调用Utility中的center函数求圆心
	node->radius = ::radius(node->center, n, d, data);  //调用Utility中的center函数求半径
	node->size = n;
	if (n <= N0) {	//叶子节点操作
		node->data = new list<float*>(data, data + n);
		node->id = new list<int>(id, id + n);
		node->isleaf = true;
	}
	else {	//非叶子节点操作
		node->isleaf = false;

		//调用Utility函数进行分割操作
		auto split = getSplitCenter(n, d, data);
		int numA = makeSplit(n, d, data, id, split);

		//通过递归直至非叶子节点变为叶子节点
		node->left = build(numA, d, data, id);
		node->right = build(n - numA, d, data + numA, id + numA);
		node->leftId = node->left->getId();
		node->rightId = node->right->getId();
	}
	return node;
}

float BallTreeNode::getBound(float* query) {
	return innerProduct(query, center, dimension) + radius * norm(query, dimension);
}

pair<char*, int> BallTreeNode::serialize() {
	char* data;
	if (isLeaf()) {
		int len = 17 + N0 * (4 + dimension * 4) + dimension * 4; // isleaf: 1; tid, dimension, radius, size: 4 each; center: d*4; ids: size*4; vectors: size*d*4
		data = new char[len];
		auto head = data;
		memcpy(data, &isleaf, 1);
		memcpy((data += 1), &tid, 4);
		memcpy((data += 4), &dimension, 4);
		memcpy((data += 4), center, dimension * 4);
		memcpy((data += dimension * 4), &radius, 4);
		memcpy((data += 4), &size, 4);
		int i = 0;
		for (auto _id : *this->id) {
			memcpy((data += 4), &_id, 4);
			i++;
		}
		i = 0;
		data += 4;
		for (auto vec : *this->data) {
			memcpy(data, vec, dimension * 4);
			data += dimension * 4;
			i++;
		}

		return make_pair(head, len);
	}
	else {
		int len = 21 + dimension * 4; // isleaf: 1; tid, dimension, leftId, rightId, radius: 4 each; center: dimension*4
		data = new char[len];
		auto head = data;

		memcpy(data, &isleaf, 1);
		memcpy((data += 1), &tid, 4);
		memcpy((data += 4), &dimension, 4);
		memcpy((data += 4), &leftId, 4);
		memcpy((data += 4), &rightId, 4);
		memcpy((data += 4), center, dimension * 4);
		memcpy((data += dimension * 4), &radius, 4);

		return make_pair(head, len);
	}
}

BallTreeNode* BallTreeNode::deserialize(const char* buffer) {
	auto head = buffer;
	auto node = new BallTreeNode();
	memcpy(&node->isleaf, buffer, 1);
	memcpy(&node->tid, (buffer += 1), 4);
	if (node->isLeaf()) {
		memcpy(&node->dimension, (buffer += 4), 4);
		node->center = new float[node->dimension];
		memcpy(node->center, (buffer += 4), node->dimension * 4);
		memcpy(&node->radius, (buffer += node->dimension * 4), 4);
		memcpy(&node->size, (buffer += 4), 4);
		auto ids = new int[node->size];
		auto vecs = new float[node->size * node->dimension];
		memcpy(ids, (buffer += 4), node->size * 4);
		memcpy(vecs, (buffer += node->size * 4), node->size * node->dimension * 4);
		node->id = new list<int>(ids, ids + node->size);
		node->data = new list<float*>();
		for (int i = 0; i < node->size; i++) {
			node->data->push_back(vecs + i * node->dimension);
		}
		delete[] ids;
		// delete[] vecs;
	}
	else {
		memcpy(&node->dimension, (buffer += 4), 4);
		memcpy(&node->leftId, (buffer += 4), 4);
		memcpy(&node->rightId, (buffer += 4), 4);
		node->center = new float[node->dimension];
		memcpy(node->center, (buffer += 4), node->dimension * 4);
		memcpy(&node->radius, (buffer += node->dimension * 4), 4);
		node->left = node->right = nullptr;
	}
	return node;
}
