#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int addStudent(int, char*, char*, float, char*);
int updateStudent(int, char*, char*, float, char*);
int deleteStudent(int);
int loadDatabase();

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
	int vflag = 0;
	char* id = NULL;
	char* lastname = NULL;
	char* major = NULL;
	char* filename = NULL;
  	int c;
  	out = stdout;
  	
  	database = (student_records*)malloc(sizeof(student_records));
  	database->prev = NULL;
  	database->next = NULL;
  	database->id = -1;
  	
  	if (argc <= 1){
  		fprintf(out, "NO QUERY PROVIDED\n");
  		return 1;
  	}
  	
  	char* fileIn = *(argv+1);
  	
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
				fprintf(out, "FAILED TO PARSE FILE\n");
				return 1;
		}
	}
	
	file = fopen(fileIn, "r+");
	loadDatabase();
  	return 0;
}

int loadDatabase(){
	char* command;
	char* fName;
	char* lName;
	char* major;
	int id;
	float gpa;
	do{
		fscanf(file, "%s", command);
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
	} while (0);
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
			add = (student_records*)malloc(sizeof(student_records));
			
			add->next = cursor;
			add->prev = cursor->prev;
			add->id = id;
			add->first_name = firstName;
			add->last_name = lastName;
			add->gpa = gpa;
			add->major = major;
			
			if (cursor->prev != NULL){
				cursor->prev->next = add;
			} else{
				database = add;
			}
			cursor->prev = add;
			return 0;
		} else if (cursor->next == NULL){
			if (cursor->id == -1){
				cursor->id = id;
				cursor->first_name = firstName;
				cursor->last_name = lastName;
				cursor->gpa = gpa;
				cursor->major = major;
			} else {
				student_records* add;
				add = (student_records*)malloc(sizeof(student_records));
			
				add->next = NULL;
				add->prev = cursor;
				add->id = id;
				add->first_name = firstName;
				add->last_name = lastName;
				add->gpa = gpa;
				add->major = major;
				
				cursor->next = add;
			}
			return 0;
		}
		cursor = cursor->next;
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
