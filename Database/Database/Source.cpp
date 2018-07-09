#include "database.h"


void input_handler(FILE *file)
{
	char *line = read_line(file);
	//char *_line = strcpy(_line, line);
	char *token = (char *)calloc(1024, sizeof(char));
	token = strtok(line, " ");
	if (strcmp(token, "tab"))
	{
		line += 4;
	}
}

int main()
{
	FILE *f = fopen("input.txt", "r");
	char *s = (char *)calloc(256, sizeof(char));
	char *s1 = (char *)calloc(256, sizeof(char));
	database *db = create_database("details_db");
	sprintf(s, "name:krushirajtula,phone:8143827431");
	create_table(db, f);
	fclose(f);
	/*f = fopen("input1.txt", "r");
	create_table(db, f);
	fclose(f);
	f = fopen("input2.txt", "r");
	create_table(db, f);
	fclose(f);*/
	update_row(get_table_by_name(db, "user_details"), 1, s);
	/*sprintf(s, "col1:krushirajtula,col3:8143827431");
	update_row(get_table_by_name(db, "other_details"), 2, s);
	sprintf(s, "cola:krushirajtula,colb:8143827431");
	update_row(get_table_by_name(db, "third_table"), 1, s);*/
	printf("%s", get_row(get_table_by_name(db, "user_details"), 1));
	/*printf("%s", get_row(get_table_by_name(db, "other_details"), 2));
	printf("%s", get_row(get_table_by_name(db, "third_table"), 1));*/
	make_db_persistent(db);
	/*printf("%s", strtok(s, ","));
	printf("%s", strtok(NULL, ","));*/
	return 0;
}