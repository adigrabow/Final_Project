/*
 * KDArray.h
 *
 *  Created on: Jul 22, 2016
 *      Author: adigrabow
 */



typedef struct SPKDArray* kdArray;
kdArray init(SPPoint* arr, int size);
int coorCompare(const void * a, const void* b);
