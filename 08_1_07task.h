#pragma once

typedef struct task_08_1_07__SFunc
{
	int logic; // значность логики
	int argc;  // количество аргументов
	int size;  // количество значений
	char *values; // массив значений функции
} task_08_1_07_SFunc;

typedef struct task_08_1_07__SFuncSet
{
	int size;
	task_08_1_07_SFunc** functions;
} task_08_1_07_SFuncSet;


// etc
void task_08_1_07_tests(void);
int task_08_1_07(const char *path);

