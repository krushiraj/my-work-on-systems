#define _CRT_SECURE_NO_WARNINGS
#define TOKEN_SIZE 128
#define LINE_SIZE 512

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#define TABLES_TABLE 0
#define STUDENTS_TABLE 1
#define MARKS_TABLE 2
#define TABLE_PAGE 3
#define DATA_PAGE 1
#define INDEX_PAGE 2

typedef struct student
{
	int id;
	char name[32];
}student;

typedef struct marks
{
	int id;
	int _marks[4];
} marks;

typedef struct data_page_1
{
	char t_id;
	char p_type;
	char unused[18];
	student data[3];
} data_page_1;

typedef struct data_page_2
{
	char t_id;
	char p_type;
	char unused[6];
	marks data[6];
} data_page_2;

typedef struct index
{
	char t_id;
	char p_type;
	char unused[2];
	int child_page_id[16];
	int key[15];
} index_page;

typedef struct buffer
{
	char free;
	char unused[7];
	int last_access_ticks;
	int page_id;
	char data[128];
} buffer;

typedef struct table_page
{
	char p_type;
	char t_id;
	char unused[6];
	char table_name[10][8];
	int root_page_id[10];
} table_page;

buffer *pool = (buffer *)calloc(4, sizeof(buffer));

int get_no_of_tokens(char* str)
{
	int count = 0;
	while (*str)
	{
		if (*str == ',')
			count++;
		str++;
	}
	return count + 1;
}

char** get_tokens(char* str, int no_of_tokens)
{
	char** tokens = (char**)calloc(no_of_tokens, sizeof(char*));
	for (size_t i = 0; i < unsigned(no_of_tokens); i++)
	{
		tokens[i] = (char*)calloc(TOKEN_SIZE, sizeof(char));
	}
	int token_index = 0;

	while (no_of_tokens)
	{
		if (token_index == 0)
		{
			tokens[token_index++] = strtok(str, ",");
		}
		else
		{
			tokens[token_index++] = strtok(NULL, ",");
		}
		no_of_tokens--;
	}
	return tokens;
}

char *strip_str(char *line)
{
	sscanf(line, "%[^\n]s", line);
	while (*line == ' ')
	{
		line += 1;
	}
	return line;
}

char *read_line(FILE *file)
{
	char *line = (char *)calloc(LINE_SIZE, sizeof(char));
	fgets(line, LINE_SIZE, file);
	strip_str(line);
	return line;
}

data_page_1 *page_students_table()
{
	int stp_count = 16;
	data_page_1 *students_table_pages = (data_page_1 *)calloc(stp_count, sizeof(data_page_1));
	int stp_index = 0;
	int i = 0;
	FILE *file = fopen("students.csv", "r");
	while (stp_index < stp_count)
	{
		char *line = read_line(file);
		int no_of_tokens = get_no_of_tokens(line);
		char **tokens = get_tokens(line, no_of_tokens);
		students_table_pages[stp_index].data[i].id = atoi(tokens[0]);
		strcpy(students_table_pages[stp_index].data[i].name, tokens[1]);
		students_table_pages[stp_index].t_id = STUDENTS_TABLE;
		students_table_pages[stp_index].p_type = DATA_PAGE;
		strcpy(students_table_pages[stp_index].unused, "");
		i++;
		if (i % 3 == 0)
		{
			i = 0;
			stp_index++;
		}
	}
	fclose(file);
	return students_table_pages;
}

data_page_2 *page_marks_table()
{
	int mtp_count = 8;
	data_page_2 *marks_table_pages = (data_page_2 *)calloc(mtp_count, sizeof(data_page_2));
	int mtp_index = 0;
	int i = 0;
	FILE *file = fopen("marks.csv", "r");
	while (mtp_index < mtp_count)
	{
		char *line = read_line(file);
		int no_of_tokens = get_no_of_tokens(line);
		char **tokens = get_tokens(line, no_of_tokens);
		marks_table_pages[mtp_index].data[i].id = atoi(tokens[0]);
		marks_table_pages[mtp_index].data[i]._marks[1] = atoi(tokens[1]); 
		marks_table_pages[mtp_index].data[i]._marks[2] = atoi(tokens[2]);
		marks_table_pages[mtp_index].data[i]._marks[3] = atoi(tokens[3]);
		marks_table_pages[mtp_index].data[i]._marks[0] = atoi(tokens[0]);
		marks_table_pages[mtp_index].t_id = MARKS_TABLE;
		marks_table_pages[mtp_index].p_type = DATA_PAGE;
		strcpy(marks_table_pages[mtp_index].unused, "");
		i++;
		if (i % 6 == 0)
		{
			i = 0;
			mtp_index++;
		}
	}
	fclose(file);
	return marks_table_pages;
}

