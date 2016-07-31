/*
 * SPConfig.c
 *
 *  Created on: Jul 30, 2016
 *      Author: adigrabow
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "SPConfig.h"
#define MAX_STRING_LEN 1024 /* max len of a line in config file is 1024 char */
#ifndef NULL
#define NULL   ((void *) 0)
#endif

/*characters MACROS:*/
#define hash ('#')
#define tab ('\t')
#define vertical ('\v')
#define newline ('\r')
#define whitespace (' ')


struct sp_config_t{

	char spImagesDirectory[MAX_STRING_LEN];
	char spImagesPrefix[MAX_STRING_LEN];
	char spImagesSuffix[MAX_STRING_LEN];/*Constraint: the string value is in the following set { .jpg , .png , .bmp , .gif }*/
	int spNumOfImages; /*Constraint: positive integer.*/
	int spPCADimension; /*positive integer in the range [10,28],default value = 20*/
	char spPCAFilename[MAX_STRING_LEN]; /*default value = pca.yml*/
	int spNumOfFeatures; /*positive integer,default value = 100 .*/
	bool spExtractionMode; /*default value = true*/
	int spNumOfSimilarImages; /*default value= 1*/
	SP_KDTREE_SPLIT_METHOD_TYPE spKDTreeSplitMethod; /*default value= MAX_SPREAD*/ //TODO where to implement this enum??
	int spKNN; /*default value= 1*/
	bool spMinimalGUI; /*default value = false*/
	int spLoggerLevel; /*default value= 3*/ /*{1,2,3,4} 1:error, 2:warning, 3:info, 4:debug*/
	char spLoggerFilename[MAX_STRING_LEN]; /*default value= stdout*/

};


/***** Functions Implementation *****/

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg){
	assert(msg == NULL);
	if(filename == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}

	SPConfig config = (SPConfig)malloc(sizeof(SPConfig));//TODO free memory!
	if(config == NULL){
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	/*setting default values*/
	config->spPCADimension = 20;
	strcpy(config->spPCAFilename, "pca.yml");
	config->spNumOfFeatures = 100;
	config->spExtractionMode = true;
	config->spMinimalGUI = false;
	config->spNumOfSimilarImages = 1;
	config->spKNN = 1;
	config->spKDTreeSplitMethod = MAX_SPREAD;



	FILE* configFile = fopen(filename, "r");
	if(configFile == NULL){
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}
	char line[MAX_STRING_LEN];
	int n = MAX_STRING_LEN;
	int valueFlag = 0; /* flag == 1 if word is value and 0 if its a variable name */

	while(fgets(line,n,configFile) != NULL){
		valueFlag = 0;
		printf("line: %s\n",line);

		if(isLineCommentLine(line))
			continue;

		char* string;
		const char * delimiter = "=";
		char value[MAX_STRING_LEN];
		char variableName[MAX_STRING_LEN];
		string = strtok(line, delimiter);

		while(string != NULL ){

			if(valueFlag == 0){
				memset(variableName, 0, sizeof(variableName));
				getCleanWordFromString(string, variableName);
				printf("variable name is: %s\n", variableName);
			}

			if(valueFlag == 1){
				memset(value, 0, sizeof(value));
				getCleanWordFromString(string, value);
				printf("value is: %s\n", value);

			}


			string = strtok(NULL, delimiter);
			valueFlag = 1;

		}


	}





	*msg = SP_CONFIG_SUCCESS;
	return config;
	/*
	 * @param msg - pointer in which the msg returned by the function is stored
	 * @return NULL in case an error occurs. Otherwise, a pointer to a struct which
	 * 		   contains all system configuration.
	 *
	 * The resulting value stored in msg is as follow:
	 * - SP_CONFIG_CANNOT_OPEN_FILE - if the configuration file given by filename cannot be open
	 * - SP_CONFIG_ALLOC_FAIL - if an allocation failure occurred
	 * - SP_CONFIG_INVALID_INTEGER - if a line in the config file contains invalid integer
	 * - SP_CONFIG_INVALID_STRING - if a line in the config file contains invalid string
	 * - SP_CONFIG_MISSING_DIR - if spImagesDirectory is missing
	 * - SP_CONFIG_MISSING_PREFIX - if spImagesPrefix is missing
	 * - SP_CONFIG_MISSING_SUFFIX - if spImagesSuffix is missing
	 * - SP_CONFIG_MISSING_NUM_IMAGES - if spNumOfImages is missing
	 * - SP_CONFIG_SUCCESS - in case of success*/




}

bool isLineValid(char* line){
	int i = 0;
	while(*(line+i) != '\0'){
		if(*(line+i) == '#'){ /* # in the middle of the line is not valid!*/
			printf("line is invalid: # in the middle of the line\n");
			return false;
		}
	}
	return true;
}

void getCleanWordFromString(char* string, char* word){
	int firstLetterIndex = 0;
	for(int i = 0; i < strlen(string); i++ ){
		if((*(string + i) != whitespace) && (*(string + i) != tab)){
			firstLetterIndex = i;
			break;
		}
	}
	//printf("getCleanWordFromString: firstLetterIndex = %d\n",firstLetterIndex);
	int k = firstLetterIndex;
	for(int j = 0; j < (strlen(string) - k); j++){
		*(word + j) = *(string + firstLetterIndex);
		firstLetterIndex++;
	}

}


bool isStringValid(char* string){
	int i = 1;

	while(*(string+i) != '\0'){
		if(*(string+i) == hash || (*(string) == whitespace) || (*(string) == tab) ||
				(*(string) == vertical) || (*(string) == newline)){ /* #,'\t','\r'.. in the middle of the line is not valid!*/
			printf("isStringValid: false\n");
			return false;
		}
	}
	printf("isStringValid: true\n");
	return true;
}

bool isLineCommentLine(char* line){
	char c = ' ';
	int i = 0;
	while(c == ' '){
		c = *(line + i);
		i++;
	}
	if(c == hash){
		printf("isLineCommentLine: true\n");
		return true;
	}
	printf("isLineCommentLine: false\n");
	return false;
}




