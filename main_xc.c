#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

typedef struct student_records{
	struct student_records* left;
	struct student_records* right;
	struct student_records* parent;
	char* first_name;
	char* last_name;
	
	char* major;
	int id;
	int filter;
	float gpa;
} student_records;

int addStudent(student_records*, int, char*, char*, float, char*);
int updateStudent(student_records*, int, char*, char*, float, char*);
int deleteStudent(student_records*, int);
int stringCopy(char*, char**);
int loadDatabase();
int processFlags(int, char*, char*, char*, int);
int stringEquals(char*, char*);
int stringToInt(char*);
int stringLen(char*, int);
int formatted(int, char*, char*, float, char*);
void freeDatabase(student_records*);

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
	database->left = NULL;
	database->right = NULL;
	database->parent = NULL;
	database->id = -1;
	database->filter = 1;
  	
	if (argc <= 2){
		printf("NO QUERY PROVIDED\n");
		freeDatabase(database);
		return 1;
  }
	
	file = fopen(*(argv+1), "r+");
	if (loadDatabase() == -1) {
		freeDatabase(database);
		fclose(file);
		return -1;
	}
  	
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
				freeDatabase(database);
				fclose(file);
				return 1;
		}
	}
	processFlags(vflag, id, lastname, major, gflag);
	
	freeDatabase(database);
	fclose(file);
	if (out != stdout) fclose(out);
	return 0;
}

void freeDatabase(student_records* base){
	if (base->left != NULL)
		freeDatabase(base->left);
	if (base->right != NULL)
		freeDatabase(base->right);
	free(base->first_name);
	free(base->last_name);
	free(base->major);
	free(base);
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
			free(gpaS);
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
					if (addStudent(database, id, fName, lName, gpa, major) == -1){
						printf("ID NOT UNIQUE\n");
						goto free_error;
					}
				} else {
					r = updateStudent(database, id, fName, lName, gpa, major);
				}
				break;
			case 'D':
				fscanf(file, "%d ", &id);
				if (id > 0)
					r = deleteStudent(database, id);
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
	free(gpaS);
	return -1;
}

student_records* findMin(student_records* cursor){
	while (cursor->left != NULL)
		cursor = cursor->left;
	return cursor;
}

void replaceNode(student_records* delete, student_records* replacement){
	if (delete->parent != NULL){
		if (delete == delete->parent->left)
			delete->parent->left = replacement;
		else 
			delete->parent->right = replacement;		
	}
	if (replacement != NULL)
		replacement->parent = delete->parent;
}

int deleteStudent(student_records* cursor, int id){
	if(cursor->id > id){
		if (cursor->left == NULL) return -1;
		return deleteStudent(cursor->left, id);
	}
	if (cursor->id < id){
		if (cursor->right == NULL) return -1;
		return deleteStudent(cursor->right, id);
	}
	
	if (cursor->left != NULL && cursor->right != NULL){
		student_records* successor = findMin(cursor->right);
		cursor->id = successor->id;
		cursor->gpa = successor->gpa;
		stringCopy(successor->major, &(cursor->major));
		stringCopy(successor->first_name, &(cursor->first_name));
		stringCopy(successor->last_name, &(cursor->last_name));
		deleteStudent(successor, successor->id);
		return 0;
	}
	else if (cursor->left != NULL)
		replaceNode(cursor, cursor->left);
	
	else if (cursor->right != NULL)
		replaceNode(cursor, cursor->right);
		
	else 
		replaceNode(cursor, NULL);
	return 0;
}