index_page *index_stp(data_page_1 *stp)
{
	index_page *stp_index = (index_page *)calloc(1, sizeof(index_page));
	int no_of_pages = 16;
	int no_of_record_per_page = 3;

	stp_index[0].child_page_id[0] = 0;
	stp_index[0].p_type = INDEX_PAGE;
	stp_index[0].t_id = STUDENTS_TABLE;
	strcpy(stp_index[0].unused, "");
	for (size_t i = 0; i < no_of_pages; i++)
	{
		stp_index[0].child_page_id[i] = stp[i].data[0].id;
		stp_index[0].key[i - 1] = stp_index[0].child_page_id[i];
	}
	return stp_index;
}

index_page *index_mtp(data_page_2 *mtp)
{
	index_page *mtp_index = (index_page *)calloc(1, sizeof(index_page));
	int no_of_pages = 16;
	int no_of_record_per_page = 3;

	mtp_index[0].child_page_id[0] = 0;
	mtp_index[0].p_type = INDEX_PAGE;
	mtp_index[0].t_id = STUDENTS_TABLE;
	strcpy(mtp_index[0].unused, "");
	for (size_t i = 0; i < no_of_pages; i++)
	{
		mtp_index[0].child_page_id[i] = mtp[i].data[0].id;
		mtp_index[0].key[i - 1] = mtp_index[0].child_page_id[i];
	}
	return mtp_index;
}

student get_student_by_id(index_page *index, int id)
{
	FILE *file = fopen("index.txt", "rb");
	int i = 0;
	data_page_1 *page = (data_page_1 *)calloc(1, sizeof(data_page_1));
	while (i < 16)
	{
		if (i == 15)
		{
			fseek(file, i * 128, SEEK_SET);
			fread(page, sizeof(data_page_1), 1, file);
			break;
		}
		if (id < index[0].key[i])
		{
			fseek(file, (long)(i * 128), SEEK_SET);
			fread(page, sizeof(data_page_1), 1, file);
			break;
		}
		else 
		{
			i++;
		}
	}
	for (i = 0; i < 3; i++)
	{
		if (page->data[i].id == id)
			return page->data[i];
	}
}

index_page *get_index_page(int offset)
{
	FILE *file = fopen("index.txt", "rb");
	fseek(file, offset, SEEK_CUR);
	index_page *index = (index_page *)calloc(1, sizeof(index_page));
	fread(index, sizeof(index_page), 1, file);
	fclose(file);
	return index;
}

student *get_students_by_range(index_page *index, int startid, int endid)
{
	FILE *file = fopen("index.txt", "rb");
	int i = 0;
	int start_page, end_page;
	data_page_1 *page = (data_page_1 *)calloc(1, sizeof(data_page_1));
	student *students = (student *)calloc(48, sizeof(student));
	int std_index = 0;
	while (i < 16)
	{
		if (i == 15)
		{
			start_page = i;
			break;
		}
		if (startid < index[0].key[i])
		{
			start_page = i;
			break;
		}
		else
		{
			i++;
		}
	}
	i = start_page;
	int first = 1;
	while (i < 16)
	{
		if (endid < index[0].key[i])
		{
			int diff = index[0].key[i] - endid;
			fseek(file, (long)(i * 128), SEEK_SET);
			fread(page, sizeof(data_page_1), 1, file);
			for (size_t j = 0; j < 3 - diff; j++)
			{
				students[std_index++] = page->data[j];
			}
			break;
		}
		else
		{
			fseek(file, (long)(i * 128), SEEK_SET);
			fread(page, sizeof(data_page_1), 1, file);
			for (size_t j = 0; j < 3; j++)
			{
				if (first && page->data[j].id == startid)
				{
					students[std_index++] = page->data[j];
					first = 0;
				}
				else if (first)
				{
					continue;
				}
				else
				{
					students[std_index++] = page->data[j];
				}
			}
			i++;
		}
	}
	fclose(file);
	return students;
}

student *get_students_by_range_contains(index_page *index, int start, int end, char *str)
{
	student *students = (student *)calloc(48, sizeof(student));
	student *res = (student *)calloc(48, sizeof(student));
	int resi = 0;
	students = get_students_by_range(index, start, end);
	for (size_t i = 0; i < end - start; i++)
	{
		if (strstr(students[i].name, str))
		{
			res[resi++] = students[i];
		}
	}
	return res;
}

int allocate()
{
	int least = INT_MAX;
	int least_index = -1;
	while (least_index == -1)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (pool[i].free && pool[i].last_access_ticks <= least)
			{
				least_index = i;
			}
		}
	}
	pool[least_index].free = 0;
	pool[least_index].last_access_ticks = (unsigned)time(NULL);
	return least_index;
}

void deallocate(int index)
{
	pool[index].free = 1;
	pool[index].last_access_ticks = INT_MAX;
}

