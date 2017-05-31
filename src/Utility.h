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
int makeSplit(int, int, float**, int*, pair<float*, float*>);

float innerProduct(float*, float*, int);

float distance2(float*, float*, int);

float distance(float*, float*, int);

float* mean(int, int, float**);

float radius(float*, int, int, float**);

float norm(float* vec, int d);

#endif
