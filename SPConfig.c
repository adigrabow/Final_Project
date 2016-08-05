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

/* image suffix values*/
#define JPG (".jpg")
#define PNG (".png")
#define BMP (".bmp")
#define GIF (".gif")

/*Internal status messages*/
#define INTERNAL_STATUS_SUCCESS ("success")
#define INTERNAL_STATUS_INVALID_CONFIG_LINE ("invalid config line")
#define INTERNAL_STATUS_CONSTRAIT_NOT_MET ("constrait not met")
#define INTERNAL_DEFAULT_STRING_VALUE ("Variable not set yet")

/*error messages MACROS*/
#define INVALID_CONFIG_LINE_ERROR_MSG ("File: %s\nLine: %d\nMessage: Invalid configuration line\n")
#define CONSTRAIT_NOT_MET_ERROR_MSG ("File: %s\nLine: %d\nMessage: Invalid value - constraint not met\n")
#define PARAMATER_IS_NOT_SET_ERROR_MSG ("File: %s\nLine: %d\nMessage: Parameter %s is not set\n")


struct sp_config_t{

	char spImagesDirectory[MAX_STRING_LEN];
	char spImagesPrefix[MAX_STRING_LEN];
	char spImagesSuffix[MAX_STRING_LEN];
	int spNumOfImages;
	int spPCADimension;
	char spPCAFilename[MAX_STRING_LEN];
	int spNumOfFeatures;
	bool spExtractionMode;
	int spNumOfSimilarImages;
	SP_KDTREE_SPLIT_METHOD_TYPE spKDTreeSplitMethod; /*default value= MAX_SPREAD*/ //TODO where to implement this enum??
	int spKNN; /*default value= 1*/
	bool spMinimalGUI;
	int spLoggerLevel;
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

	int configLineCounter = 0;
	char line[MAX_STRING_LEN];
	int n = MAX_STRING_LEN;
	int valueFlag = 0; /* flag == 1 if word is a value, and 0 if its a variable name */

