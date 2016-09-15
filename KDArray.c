/*
 * KDArray.c
 *
 *  Created on: Jul 22, 2016
 *      Author: adigrabow
 */
#include "KDArray.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

/******************
 Logger Messages
 *****************/
#define LOGGER_ERROR_KDARRAY_NULL ("Given KDArray is NULL\n")
#define LOGGER_ERROR_KDARRAY_INIT_NEGATIVE_SIZE ("KDArray Init function was called with negative size.\n")
#define LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SPPOINT ("Failed to allocate memory for SPPoint array.\n")
#define LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_KDARRAY ("Failed to allocate memory for KDArray.\n")
#define LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_MATRIX ("Failed to allocate memory for matrix.\n")
#define LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SORTED_POINT_ARR ("Failed to allocate memory for SortedPointArr.\n")
#define LOGGER_ERROR_FUNCTION_ARGUMENTS_FAILED_TO_MEET_CONSTRAINTS ("Function arguments failed to meet constraints.\n")
#define LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY ("Failed to allocate memory.\n")


/****************************
 Help Functions Declarations
 ***************************/
/*
 * This function is called only from 'Split' and since no sorting is needed,
 * it creates a new KDArray in O(n) time.
 *
 * @param	arr				the kdArray
 * @param	X				help array (see more details in PDF)
 * @param	map				map array (see more details in PDF)
 * @param	side			a flag, 0 iff left side and 1 iff right side
 *
 * @return	kdArray			on success the kdarray
 * 								on failure NULL
 ***/
kdArray initFromSplit(kdArray arr, int* X,int * map,int side);


/*
void copySPPointArray(SPPoint* destArray,SPPoint* srcArray, int size );
void destroyCopiedSPPointArray(SPPoint* array, int size);
 void destroyMatrix (int** mat, int arrayDim );
 */

/**********************************************************************/




int COOR = 0; /* global? */


struct SPKDArray {
	SPPoint* pointArray;
	int size; /*the number of points int pointArray*/
	int dim; /*num of dimensions of KD-Tree (=num of coor of each point)*/
	int** mat; /* this is the d*size 2d-array that will be sorted by the coor of each row*/
};

/* In order to sort the points - temp struct */
typedef struct sortedPoint
{
	int index; /* contains the index of the point in pointArray */
	double data; /* contain a single double data[coor] */
}sortedPoint;

typedef struct sortedPoint * sortedPointsArr;
/* in order to create map1, map2 - every point has its original index in P */
typedef struct pointWithPIndex
{
	int index; /* contains the index of the point in pointArray */
	SPPoint point; /* contain a single double data[coor] */
}pointWithPIndex;

typedef struct pointWithPIndex * pointsWithIndexArr;


/********
 Getters
 ********/

int getSizeFromKDArray(kdArray arr){

	if (NULL == arr) {
		spLoggerPrintError(LOGGER_ERROR_KDARRAY_NULL,__FILE__, __func__, __LINE__ );
		return -1;
	}
	return arr->size;
}

SPPoint* getPointArrayFromKDArray(kdArray arr){

	if (NULL == arr) {
		spLoggerPrintError(LOGGER_ERROR_KDARRAY_NULL,__FILE__, __func__, __LINE__ );
		return NULL;
	}
	SPPoint * result = (SPPoint *) malloc (sizeof(SPPoint) * getSizeFromKDArray(arr));
	if (result == NULL){
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SPPOINT,__FILE__, __func__, __LINE__ );
		return NULL;
	}
	copySPPointArray(result, arr->pointArray,getSizeFromKDArray(arr));

	return result;
}

int** getMatrixFromKDArray(kdArray arr){

	if (NULL == arr) {
		spLoggerPrintError(LOGGER_ERROR_KDARRAY_NULL,__FILE__, __func__, __LINE__ );
		return NULL;
	}
	return arr->mat;
}

int getDimFromKDArray(kdArray arr){

	if (NULL == arr) {
		spLoggerPrintError(LOGGER_ERROR_KDARRAY_NULL,__FILE__, __func__, __LINE__ );
		return -1;
	}
	return arr->dim;
}

