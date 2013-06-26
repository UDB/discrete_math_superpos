/*!
 The MIT License (MIT)

 Copyright (c) 2013 Volkov Sergey

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 This is not optimized version, but work in progress..
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "08_1_07task.h"

/*!
 * Create new string
 * \param[in] size - size of new string
 *
 * \return pointer to the new string
 */
static char* new_string(int size)
{
	return (char*) calloc((size + 1), sizeof(char));
}

/*!
 * Free string resources
 * \param[in] string - pointer to the string
 */
static void string_free(char* string)
{
	free(string);
}

/*!
 * Create new function
 * \param[in] logic - logic of the function
 * \param[in] argc - count of the arguments of the function
 * \param[in] values - value vector of the function
 *
 * \return pointer to the new function, or NULL in unsuccess case
 */
static task_08_1_07_SFunc* new_function(int logic, int argc, const char* values)
{
	if (logic != 2)
		return NULL;

	if (argc < 2 || argc > 3)
		return NULL;

	task_08_1_07_SFunc* function = (task_08_1_07_SFunc*) calloc(1,
			sizeof(task_08_1_07_SFunc));

	function->logic = logic;
	function->argc = argc;
	function->size = (1 << function->argc);
	function->values = new_string(function->size);
	strcpy(function->values, values);
	return function;
}

/*!
 * Free function resources
 * \param[in] function - pointer to the function
 */
static void function_free(task_08_1_07_SFunc* function)
{
	if (function == NULL)
		return;

	string_free(function->values);
	free(function);
}

/*!
 * Read function from the file
 * \param[in] descr - descriptor to the openned file
 * \param[out] function - pointer to the function struct
 *
 * \return 1 if success
 * 				-1 if error with read of the logic
 * 				-2 if error with read of the arguments count
 * 				-3 if error with read of the values vector of the function
 */
static int function_fread(FILE* descr, task_08_1_07_SFunc** function)
{
	int logic = 0;
	int argc = 0;

	if (fscanf(descr, "%d", &logic) != 1)
		return -1;

	if (fscanf(descr, "%d", &argc) != 1)
		return -2;

	char *values = new_string(1 << argc);
	if (fscanf(descr, "%s", values) != 1 && (int) strlen(values) < (1 << argc))
		return -3;
	*function = new_function(logic, argc, values);

	string_free(values);
	return 1;
}

/*!
 * Write function to the file
 * \param[in] descr - descriptor of the openned file
 * \param[in] function - pointer to the function struct
 */
static void function_fwrite(FILE* descr, task_08_1_07_SFunc* function)
{
	fprintf(descr, "%d %d %s", function->logic, function->argc, function->values);
}

/*!
 * Superpose the functions: f(g,h,j)
 * \param[in] function - is external function
 * \param[in] x,y,z - arguments
 *
 * \return pointer to the new function
 */
static task_08_1_07_SFunc* function_superpos(task_08_1_07_SFunc* function,
		task_08_1_07_SFunc* x, task_08_1_07_SFunc* y, task_08_1_07_SFunc* z)
{
	const char * const func_table[] =
	{ "000", "001", "010", "011", "100", "101", "110", "111" };

	task_08_1_07_SFunc* new_func;
	char* tmp = new_string(3);
	char* new_values = new_string(x->size);

	for (int i = 0; i < x->size; i++)
	{
		tmp[0] = x->values[i];
		tmp[1] = y->values[i];
		tmp[2] = z->values[i];

		int index = 0;
		for (int j = 0; j < 8; j++)
		{
			if (strcmp(func_table[j], tmp) == 0)
			{
				index = j;
				break;
			}
		}
		new_values[i] = function->values[index];
	}

	new_func = new_function(x->logic, x->argc, new_values);
	string_free(tmp);
	return new_func;
}

/*!
 * Create new set of function
 *
 * \return pointer to the set of functions struct
 */
static task_08_1_07_SFuncSet* new_function_set(void)
{
	task_08_1_07_SFuncSet* set = (task_08_1_07_SFuncSet*) calloc(1,
			sizeof(task_08_1_07_SFuncSet));
	set->functions = NULL;
	set->size = 0;
	return set;
}

