/*
 * KDArray.c
 *
 *  Created on: Jul 22, 2016
 *      Author: adigrabow
 */

# include "SPPoint.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "KDArray.h"

int COOR = 0; /* global? */


struct SPKDArray{
	SPPoint* pointArray;
	int size;
	int dim; /*num of dimensions of KD-Tree (=num of coor of each point)*/
	int** mat; /* this is the d*size array that will be sorted by the coor of each row*/
};


kdArray init(SPPoint* arr, int size){
	kdArray array = (kdArray)malloc(sizeof(struct SPKDArray));
	if(array == NULL){
		return NULL;
	}

	array->size = size;
	array->pointArray = arr;
	array->dim = spPointGetDimension(arr[0]);

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
		qsort(array->pointArray, array->size, sizeof(SPPoint), coorCompare);
		/* make sure that "index" field of SPPoint is the same as the order-index of the input array (Maayan knows :) )    */
		/* we changed pointArray - is this OKAY?*/

		for(int j = 0; j < size; j++){
			mat[i][j] = array->pointArray[j]->index;

			}
		}

	array->mat = mat;
	return array;

}

int coorCompare(const void * a, const void* b){
	SPPoint* p1 = (SPPoint*) a;
	SPPoint* p2 = (SPPoint*) b;

	return compareSPPoints(*p1,*p2,COOR);

}

int compareSPPoints(SPPoint p1, SPPoint p2 , int coordinate){

	return (p1->data[coordinate]) - (p2->data[coordinate]);

}




