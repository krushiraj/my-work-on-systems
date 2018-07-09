#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TABLE_SIZE 100

typedef struct cell
{ 
	char *value;
	cell *next;
} cell;

typedef struct row_field
{
	int *cells_v;
	int *cells_l;
	cell **cells;	
	int id;
	row_field *next;
} row_field;

typedef struct table_field
{
	row_field *head;// head for rows list
	char *table_name;
	char **col_list;
	int rows;
	int cols;
} table_field;

typedef struct database
{
	char *db_name;
	table_field *tables;
	char **table_names;
	int tables_index;
} database;

void init_tables(database *db)
{
	table_field *tables = db->tables;
	tables = (table_field *)calloc(TABLE_SIZE, sizeof(table_field));
	for (size_t i = 0; i < TABLE_SIZE; i++)
	{
		tables[i].head = NULL;
		tables[i].table_name = (char *)calloc(1024, sizeof(char));
		//table_lookup[i].row_list = (char **)calloc(1024, sizeof(char*));
		tables[i].rows = 1;
		tables[i].cols = 0;
	}
	db->tables = tables;
	db->tables_index = 0;
}

char *strip_str(char *str)
{
	while(*str == ' ')
	{
		str++;
	}
	return str;
}

int get_no_of_cols(char *line)
{
	int count = 0;
	char *token = strtok(line, " ,");
	while (token = strtok(NULL, " ,"))
	{
		count++;
	}
	return count;
}

void get_columns(char *line, table_field *table)
{
	char *_line = (char *)calloc(1024, sizeof(char));
	strcpy(_line, line);
	table->cols = get_no_of_cols(_line);
	table->col_list = (char **)calloc(table->cols, sizeof(char *));
	char *token = strtok(line, " ,");
	int index = 0;
	while (token = strtok(NULL, " ,"))
	{
		table->col_list[index] = (char *)calloc(1024, sizeof(char));
		table->col_list[index++] = token;
	}
}

int get_column_index(table_field *table, char *column)
{
	for (size_t i = 0; i < table->cols; i++)
	{
		if (!strcmp(table->col_list[i], column))
			return i;
	}
}

int get_key(char *phone_no)
{
	int total = 0;
	int key = 0;
	while (*phone_no)
	{
		total = total + *phone_no;
		phone_no = phone_no + 1;
	}
	key = total % TABLE_SIZE;
	return key;
}

cell *create_cell(char *value)
{
	cell *node = (cell *)calloc(1, sizeof(cell));
	node->value = value;
	node->next = NULL;
	return node;
}

void init_row_field(row_field *node, int cols)
{
	for (size_t i = 0; i < cols; i++)
	{
		node->cells_v[i] = -1;
	}
}

row_field *create_row_field(int no_of_cols, int current_index)
{
	row_field *node = (row_field *)calloc(1, sizeof(row_field));
	node->cells = (cell **)calloc(no_of_cols, sizeof(cell *));
	node->cells_l = (int *)calloc(no_of_cols, sizeof(int));
	node->cells_v = (int *)calloc(no_of_cols, sizeof(int));
	init_row_field(node, no_of_cols);
	node->id = current_index;
	return node;
}

row_field *insert_row(table_field *table, char **values)
{
	row_field *node = create_row_field(table->cols, table->rows);
	for (size_t i = 0; i < table->cols; i++)
	{
		if (values[i])
		{
			node->cells[i] = create_cell(values[i]);
			node->cells_v[i] += 1;
		}
	}
	return node;
}

int insert_values(table_field *table, char **values, int rowid)
{
	if (rowid == 0)
	{
		if (!table->head)
		{
			table->head = insert_row(table, values);
		}
		else
		{
			row_field *temp = table->head;
			while (temp->next)
			{
				temp = temp->next;
			}
			temp->next = insert_row(table,values);
		}
		table->rows++;
	}
	else
	{
		row_field *temp = table->head;
		row_field *row = NULL;
		while (temp)
		{
			if (temp->id == rowid)
			{
				row = temp;
				break;
			}
			temp = temp->next;
		}
		if (row)
		{
			row = insert_row(table, values);
			for (size_t i = 0; i < table->cols; i++)
			{
				temp->cells[i]->next = row->cells[i];
			}
		}
		else
		{
			return 0;
		}
	}
	return 1;
}