/*!
 * Free resources for set of function struct
 * \param[in] set - pointer to the set
 */
static void function_set_free(task_08_1_07_SFuncSet* set)
{
	if (set == NULL)
		return;

	for (int i = 0; i < set->size; i++)
	{
		function_free(set->functions[i]);
	}
	free(set);
}

/*!
 * Set of the functions is contain function?
 * \param[in] set - pointer to the set of the functions
 * \param[in] function - pointer to the function
 *
 * \return  1 if function in the set,
 * 					0 if function not in the set
 */
static int function_set_is_contain(task_08_1_07_SFuncSet* set,
		task_08_1_07_SFunc* function)
{
	if (function == NULL)
		return 0;

	for (int i = 0; i < set->size; i++)
	{
		if (strcmp(set->functions[i]->values, function->values) == 0)
		{
			return 1;
		}
	}
	return 0;
}

/*!
 * Add function to the set
 * \param[in] set - pointer to the set
 * \param[in] function - pointer to the function
 * \param[in] check_unique - if 1 then function can add only unique function
 *
 * \return 	1 if success added
 * 					0 in other cases
 */
static int function_set_add(task_08_1_07_SFuncSet* set,
		task_08_1_07_SFunc* function, int chek_unique)
{
	if (function == NULL)
		return 0;

	if (chek_unique == 1)
	{
		if (set->size > 0)
		{
			if (function_set_is_contain(set, function) == 1)
				return 0;
		}
	}

	if (set->functions == NULL)
	{
		set->functions = (task_08_1_07_SFunc**) calloc(1,
				sizeof(task_08_1_07_SFunc*));
	}
	else
	{
		set->functions = (task_08_1_07_SFunc**) realloc(set->functions,
				(set->size + 1) * sizeof(task_08_1_07_SFunc*));
	}
	set->functions[set->size++] = function;
	return 1;
}

/*!
 * Sort set of the functions by lexicographical order
 * \param[in\out] set - ponter to the set
 */
static void function_set_abc_sort(task_08_1_07_SFuncSet* set)
{
	task_08_1_07_SFunc* tmp_function;
	for (int i = 0; i < set->size; i++)
	{
		for (int j = i + 1; j < set->size; j++)
		{
			if (strcmp(set->functions[i]->values, set->functions[j]->values) > 0)
			{
				tmp_function = set->functions[i];
				set->functions[i] = set->functions[j];
				set->functions[j] = tmp_function;
			}
		}
	}
}

/*!
 * Get all functions with 2 arguments from our function whti 3 arguments
 * \param[in] function - pointer to the function
 * \param[out] set - pointer to the set
 *
 * \return count of new functions
 */
static int function_get_functions_generated_by(task_08_1_07_SFunc* function,
		task_08_1_07_SFuncSet* set)
{
	int count = 0;
	task_08_1_07_SFunc* new_func;
	function_set_add(set, new_function(2, 2, "0011"), 1);
	function_set_add(set, new_function(2, 2, "0101"), 1);

	int has_new_function = 1;
	int size = set->size;
	while (has_new_function == 1)
	{
		has_new_function = 0;
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				for (int k = 0; k < size; k++)
				{
					new_func = function_superpos(function, set->functions[i],
							set->functions[j], set->functions[k]);
//					printf("size: %d, %s \n", size, new_func->values);
					if (function_set_add(set, new_func, 1) == 1)
					{
						count++;
						has_new_function = 1;
					}
					else
					{
						function_free(new_func);
					}
				}
			}
		}
		size = set->size;
	}

	return count;
}

/*!
 * Write set of functions to the file
 * \param[in] descr - descriptor of the openned file
 * \param[in] function - pointer to the function struct
 */
static void function_set_fwrite(FILE* descr, task_08_1_07_SFuncSet* set)
{
	for (int i = 0; i < set->size; i++)
	{
		function_fwrite(descr, set->functions[i]);
		fprintf(descr, "\n");
	}

}

