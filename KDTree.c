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
 * Create a destroy function for the tree.
 * */
struct KDTreeNode{
	int dim; /* The splitting dimension */
	double val; /* The median value of the splitting dimension */
	kdTree * left; /* Pointer to the left subtree */
	kdTree * right; /* Pointer to the right subtree */
	SPPoint* data; /* Pointer to a point (only if the current node is a leaf) otherwise this field value is NULL */
};

kdTree init(kdArray kdArr){

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
		node->data = spPointCopy((kdArr->pointArray)[0]); /* copy first (and only) point */
		return node;
	}
	double * spreadArray; /* will contain all spreads for MAX_SPREAD mode */
	int i;
	double max= 0.0; /* for MAX_SPREAD */
	int index; /* the index of the dimension to split by */
	kdArray * twoKdArrays; /* saves lest and right arrays - output from split */

	/* if array size is bigger than 1 then create tree recursively */
	/* I assume that in this parameter from config struct is the requester split method - use the getter */

	if (spKDTreeSplitMethod == MAX_SPREAD){

		/* create spreadArray */
		for (i=0; i< getDimFromKDArray(kdArr); i++){
			spreadArray[i] = kdArr->pointArray[ getMatrixFromKDArray[i][0]]->data[i] -
					kdArr->pointArray[ getMatrixFromKDArray[i][kdArr->size -1]]->data[i];

			if (spreadArray[i] >= max){ /* find max spread from array */
				max = spreadArray[i];
				index = i;
			}
		}
		/* find the index of max - the first index to be found */
		for (i=0; i< getDimFromKDArray(kdArr); i++){
			if (spreadArray[i] == max){
				index = i;
				break;
			}
		}
	}

	if (spKDTreeSplitMethod == RANDOM){
		srand(time(NULL)); /*requested once in program. maybe move to main? */
		index = (int) (((double)((getDimFromKDArray(kdArr))/RAND_MAX) * rand() + 0));
	}

	else { /* INCREMENTAL ... what to do if it is the first node we create? maybe we need to use a global variable*/

	}

	twoKdArrays = Split(kdArr, index);

	/* create tree recursively */
	kdTree node = (kdTree)malloc(sizeof(struct KDTreeNode));
	if(node == NULL){
		return NULL;
	}
	node->dim = index;
	node->val = twoKdArrays[0]->pointArray[twoKdArrays[0]->size -1]->data[index]; /* last point in left array */
	node->left = init(twoKdArrays[0]); /* already allocated in split */
	node->right = init(twoKdArrays[1]); /* already allocated in split */
	node->data = NULL; /* pointArray is an array that is a pointer to point */


}
