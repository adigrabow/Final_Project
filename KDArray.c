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

int coor = 0; /* global? */


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
	array->dim = arr[0]->dim;

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
		coor = i;
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
	SPPoint** p1 = (SPPoint**) a;
	SPPoint** p2 = (SPPoint**) b;
	double d1 = p1->data[coor];
	double d2 = p2->data[coor];
	if(d1 > d1)
		return 1;
	else if(d1 < d2)
		return -1;
	return 0;

}


