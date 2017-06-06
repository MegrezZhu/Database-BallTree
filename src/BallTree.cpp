#include "BallTree.h"
#include "Utility.h"
#include "Page.h"
#include "PagePool.h"
#include <algorithm>
#include <ctime>
#include <tuple>
#include <cstdint>

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
	for (int i = 0; i < n; i++) id[i] = i + 1;	//节点中每个点对应的id

	size = n;
	dimension = d;
	root = BallTreeNode::build(n, d, data, id);		//调用建立节点函数

	//countNode();

	delete[] id;
	return true;
}

int BallTree::mipSearch(int d, float* query) {
	visitedLeafNum = 0;
	auto startTime = clock();
	auto tmp = _mipSearch(root, query);
	printf("done in %d ms. leaf counted: %d. Total leaves: %d. MIP: %10lf ID: %d\n",
		clock() - startTime, visitedLeafNum, leafNum, tmp.second, tmp.first);
	return tmp.first;
}

pair<int, float> BallTree::_mipSearch(BallTreeNode* root, float* query) {
	if (root->isLeaf()) {	//深度到达叶子节点
		// leaf
		visitedLeafNum++;
		float maxProd = innerProduct(query, *root->data->begin(), root->dimension);	//调用Utility函数求内积
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

		return make_pair(maxi, maxProd);	//到达叶子节点并计算返回当前最大内积
	}
	else {	//非子节点
		if (!root->left) root->left = restoreNode(root->leftId);
		if (!root->right) root->right = restoreNode(root->rightId);
		float leftBound = root->left->getBound(query), rightBound = root->right->getBound(query);	//求左右最大可能内积
		//根据可能内积大小判断进去哪个子树
		if (leftBound > rightBound) {
			auto leftRes = _mipSearch(root->left, query);	//进入左子树寻找比较好的当前最大内积
			if (leftRes.second >= rightBound)	//根据当前最大内积大于某节点的最大可能内积，直接淘汰该节点
				return leftRes;
			auto rightRes = _mipSearch(root->right, query);
			return leftRes.second > rightRes.second ? leftRes : rightRes;	//回溯过程中仍需要遍历其他节点搜索最大内积
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
	if (!indexPage) indexPage = pagePool->create(4);	// 如果还未加载index页，则从硬盘中加载
														// pid, sid: 页号槽号各2字节
	auto nonLeafPage = pagePool->create(21 + dimension * 4);
														// 用于保存非叶结点
	auto leafPage = pagePool->create(17 + N0 * (4 + dimension * 4) + dimension * 4);
														// 用于保存叶子结点
	int leafCount = 0, nonLeafCount = 0;				// 统计页文件中已存的item个数
	char* buffer = new char[255];
	traverse(root, [this, buffer, &leafCount, &leafPage, &nonLeafCount, &nonLeafPage, indexPath](BallTreeNode *node) {
		bool leaf = node->isLeaf();
		if (node->isLeaf()) {
			if (leafCount == leafPage->getCapacity()) { // 保存叶结点的页已满、新建一页
				sprintf(buffer, "%s/%d.page", indexPath.c_str(), leafPage->getId());
				leafPage->writeBack(buffer);
				pagePool->remove(leafPage->getId());
				leafPage = pagePool->create(17 + N0 * (4 + dimension * 4) + dimension * 4);
				leafCount = 0;
			}
			int16_t pid = leafPage->getId(), sid = leafCount;	// 用16位整数存，节省空间
			memcpy(buffer, &pid, 2);
			memcpy(buffer + 2, &sid, 2);
			indexPage->setBySlot(node->getId(), buffer);		// 存id -> <pid,sid> 的索引
			leafPage->setBySlot(leafCount++, node->serialize().first);
		}
		else {
			if (nonLeafCount == nonLeafPage->getCapacity()) {	// 保存非叶结点的页已满则新建
				sprintf(buffer, "%s/%d.page", indexPath.c_str(), nonLeafPage->getId());
				nonLeafPage->writeBack(buffer);
				pagePool->remove(nonLeafPage->getId());
				nonLeafPage = pagePool->create(21 + dimension * 4);
				nonLeafCount = 0;
			}
			int16_t pid = nonLeafPage->getId(), sid = nonLeafCount;
			memcpy(buffer, &pid, 2);
			memcpy(buffer + 2, &sid, 2);
			indexPage->setBySlot(node->getId(), buffer);
			nonLeafPage->setBySlot(nonLeafCount++, node->serialize().first);
		}
	}); 
	sprintf(buffer, "%s/%d.page", indexPath.c_str(), indexPage->getId());
	indexPage->writeBack(buffer);					// 写回索引页
	delete[] buffer;
	printf("storage done.\n");
	return true;
}

bool BallTree::restoreTree(const string& indexPath) {
	pagePool->setBaseDir(indexPath);
	indexPage = pagePool->createFromFile(0);		// 0号页文件为索引页
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
	bool isLeaf = false;	// not used anymore
	int16_t pid, sid;
	memcpy(&pid, buffer, 2);
	memcpy(&sid, buffer + 2, 2);
	return make_tuple(isLeaf, int(pid), int(sid));
}
