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

#ifndef NULL
#define NULL   ((void *) 0)
#endif

#define MAX_STRING_LEN 1024

/*characters MACROS:*/
#define HASH ('#')
#define TAB ('\t')
#define VERTICAL ('\v')
#define NEW_LINE ('\r')
#define WHITESPACE (' ')
#define EQUALS ("=")

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


/* ########################
 * Functions Implementation
 *  ######################*/

int main(){
	SP_CONFIG_MSG* msg;
	spConfigCreate("a.txt");
	return 0;
}


SPConfig spConfigCreate(const char* filename){
	if(filename == NULL){
		return NULL;
	}

	SPConfig config = (SPConfig)malloc(sizeof(SPConfig));//TODO free memory!
	if(config == NULL){
		return NULL;
	}

	assignDefaultValues(config);

	FILE* configFile = fopen(filename, "r");
	if(configFile == NULL){
		return NULL;
	}
	char line[MAX_STRING_LEN];
	int n = MAX_STRING_LEN;
	int valueFlag = 0; /* flag == 1 if word is a value, and 0 if its a variable name */

	while(fgets(line,n,configFile) != NULL){
		valueFlag = 0;
		printf("line: %s\n",line);

		if(isLineCommentLine(line))
			continue;

		char* string;
		const char * delimiter = EQUALS;
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
				printf("string = %s\n",string);
				memset(value, 0, sizeof(value));
				getCleanWordFromString(string, value);
				printf("value is: %s\n", value);
				assignValueToVariable(config, variableName, value);
				/*				if((isStringValid(variableName) == true) && (isStringValid(value) == true)){
					printf("both strings are valid\n");
					assignValueToVariable(config, variableName, value);
				}
				else{
					printf("string is not valid!\n");
					return NULL;
				}*/
			}

			string = strtok(NULL, delimiter);
			valueFlag = 1;
		}
	}

	printVariableValuesOfConfig(config);
	return config;


}
void assignDefaultValues(SPConfig config){

	config->spPCADimension = 20;
	strcpy(config->spPCAFilename, "pca.yml");
	config->spNumOfFeatures = 100;
	config->spExtractionMode = true;
	config->spMinimalGUI = false;
	config->spNumOfSimilarImages = 1;
	config->spKNN = 1;
	config->spKDTreeSplitMethod = MAX_SPREAD;

	return;
}


void printVariableValuesOfConfig(SPConfig config){
	printf("***********printing config values***********\n");
	if(config == NULL){
		printf("config is NULL\n");
		return;
	}

	printf("spImagesDirectory = %s\n",config->spImagesDirectory);
	printf("spImagesPrefix = %s\n",config->spImagesPrefix);
	printf("spImagesSuffix = %s\n",config->spImagesSuffix);
	printf("spNumOfImages = %d\n",config->spNumOfImages);
	printf("spPCADimension = %d\n",config->spPCADimension);
	printf("spPCAFilename = %s\n",config->spPCAFilename);
	printf("spNumOfFeatures = %d\n",config->spNumOfFeatures);
	printf("spNumOfSimilarImages = %d\n",config->spNumOfSimilarImages);
	printf("spKNN = %d\n",config->spKNN);
	printf("spLoggerLevel = %d\n",config->spLoggerLevel);
	printf("spLoggerFilename = %s\n",config->spLoggerFilename);
	if(config->spExtractionMode == true){
		printf("spExtractionMode = %s\n","true");
	}else{
		printf("spExtractionMode = %s\n","false");
	}
	if(config->spMinimalGUI == true){
		printf("spMinimalGUI = %s\n","true");
	}else{
		printf("spMinimalGUI = %s\n","false");
	}
	/* printing does not include:
	SP_KDTREE_SPLIT_METHOD_TYPE spKDTreeSplitMethod; default value= MAX_SPREAD*/

}