	if(filename == NULL){
		//	*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}

	SPConfig config = (SPConfig)malloc(sizeof(SPConfig));//TODO free memory!
	if(config == NULL){
		//		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	FILE* configFile = fopen(filename, "r");
	if(configFile == NULL){
		//*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}
	assignDefaultValues(config);

	while(fgets(line,n,configFile) != NULL){

		char* string;
		const char * delimiter = EQUALS;
		char value[MAX_STRING_LEN];
		char variableName[MAX_STRING_LEN];
		char statusMSG[MAX_STRING_LEN] = INTERNAL_STATUS_SUCCESS;
		configLineCounter++;
		valueFlag = 0;

		printf("line-%d: %s\n",configLineCounter,line);

		if(isLineCommentLine(line))
			continue;
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
				assignValueToVariable(config, variableName, value, statusMSG);

				if((strncmp(statusMSG, INTERNAL_STATUS_INVALID_CONFIG_LINE, 19) == 0) ||
						(strncmp(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET, 17) == 0)){
					free(config);
					fclose(filename);
					return NULL;
				}

			}

			string = strtok(NULL, delimiter);
			valueFlag = 1;
		}
	}

	if(strncmp(config->spImagesDirectory, INTERNAL_DEFAULT_STRING_VALUE,20) == 0){

		printf(PARAMATER_IS_NOT_SET_ERROR_MSG, __FILE__, configLineCounter, "spImagesDirectory");
		free(config);
		fclose(filename);
		return NULL;

	}
	if(strncmp(config->spImagesPrefix, INTERNAL_DEFAULT_STRING_VALUE,20) == 0){
		printf(PARAMATER_IS_NOT_SET_ERROR_MSG, __FILE__, configLineCounter, "spImagesPrefix");
		free(config);
		fclose(filename);
		return NULL;
	}
	if(strncmp(config->spImagesSuffix, INTERNAL_DEFAULT_STRING_VALUE,20) == 0){
		printf(PARAMATER_IS_NOT_SET_ERROR_MSG, __FILE__, configLineCounter, "spImagesSuffix");
		free(config);
		fclose(filename);
		return NULL;
	}
	if((config->spNumOfImages == -1)){
		printf(PARAMATER_IS_NOT_SET_ERROR_MSG, __FILE__, configLineCounter, "spNumOfImages" );
		free(config);
		fclose(filename);
		return NULL;
	}

	printVariableValuesOfConfig(config);
	fclose(filename);
	return config;


}
void assignDefaultValues(SPConfig config){

	strcpy(config->spImagesDirectory, INTERNAL_DEFAULT_STRING_VALUE);
	strcpy(config->spImagesPrefix, INTERNAL_DEFAULT_STRING_VALUE);
	strcpy(config->spImagesSuffix, INTERNAL_DEFAULT_STRING_VALUE);
	config->spNumOfImages = -1;

	config->spPCADimension = 20;
	strcpy(config->spPCAFilename, "pca.yml");
	config->spNumOfFeatures = 100;
	config->spExtractionMode = true;
	config->spMinimalGUI = false;
	config->spNumOfSimilarImages = 1;
	config->spKNN = 1;
	config->spKDTreeSplitMethod = MAX_SPREAD;
//TODO add spLogger Level and spLoggerFileName
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

void assignValueToVariable(SPConfig config, char* variableName, char* value, char* statusMSG){

	printf("assignValueToVariable: variableName = %s, value = %s\n",variableName, value);

	if(strncmp(variableName, "spImagesDirectory", 17) == 0){
		printf("assignValueToVariable: entered  spImagesDirectory if\n");
		strcpy(config->spImagesDirectory, value);
		memset(statusMSG, 0, sizeof(statusMSG));
		strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
		return;

	}
	if(strncmp(variableName,"spImagesPrefix",14) == 0){
		printf("assignValueToVariable: entered spImagesPrefix if\n");
		strcpy(config->spImagesPrefix, value);
		memset(statusMSG, 0, sizeof(statusMSG));
		strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
		return;
	}
	if(strncmp(variableName,"spImagesSuffix",14) == 0){
		printf("assignValueToVariable: entered spImagesSuffix if\n");
		if((strcmp(value, JPG) == 0) || (strcmp(value, PNG) == 0) ||
				(strcmp(value, BMP) == 0) || (strcmp(value, GIF) == 0)){
			strcpy(config->spImagesSuffix,value);
			memset(statusMSG, 0, sizeof(statusMSG));
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			return;
		}

		memset(statusMSG, 0, sizeof(statusMSG));
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		return;
	}
	if(strncmp(variableName,"spNumOfImages",13) == 0){
		printf("assignValueToVariable: entered spNumOfImages if\n");
		int val = atoi(value);
		if(val > 0){
			config->spNumOfImages = val;
			memset(statusMSG, 0, sizeof(statusMSG));
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			return;
		}

		memset(statusMSG, 0, sizeof(statusMSG));
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		return;
	}
	if(strncmp(variableName,"spPCADimension", 14) == 0){
		int val = atoi(value);

		if(10 <= value && value <= 28){
			config->spPCADimension = val;
			memset(statusMSG, 0, sizeof(statusMSG));
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			return;
		}
		memset(statusMSG, 0, sizeof(statusMSG));
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		return;
	}

	if(strncmp(variableName,"spPCAFilename",13) == 0){
		printf("assignValueToVariable: entered spPCAFilename if\n");
		strcpy(config->spPCAFilename,value);
		memset(statusMSG, 0, sizeof(statusMSG));
		strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
		return;
	}
	if(strncmp(variableName,"spNumOfFeatures",15) == 0){
		printf("assignValueToVariable: entered spNumOfFeatures if\n");
		int val = atoi(value);
		if(val > 0){
			config->spNumOfFeatures = val;
			memset(statusMSG, 0, sizeof(statusMSG));
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			return;
		}
		memset(statusMSG, 0, sizeof(statusMSG));
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		return;
	}

	if(strncmp(variableName,"spExtractionMode",16) == 0){
		printf("assignValueToVariable: entered spExtractionMode if\n");
		if(strncmp(value,"true",4)==0){
			config->spExtractionMode = true;
			memset(statusMSG, 0, sizeof(statusMSG));
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			return;
		}else if(strncmp(value,"false",5)==0){
			config->spExtractionMode = false;
			memset(statusMSG, 0, sizeof(statusMSG));
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			return;
		}
		memset(statusMSG, 0, sizeof(statusMSG));
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		return;


	}
	if(strncmp(variableName,"spNumOfSimilarImages",20) == 0){
		printf("assignValueToVariable: entered spNumOfSimilarImages if\n");
		int val = atoi(value);
		if(val > 0){
			config->spNumOfSimilarImages = val;
			memset(statusMSG, 0, sizeof(statusMSG));
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			return;
		}
		memset(statusMSG, 0, sizeof(statusMSG));
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		return;
	}


	if(strncmp(variableName,"spKDTreeSplitMethod",19) == 0){ //TODO
	}

	if(strncmp(variableName,"spKNN",5) == 0){
		printf("assignValueToVariable: entered spKNN if\n");
		int val = atoi(value);
		if(val > 0){
			config->spKNN = val;
			memset(statusMSG, 0, sizeof(statusMSG));
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			return;
		}
		memset(statusMSG, 0, sizeof(statusMSG));
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		return;
	}
	if(strncmp(variableName,"spMinimalGUI",12) == 0){
		printf("assignValueToVariable: entered spMinimalGUI if\n");
		if(strncmp(value,"true",4) == 0){
			config->spMinimalGUI = true;
			memset(statusMSG, 0, sizeof(statusMSG));
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			return;
		}else if(strncmp(value,"false",5) == 0){
			config->spMinimalGUI = false;
			memset(statusMSG, 0, sizeof(statusMSG));
			strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
			return;
		}
		memset(statusMSG, 0, sizeof(statusMSG));
		strcpy(statusMSG, INTERNAL_STATUS_CONSTRAIT_NOT_MET);
		printf(CONSTRAIT_NOT_MET_ERROR_MSG, __FILE__, __LINE__ );
		return;

	}
	if(strncmp(variableName,"spLoggerFilename",16) == 0){
		printf("assignValueToVariable: entered spLoggerFilename if\n");
		strcpy(config->spLoggerFilename,value);
		memset(statusMSG, 0, sizeof(statusMSG));
		strcpy(statusMSG, INTERNAL_STATUS_SUCCESS);
		return;
	}

	/*line is invalid (neither a comment/empty line nor system parameter configuration)*/
	printf(INVALID_CONFIG_LINE_ERROR_MSG, __FILE__, __LINE__);
	memset(statusMSG, 0, sizeof(statusMSG));
	strcpy(statusMSG, INTERNAL_STATUS_INVALID_CONFIG_LINE);
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
	//	printf("getCleanWordFromString: string=%s, word=%s\n",string,word);
	int firstLetterIndex = 0;
	int lastLetterIndex = 0;
	for(int i = 0; i < strlen(string); i++ ){
		if((*(string + i) != WHITESPACE) && (*(string + i) != TAB)){
			firstLetterIndex = i;
			break;
		}
	}
	//	printf("firstLetterIndex = %d\n",firstLetterIndex);
	//	printf("strlen(string)=%d\n",strlen(string));

	lastLetterIndex = strlen(string) - 1;
	for(int t = firstLetterIndex; t < strlen(string); t++){
		if((*(string + t) == WHITESPACE) || (*(string + t) == TAB)){
			//			printf("*(string + t) = %c\n",*(string + t));
			//if((*(string + t)) == WHITESPACE){
			lastLetterIndex = t;
			break;
		}
	}
	//	lastLetterIndex--;
	//	printf("lastLetterIndex = %d\n",lastLetterIndex);

	//printf("getCleanWordFromString: firstLetterIndex = %d\n",firstLetterIndex);
	int newWordLengthe = lastLetterIndex - firstLetterIndex + 1;
	//	printf("newWordLengthe = %d\n",newWordLengthe);
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
	while(c == WHITESPACE || c == TAB ){
		c = *(line + i);
		i++;
	}
	printf("isLineCommentLine: firstCharIndex = %d\n",i);
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

