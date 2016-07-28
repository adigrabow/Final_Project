/*
 * main.c
 *
 *  Created on: Jul 23, 2016
 *      Author: adigrabow
 */
#include "SPPoint.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "KDArray.h"


void MemCpyTest();
void printMatrix(int** mat, int numOfPoints, int dim);
void printKDArray(kdArray arr, int size);
void simpleInitTest();
void splitTest1();

int main(){

	//simpleInitTest();
	void splitTest1();
	printf("finished main!\n");
	return 0;
}

void simpleInitTest(){

	double data1[2] = {1.0,2.0};
	double data2[2]= {123.0,70.0};
	double data3[2]= {2.0,7.0};
	double data4[2]= {9.0,11.0};
	double data5[2]= {3.0,4.0};


	SPPoint p1 = spPointCreate(data1, 2, 0);
	SPPoint p2 = spPointCreate(data2, 2, 1);
	SPPoint p3 = spPointCreate(data3, 2, 2);
	SPPoint p4 = spPointCreate(data4, 2, 3);
	SPPoint p5 = spPointCreate(data5, 2, 4);


	SPPoint SPPointArr[5] = {p1,p2,p3,p4,p5};
	kdArray resultArray = Init(SPPointArr , 5);

	printMatrix(getMatrixFromKDArray(resultArray), 5, 2);

	return;
}


void MemCpyTest(){

	double data1[4] = {1.1,2.2,3.3,4.4};
	double data2[4]= {5.5,6.6,7.7,8.8};
	double data3[4]= {9.9,10.10,11.11,12.12};

	SPPoint p1 = spPointCreate(data1, 4, 1);
	SPPoint p2 = spPointCreate(data2, 4, 2);
	SPPoint p3 = spPointCreate(data3, 4, 3);

	SPPoint SPPointArr[3] = {p1,p1,p3};

	for(int i = 0; i < 3; i++){
		printf("p%d[0] = %f\n",i,spPointGetAxisCoor(SPPointArr[i], 0));
	}

	SPPoint* copiedArr = (SPPoint*)malloc(sizeof(SPPointArr));
	memcpy(copiedArr,SPPointArr, sizeof(SPPointArr));

	for(int i = 0; i < 3; i++){
		printf("p%d[0] = %f\n",i,spPointGetAxisCoor(copiedArr[i], 0));
	}
	return;
}


void printMatrix(int** mat, int numOfPoints, int dim){
	for(int i = 0; i < dim ; i++){
		printf("row %d: ",i);
		for(int j = 0 ; j < numOfPoints; j++){
			printf("%d,",mat[i][j]);
		}
		printf("\n");
	}
}

void splitTest1(){
	double data1[2] = {1.0,2.0};
	double data2[2]= {123.0,70.0};
	double data3[2]= {2.0,7.0};
	double data4[2]= {9.0,11.0};
	double data5[2]= {3.0,4.0};


	SPPoint p1 = spPointCreate(data1, 2, 0);
	SPPoint p2 = spPointCreate(data2, 2, 1);
	SPPoint p3 = spPointCreate(data3, 2, 2);
	SPPoint p4 = spPointCreate(data4, 2, 3);
	SPPoint p5 = spPointCreate(data5, 2, 4);


	SPPoint SPPointArr[5] = {p1,p2,p3,p4,p5};
	kdArray resultArray = Init(SPPointArr , 5);

	printf("before split\n");

	kdArray * TwoKDArrays = Split(resultArray, 1);

	printf("done split test\n");
	printMatrix(getMatrixFromKDArray(TwoKDArrays[0]), 3, 2);
	printMatrix(getMatrixFromKDArray(TwoKDArrays[1]), 2, 2);



//	kdArray leftKD = split(...)
	//sprintKDArray(leftKD, (resultArray->size)/2);

	return;
}

void printKDArray(kdArray arr, int size){
	printMatrix(getMatrixFromKDArray(arr), size, getDimFromKDArray(arr));
	return;
}

