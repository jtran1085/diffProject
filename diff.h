#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define ARGC_ERROR 1
#define TOOMANYFILES_ERROR 2
#define CONFLICTING_OUTPUT_OPTIONS 3

#define MAXSTRINGS 1024
#define MAXPARAS 4096

#define HASHLEN 200

#define BUFF_SIZE 1000

#include "para.h"
#include "util.h"

void version(void);
int diffLC(int argc, const char * argv[]);
void loadfiles(const char* filename1, const char* filename2);
void print_option(const char* name, int value);
void diff_output_conflict_error(void);
void setoption(const char* arg, const char* s, const char* t, int* value);
void showoptions(const char* file1, const char* file2);
void init_options_files(int argc, const char* argv[]);
char *safegets(char *buffer, int length, FILE *file);
int comparelines(int argc, const char *argv[]);
int diffSBS(int argc, const char * argv[]);
int diffLC(int argc, const char * argv[]);
int diffSL(int argc, const char * argv[]);
int diffnor(int argc, const char * argv[]);
int diffcon(int argc, const char * argv[]);
int diffuni(int argc, const char * argv[]);