/************************
 Functions Implementation
 ************************/

void copySPPointArray(SPPoint* destArray,SPPoint* srcArray, int size ) {

	if (NULL == destArray || NULL == srcArray) {
		return;
	}

	for (int i = 0; i < size; i++) {
		destArray[i] = spPointCopy(srcArray[i]);
	}

	return;

}

void destroyCopiedSPPointArray(SPPoint* array, int size) {

	if (NULL == array) {
		return;
	}

	for (int i = 0; i < size; i++) {
		spPointDestroy(array[i]);
	}

	return;

}

void destroyMatrix (int** mat, int arrayDim ) {

	if (NULL == mat) {
		return;
	}

	for (int i = 0; i < arrayDim; i++) {
		free(mat[i]);
	}
	return;
}



kdArray Init(SPPoint* arr, int size){

	if (NULL == arr) {
		spLoggerPrintError(LOGGER_ERROR_KDARRAY_NULL,__FILE__, __func__, __LINE__ );
		return NULL;
	}

	if(size <= 0) {
		spLoggerPrintError(LOGGER_ERROR_KDARRAY_INIT_NEGATIVE_SIZE,__FILE__, __func__, __LINE__ );
		return NULL;
	}

	/***********************
	* Variable Declarations
	***********************/

	SPPoint* copiedArr = NULL;
	SPPoint* tempArray = NULL;
	kdArray array = NULL;
	int** mat = NULL;

	/*************************/


	/*Copy the array you received in the init function*/
	copiedArr = (SPPoint*)malloc(sizeof(SPPoint)* size);

	if (NULL == copiedArr) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SPPOINT,__FILE__, __func__, __LINE__ );
		return NULL;
	}

	copySPPointArray(copiedArr, arr, size);

	tempArray = (SPPoint*)malloc(sizeof(SPPoint)* size); /* for sorting */

	if(NULL == tempArray) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SPPOINT,__FILE__, __func__, __LINE__ );
		destroyCopiedSPPointArray(copiedArr, size);	
		free(copiedArr);
		return NULL;
	}

	copySPPointArray(tempArray, arr, size);


	array = (kdArray)malloc(sizeof(struct SPKDArray));

	if(NULL == array) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_KDARRAY,__FILE__, __func__, __LINE__ );
		destroyCopiedSPPointArray(copiedArr, size);
		destroyCopiedSPPointArray(tempArray, size);
		free(copiedArr);
		free(tempArray);
		return NULL;
	}

	array->size = size;
	array->pointArray = copiedArr;
	array->dim = spPointGetDimension(copiedArr[0]);

	mat = (int**)malloc(sizeof(int*)*(array->dim)); /* Allocate mat */
	if(NULL == mat) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_MATRIX,__FILE__, __func__, __LINE__ );
		destroyCopiedSPPointArray(copiedArr, size);
		destroyCopiedSPPointArray(tempArray, size);
		free(copiedArr);
		free(tempArray);
		free(array);
		return NULL;
	}

	for (int i = 0; i < array->dim; i++) {

		mat[i] = (int*)malloc(sizeof(int)*size);

		if(mat[i] == NULL){
			spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_MATRIX,__FILE__, __func__, __LINE__ );
			for (int k = 0; k < i; k++){
				free(mat[k]);
			}
			destroyCopiedSPPointArray(copiedArr, size);
			destroyCopiedSPPointArray(tempArray,size);
			free(copiedArr);
			free(tempArray);
			free(array);
			free(mat);
			return NULL;
		}

		sortedPointsArr sortByIndexArray = (sortedPointsArr) malloc(size * sizeof(sortedPoint));

		if (NULL == sortByIndexArray) {
			spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SORTED_POINT_ARR,__FILE__, __func__, __LINE__ );
			destroyCopiedSPPointArray(copiedArr,size);
			destroyCopiedSPPointArray(tempArray, size);
			destroyMatrix(mat, array->dim);
			free(copiedArr);
			free(tempArray);
			free(array);
			free(mat);
			return NULL;
		}

		/* initialize the sorting array */
		for(int j = 0; j < size; j++)
		{
			sortByIndexArray[j].index = j;
			sortByIndexArray[j].data = spPointGetAxisCoor(array->pointArray[j], i);
		}

		/*	sort by data[i] */
		qsort(sortByIndexArray, size, sizeof(sortedPoint), compareSortPoints);

		for (int j = 0; j < size; j++)
		{
			mat[i][j] = sortByIndexArray[j].index;
		}

		/* free allocated memory for sorting */
		free(sortByIndexArray);

	}

	array->mat = mat;

	/*free all allocations*/
	destroyCopiedSPPointArray(tempArray,size);
	free(tempArray);

	return array;

}


