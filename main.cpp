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

#include "main_aux.h"
}


int main(int argc, char * argv[]){

	/**********************
	 Variables Declarations
	 **********************/

	SPBPQueue bpq = NULL;
	SPPoint* pointArrayPerImage = NULL; /* saves the points array of each image in extraction mode*/
	SPPoint * pointArrayPerImageQuery = NULL; /* saves the points array for query image*/
	SP_KDTREE_SPLIT_METHOD_TYPE splitMethod = MAX_SPREAD;
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;
	SPConfig config = NULL;
	SP_LOGGER_MSG loggerMsg = SP_LOGGER_SUCCESS;
	SP_LOGGER_LEVEL loggerLevel = SP_LOGGER_ERROR_LEVEL;
	SPPoint* pointArray = NULL;
	kdArray kdArr = NULL;
	kdTree tree = NULL;
	char const * configFilename = CONFIG_DEFAULT;
	int * allPicsCount = NULL; /* will save number of appearances all features of an image are in nearest neighbors */
	bool isDefaultConfigFilename = true; /* flag for default config filename */
	bool extractionMode = false;
	bool isGui;
	int numOfPics = 0;
	int numberOfUpdatedHits = 0; /* saves number of images updated during the k nearest neighbours in allPicsCountOrdered */
	int numOfSimilarImages = 0;
	int numOfSimilarImagesToDisplay = 0;
	int i = 0;
	int  pointerToTotalFeat = -1; /*from extractFromFiles function */
	int numOfFeats = 0; /* a pointer in which the actual number of feats extracted */
	int totalFeat = 0; /* convert the pointer to int */
	char loggerFileName[_MAX] = {0};
	char featPath [_MAX] = {0};
	char imagePath [_MAX] = {0};
	char imagePathToDisplay [_MAX] = {0}; /* saves the image path that was created in  spConfigGetImagePath */
	char query[_MAX] = {0}; /* the program will ask the user to enter an image path  */
	int size = 0; 
	int * pointerToPreviousIndex = NULL;
	int imageIndex = 0;
	
	
	char DEBUG_NUM_OF_PICS[_MAX] = {0};
	char DEBUG_NUM_OF_SIMILAR_IMG[_MAX] = {0};
	char DEBUG_SPLIT_METHOD[_MAX] = {0};
	char DEBUG_IS_GUI[_MAX] = {0};
	char DEBUG_IMG_PATH[_MAX] = {0};
	/****************
	 Creating Config
	 ***************/
	 
	/* if a config filename was entered by user */
	if (argc > 1) { 
		if (( strcmp(argv[1], ENTERED_CONFIG_FILE_NAME) == 0) && (argc == 3)){
			configFilename = argv[2];
			isDefaultConfigFilename = false; /* update flag */
		} else {
			printf(ERROR_INVALID_CONFIG_ARGS); /*invalid arguments */
			return 0;
		}
	}

	config = spConfigCreate(configFilename, &msg);
	/* treat config errors [R] messeges */

	if (msg != SP_CONFIG_SUCCESS){

		/* default filename cannot be opened */
		if(isDefaultConfigFilename ){
			printf(ERROR_DEFAULT_FILE_CONFIG);
		}

		if (msg == SP_CONFIG_CANNOT_OPEN_FILE) {
			printf(ERROR_CANNOT_OPEN_CONFIG_FILE,configFilename);
		}

		if (msg == SP_CONFIG_INVALID_ARGUMENT) {
			printf(REGULAR_MSG_SPCONFIG_INVALID_ARGUMENT);
		}

		if (msg == SP_CONFIG_ALLOC_FAIL) {
			printf(REGULAR_MSG_SPCONFIG_FAILED_TO_ALLOCATE_MEMORY);
		}

		return 0;
	}

	/***************
	Creating Logger
	 ****************/

	/* use SPConfig to get logger level and filename*/
	strcpy(loggerFileName, spConfigGetspLoggerFilename(config));
	loggerLevel = spConfigGetspLoggerLevel(config);

	if (loggerFileName == NULL) {
		printf(REGULAR_MSG_LOGGER_FILE_NAME_IS_NULL);
		return 0;
	}

	if ( (-1) == loggerLevel) {
		printf(REGULAR_MSG_LOGGER_FILE_LEVEL_IS_NEGATIVE);
		return 0;
	}

	loggerMsg = spLoggerCreate(loggerFileName, loggerLevel);

	/*make sure logger was created properly*/
	if(loggerMsg != SP_LOGGER_SUCCESS) {

		if(loggerMsg == SP_LOGGER_OUT_OF_MEMORY) {
			printf(REGULAR_MSG_SPLOGGER_FAILED_TO_ALLOCATE_MEMORY);
			return 0;
		}
		else if(loggerMsg == SP_LOGGER_CANNOT_OPEN_FILE) {
			printf(REGULAR_MSG_CANNOT_OPEN_LOGGER_FILE);
			return 0;
		}
	}

	/*********************************************
	 * from now on we can print using the logger!
	 *********************************************/
	spLoggerPrintInfo(LOGGER_INFO_LOGGER_WAS_CREATED_SUCCESSFULLY);

	fflush(stdout);

	numOfPics = spConfigGetNumOfImages(config, &msg);

	if (msg != SP_CONFIG_SUCCESS) {
		spLoggerPrintError(LOGGER_ERROR_CANNOT_GET_NUM_OF_IMAGES,
				__FILE__, __func__, __LINE__ );
		spLoggerPrintError(EXIT_FROM_MAIN_MSG,__FILE__, __func__, __LINE__ );
		spConfigDestroy(config);
		spLoggerDestroy();
		printf(EXIT_PROGRAM_MSG);
		return 0;
	}
	sprintf(DEBUG_NUM_OF_PICS,LOGGER_DEBUG_NUM_OF_PICS, numOfPics);
	spLoggerPrintDebug(DEBUG_NUM_OF_PICS, __FILE__, __func__, __LINE__ );

	numOfSimilarImages = spConfigGetspNumOfSimilarImages(config); /* getter from config - we will display them */
	splitMethod = spConfigGetspKDTreeSplitMethod(config);

	/* making sure we were able to extract the information from config*/
	if (numOfSimilarImages == (-1)) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_EXTRACT_NUM_OF_SIMILAR_IMGAGES_FROM_CONFIG,
				__FILE__, __func__, __LINE__ );
		spLoggerPrintError(EXIT_FROM_MAIN_MSG,__FILE__, __func__, __LINE__ );
		spConfigDestroy(config);
		spLoggerDestroy();
		printf(EXIT_PROGRAM_MSG);
		return 0;

	}

	if (splitMethod == INVALID_CONFIG) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_EXTRACT_SPLIT_METHOD_FROM_CONFIG,
				__FILE__, __func__, __LINE__ );
		spLoggerPrintError(EXIT_FROM_MAIN_MSG,__FILE__, __func__, __LINE__ );
		spConfigDestroy(config);
		spLoggerDestroy();
		printf(EXIT_PROGRAM_MSG);
		return 0;

	}

	sprintf(DEBUG_NUM_OF_SIMILAR_IMG,LOGGER_DEBUG_NUM_OF_SIMILAR_IMGS, numOfSimilarImages);
	spLoggerPrintDebug(DEBUG_NUM_OF_SIMILAR_IMG, __FILE__, __func__, __LINE__ );

	sprintf(DEBUG_SPLIT_METHOD, LOGGER_DEBUG_SPLIT_METHOD, splitMethod);
	spLoggerPrintDebug(DEBUG_SPLIT_METHOD, __FILE__, __func__, __LINE__ );

	isGui = spConfigMinimalGui(config, &msg);
	/* making sure we were able to extract isMinimalGui from config*/
	if (SP_CONFIG_INVALID_ARGUMENT == msg) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_EXTRACT_IS_MINIMAL_GUI,
				__FILE__, __func__, __LINE__ );
		spLoggerPrintError(EXIT_FROM_MAIN_MSG,__FILE__, __func__, __LINE__ );
		spConfigDestroy(config);
		spLoggerDestroy();
		printf(EXIT_PROGRAM_MSG);
		return 0;
	}


	if (true == isGui) {
		sprintf(DEBUG_IS_GUI,LOGGER_DEBUG_IS_GUI, "true");
		spLoggerPrintDebug(DEBUG_IS_GUI, __FILE__, __func__, __LINE__ );
	}

	if (false == isGui) {
		sprintf(DEBUG_IS_GUI,LOGGER_DEBUG_IS_GUI, "false");
		spLoggerPrintDebug(DEBUG_IS_GUI, __FILE__, __func__, __LINE__ );
	}

	/* Initialize imageProc */
	sp::ImageProc * imageProc = NULL;
	imageProc = new sp::ImageProc(config);

	/*************
	 Preprocessing
	 **************/
	spLoggerPrintInfo(LOGGER_INFO_STARTED_PREPROCESSING);

	/*check type of extraction mode */
	extractionMode = spConfigIsExtractionMode(config, &msg);

	if (SP_CONFIG_INVALID_ARGUMENT == msg) {
		spLoggerPrintError(LOGGER_ERROR_FAILED_TO_EXTRACT_EXTRACTION_MODE,
				__FILE__, __func__, __LINE__ );
		spLoggerPrintError(EXIT_FROM_MAIN_MSG,__FILE__, __func__, __LINE__ );
		delete imageProc;
		spConfigDestroy(config);
		spLoggerDestroy();
		printf(EXIT_PROGRAM_MSG);
		return 0;
	}

	/* EXTRACTION MODE */
	if (extractionMode){
		spLoggerPrintInfo(LOGGER_INFO_WORKING_IN_EXTRACTION_MODE);
		//sp::ImageProc imageProc = sp::ImageProc(config); //  init imageProc

		FILE *fp; // the file created .feat per image //
		int i,j,k;

		/*for each image*/
		for (i = 0; i < numOfPics; i++) {
			msg = spConfigGetImagePath(imagePath, config, i);

			if (SP_CONFIG_SUCCESS != msg) {
				spLoggerPrintError(LOGGER_ERROR_FAILED_TO_EXTRACT_IMAGE_PATH,
						__FILE__, __func__, __LINE__ );
				spLoggerPrintError(EXIT_FROM_MAIN_MSG,__FILE__, __func__, __LINE__ );
				spConfigDestroy(config);
				spLoggerDestroy();
				delete imageProc;
				printf(EXIT_PROGRAM_MSG);
				return 0;
			}

			sprintf(DEBUG_IMG_PATH,LOGGER_DEBUG_IMG_PATH, imagePath);
			spLoggerPrintDebug(DEBUG_IMG_PATH, __FILE__, __func__, __LINE__ );


			/* get points of image i */
			pointArrayPerImage =  imageProc->getImageFeatures(imagePath, i ,&numOfFeats);

			/*featPath is the name of the file were all the features are saved*/
			msg = spConfigGetImagePathFeat(featPath, config,i);
			if (SP_CONFIG_SUCCESS != msg) {
				spLoggerPrintError(LOGGER_ERROR_FAILED_TO_EXTRACT_FEATURE_PATH,
						__FILE__, __func__, __LINE__ );
				spLoggerPrintError(EXIT_FROM_MAIN_MSG,__FILE__, __func__, __LINE__ );
				spConfigDestroy(config);
				spLoggerDestroy();
				free(pointArrayPerImage);
				delete imageProc;
				printf(EXIT_PROGRAM_MSG);
				return 0;
			}



			fp = fopen(featPath, WRITE_TO_FILE);
			fprintf(fp, "%d#%d\n", numOfFeats,i); // first row is numOfFeats # index //

			for (k = 0; k < numOfFeats; k++){ // each row is the data of the k-th point  //
				for (j = 0; j < spPointGetDimension(pointArrayPerImage[0]); j++){ // for each feature/point //
					fprintf(fp, "%f#", spPointGetAxisCoor(pointArrayPerImage[k], j)); // each coor separated by # //
				}
				fprintf(fp, "\n"); // line down after every data array is printed to file //
			}
			fclose(fp);

			/////
			destroyCopiedSPPointArray(pointArrayPerImage,numOfFeats);
			free(pointArrayPerImage);
		}
		spLoggerPrintInfo(LOGGER_INFO_DONE_WITH_EXTRACTION_MODE);

	}

	/*NON-EXTRACTION MODE */

	pointArray = extractFromFiles(config, &pointerToTotalFeat);
	totalFeat = pointerToTotalFeat;

	/* End of PreProcessing */

	spLoggerPrintInfo(LOGGER_INFO_FINISHED_PREPROCESSING);

	/*********************
	 Main interaction loop
	 *********************/

	/* Init data structures */
	kdArr = Init(pointArray,totalFeat);

	spLoggerPrintInfo(LOGGER_INFO_DONE_WITH_KDARRAY_INIT);
	/* Please note that in case an error occurs the logger print is inside the function*/
	if ( NULL == kdArr) {
		free(pointArray);
		spLoggerPrintError(EXIT_FROM_MAIN_MSG,__FILE__, __func__, __LINE__ );
		spConfigDestroy(config);
		free(pointArrayPerImage);
		spLoggerDestroy();
		delete imageProc;
		printf(EXIT_PROGRAM_MSG);
		return 0;
	}

	srand(time(NULL)); // requested once in program. for RANDOM mode in split tree //
	size = -1; // according to moab the first split needs to start from index 0 in incremental //
	pointerToPreviousIndex = &size;

	tree = init(kdArr,pointerToPreviousIndex,splitMethod);
	spLoggerPrintInfo(LOGGER_INFO_DONE_WITH_KDTREE_INIT);

	/* Please note that in case an error occurs the logger print is inside the function*/
	if (NULL == tree) {
		spLoggerPrintError(EXIT_FROM_MAIN_MSG,__FILE__, __func__, __LINE__ );
		spConfigDestroy(config);
		spLoggerDestroy();
		delete imageProc;
		free(pointArrayPerImage);
		destroyKdArray(kdArr);
		printf(EXIT_PROGRAM_MSG);
		return 0;
	}

	printf(ENTER_QUERY);
	fflush(stdout);
	scanf("%s", query);

	while (strcmp(EXIT_PROGRAM,query) != 0){

		spLoggerPrintInfo(LOGGER_INFO_WORKING_ON_NEW_QUERY);
		allPicsCount = initCount(numOfPics);
		/* Please note that in case an error occurs the logger print is inside the function*/
		if (NULL == allPicsCount) {
			spLoggerPrintError(EXIT_FROM_MAIN_MSG,__FILE__, __func__, __LINE__ );
			spConfigDestroy(config);
			spLoggerDestroy();
			delete imageProc;
			free(pointArrayPerImage);
			destroyKdArray(kdArr);
			destroyKdTree(tree);
			printf(EXIT_PROGRAM_MSG);
			return 0;
		}

		/* get points of query image - its index will be numOfPics
		 * (images in directory are indexes 0-(numOfPics-1)) */

		pointArrayPerImageQuery =  imageProc->getImageFeatures(query, numOfPics ,&numOfFeats);
		if (NULL == pointArrayPerImageQuery) {
			spLoggerPrintError(EXIT_FROM_MAIN_MSG,__FILE__, __func__, __LINE__ );
			spConfigDestroy(config);
			spLoggerDestroy();
			delete imageProc;
			free(pointArrayPerImageQuery);
			destroyKdArray(kdArr);
			destroyKdTree(tree);
			destroyCount(allPicsCount);
			printf(EXIT_PROGRAM_MSG);
			return 0;
		}
		/* for each point of query find kNearestNeighbors */
		for (i = 0; i < numOfFeats; i++){
				bpq = initBPQ(config);

				if (NULL == bpq) {
					spLoggerPrintError(EXIT_FROM_MAIN_MSG,__FILE__, __func__, __LINE__ );
					spConfigDestroy(config);
					spLoggerDestroy();
					delete imageProc;
					free(pointArrayPerImageQuery);
					destroyKdArray(kdArr);
					destroyKdTree(tree);
					destroyCount(allPicsCount);
					printf(EXIT_PROGRAM_MSG);
					return 0;
				}

				SPPoint pointFromQuery = spPointCopy(pointArrayPerImageQuery[i]);
				kNearestNeighbors(tree,bpq,pointFromQuery);
				addToCount(bpq,allPicsCount);
				spPointDestroy(pointFromQuery);
				spBPQueueDestroy(bpq);

		}


		/* Convert from (int *) to (Img *) and order by hits */
		Img * allPicsCountOrdered = initImgArray(allPicsCount, numOfPics);


		/* checks how many pictures were updated during the search */
		for (i = 0; i < numOfPics; i++ ){
			if (allPicsCountOrdered[i].hits >0){
				numberOfUpdatedHits++;
			}
			else
				break;
		}
		numOfSimilarImagesToDisplay = (numberOfUpdatedHits < numOfSimilarImages) ? numberOfUpdatedHits : numOfSimilarImages;
		if (numberOfUpdatedHits < numOfSimilarImages) {
			spLoggerPrintWarning(LOGGER_WAR_NUM_OF_UPDATED_HITS_SMALLER_THAN_NUM_OF_SIMILAR_IMG,
					__FILE__, __func__, __LINE__);
		}

		int k;

		/* MINIMAL GUI */
		if(isGui) {
			spLoggerPrintInfo(LOGGER_INFO_USING_MINIMAL_GUI);
			for (i = 0; i < numOfSimilarImagesToDisplay; i++){ // display the first numOfSimilarImages images
				k = allPicsCountOrdered[i].index; // get the img index
				msg = spConfigGetImagePath(imagePathToDisplay, config, k);// create path

				if (SP_CONFIG_SUCCESS != msg) {
					spLoggerPrintError(LOGGER_ERROR_FAILED_TO_EXTRACT_IMAGE_PATH,
											__FILE__, __func__, __LINE__ );
					spLoggerPrintError(EXIT_FROM_MAIN_MSG,__FILE__, __func__, __LINE__ );
					spConfigDestroy(config);
					spLoggerDestroy();
					delete imageProc;
					free(pointArrayPerImage);
					destroyKdArray(kdArr);
					destroyKdTree(tree);
					destroyCount(allPicsCount);
					free(allPicsCountOrdered);
					printf(EXIT_PROGRAM_MSG);
					return 0;
				}

				imageProc->showImage(imagePathToDisplay);// present pic
			}
		}
		/* NON-MINIMAL GUI */
		else{
			spLoggerPrintInfo(LOGGER_INFO_NOT_USING_MINIMAL_GUI);
			printf(BEST_CANIDATES,query);
			fflush(stdout);
			for (i = 0; i < numOfSimilarImagesToDisplay; i++){ // display the first numOfSimilarImages images in stdout
				imageIndex = allPicsCountOrdered[i].index; // get the img index
				msg = spConfigGetImagePath(imagePathToDisplay, config, imageIndex);// create path

				if (SP_CONFIG_SUCCESS != msg) {
					spLoggerPrintError(LOGGER_ERROR_FAILED_TO_EXTRACT_IMAGE_PATH,
											__FILE__, __func__, __LINE__ );
					spLoggerPrintError(EXIT_FROM_MAIN_MSG,__FILE__, __func__, __LINE__ );
					spConfigDestroy(config);
					spLoggerDestroy();
					delete imageProc;
					destroyKdArray(kdArr);
					destroyKdTree(tree);
					free(pointArrayPerImage);
					destroyCount(allPicsCount);
					free(allPicsCountOrdered);
					printf(EXIT_PROGRAM_MSG);
					return 0;
				}
				printf("%s\n",imagePathToDisplay);
				fflush(stdout);
			}
		}

		destroyCopiedSPPointArray(pointArrayPerImageQuery,numOfFeats);
		free(pointArrayPerImageQuery);

		free(allPicsCountOrdered);
		destroyCount(allPicsCount);
		printf(ENTER_QUERY);
		fflush(stdout);
		scanf("%s", query);
	}

	printf(EXIT_PROGRAM_MSG);
	fflush(stdout);
	/******************************
	 Free all allocations and Exit
	 *****************************/
	spConfigDestroy(config);
	destroyKdTree(tree);
	destroyKdArray(kdArr);
	destroyCopiedSPPointArray(pointArray,totalFeat);
	free(pointArray);
	spLoggerPrintInfo(LOGGER_INFO_FINISHED_SUCCESFULY);
	spLoggerPrintInfo(EXIT_FROM_MAIN_MSG);
	delete imageProc;
	spLoggerDestroy();
	
	return 0;
}



