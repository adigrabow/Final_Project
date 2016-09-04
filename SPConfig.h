#ifndef SPCONFIG_H_
#define SPCONFIG_H_

#include <stdbool.h>
#include <stdio.h>
#include "SPLogger.h"


/**
 * A data-structure which is used for configuring the system.
 */

typedef enum sp_config_msg_t {
	SP_CONFIG_MISSING_DIR,
	SP_CONFIG_MISSING_PREFIX,
	SP_CONFIG_MISSING_SUFFIX,
	SP_CONFIG_MISSING_NUM_IMAGES,
	SP_CONFIG_CANNOT_OPEN_FILE,
	SP_CONFIG_ALLOC_FAIL,
	SP_CONFIG_INVALID_INTEGER,
	SP_CONFIG_INVALID_STRING,
	SP_CONFIG_INVALID_ARGUMENT,
	SP_CONFIG_INDEX_OUT_OF_RANGE,
	SP_CONFIG_SUCCESS
} SP_CONFIG_MSG;

/*I Added this*/
typedef enum sp_kdtree_split_method_t{ //TODO find out were this enum definition belongs
	MAX_SPREAD,
	RANDOM,
	INCREMENTAL,
	INVALID_CONFIG, /* We added to recognize a null config sent to spConfigGetspKDTreeSplitMethod */
}SP_KDTREE_SPLIT_METHOD_TYPE ;

typedef struct sp_config_t* SPConfig;

/**
 * Creates a new system configuration struct. The configuration struct
 * is initialized based on the configuration file given by 'filename'.
 * 
 * @param filename - the name of the configuration file
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return NULL in case an error occurs. Otherwise, a pointer to a struct which
 * 		   contains all system configuration.
 * 
 * The resulting value stored in msg is as follow:
 * - SP_CONFIG_INVALID_ARGUMENT - if filename == NULL
 * - SP_CONFIG_CANNOT_OPEN_FILE - if the configuration file given by filename cannot be open
 * - SP_CONFIG_ALLOC_FAIL - if an allocation failure occurred
 * - SP_CONFIG_INVALID_INTEGER - if a line in the config file contains invalid integer
 * - SP_CONFIG_INVALID_STRING - if a line in the config file contains invalid string
 * - SP_CONFIG_MISSING_DIR - if spImagesDirectory is missing
 * - SP_CONFIG_MISSING_PREFIX - if spImagesPrefix is missing
 * - SP_CONFIG_MISSING_SUFFIX - if spImagesSuffix is missing 
 * - SP_CONFIG_MISSING_NUM_IMAGES - if spNumOfImages is missing
 * - SP_CONFIG_SUCCESS - in case of success
 *
 *
 */
SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg);

/*
 * Returns true if spExtractionMode = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns true if spMinimalGUI = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns the number of images set in the configuration file, i.e the value
 * of spNumOfImages.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns the number of features to be extracted. i.e the value
 * of spNumOfFeatures.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Returns the dimension of the PCA. i.e the value of spPCADimension.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Given an index 'index' the function stores in imagePath the full path of the
 * ith image.
 *
 * For example:
 * Given that the value of:
 *  spImagesDirectory = "./images/"
 *  spImagesPrefix = "img"
 *  spImagesSuffix = ".png"
 *  spNumOfImages = 17
 *  index = 10
 *
 * The functions stores "./images/img10.png" to the address given by imagePath.
 * Thus the address given by imagePath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @param index - the index of the image.
 *
 * @return
 * - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 * - SP_CONFIG_INDEX_OUT_OF_RANGE - if index >= spNumOfImages
 * - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index);

/**
 * The function stores in pcaPath the full path of the pca file.
 * For example given the values of:
 *  spImagesDirectory = "./images/"
 *  spPcaFilename = "pca.yml"
 *
 * The functions stores "./images/pca.yml" to the address given by pcaPath.
 * Thus the address given by pcaPath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @return
 *  - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 *  - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config);

/**
 * Frees all memory resources associate with config. 
 * If config == NULL nothig is done.
 */
void spConfigDestroy(SPConfig config);


/*
 * Returns the PCA path.
 * @param config - the configuration structure
 * @param msg - pointer in which the msg returned by the function is stored
 * @ return
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL or pcaPath == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config);



char* spConfigGetImageDirectory(SPConfig config);


/**
 * Returns the image prefix. i.e: 'img'
 *
 * @param config - the configuration structure
 
 * @return char* with the image prefix in case of success and NULL otherwise.

 */

char* spConfigGetspImagesPrefix(SPConfig config);

/**
 * Returns the image suffix. i.e: 'jpg'
 *
 * @param config - the configuration structure
 
 * @return
			char*	- with the image suffix in case of success
			NULL	- otherwise.
 */

char* spConfigGetspImagesSuffix(SPConfig config);

/**
 * Returns the PCA file name.
 *
 * @param config - the configuration structure
 
 * @return
			char*	- with PCA file name in case of success
			NULL	- otherwise.
 */

char* spConfigGetspPCAFilename(SPConfig config);

/**
 * Returns the logger file name.
 *
 * @param config - the configuration structure
 
 * @return
			char*	- with logger file name in case of success
			NULL	- otherwise.
 */

char* spConfigGetspLoggerFilename(SPConfig config);

/**
 * Returns the number of similar images. i.e: 5 (the 5 most similar images to query will be displayed)
 *
 * @param config - the configuration structure
 
 * @return
		int with a number greater than (-1)		- in case of success
		(-1)	- otherwise.
 */
int spConfigGetspNumOfSimilarImages(SPConfig config);

/**
 * Returns the spKNN (which is the length of the BPQueue we are using in KNearestNeighburs.
 *
 * @param config - the configuration structure
 
 * @return
		int with a number greater than (-1)		- in case of success
		(-1)	- otherwise.
 */

int spConfigGetspKNN(SPConfig config);

/**
 * Returns the requested logger level.
 *
 * @param config - the configuration structure
 
 * @return one of the following defined SP_LOGGER_LEVEL:
		SP_LOGGER_ERROR_LEVEL	- (In this level only error messages are printed)
		SP_LOGGER_WARNING_ERROR_LEVEL	- (In this level only warning and error messages are printed)
		SP_LOGGER_INFO_WARNING_ERROR_LEVEL	 - (in this level info, warning and error messages are printed)
		SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL	- (in this level all message are printed)
		(-1)	- if congig is NULL. //TODO is this ok? should we add INVALID_CONFIG to the SP_LOGGER_LEVEL enum?
 */
SP_LOGGER_LEVEL spConfigGetspLoggerLevel(SPConfig config);

/**
 * Returns the requested KDTree Split Method.
 *
 * @param config - the configuration structure
 
 * @return one of the following defined SP_KDTREE_SPLIT_METHOD_TYPE:
		MAX_SPREAD
		RANDOM 
		INCREMENTAL
		INVALID_CONFIG	- if congig is NULL.
 */


SP_KDTREE_SPLIT_METHOD_TYPE spConfigGetspKDTreeSplitMethod(SPConfig config);

/**
 * Returns the requested Image Path Feat.
 *
 * @param config - the configuration structure
 
 * @return one of the following defined SP_CONFIG_MSG:
		SP_CONFIG_INVALID_ARGUMENT	- if (imagePath == NULL) or (config == NULL)
		SP_CONFIG_INDEX_OUT_OF_RANGE	- if (index >= config->spNumOfImages)
		SP_CONFIG_SUCCESS	 - in case of success 

 */

SP_CONFIG_MSG spConfigGetImagePathFeat(char* imagePath, const SPConfig config, int index);



#endif /* SPCONFIG_H_ */
