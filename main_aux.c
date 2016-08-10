<<<<<<< HEAD
/*
 * main_aux.c
 *
 *  Created on: 29 ‡“ 2016
 *      Author: Maayan Sivroni
 */

#include "SPConfig.h"
#include "SPPoint.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "KDArray.h"
#include "SPBPriorityQueue.h"
#include <limits.h> /* for INT_MIN, INT_MAX - for function extractIndexFromQuery */
#include "main_aux.h"
#include <string.h>

#define MAX 1024
#define LEN_OF_CHAR 2
/* Use malloc to create the point array.
 * For every image go to .feats and read each line (but the first...)
 * the index of each point will be shown in line 0 + total numOfFeat lines.
 * Use realloc() to change the size of the point array (keep track of the previous size)
 */


SPPoint *extractFromFiles(SPConfig config, int * size){
	SP_CONFIG_MSG* msg;
	SPPoint* pointArray = (SPPoint*) malloc(sizeof(SPPoint)* 1); /* We return an array of all the points (all features) */
	if (!pointArray){
		// PRINT ERROR?
	}
	int totalSize = 0; /*size of the point array */
	int numOfPics = spConfigGetNumOfImages(config, msg);
	int dim = spConfigGetPCADim(config,msg);
	char* imagePath; /* saves the image path that was created in  spConfigGetImagePath */
	char featPath [MAX]; /* will contain the path for image .feat */
	FILE *fp; /* the file created .feat per image */
	int i,j,k;
	const char s[LEN_OF_CHAR] = "#"; /* Delimiter */
	char line [MAX]; //TODO moab said that we cant assume about this size...
	char * c_double; /* for each double value - to be converted to double */
	char * cNumofFeat; /* numOfFeat extracted from file as string */
	char * cindex; /* index extracted from file as string */
	int numOfFeats; /* numOfFeats after convert to int */
	int index; /* index after convert to int */
	double * data; /* tmp double array containing data to init point - size=dim*/
	SPPoint point; /*point extracted */
	int point_index = 0; /* current index in pointArray */
	//SPPoint spPointCreate(double* data, int dim, int index)

	for (i=0; i<numOfPics; i++){ /* for each image */
		//	msg = spConfigGetImagePath(imagePath, config, i); /* save msg in case of errors */
		// create featPath (the string) - for the i-th picture
		fp = fopen(featPath, "r+");
		if (!fp){
			// ERROR
		}
		/* Get parameters from first line - index and actual number of extracted features  */
		fgets(line, MAX, fp);
		cNumofFeat = strtok(line, s);
		numOfFeats = atoi(cNumofFeat); /* convert to int */
		cindex = strtok(NULL, s);
		index = atoi(cindex); /* convert to int */

		totalSize += numOfFeats; /* increase size for realloc */
		pointArray = (SPPoint *) realloc(pointArray, totalSize* sizeof(SPPoint));
		if (!pointArray){
			//ERROR
		}

		for (k=0; k<numOfFeats; k++){ /* each row is a new point to create  */
			/* extract all data (double) */
			fgets(line, MAX, fp); /* get line */
			c_double = strtok(line, s); /* get first double */
			j=0;
			data[j] = atof(c_double); /* convert to double  and add to data */
			/* walk through other tokens */
			while( c_double != NULL ) {
				j++;
				c_double = strtok(NULL, s);
				data[j] = atof(c_double);
			}

			point = spPointCreate(data,dim,index);
			pointArray[point_index] = point;
			point_index++;

		}

		fclose(fp);
	}
	size = &totalSize;
	return pointArray;
}



SPBPQueue initBPQ(SPConfig config){
	/* Maintain a BPQ of the candidate nearest neighbors, called 'bpq', Set the maximum size of 'bpq' to spKNN*/
	SP_CONFIG_MSG* msg;
	// treat msg...
	return spBPQueueCreate(spConfigGetSpKNN(config,msg));
}

