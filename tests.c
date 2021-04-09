#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include "vl/string.h"

#define MAX_LINE_SIZE 4096
#define TEST "---TEST---"
#define EXPECTED "---EXPECTED---"

struct Test {
	const char *test_filepath;
	char *source;
	char *expected;
	struct Test *next;
};

struct Test *make_test(const char *);

struct Test *read_file(const char *);

int main(int argc, char **argv) {

	struct Test *test = read_file(argv[1]);	
        if (test == NULL) {
            printf("No tests executed...\n");
        }

	do {
            printf("Test source: %s\n\n", test->source);
            printf("Test expected: %s\n", test->expected);
            test = test->next;
	 } while (test != NULL);
	return 0;
}


struct Test *read_file(const char *filepath)
{
    struct stat path_stat;
    stat(filepath, &path_stat);
    int is_directory = S_ISDIR(path_stat.st_mode);

    if (is_directory) {
        DIR* FD;
        struct dirent* in_file;
        struct Test *testsuite = NULL;
        struct Test *testsuite_head = NULL;
        if (NULL == (FD = opendir (filepath)))
        {
            fprintf(stderr, "Error : Failed to open input directory - %s\n", strerror(errno));
            return NULL;
        }
       
        while ((in_file = readdir(FD)))
        {
            if (!strcmp (in_file->d_name, ".")) {
                continue;
            }

            if (!strcmp (in_file->d_name, "..")) {
                continue;
            }
            char* file = (char*)malloc(
                sizeof(char)*strlen(in_file->d_name) + strlen(filepath) + 1
            );
            strcat(file, filepath);
            strcat(file, "/");
            strcat(file, in_file->d_name);
            
            if (testsuite == NULL)
            {
                testsuite = make_test(file);
                testsuite_head = testsuite;
            }
            else
            {
                testsuite->next = make_test(file);
                testsuite = testsuite->next;
            }
        }

        return testsuite_head;
    }

    return make_test(filepath);
}


struct Test *make_test(const char *filepath)
{
	FILE *fp;
	char line[MAX_LINE_SIZE];
	int test_source = 0;

    	fp = fopen(filepath, "r");
    	if (fp == NULL) {
		perror("Problem with opening file\n");
                return NULL;
	}
	fgets(line, MAX_LINE_SIZE, fp);	
	if (strcmp(TEST, trim(line, NULL)) != 0) {
		printf("First line in test file must be a test separator '---TEST---', has: %s", line);
                return NULL;
	}
	test_source = 1;

	char *test_source_content = (char *)malloc(sizeof(char) * MAX_LINE_SIZE);
	char *test_expected_content = (char *)malloc(sizeof(char) * MAX_LINE_SIZE);

	while(fgets(line, MAX_LINE_SIZE, fp) != NULL) {
		if (strcmp(EXPECTED, trim(line, NULL)) == 0) {
			test_source = 0;
			continue;
		}

		if (test_source == 1)
	       	{
			strcat(test_source_content, line);
		} 
		else 
		{
			strcat(test_expected_content, line);
		}
	}	

   	fclose(fp);
        if (test_source == 1) {
            printf("There is no %s part of test in file: %s, please update!", EXPECTED, filepath);
            return NULL;
        }
	struct Test *test = (struct Test *)malloc(sizeof(struct Test));
	
	test->source = (char *)malloc(sizeof(char) * strlen(test_source_content));
	test->expected = (char *)malloc(sizeof(char) * strlen(test_expected_content));
	test->next = NULL;
	test->test_filepath = filepath;

	strncpy(test->source, test_source_content, strlen(test_source_content));
	strncpy(test->expected, test_expected_content, strlen(test_expected_content));

	free(test_source_content);
	free(test_expected_content);

	return test;
}
