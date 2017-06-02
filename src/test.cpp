#include <iostream>
#include <ctime>

#include "BallTree.h"
#include "Utility.h"

// #define MINIMAL
#define NETFLIX

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
#endif // NETFLIX


int main() {
	char data_path[L], query_path[L];
	char index_path[L], output_path[L];
	float** data = nullptr;
	float** query = nullptr;

	sprintf(data_path, "%s/src/dataset.txt", dataset);
	sprintf(query_path, "%s/src/query.txt", dataset);
	sprintf(index_path, "%s/index", dataset);
	sprintf(output_path, "%s/dst/answer.txt", dataset);

	srand(time(NULL));

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

	/*
	BallTree ball_tree2; 
	ball_tree2.restoreTree(index_path);
	for (int i = 0; i < qn; i++) {
		int index = ball_tree2.mipSearch(d, query[i]);
		fprintf(fout, "%d\n", index);
	}
	fclose(fout);
	*/

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