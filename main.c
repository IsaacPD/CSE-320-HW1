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
int processFlags(int, char*, char*, char*, int);
int stringEquals(char*, char*);
int stringToInt(char*);
int stringLen(char*, int);
int formatted(int, char*, char*, float, char*);

typedef struct student_records{
	struct student_records* next;
	struct student_records* prev;
	char* first_name;
	char* last_name;
	
	char* major;
	int id;
	int filter;
	float gpa;
} student_records;

student_records* database;
FILE* file;
FILE* out;

int main(int argc, char** argv) {
	int vflag = 0, gflag = 0;
	char* id = NULL;
	char* lastname = NULL;
	char* major = NULL;
	int c;
	out = stdout;
  	
  	database = (student_records*)malloc(sizeof(student_records));
  	database->prev = NULL;
  	database->next = NULL;
  	database->id = -1;
  	database->filter = 1;
  	
  	if (argc <= 2){
  		printf("NO QUERY PROVIDED\n");
  		return 1;
  	}
  	
  	file = fopen(*(argv+1), "r+");
		if (loadDatabase() == -1) return -1;
  	
  	while((c = getopt(argc, argv, "vi:f:m:o:g")) != -1){
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
				if (access(optarg, F_OK) == 0){
					char yn;
					printf("The file already exists. Would you like to override it? (y/n): ");
					scanf(" %c", &yn);
					if (yn == 'y'){
						out = fopen(optarg, "w");
					}
					else {
						printf("FILE EXISTS\n");
						return 1;
					}
				} else {
					out = fopen(optarg, "w");
				}
				break;
			case 'g':
				gflag = 1;
				break;
			default:
				printf("FAILED TO PARSE FILE\n");
				return 1;
		}
	}
	processFlags(vflag, id, lastname, major, gflag);
	free(database);
	fclose(file);
	return 0;
}

int loadDatabase(){
	char* command = (char*)malloc(7 * sizeof(char));
	char* fName = (char*)malloc(21 * sizeof(char));
	char* lName = (char*)malloc(21 * sizeof(char));
	char* major = (char*)malloc(14 * sizeof(char));
	char* gpaS = (char*)malloc(15 * sizeof(char));
	int id;
	float gpa;
	int c, r;
	do{
		c = fscanf(file, "%s", command);
		if (c == EOF) {
			free(command);
			free(fName);
			free(lName);
			free(major);
			return 0;
		}

		switch(*command){
			case 'A':
			case 'U':
				fscanf(file, "%d %s %s %s ", &id, fName, lName, gpaS);
				fseek(file, (-1 - stringLen(gpaS, 0)), SEEK_CUR);
				fscanf(file, " %f %s", &gpa, major);
				*fName = toupper(*fName);
				*lName = toupper(*lName);
				int i;
				for (i = 0; i < 3; i++)
					*(major + i) = toupper(*(major + i));
				
				if (formatted(id, fName, lName, gpa, major) == 0 || stringLen(gpaS, 0) != 4){
					printf("FAILED TO PARSE FILE\n");
					goto free_error;
				}
				
				if (*command == 'A'){
					if (addStudent(id, fName, lName, gpa, major) == -1){
						printf("ID NOT UNIQUE\n");
						goto free_error;
					}
				} else {
					r = updateStudent(id, fName, lName, gpa, major);
				}
				break;
			case 'D':
				fscanf(file, "%d ", &id);
				if (id > 0)
					r = deleteStudent(id);
				else
					printf("FAILED TO PARSE FILE\n");
				break;
			default:
				printf("FAILED TO PARSE FILE\n");
				goto free_error;
		}
		if (r == -1){
			printf("STUDENT RECORD CANNOT BE DELETED NOR UPDATED\n");
			goto free_error;
		}
	} while (1);
free_error:
	free(command);
	free(fName);
	free(lName);
	free(major);
	return -1;
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
				add->filter = 1;
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
	int len = stringLen(source, 0);
	
	char* temp = (char*)malloc((len+1) * sizeof(char));
	for(i = 0; i <= len; i++)
		*(temp+i) = *(source+i);
	free(*dest);
	(*dest) = temp;
	return 0;
}

int processFlags(int vflag, char* id, char* lastName, char* major, int gflag){
	student_records* cursor = database;
	int numStudents = 0;
	float average = 0.0;
	int total = 0;
	while(cursor != NULL){
		average = average + cursor->gpa;
		total++;
		cursor = cursor->next;
		numStudents++;
	}
	average = average/(float)total;
	
	if (gflag)
		fprintf(out, "%.2f\n", average);
	
	if (vflag && id == NULL && lastName == NULL && major == NULL){
		cursor = database;
		while(cursor != NULL){
			fprintf(out, "%d %s %s %.2f %s\n", cursor->id, cursor->first_name, cursor->last_name, cursor->gpa, cursor->major);
			cursor = cursor->next;
		}
		return 0;
	} else if (!vflag){
		if (formatted(id == NULL ? 3 : stringToInt(id),	"Default",
			 	lastName == NULL ? "Default" : lastName, 3.00,
				major == NULL ? "TST" : major) == 0){
	 		printf("OTHER ERROR\n");
	 		return -1;
	 	}
	 	
		if (id != NULL){ //MATCH ID
			cursor = database;
			int i = stringToInt(id);
			while(cursor != NULL){
				if (i != cursor->id){
					cursor->filter = 0;
					numStudents--;
				}
				cursor = cursor->next;
			}
		}
		if (lastName != NULL){
			cursor = database;
			while(cursor != NULL){
				if (stringEquals(cursor->last_name, lastName) == 0 && cursor->filter){
					cursor->filter = 0;
					numStudents--;
				}
				cursor = cursor->next;
			}
		} 
		if (major != NULL){
			cursor = database;
			while(cursor != NULL){
				if (stringEquals(cursor->major, major) == 0 && cursor->filter){
					cursor->filter = 0;
					numStudents--;
				}
				cursor = cursor->next;
			}
		}
		if (numStudents > 0){
			cursor = database;
			while(cursor != NULL){
				if(cursor->filter)
					fprintf(out, "%d %s %s %.2f %s\n", cursor->id, cursor->first_name, cursor->last_name, cursor->gpa, cursor->major);
				cursor = cursor->next;
			}
		} else {
			printf("STUDENT RECORD NOT FOUND");
		}
		return 0;
		}
}

int stringEquals(char* s1, char* s2){
	int i;
	for (i = 0; *(s1+i) != '\0'; i++){
		if (toupper(*(s1+i)) != toupper(*(s2+i)))
			return 0;
	}
	return 1;
}

int stringLen(char* s, int checkAlpha){
	int i, len = 0;
	for(i = 0; *(s+i) != '\0'; i++){
			len++;
			if (checkAlpha && isalpha(*(s+i)) == 0) return -1;
	}
	return len;
}

int stringToInt(char* s){
	int i, num = 0;
	int len = stringLen(s, 0);

	for(i = len-1; i >=0; i--){
		if (isalpha(*(s+i))) return -1;
		num = num + (*(s+i)- '0') * (int)pow(10, (len - 1) - i);
	}
	return num;
}

int formatted(int id, char* fName, char* lName, float gpa, char* major){
	int fLen = stringLen(fName, 1);
	int lLen = stringLen(lName, 1);
	int mLen = stringLen(major, 0);
	if(id > 0 && fLen >= 3 && fLen <= 10
				&& lLen >= 3 && lLen <= 10
				&& mLen == 3 && gpa >= 1 && gpa <= 4)
		return 1;
	else return 0;
}
