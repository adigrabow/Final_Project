/*
 * KDTree.c
 *
 *  Created on: 27 αιεμ 2016
 *      Author: Maayan Sivroni
 */

#include "KDArray.h"
#include "KDTree.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

/* Note: the name we need to define is KDTreeNode. Maybe the typedef is wrong?
 * spKDTreeSplitMethod should be part of the config struct.
 * */
struct KDTreeNode{
	int dim; /* The splitting dimension */
	double val; /* The median value of the splitting dimension */
	kdTree * left; /* Pointer to the left subtree */
	kdTree * right; /* Pointer to the right subtree */
	SPPoint* data; /* Pointer to a point (only if the current node is a leaf) otherwise this field value is NULL */
};

kdTree init(void){ /*what are the arguments? */
	kdArray kdArr;
	//TODO = Init( arr, size); /* Maybe it is needed to create arr first? Where? */

	/* if array size is 1 then create node and return it */
	if (kdArr->size == 1){ /* use getter instead */
		kdTree node = (kdTree)malloc(sizeof(struct KDTreeNode));
		if(node == NULL){
			return NULL;
		}
		node->dim = -1;
		node->val = -1.0;
		node->left = NULL;
		node->right = NULL;
		node->data = (kdArr->pointArray); /* pointArray is an array that is a pointer to point */
		return node;
	}
	double * spreadArray; /* will contain all spreades  */
	int i;
	/* if array size is bigger than 1 then create tree recursively */
	/* I assume that in this parameter from config struct is the requester split method */
	if (spKDTreeSplitMethod == MAX_SPREAD){

		/* create spreadArray */
		for (i=0; i< getDimFromKDArray(kdArr); i++){
			spreadArray[i] = kdArr->pointArray[ getMatrixFromKDArray[i][0]]->data[i] -
							 kdArr->pointArray[ getMatrixFromKDArray[i][kdArr->size -1]]->data[i];
		}
	}

}
