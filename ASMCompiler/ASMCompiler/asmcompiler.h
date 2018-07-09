#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE_LEN 1024
#define TAB_COLS 6

char commands[15][30] = {
	"ADD",
	"EQ",
	"GT",
	"GTEQ",
	"IF",
	"JMP",
	"LT",
	"LTEQ",
	"MOVM",
	"MOVR",
	"MUL",
	"PRINT",
	"READ",
	"SUB"
};

char registers[8] = { 0 };
int **intermediate_table;
int table_index = 0;
int line_count = 0;

void init_intermediate_table()
{
	intermediate_table = (int **)calloc(MAX_LINE_LEN, sizeof(int *));
	for (size_t i = 0; i < MAX_LINE_LEN; i++)
	{
		intermediate_table[i] = (int *)calloc(6, sizeof(int));
	}
}

char *read_line(FILE *file)
{
	char *line = (char *)calloc(MAX_LINE_LEN, sizeof(char));
	fgets(line, MAX_LINE_LEN, file);
	sscanf(line, "%[^\n]s", line);
	line_count++;
	return line;
}

char **get_tokens(char *line)
{
	char **tokens = (char **)calloc(6, sizeof(char *));
	for (size_t i = 0; i < MAX_LINE_LEN; i++)
	{
		tokens[i] = (char *)calloc(MAX_LINE_LEN, sizeof(char));
	}
	int index = 0;
	char *token = (char *)calloc(MAX_LINE_LEN, sizeof(char));
	token = strtok(line, " []");
	while (token)
	{
		tokens[index++] = token;
		token = strtok(line, " []");
	}
	return tokens;
}

int search_command(char *token)
{
	int left = 0, right = strlen(token) - 1;
	while (left <= right)
	{
		int mid = (left + right) / 2;
		int compare = strcmp(token, commands[mid]);
		if (!compare)
		{
			return mid;
		}
		else if (compare > 0)
		{
			right = mid - 1;
		}
		else
		{
			left = mid + 1;
		}
	}
	return -1;
}

int get_opcode(char *token)
{
	int opcode = search_command(token);
	return opcode;
}

void asm_compile(FILE *file)
{
	char *line = read_line(file);
	char **tokens = get_tokens(line);
	for (size_t i = 0; i < TAB_COLS; i++)
	{
		if (!i)
		{
			intermediate_table[table_index][i] = line_count;
		}
		intermediate_table[table_index][i] = get_opcode(tokens[i]);
	}
	table_index++;
}