// gcc -I/usr/local/include -L/usr/local/lib -ljson-c -lxml2 -Wall 2013510126.c -o 2013510126.out

#include <stdio.h> //standart buffered input/output
#include <string.h> //string operations
#include <stdbool.h> //for Boolean type and values
#include <stdlib.h> //standart library definitions
#include <locale.h>
#include <stddef.h>
#include <json-c/json.h>
#include "utf8_decode.h"


    int total_rec_number;
    char *buff = 0;
    char dataFileName[15];
    char indexFileName[15];
    int recordLength;
    char keyEncoding[5];
    int keyStart;
    int keyEnd;
    int indexFieldLength;
    char order[5];
    

// converting string to binary method for binary comparison
char* stringToBinary(char* s) {
    if(s == NULL) return 0; /* no input string */
    size_t len = strlen(s);
    char *binary = malloc(len*8 + 1); // each char is one byte (8 bits) and + 1 at the end for null terminator
    binary[0] = '\0';
    for(size_t i = 0; i < len; ++i) {
        char ch = s[i];
        for(int j = 7; j >= 0; --j){
            if(ch & (1 << j)) {
                strcat(binary,"1");
            } else {
                strcat(binary,"0");
            }
        }
    }
    return binary;
}     

int binaryFileSearch(FILE* fp, const char name1[indexFieldLength], int firstIdx, int lastIdx){
	printf("\nRunning Binary Search %d %d\n", firstIdx, lastIdx);
	char* first = malloc(indexFieldLength + 1);
    char* last = malloc(indexFieldLength + 1); 
    char* middle = malloc(indexFieldLength + 1);
    first[indexFieldLength] = '\0';
    last[indexFieldLength] = '\0';
    middle[indexFieldLength] = '\0';

	int returnData;
	
	// Calculate the middle Index
	int middleIdx = (firstIdx+lastIdx)/2;
    int temp;
	// Read first record and return if it is the searched one.
	fseek(fp, firstIdx*(sizeof(int) + indexFieldLength) , SEEK_SET);
    fread(&temp, sizeof(int),1,fp);
	fread(first, indexFieldLength, 1, fp);
	if(strcmp(first,name1) == 0)
	{
		returnData=temp;
		return returnData;
	}
	// Read last record and return if it is the searched one.
	fseek(fp, lastIdx*(sizeof(int) + indexFieldLength), SEEK_SET);
    fread(&temp, sizeof(int),1,fp);
	fread(last, indexFieldLength, 1, fp);
	if(strcmp(last,name1) == 0)
	{
		returnData=temp;
		return returnData;
	}
	// Recursion exit condition, If middle index is equal to first or last index
	// required comparisons are already made, so record is not found.
	// Create and return an empty person.
	if(middleIdx==firstIdx || middleIdx == lastIdx) {
		int d=-1;
        printf("Record is not found!! \n");
		return d;
	}

	// Read the middle record and return if it is the searched one.
	fseek(fp, middleIdx*(sizeof(int) + indexFieldLength), SEEK_SET);
    fread(&temp, sizeof(int),1,fp);
	fread(middle, indexFieldLength, 1, fp);
	if(strcmp(middle,name1) == 0)
	{
		returnData=temp;
		return returnData;
	}
	// Determine the record position and recursively call with appropriate attributes.
	if(strcmp(middle,name1)>0) {
		return binaryFileSearch(fp, name1, firstIdx+1, middleIdx-1);
	} 
	else {
		return binaryFileSearch(fp, name1, middleIdx+1, lastIdx-1);
	}
}

int findRecordByName(char *name1) {
    // Open the file
    FILE* inputFile;
    inputFile = fopen(indexFileName, "rb");

    // Calculate initial first and last indexes.
    int firstIdx = 0;
    fseek(inputFile, total_rec_number * (sizeof(int) + indexFieldLength), SEEK_SET);
    int lastIdx = (ftell(inputFile)/(sizeof(int) + indexFieldLength))-1;

    // Initiate the search.
    int result = binaryFileSearch(inputFile, name1, firstIdx, lastIdx);
    fclose(inputFile);
    return result;
}




void printMenu(int* answer){

	//print the user menu
	printf("You can perform the following tasks: \n");
	printf("(1) Open and read json file: \n");
	printf("(2) Create index \n");
	printf("(3) Search \n");
	printf("(4) Close and exit \n");
	printf("Please Select one... \n");
	scanf("%d",answer);
}

/* Json-Parser Part  */