void assignValueToVariable(SPConfig config, char* variableName, char* value){

	printf("assignValueToVariable: variableName = %s, value = %s\n",variableName, value);

	if(strncmp(variableName, "spImagesDirectory", 17) == 0){
		printf("assignValueToVariable: entered if\n");
		strcpy(config->spImagesDirectory, value);
	}
	else if(strncmp(variableName,"spImagesPrefix",14) == 0){
		printf("assignValueToVariable: entered if\n");
		strcpy(config->spImagesPrefix, value);
	}
	else if(strncmp(variableName,"spImagesSuffix",14) == 0){
		printf("assignValueToVariable: entered if\n");
		strcpy(config->spImagesSuffix,value);
	}
	else if(strncmp(variableName,"spNumOfImages",13) == 0){
		printf("assignValueToVariable: entered if\n");
		int val = atoi(value);
		config->spNumOfImages = val;
	}
	else if(strncmp(variableName,"spPCADimension", 14) == 0){
		printf("assignValueToVariable: entered if\n");
		int val = atoi(value);
		config->spPCADimension = val;
	}
	else if(strncmp(variableName,"spPCAFilename",13) == 0){
		printf("assignValueToVariable: entered if\n");
		strcpy(config->spPCAFilename,value);
	}
	else if(strncmp(variableName,"spNumOfFeatures",15) == 0){
		printf("assignValueToVariable: entered if\n");
		int val = atoi(value);
		config->spNumOfFeatures = val;
	}
	else if(strncmp(variableName,"spExtractionMode",16) == 0){
		printf("assignValueToVariable: entered if\n");
		if(strncmp(value,"true",4)==0){
			config->spExtractionMode = true;
		}else{
			config->spExtractionMode = false;
		}
	}
	else if(strncmp(variableName,"spNumOfSimilarImages",20) == 0){
		printf("assignValueToVariable: entered if\n");
		int val = atoi(value);
		config->spNumOfSimilarImages = val;
	}
	else if(strncmp(variableName,"spKDTreeSplitMethod",19) == 0){ //TODO
	}

	else if(strncmp(variableName,"spKNN",5) == 0){
		printf("assignValueToVariable: entered if\n");
		int val = atoi(value);
		config->spKNN = val;
	}
	else if(strncmp(variableName,"spMinimalGUI",12) == 0){
		printf("assignValueToVariable: entered if\n");
		if(strncmp(value,"true",4) == 0){
			config->spMinimalGUI = true;
		}else{
			config->spMinimalGUI = false;
		}
	}
	else if(strncmp(variableName,"spLoggerFilename",16) == 0){
		printf("assignValueToVariable: entered if\n");
		strcpy(config->spLoggerFilename,value);
	}
	/*line is invalid (neither a comment/empty line nor system parameter configuration)*/
	else{
		/*“File: <the configuration filename>
Line: <the number of the invalid line>
Message: Invalid configuration line”*/
	}
	return;

	/*
	SP_KDTREE_SPLIT_METHOD_TYPE spKDTreeSplitMethod; default value= MAX_SPREAD //TODO where to implement this enum??
	int spLoggerLevel; default value= 3 {1,2,3,4} 1:error, 2:warning, 3:info, 4:debug
	 */
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
	printf("getCleanWordFromString: string=%s, word=%s\n",string,word);
	int firstLetterIndex = 0;
	int lastLetterIndex = 0;
	for(int i = 0; i < strlen(string); i++ ){
		if((*(string + i) != WHITESPACE) && (*(string + i) != TAB)){
			firstLetterIndex = i;
			break;
		}
	}
	printf("firstLetterIndex = %d\n",firstLetterIndex);
	printf("strlen(string)=%d\n",strlen(string));

	lastLetterIndex = strlen(string) - 1;
	for(int t = firstLetterIndex; t < strlen(string); t++){
		if((*(string + t) == WHITESPACE) || (*(string + t) == TAB)){
			printf("*(string + t) = %c\n",*(string + t));
			//if((*(string + t)) == WHITESPACE){
			lastLetterIndex = t;
			break;
		}
	}
	//	lastLetterIndex--;
	printf("lastLetterIndex = %d\n",lastLetterIndex);

	//printf("getCleanWordFromString: firstLetterIndex = %d\n",firstLetterIndex);
	int newWordLengthe = lastLetterIndex - firstLetterIndex + 1;
	printf("newWordLengthe = %d\n",newWordLengthe);
	for(int j = 0; j < newWordLengthe; j++){
		if(*(string + firstLetterIndex) != '\n'){
			*(word + j) = *(string + firstLetterIndex);
			firstLetterIndex++;
		}

	}
	printf("END OF FUNC getCleanWordFromString: word=%s\n",word);

}



bool isStringValid(char* string){
	if(string == NULL){
		printf("string is NULL\n");
		return false;
	}
	int stringLength = strlen(string);

	printf("isStringValid: string = %s, strlen = %d\n",string,stringLength);
	int i = 0;

	while(*(string+i) != '\0'){
		printf("isStringValid: *(string + %d) = %c\n",i,*(string+i));
		if(*(string+i) == HASH || (*(string+i) == WHITESPACE) || (*(string + i) == TAB) ||
				(*(string + i) == VERTICAL) || (*(string + i) == NEW_LINE)){ /* #,'\t','\r'.. in the middle of the line is not valid!*/
			printf("isStringValid: false\n");
			return false;
		}
		i++;
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
	if(c == HASH){
		printf("isLineCommentLine: true\n");
		return true;
	}
	printf("isLineCommentLine: false\n");
	return false;
}

/*************************************************
 * real code (second function is for testing)!!!
 * ************************************************/
/*

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

	setting default values
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
	int valueFlag = 0;  flag == 1 if word is value and 0 if its a variable name

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
				assignValueToVariable(config, variableName, value);
			}

			string = strtok(NULL, delimiter);
			valueFlag = 1;
		}
	}

	printVariableValuesOfConfig(config);
 *msg = SP_CONFIG_SUCCESS;
	return config;


}
 */
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
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg != NULL);
	if(config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	if(config->spExtractionMode == true){
		*msg = SP_CONFIG_SUCCESS;
		return true;
	}else{
		*msg = SP_CONFIG_SUCCESS; //TODO make sure if "false" is also considered success
		return false;
	}

 }