int compareSortPoints(const void * ptr1, const void * ptr2) {

	sortedPoint * a = (sortedPoint *)ptr1;
	sortedPoint * b = (sortedPoint *)ptr2;

	if (a->data > b->data) {
		return 1;
	}
	else if (a->data < b->data) {
		return -1;
	}

	if (a->index > b->index) {
		return 1;
	}
	else if (a->index < b->index) {
		return -1;
	}


	return 0;
}


kdArray * Split(kdArray kdArr, int coor) { /* coor = number of dimension that we split by */

	if ((NULL == kdArr) || (coor < 0)) { /* check for invalid arguments */
		spLoggerPrintError(LOGGER_ERROR_FUNCTION_ARGUMENTS_FAILED_TO_MEET_CONSTRAINTS,__FILE__, __func__, __LINE__ );
		return NULL;
	}

	/***********************
	 * Variable Declarations
	 ***********************/

	int size = getSizeFromKDArray(kdArr);
	SPPoint * P = getPointArrayFromKDArray(kdArr);
	kdArray * result = (kdArray*) malloc(sizeof(kdArray) * 2); /* result array will include left and right array */
	int halfIndex = 0;
	int * X = NULL;
	int * map1 = NULL;
	int * map2 = NULL;
	SPPoint * P1 = NULL;
	SPPoint * P2 = NULL;
	kdArray Left;
	kdArray Right;
	int i;
	pointsWithIndexArr  P1_with_indexes = NULL;
	pointsWithIndexArr P2_with_indexes = NULL;

	/*************************/


	if (NULL == result) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_KDARRAY,__FILE__, __func__, __LINE__ );
		destroyCopiedSPPointArray(P,getSizeFromKDArray(kdArr));
		free(P);
		return NULL;
	}


	halfIndex = (size + 1) / 2; /* half is index n/2 rounded up */

	X = (int *) malloc(sizeof(int)*size);
	if (NULL == X) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,__FILE__, __func__, __LINE__ );
		destroyCopiedSPPointArray(P,getSizeFromKDArray(kdArr));
		free(P);
		free(result);
		return NULL;
	}

	map1 = (int *) malloc(sizeof(int)*size);
	if (NULL == map1) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,__FILE__, __func__, __LINE__ );
		destroyCopiedSPPointArray(P,getSizeFromKDArray(kdArr));
		free(P);
		free(X);
		free(result);
		return NULL;
	}

	map2 = (int *) malloc(sizeof(int)*size);
	if (NULL == map2) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,__FILE__, __func__, __LINE__ );
		destroyCopiedSPPointArray(P,getSizeFromKDArray(kdArr));
		free(P);
		free(X);
		free(result);
		free(map1);
		return NULL;
	}

	P1 = (SPPoint*)malloc(halfIndex * sizeof(SPPoint));
	if(NULL == P1) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,__FILE__, __func__, __LINE__ );
		destroyCopiedSPPointArray(P,getSizeFromKDArray(kdArr));
		free(P);
		free(X);
		free(result);
		free(map1);
		free(map2);
		return NULL;
	}

	P2 = (SPPoint*)malloc((size - halfIndex) * sizeof(SPPoint));
	if (NULL == P2) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,__FILE__, __func__, __LINE__ );
		destroyCopiedSPPointArray(P,getSizeFromKDArray(kdArr));
		free(P);
		free(X);
		free(result);
		free(map1);
		free(map2);
		destroyCopiedSPPointArray(P1,halfIndex);
		free(P1);
		return NULL;
	}


	/* if (side = 0) the we build the left half, (side = 1) for the right side. Used in initFromSplit(X,map-i,side) */
	int side;
	for (int i = 0; i < size; i++){ /* Build X array */ /* use getMatrix instead of kdArr->mat */
		if (i< halfIndex){
			/* elements in Left */
			X[(getMatrixFromKDArray(kdArr))[coor][i]] = 0;
		}
		else{ /* elements in Right */
			X[(getMatrixFromKDArray(kdArr))[coor][i]] = 1 ;
		}
	}

	i = 0;
	int k = 0;
	for (int j = 0; j < size; j++){
		if (X[j] == 0){
			P1[i] = spPointCopy(P[j]);
			i++;

		}else{
			P2[k] = spPointCopy(P[j]);
			k++;
		}
	}

	/* create P1 P2 with indexes in order to build map1 map2 */
	P1_with_indexes = (pointsWithIndexArr)malloc(halfIndex * sizeof(pointWithPIndex));
	if (!P1_with_indexes){
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,__FILE__, __func__, __LINE__ );
		destroyCopiedSPPointArray(P,getSizeFromKDArray(kdArr));
		free(P);
		free(X);
		free(result);
		free(map1);
		free(map2);
		destroyCopiedSPPointArray(P1,halfIndex);
		free(P1);
		destroyCopiedSPPointArray(P2,(size-halfIndex));
		free(P2);
		return NULL;
	}

	P2_with_indexes = (pointsWithIndexArr)malloc((size - halfIndex) * sizeof(pointWithPIndex));
	if (!P2_with_indexes){
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY,__FILE__, __func__, __LINE__ );
		destroyCopiedSPPointArray(P,getSizeFromKDArray(kdArr));
		free(P);
		free(X);
		free(result);
		free(map1);
		free(map2);
		destroyCopiedSPPointArray(P1,halfIndex);
		free(P1);
		destroyCopiedSPPointArray(P2,(size-halfIndex));
		free(P2);
		free(P1_with_indexes);
		return NULL;
	}
	i = 0;
	k = 0;
	for (int j = 0; j < size; j++) {
		if (X[j] == 0){
			P1_with_indexes[i].point = spPointCopy(P[j]);
			P1_with_indexes[i].index = j; /*index in P - main array */
			i++;
		}else{
			P2_with_indexes[k].point = spPointCopy(P[j]);
			P2_with_indexes[k].index = j;
			k++;
		}
	}


	/* Build map1, map2 - arrays including indexes of points if point is in map-i and (-1) otherwise */
	/* Build map1 */
	for (i = 0; i < size; i++){ /* init default values */
		map1[i] = -1;
	}
	for (i = 0; i < halfIndex; i++){
		map1[ P1_with_indexes[i].index] = i;
	}

	/* Build map2 */
	for (i = 0; i < size; i++){ /* init default values */
		map2[i] = -1;
	}
	for (i = 0; i < (size - halfIndex); i++){
		map2[ P2_with_indexes[i].index] = i;
	}

	/* free P1_with_indexes */
	for (i = 0; i < halfIndex; i++){
		spPointDestroy(P1_with_indexes[i].point);

	}
	free(P1_with_indexes);


	/* free P2_with_indexes */
	for (i = 0; i < (size - halfIndex); i++){
		spPointDestroy(P2_with_indexes[i].point);

	}
	free(P2_with_indexes);


	/* Build left and right */
	side = 0; /* indicator for left side in init function */
	Left = initFromSplit(kdArr,X,map1,side); /* create Left */
	side = 1; /* indicator for right side in init function */
	Right = initFromSplit(kdArr,X,map2,side); /* create Right */

	/* free allocations */

	free(X);
	free(map1);
	free(map2);
	destroyCopiedSPPointArray(P1,halfIndex);
	free(P1);
	destroyCopiedSPPointArray(P2,(size-halfIndex));
	free(P2);
	destroyCopiedSPPointArray(P,getSizeFromKDArray(kdArr));
	free(P);

	/* update result */
	result[0] = Left;
	result[1] = Right;
	return result;

}