static void json_parse(json_object *jobj)
{   
    enum json_type type;
    json_object_object_foreach(jobj, key, val)
    {   
        type = json_object_get_type(val);
        if(strcmp(key,"dataFileName") == 0){
            strcpy(dataFileName,json_object_get_string(val));
        }
        else if(strcmp(key,"indexFileName")== 0 ){
            strcpy(indexFileName,json_object_get_string(val));
        }
        else if(strcmp(key,"recordLength")== 0 ){
            recordLength = json_object_get_int(val);
        }
        else if(strcmp(key,"keyEncoding")== 0 ){
            strcpy(keyEncoding,json_object_get_string(val));
        }
        else if(strcmp(key,"keyStart")== 0 ){
            keyStart = json_object_get_int(val);  
        }
        else if(strcmp(key,"keyEnd")== 0 ){
            keyEnd = json_object_get_int(val);
        }
        else if(strcmp(key,"order")== 0 ){
            strcpy(order,json_object_get_string(val));
        }
    }
}


static void json_prep(char *a1)
{
    char *buffer2 = 0;
    long length;
    FILE *f = fopen(a1, "rb");

    if(!f){
        printf("json file not exist");
    }
    // here we take the file as a string in memory.
    if (f)
    {
        printf("ftell is: %lu\n", ftell(f));
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        printf("ftell is: %lu\n", ftell(f));
        fseek(f, 0, SEEK_SET);
        buffer2 = malloc(length + 1);
        if (buffer2)
        {
            fread(buffer2, 1, length, f);
        }
        fclose(f);
        buffer2[length] = '\0';
    }

    if (buffer2)
    {
        json_object *jobj = json_tokener_parse(buffer2);
        json_parse(jobj);
    }
}

/* Json-Parser Part Ended  */

void createIndexFile(){
    
    indexFieldLength = keyEnd - keyStart;
    long filelen;
    FILE* fp = fopen(dataFileName, "r+b");
    FILE *indexFile = fopen(indexFileName,"w+b");
    if(!fp){
        printf("error here");
    }
     if(fp != NULL)
    {
        fseek(fp,0,SEEK_END);
        filelen = ftell(fp);
        rewind(fp);
        total_rec_number = filelen/recordLength;
        char * indexElements = (char*) malloc(total_rec_number * indexFieldLength);
    
        printf("total rec no is: %d \n",total_rec_number);
        printf("filelength is: %lu \n",filelen);
        printf("index field length is: %d \n", indexFieldLength);
        printf("index address is: %p\n",indexElements);
        
        // this method creates index file without sorting.. only writing incrementing index numbers 1,2,3,4... and values..
        for(int c=0; c<=total_rec_number-1; c++){
        fseek(fp,c*recordLength+keyStart,SEEK_SET); 
        fread(&indexElements[c*indexFieldLength],indexFieldLength,1,fp);
        fwrite(&c,sizeof(int),1,indexFile);
        fwrite(&indexElements[c*indexFieldLength],indexFieldLength,1,indexFile);
        }
        printf("index address is for 0: %p\n",&indexElements[0]);
        int temp;

        // to determine if the index file has been correctly created.
        for(int i=0;i<total_rec_number;i++){
            fseek(indexFile,i * (sizeof(int)+indexFieldLength),SEEK_SET);
            fread(&temp,sizeof(int),1,indexFile);
            printf("%d. index is: %d \n",i,temp);
        }            
    }

     //here is sorting part.. one by one comparison and swap if needed.
      for(int i=0;i<total_rec_number-1;i++){
        for(int j=0;j<total_rec_number-1;j++){
            fseek(indexFile,(sizeof(int)+indexFieldLength) * j,SEEK_SET);
            char* buf1 = malloc(40);
            char* buf2 = malloc(40);
            int temp1,temp2;
        
            fread(&temp1,sizeof(int),1,indexFile);
            fread(buf1,indexFieldLength,1,indexFile);
            fread(&temp2,sizeof(int),1,indexFile);
            fread(buf2,indexFieldLength,1,indexFile);
            
            if(strcmp(keyEncoding,"CHR") == 0){
                if(strcmp(order,"ASC")==0){
                    // comparing as char 
                    if(strcmp(buf1,buf2) > 0){
                    fseek(indexFile,(sizeof(int)+indexFieldLength) * j,SEEK_SET);
                    fwrite(&temp2,sizeof(int),1,indexFile);
                    fwrite(buf2,indexFieldLength,1,indexFile);
                    fwrite(&temp1,sizeof(int),1,indexFile);
                    fwrite(buf1,indexFieldLength,1,indexFile);
                    }
                }
                else if(strcmp(order,"DESC")==0){
                    if(strcmp(buf1,buf2) < 0){
                    fseek(indexFile,(sizeof(int)+indexFieldLength) * j,SEEK_SET);
                    fwrite(&temp2,sizeof(int),1,indexFile);
                    fwrite(buf2,indexFieldLength,1,indexFile);
                    fwrite(&temp1,sizeof(int),1,indexFile);
                    fwrite(buf1,indexFieldLength,1,indexFile);
                    }
                }else{
                    printf("Wrong Json Input. Encoding: CHR, Order: Wrong!");
                }
            }
            // comparing as binary
            else if(strcmp(keyEncoding,"BIN") == 0){
                char * tempbuf1 = malloc(100);
                char * tempbuf2 = malloc(100);
                tempbuf1 = stringToBinary(buf1);
                tempbuf2 = stringToBinary(buf2);
                
                if(strcmp(order,"ASC")==0){
                    if(strcmp(tempbuf1,tempbuf2)>0){
                      fseek(indexFile,(sizeof(int)+indexFieldLength) * j,SEEK_SET);
                    fwrite(&temp2,sizeof(int),1,indexFile);
                    fwrite(buf2,indexFieldLength,1,indexFile);
                    fwrite(&temp1,sizeof(int),1,indexFile);
                    fwrite(buf1,indexFieldLength,1,indexFile);      
                    }            
                }
                else if(strcmp(order,"DESC")==0){
                    if(strcmp(tempbuf1,tempbuf2)<0){
                    fseek(indexFile,(sizeof(int)+indexFieldLength) * j,SEEK_SET);
                    fwrite(&temp2,sizeof(int),1,indexFile);
                    fwrite(buf2,indexFieldLength,1,indexFile);
                    fwrite(&temp1,sizeof(int),1,indexFile);
                    fwrite(buf1,indexFieldLength,1,indexFile);
                    }      
                }
                else{
                    printf("Wrong Json input.. Encoding: BIN, Order: Wrong!");
                }
                free(tempbuf1);
                free(tempbuf2);
            }
            else if(strcmp(keyEncoding,"UTF")==0){
                if(strcmp(order,"ASC")==0){

                }
                else if(strcmp(order,"DESC")==0){

                }
                else{
                    printf("Wrong Json input.. Encoding: BIN, Order: Wrong!");
                }
            }
                        
            free(buf1);
            free(buf2);
            printf("\n");
        }
     }
     for(int i=0;i<total_rec_number;i++){
            char* buf3 = malloc(40);
            int t1;
            fseek(indexFile,i*(sizeof(int) + indexFieldLength),0);
            fread(&t1,sizeof(int),1,indexFile);
            fread(buf3,indexFieldLength,1,indexFile);
            printf("%d. index number: %d, its value is: %s \n",i,t1,buf3);
            free(buf3);
        }

    fclose(indexFile);
    fclose(fp);
}