void  kNearestNeighbors(kdTree curr , SPBPQueue bpq, SPPoint P, int dim, SPConfig config){ /* Execute this function for each feature in query */
	//TODO is it okay to find also P in the tree?
	// I added dim by myself
	// bpq's first element has the lowest value (=lowest distance)
	if (!bpq || !P || !curr){ /* check invalid arguments */ //TODO dim negative?
		return;
	}

	/* Add the current point to the BPQ. Note that this is a no-op if the
	 * point is not as good as the points we've seen so far.*/

	if (kdTreeGetDimension(curr)== -1){ /* curr is LEAF */
		// calculate distance between leaf and requested point P
		double distance = spPointL2SquaredDistance(kdTreeGetData(curr),P);
		// Create the element try to insert it
		int index = spPointGetIndex(kdTreeGetData(curr));
		SPListElement element = spListElementCreate(index, distance);
		// Try to insert to bpq
		SP_BPQUEUE_MSG msg = spBPQueueEnqueue(bpq, element);
		// maybe treat msg?
		return;
	}

	double * P_data =spPointGetData(P);
	double valOfP = P_data[dim];
	free(P_data);

	if (valOfP <= kdTreeGetVal(curr)){ // Decide in which subtree continue the search
		kNearestNeighbors(kdTreeGetLeft(curr) , bpq,  P,  dim,config); //recursively search the left subtree
	}
	else {
		kNearestNeighbors(kdTreeGetRight(curr) , bpq,  P,  dim,config); //recursively search the right subtree
	}

	//|curr.val - P[curr.dim]| is less than the priority of the max-priority element of bpq
	double checkDistance = fabs(kdTreeGetVal(curr) - valOfP);


	if (!spBPQueueIsFull(bpq) || (checkDistance < spBPQueueMaxValue(bpq)) ){
		SP_CONFIG_MSG* msg;
		// Get next axis and recursively search the other subtree on this next axis
		int index = (dim +1) % spConfigGetPCADim(config,msg); // Check with Adi that this is actually the number of dimensions
		kNearestNeighbors(curr, bpq,  P,  index,config); // Not sure about that line...
	}

}


void addToCount(SPBPQueue bpq,int * allPicsCount){
	int size = spBPQueueSize(bpq);
	SPListElement element;

	for (int i = 0; i < size; i++){
		element = spBPQueuePeek(bpq); /* look at one element */
		allPicsCount[spListElementGetIndex(element)]++; /*increase count for each image - each index */
		spBPQueueDequeue(bpq); /* dequeue from queue */

	}
}

int extractIndexFromQuery(char * query){ //WORKING
	//printf("query: ./images/img10.png, printed: %s\n", query);
	/* extract index of image: between last `/` and last `.` */
	/* EXAMPLE: "./images/img10.png" ====> 10 */
	const char s1[LEN_OF_CHAR] = "/"; /* Delimiter 1 */
	const char s2[LEN_OF_CHAR] = "."; /* Delimiter 2 */

	int index; /* output */
	char * str;

	/* find last appearance of /  */
	char * tmp = strtok(query, s1);
	//printf("%s!!",tmp);
	while( tmp != NULL ) {
		tmp = strtok(NULL, s1);
		if (tmp !=NULL){
			str = tmp;
		}
	}
	/* now str == img10.png */
	//printf("goal: img10.png, printed: %s\n", str);
	char * tmp2 = strtok(str, s2); /* get string before . */
	//printf("goal: img10, printed: %s\n", tmp2);


	sscanf(tmp2, "%*[^0123456789]%d", &index);
	return index;


}

int * initCount(int numOfPics){// WORKING
	if(numOfPics >= 0){
	int* result = (int*) malloc(sizeof(int)* numOfPics);
		if (!result){
			// PRINT ERROR?
		}
		return result;
	}
	else{
		printf("numOfPics is negative\n");
		return NULL;
	}
}

void destroyCount(int * array){// WORKING

		free(array);

}




int compareHits (const void * a, const void * b){//WORKING

	struct Img * f1 = (Img *) a;
	struct Img * f2 = (Img *) b;

	return(f2->hits - f1->hits);

}

Img * initImgArray(int * allPicsCount, int numOfPics){ //WORKING
	/* Convert from (int *) to (Img *) and order by hits */
	Img * hitsArray = (Img *) malloc(numOfPics* sizeof(Img)); /*an array with "num" entries - to count the hits*/
	if ( !hitsArray ){
		//Allocation error
	}
	/*initialize struct array with empty images*/
	for(int i = 0; i < numOfPics; i++){
		hitsArray[i].index = i;
		hitsArray[i].hits = 0;
	}
	/* update Img element with the correct hits */
	for(int j = 0; j < numOfPics; j++){
		hitsArray[j].hits = allPicsCount[j];
	}

	qsort(hitsArray, numOfPics, sizeof(Img),compareHits);
	return hitsArray;

}
=======
/*
 * main_aux.c
 *
 *  Created on: 29 αιεμ 2016
 *      Author: Maayan Sivroni
 */