kdArray initFromSplit(kdArray arr, int* X,int * map,int side){

	/***********************
	* Variable Declarations
	 ***********************/
	int i = 0;
	int size;
	SPPoint* copiedPointArr = NULL;
	kdArray array = NULL;
	SPPoint* pointArr = NULL;
	int** mat = NULL;
	int row = -1;
	int col = 0;
	/***********************/


	/* Calculate size of kdArray */
	if (side == 0) /* Left side size */
		size = (arr->size + 1) / 2;
	else /* Right side size */
		size = arr->size - ((arr->size + 1) / 2);

	/*Copy the point-array we received in the init function*/
	copiedPointArr = getPointArrayFromKDArray(arr);

	array = (kdArray)malloc(sizeof(struct SPKDArray)); /* output array */

	if(NULL == array) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_KDARRAY,__FILE__, __func__, __LINE__ );
		destroyCopiedSPPointArray(copiedPointArr, getSizeFromKDArray(arr));
		free(copiedPointArr);
		return NULL;
	}

	array->size = size;
	pointArr = (SPPoint*)malloc(sizeof(SPPoint)* size); /* allocate new point array */

	if(NULL == pointArr) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_SPPOINT,__FILE__, __func__, __LINE__ );
		destroyCopiedSPPointArray(copiedPointArr, getSizeFromKDArray(arr));
		free(copiedPointArr);
		destroyKdArray(array);
		return NULL;
	}

	/* for each index in X - if (X[i] == side ) then pointArr[ map[i] ] =  copiedPointArr[i] */
	for (i = 0; i < (arr->size); i++) {
		if (X[i] == side){
			pointArr[map[i]] = spPointCopy(copiedPointArr[i]);
		}
	}

	array->pointArray = pointArr;
	array->dim = spPointGetDimension(copiedPointArr[0]);
	mat = (int**)malloc(sizeof(int*)*(array->dim));

	if(NULL == mat) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_MATRIX,__FILE__, __func__, __LINE__ );
		destroyCopiedSPPointArray(copiedPointArr, getSizeFromKDArray(arr));
		free(copiedPointArr);
		free(pointArr);
		destroyKdArray(array);
		return NULL;
	}

	for(int i = 0; i < array->dim; i++){
		mat[i] = (int*)malloc(sizeof(int)*size);
		if(mat[i]==NULL){
			spLoggerPrintError(LOGGER_ERROR_FAILED_TO_ALLOCATE_MEM_FOR_MATRIX,__FILE__, __func__, __LINE__ );
			for (int k=0;k<i;k++){
				free(mat[k]);
			}
			destroyCopiedSPPointArray(copiedPointArr, getSizeFromKDArray(arr));
			free(copiedPointArr);
			free(pointArr);
			destroyKdArray(array);
			free(mat);
			return NULL;
		}
	}


	for(int i = 0; i < arr->dim ; i++){
		col = 0;
		row++;
		for(int j = 0; j < arr->size ; j++){
			/*build right side*/
			if((X[arr->mat[i][j]] == 1) && (side == 1)){
				mat[row][col] = map[arr->mat[i][j]];
				col++;

			}
			/*build left side*/
			if(X[arr->mat[i][j]] == 0 && (side == 0)){
				mat[row][col] = map[arr->mat[i][j]];
				col++;
			}
		}
	}

	destroyCopiedSPPointArray(copiedPointArr, getSizeFromKDArray(arr));
	free(copiedPointArr);

	array->mat = mat;
	return array;

}

void destroyKdArray(kdArray arr){

	if (NULL == arr) {
		return;
	}

	int i;
	for( i = 0; i < arr->dim; i++){
		free(arr->mat[i]);
	}

	free(arr->mat);

	destroyCopiedSPPointArray(arr->pointArray,arr->size);
	free(arr->pointArray);
	free(arr);
}
