#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int addStudent(int, char*, char*, float, char*);
int updateStudent(int, char*, char*, float, char*);
int deleteStudent(int);

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

int main(int argc, char** argv) {
	int vflag = 0;
	char* id = NULL;
	char* lastname = NULL;
	char* major = NULL;
	char* filename = NULL;
  	int c;
  	
  	database = (student_records*)malloc(sizeof(student_records));
  	
  	printf("%d\n", argc);
  	
  	if (argc <= 1){
  		printf("NO QUERY PROVIDED\n");
  		return 1;
  	}
  	
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
				filename = optarg;
				break;
			default:
				printf("FAILED TO PARSE FILE\n");
				return 1;
		}
	}
  	return 0;
}

int deleteStudent(int id){
	student_records* cursor = database;
	while(cursor != NULL){
		if (cursor->id == id){
			cursor->prev->next = cursor->next;
			cursor->next->prev = cursor->prev;
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
		} else if (cursor->id > id){
			student_records* add;
			add->next = cursor;
			add->prev = cursor->prev;
			add->id = id;
			add->first_name = firstName;
			add->last_name = lastName;
			add->gpa = gpa;
			add->major = major;
			cursor->prev->next = add;
			cursor->prev = add;
			return 0;
		} else {
			cursor = cursor->next;
		}
	}
}

int updateStudent(int id, char* firstName, char* lastName, float gpa, char* major){
	student_records* cursor = database;
	while(cursor != NULL){
		if (cursor->id == id){
			free(cursor->first_name);
			free(cursor->last_name);
			free(cursor->major);
			cursor->first_name = firstName;
			cursor->last_name = lastName;
			cursor->gpa = gpa;
			cursor->major = major;
			return 0;
		} else {
			cursor = cursor->next;
		}
	}
	return -1;
}