#include "SPConfig.h"
#include "SPPoint.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "KDArray.h"
#include "SPBPriorityQueue.h"
#include <limits.h> /* for INT_MIN, INT_MAX - for function extractIndexFromQuery */
#include "main_aux.h"
#include <string.h>

#define MAX 1024
#define LEN_OF_CHAR 2
/* Use malloc to create the point array.
 * For every image go to .feats and read each line (but the first...)
 * the index of each point will be shown in line 0 + total numOfFeat lines.
 * Use realloc() to change the size of the point array (keep track of the previous size)
 */


SPPoint *extractFromFiles(SPConfig config, int * size){
	SP_CONFIG_MSG* msg;
	SPPoint* pointArray = (SPPoint*) malloc(sizeof(SPPoint)* 1); /* We return an array of all the points (all features) */
	if (!pointArray){
		// PRINT ERROR?
	}
	int totalSize = 0; /*size of the point array */
	int numOfPics = spConfigGetNumOfImages(config, msg);
	int dim = spConfigGetPCADim(config,msg);
	char* imagePath; /* saves the image path that was created in  spConfigGetImagePath */
	char featPath [MAX]; /* will contain the path for image .feat */
	FILE *fp; /* the file created .feat per image */
	int i,j,k;
	const char s[LEN_OF_CHAR] = "#"; /* Delimiter */
	char line [MAX]; //TODO moab said that we cant assume about this size...
	char * c_double; /* for each double value - to be converted to double */
	char * cNumofFeat; /* numOfFeat extracted from file as string */
	char * cindex; /* index extracted from file as string */
	int numOfFeats; /* numOfFeats after convert to int */
	int index; /* index after convert to int */
	double * data; /* tmp double array containing data to init point - size=dim*/
	SPPoint point; /*point extracted */
	int point_index = 0; /* current index in pointArray */
	//SPPoint spPointCreate(double* data, int dim, int index)

	for (i=0; i<numOfPics; i++){ /* for each image */
		//	msg = spConfigGetImagePath(imagePath, config, i); /* save msg in case of errors */
		// create featPath (the string) - for the i-th picture
		fp = fopen(featPath, "r+");
		if (!fp){
			// ERROR
		}
		/* Get parameters from first line - index and actual number of extracted features  */
		fgets(line, MAX, fp);
		cNumofFeat = strtok(line, s);
		numOfFeats = atoi(cNumofFeat); /* convert to int */
		cindex = strtok(NULL, s);
		index = atoi(cindex); /* convert to int */

		totalSize += numOfFeats; /* increase size for realloc */
		pointArray = (SPPoint *) realloc(pointArray, totalSize* sizeof(SPPoint));
		if (!pointArray){
			//ERROR
		}

		for (k=0; k<numOfFeats; k++){ /* each row is a new point to create  */
			/* extract all data (double) */
			fgets(line, MAX, fp); /* get line */
			c_double = strtok(line, s); /* get first double */
			j=0;
			data[j] = atof(c_double); /* convert to double  and add to data */
			/* walk through other tokens */
			while( c_double != NULL ) {
				j++;
				c_double = strtok(NULL, s);
				data[j] = atof(c_double);
			}

			point = spPointCreate(data,dim,index);
			pointArray[point_index] = point;
			point_index++;

		}

		fclose(fp);
	}
	size = &totalSize;
	return pointArray;
}



SPBPQueue initBPQ(SPConfig config){
	/* Maintain a BPQ of the candidate nearest neighbors, called 'bpq', Set the maximum size of 'bpq' to spKNN*/
	SP_CONFIG_MSG* msg;
	// treat msg...
	return spBPQueueCreate(spConfigGetSpKNN(config,msg));
}

