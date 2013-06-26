#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "08_1_07task.h"

int main(int argc, const char *argv[])
{
	task_08_1_07_tests();
	int result = task_08_1_07("in.txt");
	printf("result: %d\n", result);
	return result;
}
