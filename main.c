#include <stdio.h>
#include <unistd.h>

int addStudent(int, char*, char*, int, float, char*);
int updateStudent(int, char*, char*, int, float, char*);
int deleteStudent(int);

typedef struct student_records{
	struct student_records* next = NULL;
	struct student_records* prev = NULL;
	char* first_name;
	char* last_name;

	char* major;
	int id;
	float gpa;
} student_records;

student_records* database = NULL;

int main(int argc, char** argv) {
	int vflag = 0;
	char* id = NULL;
	char* lastname = NULL;
	char* major = NULL;
	char* filename = NULL;
  	int c;
  	
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
		if (cursor.id == id){
			cursor.prev.next = cursor.next;
			cursor.next.prev = cursor.prev;
			free(cursor);
			return 0;
		} else {
			cursor = cursor.next;
		}
	}
	return -1;
}
