/*
 * KDArray.c
 *
 *  Created on: Jul 22, 2016
 *      Author: adigrabow
 */

#include "SPPoint.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "KDArray.h"
#include <string.h>

int COOR = 0; /* global? */


struct SPKDArray{
	SPPoint* pointArray;
	int size;
	int dim; /*num of dimensions of KD-Tree (=num of coor of each point)*/
	int** mat; /* this is the d*size 2d-array that will be sorted by the coor of each row*/
};


kdArray Init(SPPoint* arr, int size){
	/*Copy the array you received in the init function*/

	SPPoint* copiedArr = (SPPoint*)malloc(sizeof(SPPoint)* size);
	memcpy(copiedArr,arr, sizeof(SPPoint)* size);


	kdArray array = (kdArray)malloc(sizeof(struct SPKDArray));
	if(array == NULL){
		return NULL;
	}

	array->size = size;
	array->pointArray = copiedArr;

	array->dim = spPointGetDimension(copiedArr[0]);

	int** mat = (int**)malloc(sizeof(int*)*(array->dim));
	if(mat == NULL){
		free(array);
		return NULL;
		}

	for(int i = 0; i < array->dim; i++){
		mat[i] = (int*)malloc(sizeof(int)*size);
		if(mat[i]==NULL){
			 for (int k=0;k<i;k++){
				free(mat[k]);
				}
				free(mat);
				return NULL;
		}

		COOR = i;
		//printf("Init before qsort\n");

		qsort(array->pointArray, array->size, sizeof(SPPoint), coorCompare);


		/* make sure that "index" field of SPPoint is the same as the order-index of the input array (Maayan knows :) )    */
		/* we changed pointArray - is this OKAY?*/


		for(int j = 0; j < size; j++){
			mat[i][j] = spPointGetIndex(array->pointArray[j]);

			}

		}
	//printf("finished Init\n");

	array->mat = mat;
	return array;

}

int coorCompare(const void * a, const void* b){

	SPPoint p1 = *(SPPoint*) a;
	SPPoint p2 = *(SPPoint*) b;

/*

	printf("p1[0] = %f\n",spPointGetAxisCoor(p1, 0));
	printf("p2[0] = %f\n",spPointGetAxisCoor(p2, 0));
	printf("p1 dim = %d\n",spPointGetDimension(p1));
	printf("p2 dim = %d\n",spPointGetDimension(p2));*/



	return compareSPPoints(p1,p2,COOR);


}


int compareSPPoints(SPPoint p1, SPPoint p2 , int coordinate){
//	printf("entered compareSPPoints\n");
//	printf("coordinate = %d\n", coordinate);
	double num1 = spPointGetAxisCoor(p1,coordinate);
	double num2 = spPointGetAxisCoor(p2,coordinate);


	//printf("compareSPPoints result is %f\n", num1 - num2);
	if(num1 > num2){
		return 1;
	}
	if(num1 < num2){
		return -1;
	}
	return 0;


}





int** getMatrixFromKDArray(kdArray arr){
	assert(arr != NULL);
	return arr->mat;
}

int getDimFromKDArray(kdArray arr){
	assert(arr != NULL);
	return arr->dim;
}



/* important comments:
 * Add documentation in header file.
 * init:
 * 	2. Check invalid arguments at first.
 * 	3. Create destroy function.
 * 	4. I included point.c because eclipse didn't recognize the struct.
 * Split:
 * 	1. Check types: requested SPKDArray and not kdArray... maybe we should change typedef?
 * 	2. Use getters (and create them for each field in struct)
 *
 */

