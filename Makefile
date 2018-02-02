all:
	gcc main.c -o student_records -lm
clean:
	rm -f student_records
