/*
 * main_aux.h
 *
 *  Created on: 29 αιεμ 2016
 *      Author: Maayan Sivroni
 */

typedef struct Img {
	int index; /*this will store the index of the image this feature belongs to */
	int hits; /* this will store the number of hits this img had */

}Img;

#include "KDTree.h"
#include "SPConfig.h"
#include "SPBPriorityQueue.h"


#define _MAX 1024
#define CONFIG_DEFAULT ("spcbir.config")
#define ENTERED_CONFIG_FILE_NAME ("-c")
#define EXIT_PROGRAM ("<>")
#define EXIT_CONSOLE_MSG ("Exiting...\n")
#define WRITE_TO_FILE ("w+")
#define BEST_CANIDATES ("Best candidates for - %s - are:\n")
#define ENTER_QUERY ("Please enter an image path:\n")

/********************
 [R] Regular Messages
 ********************/

#define ERROR_DEFAULT_FILE_CONFIG ("The default configuration file 'spcbir.config' couldn't be open.\n")
#define ERROR_INVALID_CONFIG_ARGS ("Invalid command line : use -c <config_filename>\n")
#define ERROR_CANNOT_OPEN_CONFIG_FILE ("The configuration file %s couldn't be open\n")
#define REGULAR_MSG_SPCONFIG_INVALID_ARGUMENT ("The function 'spConfigCreate' received invalid argument.\n")
#define REGULAR_MSG_SPCONFIG_FAILED_TO_ALLOCATE_MEMORY ("The function 'spConfigCreate' failed to allocate memory.\n")
#define REGULAR_MSG_LOGGER_FILE_NAME_IS_NULL ("Logger file name was NULL.\n")
#define REGULAR_MSG_LOGGER_FILE_LEVEL_IS_NEGATIVE ("Logger level was negative.\n")
#define REGULAR_MSG_CANNOT_OPEN_LOGGER_FILE ("Failed to open logger file.\n")
#define REGULAR_MSG_CANNOT_OPEN_LOGGER_FILE ("Failed to open logger file.\n")
#define REGULAR_MSG_SPLOGGER_FAILED_TO_ALLOCATE_MEMORY ("The function 'spLoggerCreate' failed to allocate memory.\n")

/****************
 Logger Messages
 ****************/
#define LOGGER_ERROR_FUNCTION_ARGUMENTS_FAILED_TO_MEET_CONSTRAINTS ("Function arguments failed to meet constraints.\n")
#define LOGGER_ERROR_FAILED_TO_ALLOCATE_MEMORY ("Failed to allocate memory.\n")
#define LOGGER_ERROR_CANNOT_GET_NUM_OF_IMAGES ("Failed to extract numOfImages from SPConfig.\n")
#define LOGGER_ERROR_FAILED_TO_EXTRACT_SPLIT_METHOD_FROM_CONFIG ("Failed to extract splitMethod from SPConfig.\n")
#define LOGGER_ERROR_FAILED_TO_EXTRACT_NUM_OF_SIMILAR_IMGAGES_FROM_CONFIG ("Failed to extract numOfSimilarImages from SPConfig.\n")
#define LOGGER_ERROR_FAILED_TO_EXTRACT_IS_MINIMAL_GUI ("Failed to extract isMinimalGui from SPConfig.\n")
#define LOGGER_ERROR_FAILED_TO_EXTRACT_EXTRACTION_MODE ("Failed to extract extractionMode from SPConfig.\n")
#define LOGGER_ERROR_FAILED_TO_EXTRACT_IMAGE_PATH ("Failed to extract imagePath from SPConfig.\n")
#define LOGGER_ERROR_FAILED_TO_EXTRACT_FEATURE_PATH ("Failed to extract feature path from SPConfig.\n")
#define LOGGER_ERROR_FAILED_TO_INIT_KDARRAY ("Failed to use the KDArray 'init' function.\n")
#define EXIT_FROM_MAIN_MSG ("Exiting program...\n")

