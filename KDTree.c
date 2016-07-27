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

/* Note: the name we need to define is KDTreeNode. Maybe the typedef is wrong? */
struct KDTreeNode{
	int dim; /* The splitting dimension */
	double val; /* The median value of the splitting dimension */
	kdTree * left; /* Pointer to the left subtree */
	kdTree * right; /* Pointer to the right subtree */
	SPPoint* data; /* Pointer to a point (only if the current node is a leaf) otherwise this field value is NULL */
};

kdTree init(void){ /*what are the arguments? */
	Init(SPPoint* arr, int size)


}
