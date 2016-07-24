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

	printf("entered Init\n");

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

		printf("Init after qsort\n");

		/* make sure that "index" field of SPPoint is the same as the order-index of the input array (Maayan knows :) )    */
		/* we changed pointArray - is this OKAY?*/


		for(int j = 0; j < size; j++){
			mat[i][j] = spPointGetIndex(array->pointArray[j]);
				//	array->pointArray[j]->index;

			}
		printf("Init after for with index\n");

		}
	//printf("finished Init\n");

	array->mat = mat;
	return array;

}

int coorCompare(const void * a, const void* b){
	printf("entered coorCompare\n");

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