#define LOGGER_INFO_LOGGER_WAS_CREATED_SUCCESSFULLY ("Logger was created successfully.\n")
#define LOGGER_INFO_STARTED_PREPROCESSING ("Starting preprocessing.\n")
#define LOGGER_INFO_WORKING_IN_EXTRACTION_MODE ("Working in extraction mode.\n")
#define LOGGER_INFO_WORKING_IN_NON_EXTRACTION_MODE ("Working in non-extraction mode.\n")
#define LOGGER_INFO_FINISHED_PREPROCESSING ("Finished preprocessing.\n")
#define LOGGER_INFO_USING_MINIMAL_GUI ("Using minimal GUI.\n")
#define LOGGER_INFO_NOT_USING_MINIMAL_GUI ("Not using minimal GUI.\n")
#define LOGGER_INFO_DONE_WITH_EXTRACTION_MODE ("Done with extraction mode.\n")
#define LOGGER_INFO_WORKING_ON_NEW_QUERY ("Started main-loop for a new query.\n")
#define LOGGER_INFO_FINISHED_SUCCESFULY ("Succesfuly finished main function.\n")

#define LOGGER_WAR_NUM_OF_UPDATED_HITS_SMALLER_THAN_NUM_OF_SIMILAR_IMG ("Number of updated hits is smaller than number of similar images.\n The num of images to be displayed will be smaller than expected.\n")



/**
 * The function extracts all features (points) from all the images in directory.
 *
 * @param config - the configuration structure
 * @param size - an address to store the number of all features extracted.
 *
 * @return
 *  NULL if allocation error or failed to open file
 *  in any other case, returns the points array
 *  */

SPPoint *extractFromFiles(SPConfig config, int * size);

/**
 * The function creates and returns a SPBPriorityQueue.
 * Maintain a BPQ of the candidate nearest neighbors, called 'bpq'.
 * Sets the maximum size of 'bpq' to spKNN
 *
 * @param config - the configuration structure
 *
 * @return
 *  NULL if allocation error
 *  in any other case, returns the SPBPriorityQueue
 *  */

SPBPQueue initBPQ(SPConfig config);

/**
 * The function searches for k-nearest neighbors of queryPoint and updates bpq.
 *
 * @param config - the configuration structure
 *
 * nothing is done if the function is gived an invalid argument :
 * (NULL == currNode || NULL == bpq || NULL == queryPoint )
 * or if the dimension of the points in the kdTree mismatch to the dimension of queryPoint
 * or if allocation error.
 *  */

void kNearestNeighbors(kdTree currNode, SPBPQueue bpq, SPPoint queryPoint);

/**
 * The function extracts the index from a given query (in stdin).
 *
 * @param query - a char array that the user types to stdin including the path of the query image
 *
 * @return
 *  We can assume that the query is valid.
 *  Returns the index of the image.
 *  */

int extractIndexFromQuery(char * query);

/**
 * The function initialize and returns an int array.
 * All cells in array are zeroes.
 * The array will locate the "hits" from kNearestNeighbors search -
 * aka in index i will be the number of "hits" for image i.
 * The size of array will be numOfPics;
 *
 * @param numOfPics - number of pictures
 *
 * @return
 *  Returns NULL if numOfPics is negative.
 *  In any other case, returns an int array.
 *  */

int * initCount(int numOfPics);

/**
 * The function destroys the given array.
 *
 * @param array - an array if ints to be destroyed
 *
 * @return
 *  nothing is done if the array is NULL
 *  In any other case, destroys the array.
 *  */

void destroyCount(int * array);

/**
 * The function addes to the array allPicsCount the results given from bpq.
 * The cell i in the array will contain the updates number of "hits" for image i after the search.
 *
 * @param bpq - a queue of features
 * @param allPicsCount - an int array which contains all the hits
 *
 * @return
 *  nothing is done if NULL == allPicsCount or NULL == bpq
 *  In any other case, the allPicsCount array is being updated.
 *  */

void addToCount(SPBPQueue bpq,int * allPicsCount);

/**
 * The function creates and returns and Img array which contains the number of hits
 * for each image in a whole struct.
 * The array will be sorted by the hits field of struct Img by using the compare function compareHits.
 *
 * @param allPicsCount - an int array which contains all the hits
 * @param numOfPics - size of allPicsCount
 *
 * @return
 *  return NULL if allPicsCount is NULL
 *  In any other case, an Img array is returned.
 *  */

Img * initImgArray(int * allPicsCount, int numOfPics);

/**
 * The function sorts by the hits field in struct Img.
 *
 * @param a - first Img object to be compared
 * @param b - second Img object to be compared
 *
 * @return
 *  returns an int (f2->hits - f1->hits)
 *  */
int compareHits (const void * a, const void * b);