char *read_line(FILE *file)
{
	char *line = (char *)calloc(1024, sizeof(char));
	fgets(line, 1024, file);
	sscanf(line, "%[^\n]s", line);
	return line;
}

void get_tokens_and_insert(table_field *table, char *line)
{
	char *token = (char *)calloc(1024, sizeof(char));
	token = strtok(line, ",");
	int rowid = atoi(token);
	token = strtok(NULL, ",");
	char **values = (char **)calloc(table->cols, sizeof(char *));
	for (size_t i = 0; i < table->cols; i++)
	{
		values[i] = (char *)calloc(256, sizeof(char));
		if(!strcmp(token, " ")) values[i] = NULL;
		else values[i] = token;
		token = strtok(NULL, ",");
	}
	insert_values(table, values, rowid);
}

void read_and_insert(table_field *table, FILE *file)
{
	row_field *head = table->head;
	char *line = read_line(file);
	if (table->rows == 1)
	{
		get_columns(line, table);
		line = read_line(file);
	}
	while (line[0] != NULL)
	{
		get_tokens_and_insert(table, line);
		line = read_line(file);
	}
}

void put_row(table_field *table, char *values)
{
	get_tokens_and_insert(table, values);
}

char **set_values(table_field *table, char **values, char *key_val)
{
	char *token = (char *)calloc(1024, sizeof(char));
	int i = 0;
	while (key_val)
	{
		if (key_val[0] == ':')
			break;
		token[i] = key_val[0];
		i++;
		key_val += 1;
	}
	key_val += 1;
	int index = get_column_index(table, token);
	strcpy(token, key_val);
	values[index] = (char *)calloc(1024, sizeof(char));
	values[index] = token;
	return values;
}

void update_row(table_field *table, int rowid, char *key_vals)
{
	char **values = (char **)calloc(table->cols, sizeof(char *));
	char *token = (char *)calloc(1024, sizeof(char));
	token = strtok(key_vals, ","); 
	if(token)sscanf(token, "%s", token);
	while (token)
	{
		values = set_values(table ,values, token);
		token = strtok(NULL, ",");
		if(token)sscanf(token, "%s", token);
	}
	insert_values(table, values, rowid);
}

row_field *search_for_row(row_field *head, int id)
{
	row_field *temp = head;
	while (temp)
	{
		if (temp->id == id)
			return temp;
		temp = temp->next;
	}
	return NULL;
}

cell *get_last_commited(cell *col)
{
	cell *temp = col;
	while (temp->next)
	{
		temp = temp->next;
	}
	return temp;
}

char *get_details_from_row(table_field *table, row_field *row, int del)
{
	char *row_str = (char *)calloc(1024, sizeof(char));
	int i;
	cell **cells = row->cells;
	for (i = 0; i < table->cols - 1; i++)
	{
		strcat(row_str, get_last_commited(cells[i])->value);
		strcat(row_str, ", ");
	}
	strcat(row_str, cells[i]->value);
	strcat(row_str, "\n");
	if (del) free(row);
	return row_str;
}

char *get_row(table_field *table, int id)
{
	row_field *row;
	if (row = search_for_row(table->head, id))
	{
		return get_details_from_row(table, row, 0);
	}
	return NULL;
}

char *delete_row(table_field *table, row_field *head, int id)
{
	row_field *prev = NULL;
	row_field *temp = head;
	if (temp->id == id)
	{
		head = temp->next;
		return get_details_from_row(table, temp, 1);
	}
	while (temp)
	{
		if (temp->id == id)
			break;
		prev = temp;
		temp = temp->next;
	}
	prev->next = temp->next;
	return get_details_from_row(table, temp, 1);
}

