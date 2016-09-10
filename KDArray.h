/*
 * KDArray.h
 *
 *  Created on: Jul 22, 2016
 *      Author: adigrabow
 */
#ifndef KDARRAY_H_
#define KDARRAY_H_
#include "SPLogger.h"
#include "SPPoint.h"

typedef struct SPKDArray* kdArray;

/**
 * The function creates and returns a kdArray.
 * Maintain a matrix - in row i the points are ordered by the i-th coordinate (total of dimension rows)
 *
 * @param arr - array of points,  contains All features from ALL images
 * @param size - number of features in arr
 *
 * @return
 *  NULL if allocation error or invalid arguments (negative size or arr is NULL)
 *  in any other case, returns the kdArray
 *  */

kdArray Init(SPPoint* arr, int size);

/**
 * The function is a getter for the mat field in struct kdArray. (matrix)
 *
 * @param arr - the given kdArray
 *
 * @return
 *  NULL if arr is NULL
 *  in any other case, returns the mat field
 *  */

int** getMatrixFromKDArray(kdArray arr);

/**
 * The function is a getter for the dim field in struct kdArray. (number of coordinates)
 *
 * @param arr - the given kdArray
 *
 * @return
 *  NULL if arr is NULL
 *  in any other case, returns the dim field
 *  */

int getDimFromKDArray(kdArray arr);

/**
 * The function is a getter for the pointArray field in struct kdArray.
 *
 * @param arr - the given kdArray
 *
 * @return
 *  NULL if arr is NULL
 *  in any other case, returns the pointArray field
 *  */

SPPoint* getPointArrayFromKDArray(kdArray arr);

/**
 * The function is a getter for the size field in struct kdArray. (number of points in the kdArray)
 *
 * @param arr - the given kdArray
 *
 * @return
 *  NULL if arr is NULL
 *  in any other case, returns the size field
 *  */

int getSizeFromKDArray(kdArray arr);

/**
 * The function splits the kdArray in half by a given coordinate.
 * For further information,  approach to PDF.
 *
 * @param arr - the given kdArray
 * @param coor - the coordinate the function splits by
 *
 * @return
 *  NULL if arr is NULL or coor is negative or if allocation error
 *  in any other case, returns two kdArrays
 *  */

kdArray * Split(kdArray kdArr, int coor);

/**
 * The function destroys the mat field, pointArray field and then frees the object arr.
 *
 * @param arr - the given kdArray
 *
 * @return
 *  nothing happens if arr is NULL
 *  */

void destroyKdArray(kdArray arr);

/**
 * The function compares two sortedPoint objects by the data field.
 * In case the data field are equivilent - sorts by index.
 *
 * @param ptr1 - first sortedPoint to be compared
 * @param ptr2 - second sortedPoint to be compared
 * @return
 *  (a->data - b->data) and if needed (a->index - b->index)
 *  */
int compareSortPoints(const void * ptr1, const void * ptr2);

void destroySPPointArray(SPPoint * arr, int size);

#endif /* KDARRAY_H_ */
