#include <cstdio>
#include <cmath>
#include <algorithm>

#include "Utility.h"

using namespace std;

bool read_data(
	int n,
	int d,
	float** &data,
	const char* file_name)
{
	FILE* fin = fopen(file_name, "r");
	if (!fin) {
		printf("%s doesn't exist!\n", file_name);
		return false;
	}

	int id;
	data = new float*[n];
	for (int i = 0; i < n; i++) {
		data[i] = new float[d];
		fscanf(fin, "%d", &id);
		for (int j = 0; j < d; j++) {
			fscanf(fin, "%f", &data[i][j]);
		}
	}

	printf("Finish reading %s\n", file_name);
	fclose(fin);

	return true;
}

float innerProduct(float *a, float *b, int d) {
	float res = 0;
	for (int i = 0; i < d; i++) res += a[i] * b[i];
	return res;
}

float distance2(float *a, float *b, int d) {
	float dis2 = 0;
	for (int i = 0; i < d; i++) dis2 += powf(a[i] - b[i], 2);
	return dis2;
}

float distance(float *a, float *b, int d) {
	return sqrt(distance2(a, b, d));
}

float* mean(int n, int d, float** data) {
	float *res = new float[d] {.0f};

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < d; j++) {
			res[j] += data[i][j];
		}
	}
	for (int i = 0; i < d; i++) res[i] /= n;

	return res;
}

float* center(int n, int d, float** data) {
	/*
	auto split = getSplitCenter(n, d, data);
	float* res = new float[d];
	for (int i = 0; i < d; i++) res[i] = (split.first[i] + split.second[i]) / 2;
	return res;
	*/
	return mean(n, d, data);
}

float radius(float* center, int n, int d, float** data) {
	float res = 0;
	for (int i = 0; i < n; i++) {
		res = max(res, distance2(center, data[i], d));
	}
	return sqrtf(res);
}

float norm(float* vec, int d) {
	float dis2 = 0;
	for (int i = 0; i < d; i++) dis2 += powf(vec[i], 2);
	return sqrt(dis2);
}

float* getFarestVector(float* ori, int n, int d, float** data) {
	float* maxV = data[0];
	float maxD = distance2(maxV, ori, d);
	for (int i = 1; i < n; i++) {
		auto tmp = distance2(data[i], ori, d);
		if (tmp > maxD) {
			maxD = tmp;
			maxV = data[i];
		}
	}
	return maxV;
}

pair<float*, float*> getSplitCenter(int n, int d, float** data) {
	float *randOne = data[rand() % n];

	float *A = getFarestVector(randOne, n, d, data);
	float *B = getFarestVector(A, n, d, data);

	return make_pair(A, B);
}

int makeSplit(int n, int d, float** data, int* id, pair<float*, float*> centers) {
	int numA = 0, numB = 0;
	while (numA + numB < n) {
		if (distance2(data[numA], centers.first, d) > distance2(data[numA], centers.second, d)) {
			++numB;
			swap(data[numA], data[n - numB]);
			swap(id[numA], id[n - numB]);
		}
		else {
			++numA;
		}
	}

	return numA;
}


pair<int, float> naiveSolve(float* query, int n, int d, float** data) {
	int maxi = 0;
	float MIP = innerProduct(query, data[0], d);
	for (int i = 1; i < n; i++) {
		auto prod = innerProduct(query, data[i], d);
		if (prod > MIP) {
			MIP = prod;
			maxi = i;
		}
	}
	return make_pair(maxi + 1, MIP);
}