void get_join_marks_desc(table_page *table_index, int desc)
{
	FILE *file = fopen("index.txt", "rb");
	for (size_t i = 0; i < 4; i++)
	{
		pool[i].page_id = pool[i].last_access_ticks = INT_MAX;
		strcpy(pool[i].data, "");
		strcpy(pool[i].unused, "");
		pool[i].free = 1;
	}
	int table1_id = table_index->root_page_id[0], table2_id = table_index->root_page_id[1];
	int offset1 = 0;
	int offset2 = table1_id + 1;
	int stud_no = 0, marks_no = 0, stud_count = 0, marks_count = 0;
	int stud_index1 = -1, stud_index2 = -1, mark_index = -1;
	index_page *index = (index_page *)calloc(1, sizeof(index_page));
	while (true)
	{
		if (stud_index1 == -1)
		{
			fseek(file, (long)(offset1 + stud_no) * 128, SEEK_CUR);
			stud_index1 = allocate();
			fread(&pool[stud_index1].data, sizeof(data_page_1), 1, file);
			stud_no++;
		}
		if (stud_index2 == -1)
		{
			fseek(file, (long)(offset1 + stud_no) * 128, SEEK_CUR);
			stud_index2 = allocate();
			fread(&pool[stud_index2].data, sizeof(data_page_1), 1, file);
			stud_no++;
		}
		if (mark_index == -1)
		{
			fseek(file, (long)(offset2 + marks_no) * 128, SEEK_CUR);
			mark_index = allocate();
			fread(&pool[mark_index].data, sizeof(data_page_2), 1, file);
			marks_no++;
		}
		if (stud_count < 3)
		{
			printf("%d\t\t%s\t\t", ((data_page_1 *)(&pool[stud_index1].data[0]))->data[stud_count].id,
				((data_page_1 *)(&pool[stud_index1].data[0]))->data[stud_count].name);
			stud_count++;
		}
		else
		{
			deallocate(stud_index1);
			stud_index1 = stud_index2;
			stud_index2 = -1;
		}
		if (marks_count < 6)
		{
			printf("%d\t\t%d\t\t%d\t\t%d\t\t\n", ((data_page_2 *)(&pool[mark_index].data[0]))->data[stud_count]._marks[0],
				((data_page_2 *)(&pool[mark_index].data[0]))->data[stud_count]._marks[1],
				((data_page_2 *)(&pool[mark_index].data[0]))->data[stud_count]._marks[2],
				((data_page_2 *)(&pool[mark_index].data[0]))->data[stud_count]._marks[3]);
			marks_count++;
		}
		else
		{
			deallocate(mark_index);
			mark_index = -1;
		}
	}
}

table_page *get_table_page()
{
	FILE *file = fopen("index.txt", "rb");
	fseek(file, -128, SEEK_END);
	table_page *index = (table_page *)calloc(1, sizeof(table_page));
	fread(index, sizeof(table_page), 1, file);
	fclose(file);
	return index;
}

int main()
{
	FILE *file = fopen("index.txt", "wb");
	data_page_1 *students_table_page = page_students_table();
	data_page_2 *marks_table_page = page_marks_table();
	index_page *stp_index = index_stp(students_table_page);
	index_page *mtp_index = index_mtp(marks_table_page);
	table_page tp;
	tp.root_page_id[0] = 16;
	tp.root_page_id[1] = 24;
	//write_index_page(index_file, stp_index, students_table_page);
	fwrite(students_table_page, sizeof(data_page_1), 16, file);
	fwrite(marks_table_page, sizeof(data_page_2), 8, file);
	fwrite(stp_index, sizeof(index_page), 1, file);
	fwrite(mtp_index, sizeof(index_page), 1, file);
	fclose(file);
	//table_page *table_index = get_table_page();
	/*int id, id1;
	printf("enter id for student: ");
	scanf("%d", &id);
	scanf("%d", &id1);
	student s = get_student_by_id(stp_index, id);
	printf("%d %s\n", s.id, s.name);
	student *s1 = get_students_by_range(stp_index, id, id1);
	printf("ID\t\tNAME\n");
	for (size_t i = 0; i < id1 - id; i++)
	{
		printf("%d\t\t%s\n", s1[i].id, s1[i].name);
	}
	student *s2 = get_students_by_range(stp_index, id, id1);
	char str[256];
	scanf("%s", str);
	printf("contains: %s\n", str);
	s2 = get_students_by_range_contains(stp_index, id, id1, str);
	int size = 0;
	while (s2[size++].id);
	for (size_t i = 0; i < size - 1; i++)
	{
		printf("%d\t\t%s\n", s2[i].id, s2[i].name);
	}*/
	get_join_marks_desc(&tp,0);
	return 0;
}