/*!
 * Test add function to set
 */
static int test_function_set_add()
{

	int is_valid = 1;
	task_08_1_07_SFuncSet* set = new_function_set();

	function_set_add(set, new_function(2, 2, "0110"), 1);
	function_set_add(set, new_function(2, 2, "0000"), 1);
	function_set_add(set, new_function(2, 2, "1111"), 1);

	if (set->size != 3)
	{
		is_valid = 0;
		printf("\n    need size [3] bur get [%d] \n", set->size);
	}

	if (strcmp(set->functions[0]->values, "0110") != 0)
	{
		is_valid = 0;
		printf("\n   element 0 need [0110] bur get [%s] \n",
				set->functions[0]->values);
	}

	if (strcmp(set->functions[1]->values, "0000") != 0)
	{
		is_valid = 0;
		printf("\n   element 1 need [0000] bur get [%s] \n",
				set->functions[1]->values);
	}

	if (strcmp(set->functions[2]->values, "1111") != 0)
	{
		is_valid = 0;
		printf("\n   element 2 need [1111] bur get [%s] \n",
				set->functions[2]->values);
	}

	function_set_free(set);
	return is_valid;

}

/*!
 * Test set is contain
 */
static int test_function_set_is_countain()
{
	int is_valid = 1;

	task_08_1_07_SFuncSet* set = new_function_set();
	task_08_1_07_SFunc* func;
	function_set_add(set, new_function(2, 2, "0110"), 1);
	function_set_add(set, new_function(2, 2, "0000"), 1);
	function_set_add(set, new_function(2, 2, "1111"), 1);

	/****************************************************************************/
	func = new_function(2, 2, "0110");
	int is_contain = function_set_is_contain(set, func);
	if (is_contain != 1)
	{
		is_valid = 0;
		printf("\n    value[%s] need [1] get [%d] \n", func->values, is_contain);
	}
	function_free(func);
	/****************************************************************************/
	func = new_function(2, 2, "1100");
	is_contain = function_set_is_contain(set, func);
	if (is_contain == 1)
	{
		is_valid = 0;
		printf("\n  value[%s] need [0] get [%d] \n", func->values, is_contain);
	}
	function_free(func);
	/****************************************************************************/

	return is_valid;
}

/*!
 * Test create function
 */
static int test_function_new_function()
{
	int is_valid = 1;
	task_08_1_07_SFunc* function;

	/****************************************************************************/
	function = new_function(2, 3, "10010001");
	if (function == NULL)
	{
		is_valid = 0;
		printf("function is null\n");
	}
	else
	{
		if (function->logic != 2)
		{
			is_valid = 0;
			printf("function logic: need [2], but it's [%d]\n", function->logic);
		}
		if (function->argc != 3)
		{
			is_valid = 0;
			printf("function argc: need [3], but it's [%d]\n", function->argc);
		}
		if (function->size != (1 << function->argc))
		{
			is_valid = 0;
			printf("function logic: need [%d], but it's [%d]\n",
					(1 << function->argc), function->size);
		}
		if (strcmp(function->values, "10010001") != 0)
		{
			is_valid = 0;
			printf("function values: need [10010001], but it's [%s]\n",
					function->values);
		}
	}
	function_free(function);
	/****************************************************************************/
	function = new_function(2, 2, "1001");
	if (function == NULL)
	{
		is_valid = 0;
		printf("function is null\n");
	}
	else
	{
		if (function->logic != 2)
		{
			is_valid = 0;
			printf("function logic: need [2], but it's [%d]\n", function->logic);
		}
		if (function->argc != 2)
		{
			is_valid = 0;
			printf("function argc: need [2], but it's [%d]\n", function->argc);
		}
		if (function->size != (1 << function->argc))
		{
			is_valid = 0;
			printf("function logic: need [%d], but it's [%d]\n",
					(1 << function->argc), function->size);
		}
		if (strcmp(function->values, "1001") != 0)
		{
			is_valid = 0;
			printf("function values: need [1001], but it's [%s]\n", function->values);
		}
	}
	function_free(function);
	/****************************************************************************/
	function = new_function(1, 3, "10010001");
	if (function != NULL)
	{
		is_valid = 0;
		printf("function mus be [null], but it's [%p]\n", function);
	}
	function_free(function);
	/****************************************************************************/
	function = new_function(2, 0, "1001");
	if (function != NULL)
	{
		is_valid = 0;
		printf("function mus be [null], but it's [%p]\n", function);
	}
	function_free(function);
	/****************************************************************************/

	return is_valid;
}

