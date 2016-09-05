/*
 * KDTree.h
 *
 *  Created on: 27 ‡ŽŒ“ 2016
 *      Author: Maayan Sivroni
 */
#ifndef KDTREE_H_
#define KDTREE_H_


#include "SPConfig.h"
#include "KDArray.h"

typedef struct KDTreeNode* kdTree;

/**
 * The function creates recursively and returns a kdTree.
 * If leaf - then add the point to the data field.
 * else - update the val field to be the median, and split by one of the split methods
 * (For further explenations refer to the PDF)
 *
 *
 * @param kdArr - a kdArray contains all the features ordered by each of the coordinates
 * @param int - pointer to the index the function sorts by
 * @param splitMethod - an enum contains one of the following:
 * MAX_SPREAD,
 *	RANDOM,
 *	INCREMENTAL,
 *	INVALID_CONFIG.
 *
 * @return
 *  NULL if allocation error
 *  in any other case, returns the kdTree
 *  */

kdTree init(kdArray kdArr, int * index, SP_KDTREE_SPLIT_METHOD_TYPE splitMethod);

/**
 * A getter for the dim field.
 *
 * @param node - a kdTree
 *
 *
 * @return
 *  -1 if node is NULL
 *  in any other case, returns the dim field
 *  */

int kdTreeGetDimension(kdTree node);

/**
 * A getter for the val field.
 *
 * @param node - a kdTree
 *
 *
 * @return
 *  -1.0 if node is NULL
 *  in any other case, returns the val field
 *  */

double kdTreeGetVal(kdTree node);

/**
 * A getter for the left field. (left sub-kdTree)
 *
 * @param node - a kdTree
 *
 *
 * @return
 *  NULL if node is NULL
 *  in any other case, returns the left field
 *  */

kdTree  kdTreeGetLeft(kdTree node);

/**
 * A getter for the right field. (right sub-kdTree)
 *
 * @param node - a kdTree
 *
 *
 * @return
 *  NULL if node is NULL
 *  in any other case, returns the right field
 *  */

kdTree  kdTreeGetRight(kdTree node);

/**
 * A getter for the data field. (a point if node is leaf)
 *
 * @param node - a kdTree
 *
 *
 * @return
 *  NULL if node is NULL
 *  in any other case, returns the data field
 *  */

SPPoint kdTreeGetData(kdTree node);

/**
 * The function destroys the node recursively
 *
 * @param node - a kdTree
 *
 *
 * @return
 *  noting happens if node is NULL
 *  */

void destroyKdTree(kdTree node);

#endif /* KDTREE_H_ */
