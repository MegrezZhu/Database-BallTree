#ifndef __UTILITY_H
#define __UTILITY_H

#include <utility>
#define L 256

using namespace std;

bool read_data(
	int n,
	int d,
	float** &data,
	const char* file_name);

pair<float*, float*> getSplitCenter(int n, int d, float** data);

/*
	using in-place swapping to split elements into two group
	@return number of elements in the first group
*/
int makeSplit(int n, int d, float** data, int* id, pair<float*, float*> center);

float innerProduct(float* vecA, float* vecB, int d);

float distance2(float* vecA, float* vecB, int d);

float distance(float* vecA, float* vecB, int d);

float* mean(int n , int d, float** data);

float radius(float* center, int n, int d, float** data);

float norm(float* vec, int d);

float* getFarestVector(float* ori, int n, int d, float** data);

#endif