/*!
 * Test superposition
 */
static int test_function_superposition()
{
	int is_valid = 1;

	task_08_1_07_SFunc* function = new_function(2, 3, "01110011");
	task_08_1_07_SFunc* f1 = new_function(2, 2, "0011");
	task_08_1_07_SFunc* f2 = new_function(2, 2, "1010");
	task_08_1_07_SFunc* new_func;

	/****************************************************************************/
	new_func = function_superpos(function, f1, f2, f1);
	if (strcmp(new_func->values, "1010") != 0)
	{
		is_valid = 0;
		printf("\n    need [1010] get [%s] \n", new_func->values);
	}
	function_free(new_func);
	/****************************************************************************/
	new_func = function_superpos(function, f1, f1, f2);
	if (strcmp(new_func->values, "1011") != 0)
	{
		is_valid = 0;
		printf("\n    need [1011] get [%s] \n", new_func->values);

	}
	function_free(new_func);
	/****************************************************************************/
	function_free(f2);
	function_free(f1);
	function_free(function);

	return is_valid;
}

/*!
 * Test abs sort
 */
static int test_function_set_abc_sort()
{
	int is_valid = 1;

	task_08_1_07_SFuncSet* set = new_function_set();
	function_set_add(set, new_function(2, 2, "111"), 1);
	function_set_add(set, new_function(2, 2, "101"), 1);
	function_set_add(set, new_function(2, 2, "001"), 1);
	function_set_add(set, new_function(2, 2, "011"), 1);

	function_set_abc_sort(set);

	if (strcmp(set->functions[0]->values, "001") != 0
			|| strcmp(set->functions[1]->values, "011") != 0
			|| strcmp(set->functions[2]->values, "101") != 0
			|| strcmp(set->functions[3]->values, "111") != 0)
	{
		is_valid = 0;
		printf("wrong sort: \n");
		for (int i = 0; i < set->size; i++)
		{
			printf("[%s]\n", set->functions[i]->values);
		}
	}

	return is_valid;
}

static int test_global(void)
{
	int is_valid = 1;

	char* file_in = (char*) calloc(50, sizeof(char));
	char* file_out = (char*) calloc(50, sizeof(char));
	char* tmp = (char*) calloc(50, sizeof(char));
	for (int i = 0; i < (1 << 8); i++)
	{
		for (int j = 7; j >= 0; j--)
		{
			tmp[8 - j - 1] = ((i >> j) & 1) + '0';
		}
		sprintf(file_in, "%s%s%s", "tests/", tmp, ".in.txt");
		sprintf(file_out, "%s%s%s", "tests/", tmp, ".out.txt");

		FILE* input_file = fopen(file_in, "r");
		if (input_file == NULL)
		{
			is_valid = 0;
			printf("couldn't open file [%s]\n", file_in);
			return is_valid;
		}
		FILE* output_file = fopen(file_out, "r");
		if (output_file == NULL)
		{
			is_valid = 0;
			fclose(input_file);
			printf("couldn't open file [%s]\n", file_out);
			return is_valid;
		}

		FILE* temp_file = fopen("tests/temp.out.txt", "w+");
		if (temp_file == NULL)
		{
			is_valid = 0;
			fclose(input_file);
			fclose(output_file);
			printf("couldn't open file [%s]\n", "tests/temp.out.txt");
			return is_valid;
		}

		task_08_1_07_SFunc* function;
		if (function_fread(input_file, &function) != 1)
		{
			is_valid = 0;
			printf("couldn't read from file [%s]\n", file_in);
			fclose(input_file);
			fclose(output_file);
			return is_valid;
		}

		task_08_1_07_SFuncSet* set = new_function_set();
		function_get_functions_generated_by(function, set);
		function_set_abc_sort(set);
		function_set_fwrite(temp_file, set);
		function_set_free(set);

		rewind(output_file);
		rewind(temp_file);

		char a,b;
		while((a=fgetc(output_file)) != EOF)
		{
			b=fgetc(temp_file);
			if(a != b)
			{
				is_valid = 0;
				printf("Wrong answer in file: [%s]\n", file_in);
			}
		}

		fclose(temp_file);
		fclose(input_file);
		fclose(output_file);



	}

	return is_valid;

}