kdArray * Split(kdArray kdArr, int coor) { /* coor = number of dimension that we split by */
	if ((!kdArr) || (coor < 0)) { /* check for invalid arguments */
		return NULL;
	}

	kdArray * result = (kdArray*) malloc(sizeof(struct SPKDArray) * 2); /* result array will include left and right array */
	if (!result) { /* Allocation error */
		return NULL;
	}
	int size = kdArr->size;
	int halfIndex = (size + 1) / 2; /* half is index n/2 rounded up */
	int flag = 0; /* for creating maps : 0=not found in pi, else 1 */
	//TODO for all the arrays: maybe use malloc?

	int * X = (int *) malloc(sizeof(int)*size);
	if (!X) { /* Allocation error */
		free(result);
		return NULL;
	}

	int * map1 = (int *) malloc(sizeof(int)*size);
	if (!map1) { /* Allocation error */
		free(X);
		free(result);
		return NULL;
	}

	int * map2 = (int *) malloc(sizeof(int)*size);
	if (!map2) { /* Allocation error */
		free(map1);
		free(X);
		free(result);
		return NULL;
	}

	SPPoint * P1;
	SPPoint * P2;
	kdArray Left;
	kdArray Right;
	int i;
	/* if (side = 0) the we build the left half, (side = 1) for the right side. Used in initFromSplit(X,map-i,side) */
	int side;

	for (int i=0; i<size; i++){ /* Build X array */ /* use getMatrix instead of kdArr->mat */
		if (i< halfIndex) /* elements in Left */
			X[ (kdArr->mat[coor][i]) ] = 0 ;
		else /* elements in Right */
			X[ (kdArr->mat[coor][i]) ] = 1 ;
	}


	//for (int i=0; i<halfIndex; i++){ /* size of P1 is halfIndex */
	//P1[i] = kdArr->pointArray(kdArr->mat[coor][i]);

	/* Build P1, P2  - arrays including left and right, elements are points */
	i=0;
	for (int j=0; j< size; j++){
		if (X[j] == 0){
			P1[i] = kdArr->pointArray[j];
			i++;
		}
	}
	i=0;
	for (int j=0; j< size; j++){
		if (X[j] == 1){
			P2[i] = kdArr->pointArray[j];
			i++;
		}
	}


	/* Build map1, map2 - arrays including indexes of points if point is in map-i and (-1) otherwise */
	for (int i=0; i<size; i++){ /* map1 */
		flag = 0; /* at the beginning of iteration - point not found yet */
		for (int j=0; j < halfIndex; j++){ /* iterate in p1 */
			if (spPointGetIndex(kdArr->pointArray[i]) == spPointGetIndex(P1[j])){
				flag = 1;
				map1[i] = j;
			}
		}

		if (flag == 0){ /* not found in p-i */
			map1[i] = -1;
		}

	}

	for (int i=0; i<size; i++){ /* map2 */
		flag = 0; /* at the beginning of iteration - point not found yet */
		for (int j=0; j < (size-halfIndex); j++){ /* iterate in p2 */
			if (spPointGetIndex(kdArr->pointArray[i]) == spPointGetIndex(P2[j])){
				flag = 1;
				map2[i] = j;
			}
		}
		if (flag == 0){ /* not found in p-i */
			map2[i] = -1;
		}

	}

	/* Build left and right */
	side = 0; /* indicator for left side in init function */
	Left = initFromSplit(kdArr,X,map1,side); /* create Left */
	side = 1; /* indicator for right side in init function */
	Right = initFromSplit(kdArr,X,map2,side); /* create Right */

	/* free allocations */
	free(X);
	free(map1);
	free(map2);

	/* update result */
	result[0] = Left;
	result[1] = Right;
	return result;

}

kdArray initFromSplit(kdArray arr, int* X,int * map,int side){
	int i=0;

	/* Calculate size of kdArray */
	int size;
	if (side ==0) /* Left side size */
		size = (arr->size + 1) / 2;
	else /* Right side size */
		size = arr->size - ((arr->size + 1) / 2);

	/*Copy the point-array we received in the init function*/
	SPPoint* copiedPointArr = (SPPoint*)malloc(sizeof(SPPoint)* arr->size); /* copy original point array */
	memcpy(copiedPointArr,arr->pointArray, sizeof(SPPoint)* arr->size);

	kdArray array = (kdArray)malloc(sizeof(struct SPKDArray)); /* output array */
	if(!array){ /* Allocation error */
		free(copiedPointArr);
		return NULL;
	}

	array->size = size;

	SPPoint* pointArr = (SPPoint*)malloc(sizeof(SPPoint)* size); /* allocate new point array */
	if(!pointArr){ /*Allocation error */
			free(copiedPointArr);
			free(array);
			return NULL;
		}

	/* for each index in X - if (X[i] == side ) then pointArr[ map[i] ] =  copiedPointArr[i] */
	for (i=0;i<arr->size;i++){
		if (X[i] == side)
			pointArr[ map[i] ] =  copiedPointArr[i];
	}
	array->pointArray = pointArr;


	array->dim = spPointGetDimension(copiedPointArr[0]);

	int** mat = (int**)malloc(sizeof(int*)*(array->dim));
	if(mat == NULL){
		free(copiedPointArr);
		free(pointArr);
		free(array);
		return NULL;
	}

	for(int i = 0; i < array->dim; i++){
		mat[i] = (int*)malloc(sizeof(int)*size);
		if(mat[i]==NULL){
			for (int k=0;k<i;k++){
				free(mat[k]);
			}
			free(copiedPointArr);
			free(pointArr);
			free(array);
			free(mat);
			return NULL;
		}

		for(int j = 0; j < size; j++){ /* insert updated indexes (according to p1 or p2) to the new mat */
			mat[i][j] = map[ arr->mat[i][j] ];

		}

	}
	/* maybe now we can use free(copiedPointArr) ?? */

	array->mat = mat;
	return array;

}
/*

void destroyKdArray(kdArray arr){

	for(int i = 0; i < array->dim; i++){ //maybe arr->dim ?
		free(arr->mat[i]);
	}
	free(arr->mat);
	free(arr->pointArray);
	free(arr);
}
*/