void create_table(database *db, FILE *file)
{
	char *table_name = read_line(file);
	table_field *tables = db->tables;
	int tables_index = db->tables_index;
	db->table_names[tables_index] = tables[tables_index].table_name = table_name;
	read_and_insert(&tables[tables_index], file);
	db->tables_index += 1;
}

void init_db(database *db)
{
	db->db_name = (char *)calloc(1024, sizeof(char));
	char **table_names = (char **)calloc(TABLE_SIZE, sizeof(char *));
	for (size_t i = 0; i < TABLE_SIZE; i++)
	{
		table_names[i] = (char *)calloc(1024, sizeof(char));
	}
	db->table_names = table_names;
	init_tables(db);
}

database *create_database(char *db_name)
{
	database *db = (database *)calloc(1, sizeof(database));
	init_db(db);
	db->db_name = db_name;
	return db;
}

table_field *get_table_by_name(database *db, char *table_name)
{
	for (size_t i = 0; i < db->tables_index; i++)
	{
		if (!strcmp(table_name, db->table_names[i]))
		{
			return &db->tables[i];
		}
	}
	return NULL;
}

void make_col_persistent(table_field *table, int col)
{
	char *filename = (char *)calloc(1024, sizeof(char));
	strcpy(filename, table->table_name);
	strcat(filename, table->col_list[col]);
	char *ext = (char *)calloc(5, sizeof(char));
	sprintf(ext, ".txt");
	strcat(filename, ext);
	FILE *file = fopen(filename, "w");
	row_field *head = table->head;
	row_field *temp = head;
	fprintf(file, "rowid %s\n", table->col_list[col]);
	while (temp)
	{
		cell *_temp = temp->cells[col];
		while (_temp)
		{
			fprintf(file, "%d %s\n", temp->id, _temp->value);
			_temp = _temp->next;
		}
		temp = temp->next;
	}
	fclose(file);
}

void make_table_persistent(table_field *table)
{
	char *filename = (char *)calloc(1024, sizeof(char));
	strcpy(filename, table->table_name);
	char *ext = (char *)calloc(5, sizeof(char));
	sprintf(ext, ".txt");
	strcat(filename, ext);
	FILE *file = fopen(filename, "w");
	fprintf(file, "%s\n", table->table_name);
	for (size_t i = 0; i < table->cols; i++)
	{
		fprintf(file, "%s\n", table->col_list[i]);
		make_col_persistent(table, i);
	}
	fclose(file);
}

void write_tablenames_to_dbfile(database *db, FILE *file)
{
	for (size_t i = 0; i < db->tables_index; i++)
	{
		fprintf(file, "%s %d\n", db->table_names[i], db->tables[i].cols);
		make_table_persistent(&(db->tables[i]));
	}
}

void make_db_persistent(database *db)
{
	char *db_filename = (char *)calloc(1024, sizeof(char));
	strcpy(db_filename, db->db_name);
	char *ext = (char *)calloc(5, sizeof(char));
	sprintf(ext, ".txt");
	strcat(db_filename, ext);
	FILE *db_file = fopen(db_filename, "w");
	write_tablenames_to_dbfile(db, db_file);
	fclose(db_file);
}

void load_table_from_file(char *filename)
{
	FILE *file = fopen(filename, "r");
	char *line = (char *)calloc(1024, sizeof(char));
	while (line = read_line(file))
	{
		char *ext = (char *)calloc(5, sizeof(char));
		sprintf(ext, ".txt");
		strcat(line, ext);
	}
}

void load_db_from_file(char *db_filename)
{
	FILE *db_file = fopen(db_filename, "r");
	char *line = (char *)calloc(1024, sizeof(char));
	while (line = read_line(db_file))
	{
		char *ext = (char *)calloc(5, sizeof(char));
		sprintf(ext, ".txt");
		strcat(line, ext);

	}
}