void task_08_1_07_tests(void)
{
	printf("===== TEST PROGRAMM:=====\n\n");

	/****************************************************************************
	 * Function Part
	 ****************************************************************************/
	printf("[TEST]: Functions for function(oh, eah):\n");
	/*! [START]Test create functions */
	printf("  1) Test create function: ");
	if (test_function_new_function() == 1)
	{
		printf("\033[032m [OK] \033[0m\n");
	}
	else
	{
		printf("\033[031m [ERROR] \033[0m\n");
		exit(1);
	}
	/*! [END] Test create functions*/
	/*! [START]Test superpositions of functions */
	printf("  2) Test superpositions: ");
	if (test_function_superposition() == 1)
	{
		printf("\033[032m [OK] \033[0m\n");
	}
	else
	{
		printf("\033[031m [ERROR] \033[0m\n");
		exit(1);
	}
	/*! [END] Test superpositions of functions*/

	/****************************************************************************
	 * Set of Functions Part
	 ****************************************************************************/
	printf("[TEST]: Functions for set of functions:\n");
	/*! [START]Test add function*/
	printf("  1) Test add function: ");
	if (test_function_set_add() == 1)
	{
		printf("\033[032m [OK] \033[0m\n");
	}
	else
	{
		printf("\033[031m [ERROR] \033[0m\n");
		exit(1);
	}
	/*! [END] Test add function*/
	/*! [START]Test is contain*/
	printf("  2) Test is contain function: ");
	if (test_function_set_is_countain() == 1)
	{
		printf("\033[032m [OK] \033[0m\n");
	}
	else
	{
		printf("\033[031m [ERROR] \033[0m\n");
		exit(1);
	}
	/*! [END] Test is contain*/
	/*! [START]Test abc sort*/
	printf("  3) Test abc sort of functions set: ");
	if (test_function_set_abc_sort() == 1)
	{
		printf("\033[032m [OK] \033[0m\n");
	}
	else
	{
		printf("\033[031m [ERROR] \033[0m\n");
		exit(1);
	}
	/*! [END] Test abc sort*/
	/****************************************************************************
	 * Global
	 ****************************************************************************/
	printf("[TEST]: Global:\n");
	/*! [START]Test global*/
	printf("  1) Test global: ");
	if (test_global() == 1)
	{
		printf("\033[032m [OK] \033[0m\n");
	}
	else
	{
		printf("\033[031m [ERROR] \033[0m\n");
		exit(1);
	}
	/*! [END] Test add function*/

	printf("\n\n");

}

int task_08_1_07(const char *path)
{
	FILE* input_file = fopen(path, "r");
	if (input_file == NULL)
		return 1;

	FILE* output_file = fopen("08_1_07out.txt", "w");
	if (output_file == NULL)
	{
		fclose(input_file);
		return 2;
	}

	task_08_1_07_SFunc* function;
	if (function_fread(input_file, &function) != 1)
	{
		fclose(input_file);
		fclose(output_file);
		return 3;
	}

	task_08_1_07_SFuncSet* set = new_function_set();
	function_get_functions_generated_by(function, set);
	function_set_abc_sort(set);
	function_set_fwrite(output_file, set);
	function_set_free(set);

	fclose(input_file);
	fclose(output_file);

	return 0;
}