int main(int argc, char* argv[])
{      
    int answer;
	int who;
    ab:
    printMenu(&answer);
    while(answer>5 || answer<1)
	{
		printf("\nEnter a valid choice by pressing ENTER key again");
		printMenu(&answer);
	}
    char jsonFileName[30];
    long length;
    switch(answer){
        case 1:
        printf("enter a appropiate json file name:");
        scanf("%s", jsonFileName);
        json_prep(jsonFileName);
        FILE *f = fopen(dataFileName, "rb");
        if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buff = malloc(length + 1);
        if (buff)
        {
            fread(buff, 1, length, f);
        }
        fclose(f);
        buff[length] = '\0';
        goto ab;
    }
        break;
        case 2:
        createIndexFile();
        goto ab;
        break;
        case 3:
            printf("Enter the name that you want to search :");
			char* name1 = malloc(indexFieldLength+1);
            name1[indexFieldLength] = '\0';
			scanf("%s", name1);
            printf("%s",name1);
			who = findRecordByName(name1);
            printf("Was the index found?");
            printf(" %d",who);
            FILE* file1;
    	    file1 = fopen(indexFileName,"rb");
            if(who >-1){
				fseek(file1, who* (sizeof(int) + indexFieldLength), SEEK_SET);
                int tempy;
                char* buff5 = malloc(indexFieldLength+1);
                buff5[indexFieldLength] = '\0';
				fread(&tempy, sizeof(int), 1,file1);
				fread(buff5, indexFieldLength, 1,file1);
				printf("found index number is: %d, value is: %s", tempy,buff5);
            }
            else 
				printf("Not found...");
            goto ab;
            break;
        case 4:
        printf("Program is terminating \n");
        free(buff);
			break;    

    }   
}