void  kNearestNeighbors(kdTree curr , SPBPQueue bpq, SPPoint P, int dim, SPConfig config){ /* Execute this function for each feature in query */
	//TODO is it okay to find also P in the tree?
	// I added dim by myself
	// bpq's first element has the lowest value (=lowest distance)
	if (!bpq || !P || !curr){ /* check invalid arguments */ //TODO dim negative?
		return;
	}

	/* Add the current point to the BPQ. Note that this is a no-op if the
	 * point is not as good as the points we've seen so far.*/

	if (kdTreeGetDimension(curr)== -1){ /* curr is LEAF */
		// calculate distance between leaf and requested point P
		double distance = spPointL2SquaredDistance(kdTreeGetData(curr),P);
		// Create the element try to insert it
		int index = spPointGetIndex(kdTreeGetData(curr));
		SPListElement element = spListElementCreate(index, distance);
		// Try to insert to bpq
		SP_BPQUEUE_MSG msg = spBPQueueEnqueue(bpq, element);
		// maybe treat msg?
		return;
	}

	double * P_data =spPointGetData(P);
	double valOfP = P_data[dim];
	free(P_data);

	if (valOfP <= kdTreeGetVal(curr)){ // Decide in which subtree continue the search
		kNearestNeighbors(kdTreeGetLeft(curr) , bpq,  P,  dim,config); //recursively search the left subtree
	}
	else {
		kNearestNeighbors(kdTreeGetRight(curr) , bpq,  P,  dim,config); //recursively search the right subtree
	}

	//|curr.val - P[curr.dim]| is less than the priority of the max-priority element of bpq
	double checkDistance = fabs(kdTreeGetVal(curr) - valOfP);


	if (!spBPQueueIsFull(bpq) || (checkDistance < spBPQueueMaxValue(bpq)) ){
		SP_CONFIG_MSG* msg;
		// Get next axis and recursively search the other subtree on this next axis
		int index = (dim +1) % spConfigGetPCADim(config,msg); // Check with Adi that this is actually the number of dimensions
		kNearestNeighbors(curr, bpq,  P,  index,config); // Not sure about that line...
	}

}


void addToCount(SPBPQueue bpq,int * allPicsCount){
	int size = spBPQueueSize(bpq);
	SPListElement element;

	for (int i = 0; i < size; i++){
		element = spBPQueuePeek(bpq); /* look at one element */
		allPicsCount[spListElementGetIndex(element)]++; /*increase count for each image - each index */
		spBPQueueDequeue(bpq); /* dequeue from queue */

	}
}

int extractIndexFromQuery(char * query){ //WORKING
	//printf("query: ./images/img10.png, printed: %s\n", query);
	/* extract index of image: between last `/` and last `.` */
	/* EXAMPLE: "./images/img10.png" ====> 10 */
	const char s1[LEN_OF_CHAR] = "/"; /* Delimiter 1 */
	const char s2[LEN_OF_CHAR] = "."; /* Delimiter 2 */

	int index; /* output */
	char * str;

	/* find last appearance of /  */
	char * tmp = strtok(query, s1);
	//printf("%s!!",tmp);
	while( tmp != NULL ) {
		tmp = strtok(NULL, s1);
		if (tmp !=NULL){
			str = tmp;
		}
	}
	/* now str == img10.png */
	//printf("goal: img10.png, printed: %s\n", str);
	char * tmp2 = strtok(str, s2); /* get string before . */
	//printf("goal: img10, printed: %s\n", tmp2);


	sscanf(tmp2, "%*[^0123456789]%d", &index);
	return index;


}

int * initCount(int numOfPics){// WORKING
	if(numOfPics >= 0){
	int* result = (int*) malloc(sizeof(int)* numOfPics);
		if (!result){
			// PRINT ERROR?
		}
		return result;
	}
	else{
		printf("numOfPics is negative\n");
		return NULL;
	}
}

void destroyCount(int * array){// WORKING

		free(array);

}




int compareHits (const void * a, const void * b){//WORKING

	struct Img * f1 = (Img *) a;
	struct Img * f2 = (Img *) b;

	return(f2->hits - f1->hits);

}

Img * initImgArray(int * allPicsCount, int numOfPics){ //WORKING
	/* Convert from (int *) to (Img *) and order by hits */
	Img * hitsArray = (Img *) malloc(numOfPics* sizeof(Img)); /*an array with "num" entries - to count the hits*/
	if ( !hitsArray ){
		//Allocation error
	}
	/*initialize struct array with empty images*/
	for(int i = 0; i < numOfPics; i++){
		hitsArray[i].index = i;
		hitsArray[i].hits = 0;
	}
	/* update Img element with the correct hits */
	for(int j = 0; j < numOfPics; j++){
		hitsArray[j].hits = allPicsCount[j];
	}

	qsort(hitsArray, numOfPics, sizeof(Img),compareHits);
	return hitsArray;

}
>>>>>>> 105c0abecfc8dba895e59e30b24cc0750a6104ef
