#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

int addStudent(int, char*, char*, float, char*);
int updateStudent(int, char*, char*, float, char*);
int deleteStudent(int);
int stringCopy(char*, char**);
int loadDatabase();
int processFlags(int, char*, char*, char*);
int stringEquals(char*, char*);
int stringToInt(char*);

typedef struct student_records{
	struct student_records* next;
	struct student_records* prev;
	char* first_name;
	char* last_name;

	char* major;
	int id;
	float gpa;
} student_records;

student_records* database;
FILE* file;
FILE* out;

int main(int argc, char** argv) {
	int vflag;
	char* id = NULL;
	char* lastname = NULL;
	char* major = NULL;
	int c;
	out = stdout;
  	
  	database = (student_records*)malloc(sizeof(student_records));
  	database->prev = NULL;
  	database->next = NULL;
  	database->id = -1;
  	
  	if (argc <= 2){
  		fprintf(out, "NO QUERY PROVIDED\n");
  		return 1;
  	}
  	
  	file = fopen(*(argv+1), "r+");
	loadDatabase();
  	
  	while((c = getopt(argc, argv, "vi:f:m:o:")) != -1){
		switch(c){
			case 'v':
				vflag = 1;
				break;
			case 'i':
				id = optarg;
				break;
			case 'f':
				lastname = optarg;
				break;
			case 'm':
				major = optarg;
				break;
			case 'o':
				out = fopen(optarg, "w");
				break;
			default:
				fprintf(out, "FAILED TO PARSE FILE\n");
				return 1;
		}
	}
	processFlags(vflag, id, lastname, major);
	
	fclose(file);
	return 0;
}

int loadDatabase(){
	char command[255];
	char fName[255];
	char lName[255];
	char major[255];
	int id;
	float gpa;
	int c;
	do{
		c = fscanf(file, "%s", command);
		if (c == EOF) return 0;

		switch(*command){
			case 'A':
				fscanf(file, "%d %s %s %f %s ", &id, fName, lName, &gpa, major);
				addStudent(id, fName, lName, gpa, major);
				break;
			case 'D':
				fscanf(file, "%d ", &id);
				deleteStudent(id);
				break;
			case 'U':
				fscanf(file, "%d %s %s %f %s ", &id, fName, lName, &gpa, major);
				updateStudent(id, fName, lName, gpa, major);
				break;
			default:
				return -1;
		}
	} while (1);
}

int deleteStudent(int id){
	student_records* cursor = database;
	while(cursor != NULL){
		if (cursor->id == id){
			if (cursor->prev != NULL){
				cursor->prev->next = cursor->next;
			}
			if (cursor->next != NULL){
				cursor->next->prev = cursor->prev;
			}
			free(cursor->first_name);
			free(cursor->last_name);
			free(cursor->major);
			free(cursor);
			return 0;
		} else {
			cursor = cursor->next;
		}
	}
	return -1;
}

int addStudent(int id, char* firstName, char* lastName, float gpa, char* major){
	student_records* cursor = database;
	while(cursor != NULL){
		if (cursor->id == id){
			return -1;
		} else if (cursor->next == NULL && cursor->id < id){
			if (cursor->id == -1){
				cursor->id = id;
				stringCopy(firstName, &(cursor->first_name));
				stringCopy(lastName, &(cursor->last_name));
				cursor->gpa = gpa;
				stringCopy(major, &(cursor->major));
			} else {
				student_records* add;
				add = (student_records*)malloc(sizeof(student_records));
				add->next = NULL;
				add->prev = cursor;
				add->id = id;
				stringCopy(firstName, &(add->first_name));
				stringCopy(lastName, &(add->last_name));
				add->gpa = gpa;
				stringCopy(major, &(add->major));
				
				cursor->next = add;
			}
			return 0;
		} else if (cursor->id > id){
			student_records* add;
			add = (student_records*)malloc(sizeof(student_records));
			
			add->next = cursor;
			add->prev = cursor->prev;
			add->id = id;
			stringCopy(firstName, &(add->first_name));
			stringCopy(lastName, &(add->last_name));
			add->gpa = gpa;
			stringCopy(major, &(add->major));
			
			if (cursor->prev != NULL){
				cursor->prev->next = add;
			} else{
				database = add;
			}
			cursor->prev = add;
			return 0;
		}
		cursor = cursor->next;
	}
}

int updateStudent(int id, char* firstName, char* lastName, float gpa, char* major){
	student_records* cursor = database;
	while(cursor != NULL){
		if (cursor->id == id){
			stringCopy(firstName, &(cursor->first_name));
			stringCopy(lastName, &(cursor->last_name));
			cursor->gpa = gpa;
			stringCopy(major, &(cursor->major));
			return 0;
		} else {
			cursor = cursor->next;
		}
	}
	return -1;
}

int stringCopy(char* source, char** dest){
	int i;
	int len = 0;
	for(i = 0; *(source+i) != '\0'; i++)
		len++;
	char* temp = (char*)malloc((len+1) * sizeof(char));
	for(i = 0; i <= len; i++)
		*(temp+i) = *(source+i);
	(*dest) = temp;
	return 0;
}

int processFlags(int vflag, char* id, char* lastName, char* major){
	if (vflag && id == NULL && lastName == NULL && major == NULL){
		student_records* cursor = database;
		while(cursor != NULL){
			fprintf(out, "%d %s %s %.2f %s\n", cursor->id, cursor->first_name, cursor->last_name, cursor->gpa, cursor->major);
			cursor = cursor->next;
		}
	} 
	if (id != NULL){ //MATCH ID
		student_records* cursor = database;
		int i = stringToInt(id);
		while(cursor != NULL){
			if (i == cursor->id){
				fprintf(out, "%d %s %s %.2f %s\n", cursor->id, cursor->first_name, cursor->last_name, cursor->gpa, cursor->major);
				break;
			}
			cursor = cursor->next;
		}
	}
	if (lastName != NULL){
		student_records* cursor = database;
		while(cursor != NULL){
			if (stringEquals(cursor->last_name, lastName))
				fprintf(out, "%d %s %s %.2f %s\n", cursor->id, cursor->first_name, cursor->last_name, cursor->gpa, cursor->major);
			cursor = cursor->next;
		}
	} 
	if (major != NULL){
		student_records* cursor = database;
		while(cursor != NULL){
			if (stringEquals(cursor->major, major))
				fprintf(out, "%d %s %s %.2f %s\n", cursor->id, cursor->first_name, cursor->last_name, cursor->gpa, cursor->major);
			cursor = cursor->next;
		}
	}
	return 0;
}

int stringEquals(char* s1, char* s2){
	int i;
	for (i = 0; *(s1+i) != '\0'; i++){
		if (toupper(*(s1+i)) != toupper(*(s2+i)))
			return 0;
	}
	return 1;
}

int stringToInt(char* s){
	int i, num = 0;
	int len = 0;
	for(i = 0; *(s+i) != '\0'; i++)
		len++;
	
	for(i = len-1; i >=0; i--){
		num = num + (*(s+i)- '0') * (int)pow(10, (len - 1) - i);
	}
	return num;
}