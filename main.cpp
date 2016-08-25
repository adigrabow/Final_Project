
/*include c library */
#include <cstdlib>
#include <cstddef>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <cstring>
#include <ctime>
#include <cstdbool>
#include "SPImageProc.h"
using namespace std;

extern "C"{
/*include your own C source files */
//#include "KDArray.h"
//#include "KDTree.h"
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPConfig.h"
#include "main_aux.h"
}

//#define MAX 1024

int main(int argc, char * argv[]){
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;
	//SPConfig config = spConfigCreate("a.txt", &msg);
	//TODO we have a problem with the config, temporary Im using another function
	SPConfig config = spConfigAlternativeCreate();
	printf("finished create config!\n");

	//TODO treat msg
	printf("finished config\n");
	int numOfPics = spConfigGetNumOfImages(config, &msg);
	//TODO treat msg
	SPPoint* pointArray ;
	kdArray kdArr;
	kdTree tree ;
	//int * pointerToTotalFeat; /*from extractFromFiles function */
	int  pointerToTotalFeat = -1; /*from extractFromFiles function */
	int totalFeat; /* convert the pointer to int */
	char query[_MAX]; /* the program will ask the user to enter an image path  */
	const char *EXIT = "<>"; /*if query == EXIT then end program */
	int index; /* extracted from query */
	int * allPicsCount; /* will save number of appearances all features of an image are in nearest neighbors */
	int i;
	SPBPQueue bpq;
	int numOfSimilarImages = spConfigGetspNumOfSimilarImages(config); /* getter from config - we will display them */
	bool isGui;
	int numOfFeats; /* a pointer in which the actual number of feats extracted */
	SPPoint* pointArrayPerImage; /* saves the points array of each image */
	SP_KDTREE_SPLIT_METHOD_TYPE splitMethod = spConfigGetspKDTreeSplitMethod(config);
	//char * featPath ; /* will contain the path for image .feat */
	char featPath [_MAX] ; /* will contain the path for image .feat */
	//char* imagePath; /* saves the image path that was created in  spConfigGetImagePath */
	char imagePath [_MAX]; /* saves the image path that was created in  spConfigGetImagePath */
	char imagePathToDisplay [_MAX]; /* saves the image path that was created in  spConfigGetImagePath */

	if (spConfigMinimalGui(config, &msg)){ /* getter from config */
		//TODO treat msg
		isGui = true;
	}
	else{
		isGui = false;
	}
	sp::ImageProc imageProc = sp::ImageProc(config); //  init imageProc //TODO I put this line outside the "if"
	// Preprocessing - check type of extraction mode //
	if (spConfigIsExtractionMode(config, &msg)){ /* EXTRACTION MODE */
		//TODO treat msg
		//sp::ImageProc imageProc = sp::ImageProc(config); //  init imageProc

		FILE *fp; // the file created .feat per image //
		int i,j,k;
		for (i=0; i<numOfPics; i++){ // for each image//
			msg = spConfigGetImagePath(imagePath, config, i); // save msg in case of errors //
			//TODO Treat errors...
			// get points of image i //
			pointArrayPerImage =  imageProc.getImageFeatures(imagePath, i ,&numOfFeats);
			spConfigGetImagePathFeat(featPath, config,i);// create featPath - output file //
			fp = fopen(featPath, "w+");
			fprintf(fp, "%d#%d\n", numOfFeats,i); // first row is numOfFeats # index //
			for (k=0; k<numOfFeats; k++){ // each row is the data of the k-th point  //
				for (j=0; j<spPointGetDimension(pointArrayPerImage[0]);j++){ // for each feature/point //
					fprintf(fp, "%f#", spPointGetAxisCoor(pointArrayPerImage[k], j)); // each coor separated by # //
				}
				fprintf(fp, "\n"); // line down after every data array is printed to file //
			}
			fclose(fp);
		}

	}
	else { // NON-EXTRACTION MODE //
	}


	pointArray = extractFromFiles(config, &pointerToTotalFeat);
	totalFeat = pointerToTotalFeat;

	// End of PreProcessing //

	// init data structures //
	kdArr = Init(pointArray,totalFeat);
	srand(time(NULL)); // requested once in program. for RANDOM mode in split tree //
	int size =-1 ; // according to moab the first split needs to start from index 0 in incremental //
	int * ptr = &size;
	tree = init(kdArr,ptr,splitMethod);
	// until this point I think that everything works! :-)

	// QUERY: “Please enter an image path:\n”
	scanf("%s", query);

	while (strcmp(EXIT,query)!=0){

		index = extractIndexFromQuery(query); // extract index from image //
		allPicsCount = initCount(numOfPics); // init the count array //

		for (i=0;i<totalFeat;i++){ // for each point of image-(index) find kNearestNeighbors //
			if (spPointGetIndex(pointArray[i]) == index){
				bpq = initBPQ(config); // init queue //
				SPPoint p = spPointCopy(pointArray[i]);
				int dim = 0;
				kNearestNeighbors(tree,bpq,p,dim,config);
				addToCount(bpq,allPicsCount);
				spBPQueueDestroy(bpq);
			}
		}

		// Convert from (int *) to (Img *) and order by hits
		Img * allPicsCountOrdered = initImgArray(allPicsCount, numOfPics);
		int k;
		if(isGui){ //MINIMAL GUI
			for (i=0;i<numOfSimilarImages;i++){ // display the first numOfSimilarImages images
				k = allPicsCountOrdered[i].index; // get the img index
				spConfigGetImagePath(imagePathToDisplay, config, k);// create path
				imageProc.showImage(imagePathToDisplay);// present pic
			}
		}
		else{//NON-MINIMAL GUI
			printf("Best candidates for - %s - are:\n",query);
			for (i=0;i<numOfSimilarImages;i++){ // display the first numOfSimilarImages images in stdout
				k = allPicsCountOrdered[i].index; // get the img index
				spConfigGetImagePath(imagePathToDisplay, config, k);// create path
				printf("%s\n",imagePathToDisplay);
			}
		}

		free(allPicsCountOrdered);
		destroyCount(allPicsCount);
		// QUERY: “Please enter an image path:\n”
		scanf("%s", query);
	}

	// Free all allocations
	spConfigDestroy(config);
	destroyKdTree(tree);
	destroyKdArray(kdArr);
	free(pointArray);//TODO maybe we need to free every point in pointArray and then free the array?
	return 0;
}


