/*
 * KDArray.h
 *
 *  Created on: Jul 22, 2016
 *      Author: adigrabow
 */



typedef struct SPKDArray* kdArray;


kdArray Init(SPPoint* arr, int size);
int coorCompare(const void * a, const void* b);
int compareSPPoints(SPPoint p1, SPPoint p2 , int coordinate);
int** getMatrixFromKDArray(kdArray arr);
int getDimFromKDArray(kdArray arr);

kdArray * Split(kdArray kdArr, int coor);
kdArray initFromSplit(kdArray arr, int* X,int * map,int side);
void destroyKdArray(kdArray arr);
