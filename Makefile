all:
	gcc main.c -o student_records -lm
	gcc main_xc.c -o student_records_xc -lm
clean:
	rm -f student_records
