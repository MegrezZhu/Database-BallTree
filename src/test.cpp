#include <iostream>
#include <ctime>

#include "BallTree.h"
#include "Utility.h"
#include "Page.h"

#define MNIST

#ifdef MNIST
char dataset[L] = "Mnist";
int n = 60000, d = 50;
int qn = 1000;
#endif

#ifdef YAHOO
char dataset[L] = "Yahoo";
int n = 624, d = 300;
int qn = 1000;
#endif

#ifdef MINIMAL
char dataset[L] = "Minimal";
int n = 25, d = 2;
int qn = 1;
#endif

#ifdef NETFLIX
char dataset[L] = "Netflix";
int n = 17770, d = 50;
int qn = 1000;
#endif

#ifdef RANDOM
char dataset[L] = "Random";
int n = 1000, d = 10;
int qn = 10;
#endif

int mainTest();
void pageTest();
void readTest();
int simpleTest();
int naive();

int main() {
	srand(2333);
	mainTest();
	//simpleTest();
	//pageTest();
	//readTest();
	//naive();
	system("pause");
}

void pageTest() {
	cout << sizeof(int);
	auto page = Page::create(2, 5);
	page->setBySlot(0, "1234");
	page->setBySlot(1, "4567");
	page->writeBack("tmp/1.page");
}

int simpleTest() {
	char data_path[L], query_path[L];
	char index_path[L], output_path[L];
	float** data = nullptr;
	float** query = nullptr;

	sprintf(data_path, "%s/src/dataset.txt", dataset);
	sprintf(query_path, "%s/src/query.txt", dataset);
	sprintf(index_path, "%s/index", dataset);
	sprintf(output_path, "%s/dst/answer.txt", dataset);

	if (!read_data(n, d, data, data_path)) {
		system("pause");
		return 1;
	}

	BallTree ball_tree1;
	ball_tree1.buildTree(n, d, data);

	if (!read_data(qn, d, query, query_path));
	FILE* fout = fopen(output_path, "w");
	if (!fout) {
		printf("can't open %s!\n", output_path);
		system("pause");
		return 1;
	}

	for (int i = 0; i < qn; i++) {
		cout << i << ": ";
		int index = ball_tree1.mipSearch(d, query[i]);
		fprintf(fout, "%d\n", index);
	}
	fclose(fout);

	for (int i = 0; i < n; i++) {
		delete[] data[i];
	}

	for (int i = 0; i < qn; i++) {
		delete[] query[i];
	}

	system("pause");
	return 0;
}

int naive() {
	char data_path[L], query_path[L];
	char index_path[L], output_path[L];
	float** data = nullptr;
	float** query = nullptr;

	sprintf(data_path, "%s/src/dataset.txt", dataset);
	sprintf(query_path, "%s/src/query.txt", dataset);
	sprintf(index_path, "%s/index", dataset);
	sprintf(output_path, "%s/dst/answer_naive.txt", dataset);

	if (!read_data(n, d, data, data_path)) {
		system("pause");
		return 1;
	}

	if (!read_data(qn, d, query, query_path));
	FILE* fout = fopen(output_path, "w");
	if (!fout) {
		printf("can't open %s!\n", output_path);
		system("pause");
		return 1;
	}

	for (int i = 0; i < qn; i++) {
		int index = naiveSolve(query[i], n, d, data).first;
		fprintf(fout, "%d\n", index);
	}
	fclose(fout);

	/*
	for (int i = 0; i < qn; i++) {
	cout << i << ": ";
	int index = ball_tree1.mipSearch(d, query[i]);
	fprintf(fout, "%d\n", index);
	}
	fclose(fout);
	*/

	for (int i = 0; i < n; i++) {
		delete[] data[i];
	}

	for (int i = 0; i < qn; i++) {
		delete[] query[i];
	}

	return 0;
}

int mainTest() {
	char data_path[L], query_path[L];
	char index_path[L], output_path[L];
	float** data = nullptr;
	float** query = nullptr;

	sprintf(data_path, "%s/src/dataset.txt", dataset);
	sprintf(query_path, "%s/src/query.txt", dataset);
	sprintf(index_path, "%s/index", dataset);
	sprintf(output_path, "%s/dst/answer.txt", dataset);

	if (!read_data(n, d, data, data_path)) {
		system("pause");
		return 1;
	}

	BallTree ball_tree1;
	ball_tree1.buildTree(n, d, data);
	ball_tree1.storeTree(index_path);

	if (!read_data(qn, d, query, query_path));
	FILE* fout = fopen(output_path, "w");
	if (!fout) {
		printf("can't open %s!\n", output_path);
		system("pause");
		return 1;
	}

	BallTree ball_tree2; 
	ball_tree2.restoreTree(index_path);
	for (int i = 0; i < qn; i++) {
		int index = ball_tree2.mipSearch(d, query[i]);
		fprintf(fout, "%d\n", index);
	}
	fclose(fout);

	for (int i = 0; i < n; i++) {
		delete[] data[i];
	}

	for (int i = 0; i < qn; i++) {
		delete[] query[i];
	}
	
	return 0;
}

void readTest() {
	char data_path[L], query_path[L];
	char index_path[L], output_path[L];
	float** data = nullptr;
	float** query = nullptr;

	sprintf(data_path, "%s/src/dataset.txt", dataset);
	sprintf(query_path, "%s/src/query.txt", dataset);
	sprintf(index_path, "%s/index", dataset);
	sprintf(output_path, "%s/dst/answer.txt", dataset);

	if (!read_data(n, d, data, data_path)) {
		system("pause");
	}

	if (!read_data(qn, d, query, query_path));
	FILE* fout = fopen(output_path, "w");
	if (!fout) {
		printf("can't open %s!\n", output_path);
		system("pause");
	}

	BallTree ball_tree2;
	ball_tree2.restoreTree(index_path);
	for (int i = 0; i < qn; i++) {
		int index = ball_tree2.mipSearch(d, query[i]);
		fprintf(fout, "%d\n", index);
	}
	fclose(fout);

	/*
	for (int i = 0; i < qn; i++) {
	cout << i << ": ";
	int index = ball_tree1.mipSearch(d, query[i]);
	fprintf(fout, "%d\n", index);
	}
	fclose(fout);
	*/

	for (int i = 0; i < n; i++) {
		delete[] data[i];
	}

	for (int i = 0; i < qn; i++) {
		delete[] query[i];
	}
}