int addStudent(student_records* cursor, int id, char* firstName, char* lastName, float gpa, char* major){
	if (cursor->id == id){
		return -1;
	} else if (cursor->id < id){
		if(cursor->right != NULL)
			return addStudent(cursor->right, id, firstName, lastName, gpa, major);
		
		if (cursor->id == -1){
			cursor->id = id;
			stringCopy(firstName, &(cursor->first_name));
			stringCopy(lastName, &(cursor->last_name));
			cursor->gpa = gpa;
			stringCopy(major, &(cursor->major));
			return 0;
		}
		
		student_records* add;
		add = (student_records*)malloc(sizeof(student_records));
		add->right = NULL;
		add->left = NULL;
		add->parent = cursor;
		add->filter = 1;
		add->id = id;
		stringCopy(firstName, &(add->first_name));
		stringCopy(lastName, &(add->last_name));
		add->gpa = gpa;
		stringCopy(major, &(add->major));
		cursor->right = add;

		return 0;
	} else {
		if (cursor->left != NULL)
			return addStudent(cursor->left, id, firstName, lastName, gpa, major);
			
		student_records* add;
		add = (student_records*)malloc(sizeof(student_records));
		
		add->left = NULL;
		add->right = NULL;
		add->parent = cursor;
		add->id = id;
		add->filter = 1;
		stringCopy(firstName, &(add->first_name));
		stringCopy(lastName, &(add->last_name));
		add->gpa = gpa;
		stringCopy(major, &(add->major));
		cursor->left = add;
		
		return 0;
	}
}

int updateStudent(student_records* cursor, int id, char* firstName, char* lastName, float gpa, char* major){
	if (cursor->id == id){
		stringCopy(firstName, &(cursor->first_name));
		stringCopy(lastName, &(cursor->last_name));
		cursor->gpa = gpa;
		stringCopy(major, &(cursor->major));
		return 0;
	} else if(cursor->id > id) {
		if (cursor->left == NULL) return -1;
		return updateStudent(cursor->left, id, firstName, lastName, gpa, major);
	} else {
		if (cursor->right == NULL) return -1;
		return updateStudent(cursor->right, id, firstName, lastName, gpa, major);
	}
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

float calcAverage(student_records* cursor, int* total){
	(*total)++;
	float sum = cursor->gpa;
	if (cursor->left != NULL)
		sum += calcAverage(cursor->left, total);
	if (cursor->right != NULL)
		sum += calcAverage(cursor->right, total);	
	return sum;
}

void printTree(student_records* cursor){
	if (cursor->left != NULL)
		printTree(cursor->left);
	if(cursor->filter)
		fprintf(out, "%d %s %s %.2f %s\n", cursor->id, cursor->first_name, cursor->last_name, cursor->gpa, cursor->major);
	if (cursor->right != NULL)
		printTree(cursor->right);
}

void filterID(student_records* cursor, int id, int* students){
	if (*students == 0) return;
	
	if (cursor->left != NULL)
		filterID(cursor->left, id, students);
	if(cursor->filter && cursor->id != id){
		cursor->filter = 0;
		(*students)--;
	}
	if (cursor->right != NULL)
		filterID(cursor->right, id, students);
}

void filterMajor(student_records* cursor, char* major, int* students){
	if (*students == 0) return;
	
	if (cursor->left != NULL)
		filterMajor(cursor->left, major, students);
	if(cursor->filter && stringEquals(major, cursor->major) == 0){
		cursor->filter = 0;
		(*students)--;
	}
	if (cursor->right != NULL)
		filterMajor(cursor->right, major, students);
}

void filterLastName(student_records* cursor, char* lName, int* students){
	if (*students == 0) return;
	
	if (cursor->left != NULL)
		filterLastName(cursor->left, lName, students);
	if(cursor->filter && stringEquals(lName, cursor->last_name) == 0){
		cursor->filter = 0;
		(*students)--;
	}
	if (cursor->right != NULL)
		filterLastName(cursor->right, lName, students);
}

int processFlags(int vflag, char* id, char* lastName, char* major, int gflag){
	int numStudents = 0;
	float average = calcAverage(database, &numStudents)/(float)numStudents;
	student_records* cursor;
	if (gflag)
		fprintf(out, "%.2f\n", average);
	
	if (vflag && id == NULL && lastName == NULL && major == NULL){
		printTree(database);
		return 0;
	} else if (!vflag){
		if (formatted(id == NULL ? 3 : stringToInt(id),	"Default",
			 	lastName == NULL ? "Default" : lastName, 3.00,
				major == NULL ? "TST" : major) == 0){
	 		printf("OTHER ERROR\n");
	 		return -1;
	 	}
		if (id != NULL) //MATCH ID
			filterID(database, stringToInt(id), &numStudents);
		if (lastName != NULL)
			filterLastName(database, lastName, &numStudents);
		if (major != NULL)
			filterMajor(database, major, &numStudents);
		if (numStudents > 0)
			printTree(database);
		else 
			printf("STUDENT RECORD NOT FOUND");
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
