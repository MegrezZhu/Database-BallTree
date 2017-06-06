#pragma once

#include "PagePool.h"

#include <list>
#include <fstream>
#include <functional>
using namespace std;

// #define MINIMAL

#ifdef MINIMAL

#define N0 2

#endif
#ifndef MINIMAL

#define N0 20

#endif

class BallTreeNode {
public:
	int size, dimension;	//��������С��γ��
	list<float*> *data;		//�ڵ��еĵ�ľ�������
	list<int> *id;			//�ڵ���ÿ�����id
	float *center;			//Բ��
	float radius;			//�뾶
	bool isleaf;			//�Ƿ�Ҷ�ӽڵ�
	int tid;				//�ڵ㱾��id
	int leftId, rightId;	//�����ӽڵ�id
	static int _tid;		

	BallTreeNode();
	BallTreeNode *left, *right;

	~BallTreeNode();

	//Node Building
	static BallTreeNode* build(
		int n,
		int d,
		float** data,
		int* id);

	static BallTreeNode* buildFromPage(int pid, int sid);

	// optional
	bool insertData(
		float* data);

	// optional
	bool deleteData(
		float* data);

	// optional
	bool buildQuadTree(
		int n,
		int d,
		float** data);

	float getBound(float* query);

	/* 
		���л�������д�ļ�ʱ�Ķ�������
		@return pair<����ͷָ�룬���ݳ���>
	*/
	pair<char*, int> serialize();	

	/*
		�����л����Ӷ��������лָ�һ�����
	*/
	static BallTreeNode* deserialize(const char* buffer);

	bool isLeaf() { return isleaf; }

	const int& getId() { return tid; }
};
