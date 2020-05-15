//
//  diff_02.c
//  diff
//
//  Created by William McCarthy on 4/29/19.
//  Copyright © 2019 William McCarthy. All rights reserved.
//


#include "diff.h"

void version(void) {
  printf("\n\n\ndiff (CSUF diffutils) 1.0.0\n");
  printf("Copyright (C) 2014 CSUF\n");
  printf("This program comes with NO WARRANTY, to the extent permitted by law.\n");
  printf("You may redistribute copies of this program\n");
  printf("under the terms of the GNU General Public License.\n");
  printf("For more information about these matters, see the file named COPYING.\n");
  printf("Written by -insert name here-\n\n");
}

char buf[BUFLEN];
char *strings1[MAXSTRINGS], *strings2[MAXSTRINGS];
int showversion = 0, showbrief = 0, ignorecase = 0, report_identical = 0, showsidebyside = 0;
int showleftcolumn = 0, showunified = 0, showcontext = 0, suppresscommon = 0, diffnormal = 0;

int count1 = 0, count2 = 0, different = 0;

const char* files[2] = { NULL, NULL };


void loadfiles(const char* filename1, const char* filename2) {
  memset(buf, 0, sizeof(buf));
  memset(strings1, 0, sizeof(strings1));
  memset(strings2, 0, sizeof(strings2));
  
  FILE *fin1 = openfile(filename1, "r");
  FILE *fin2 = openfile(filename2, "r");
  
  while (!feof(fin1) && fgets(buf, BUFLEN, fin1) != NULL) { strings1[count1++] = strdup(buf); }  fclose(fin1);
  while (!feof(fin2) && fgets(buf, BUFLEN, fin2) != NULL) { strings2[count2++] = strdup(buf); }  fclose(fin2);
  
  if (count1 != count2) { different = 1;  return; }
  for (int i = 0, j = 0; i < count1 && j < count2;  ++i, ++j) {
    if (strcmp(strings1[i], strings2[j]) != 0) { different = 1;  return; }
  }
}

void print_option(const char* name, int value) { printf("%17s: %s\n", name, yesorno(value)); }

void diff_output_conflict_error(void) {
  fprintf(stderr, "diff: conflicting output style options\n");
  fprintf(stderr, "diff: Try `diff --help' for more information.)\n");
  exit(CONFLICTING_OUTPUT_OPTIONS);
}

void setoption(const char* arg, const char* s, const char* t, int* value) {
  if ((strcmp(arg, s) == 0) || ((t != NULL && strcmp(arg, t) == 0))) {
    *value = 1;
  }
}

void showoptions(const char* file1, const char* file2) {
  printf("diff options...\n");
  print_option("diffnormal", diffnormal);
  print_option("show_version", showversion);
  print_option("show_brief", showbrief);
  print_option("ignorecase", ignorecase);
  print_option("report_identical", report_identical);
  print_option("show_sidebyside", showsidebyside);
  print_option("show_leftcolumn", showleftcolumn);
  print_option("suppresscommon", suppresscommon);
  print_option("showcontext", showcontext);
  print_option("show_unified", showunified);
  
  printf("file1: %s,  file2: %s\n\n\n", file1, file2);
  
  printline();
}


void init_options_files(int argc, const char* argv[]) {
  int cnt = 0;
  //const char* files[2] = { NULL, NULL };
  int size = argc;
  
  while (argc-- > 0) {
    const char* arg = *argv;
    setoption(arg, "-v",       "--version",                  &showversion);
    setoption(arg, "-q",       "--brief",                    &showbrief);
    setoption(arg, "-i",       "--ignore-case",              &ignorecase);
    setoption(arg, "-s",       "--report-identical-files",   &report_identical);
    setoption(arg, "--normal", NULL,                         &diffnormal);
    setoption(arg, "-y",       "--side-by-side",             &showsidebyside);
    setoption(arg, "--left-column", NULL,                    &showleftcolumn);
    setoption(arg, "--suppress-common-lines", NULL,          &suppresscommon);
    setoption(arg, "-c",       "--context",                  &showcontext);
    setoption(arg, "-u",       "showunified",                &showunified);
    if (arg[0] != '-') {
      if (cnt == 2) {
        fprintf(stderr, "apologies, this version of diff only handles two files\n");
        fprintf(stderr, "Usage: ./diff [options] file1 file2\n");
        exit(TOOMANYFILES_ERROR);
      } 
      else { files[cnt++] = arg; }
    }
    ++argv;   // DEBUG only;  move increment up to top of switch at release
  }

  if (!showcontext && !showunified && !showsidebyside && !showleftcolumn) {
    diffnormal = 1;
  }

  if (showversion) { version();  exit(0); }
  
  if (((showsidebyside || showleftcolumn) && (diffnormal || showcontext || showunified)) ||
      (showcontext && showunified) || (diffnormal && (showcontext || showunified))) {

    diff_output_conflict_error();
  }
  
//  showoptions(files[0], files[1]);
  loadfiles(files[0], files[1]);

  if (report_identical && !different) { printf("The files are identical.\n\n");   exit(0); }

  if (showbrief && different) { printf("The files are different.\n\n");   exit(0); }
  else if(showbrief) exit(0);
}

//NON PROFESSOR CODE BELOW

char *safegets(char *buffer, int length, FILE *file){//gets line without /0
  char *ptr;
  int len;      
  
  if (buffer != NULL){
    ptr = fgets(buffer, length, file);
    if (ptr != NULL){ 
      len = strlen(buffer);
      if (len > 0){
        if (buffer[len - 1] == '\n'){ buffer[len - 1] = '\0';}
      }
    }
    return ptr;
  }       
        
  return NULL;
}

int comparelines(int argc, const char *argv[]){
  FILE *leftFile, *rightFile;
  char buff1[BUFF_SIZE], buff2[BUFF_SIZE];
  char *ptr1, *ptr2;
  unsigned long lineNum = 0;
        
  if (argc != 3){
    fprintf(stderr, "only use two files\n");
    return 0;
  }    
  if (!(leftFile = fopen(argv[1], "r"))){
    fprintf(stderr, "Couldn't open %s\n", argv[1]);     
    return 0;
  }     
  if (!(rightFile = fopen(argv[2], "r"))){
    fprintf(stderr, "Couldn't open %s", argv[2]);
    fclose(leftFile);
    return 0;
  }     
  /* read through each file, line by line */
  ptr1 = safegets(buff1, BUFF_SIZE, leftFile);
  ptr2 = safegets(buff2, BUFF_SIZE, rightFile);
  ++lineNum;
        
  /* stop when either we've exhausted either file's data */
  while (ptr1 != NULL && ptr2 != NULL){
    if (strcmp(buff1, buff2) != 0){     // compare the two lines
      printf("Difference:\n");
      printf("%lu\t\"%s\" != \"%s\"\n\n", lineNum, buff1, buff2);
      }
    ptr1 = safegets(buff1, BUFF_SIZE, leftFile);
    ptr2 = safegets(buff2, BUFF_SIZE, rightFile);
    ++lineNum;
  }       

  fclose(leftFile);
  fclose(rightFile);      
  return EXIT_SUCCESS;    
}

int diffSBS(int argc, const char * argv[]) {//SIDE BY SIDE
  para* p = para_first(strings1, count1);
  para* q = para_first(strings2, count2);
  int foundmatch = 0;

  para* qlast = q;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print(q, printright);
        q = para_next(q);
        qlast = q;
      }
      para_print(q, printboth);
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print(p, printleft);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print(q, printright);
    q = para_next(q);
  }
  return 0;
}

int diffLC(int argc, const char * argv[]) { //DIFF Y LEFT COLUMN
  para* p = para_first(strings1, count1);
  para* q = para_first(strings2, count2);
  int foundmatch = 0;

  para* qlast = q;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print(q, printright);
        q = para_next(q);
        qlast = q;
      }
      para_print(q, printequalleft);
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print(p, printleft);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print(q, printright);
    q = para_next(q);
  }
  return 0;
}

int diffSL(int argc, const char * argv[]) {//SUPRESS LINES
  para* p = para_first(strings1, count1);
  para* q = para_first(strings2, count2);
  int foundmatch = 0;

  para* qlast = q;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_print(q, printright);
        q = para_next(q);
        qlast = q;
      }
      p = para_next(p);
      q = para_next(q);
    } else {
      para_print(p, printleft);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_print(q, printright);
    q = para_next(q);
  }
  return 0;
}

int diffnor(int argc, const char * argv[]) {//NORMAL DIFF
  para* p = para_first(strings1, count1);
  para* q = para_first(strings2, count2);
  int foundmatch = 0;

  para* qlast = q;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_printnor(q, printrightnor, 1);
        q = para_next(q);
        qlast = q;
      }
      p = para_next(p);
      q = para_next(q);
    } else {
      para_printnor(p, printleftnor, 0);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_printnor(q, printrightnor, 1);
    q = para_next(q);
  }
  return 0;
}

int diffcon(int argc, const char * argv[]) {//CONTEXT DIFF
  struct stat filestat;
  stat(files[0],&filestat);
  printf("*** %s %s", files[0], ctime(&filestat.st_mtime));
  stat(files[1],&filestat);
  printf("--- %s %s", files[1], ctime(&filestat.st_mtime));
  
  para* p = para_first(strings1, count1);
  para* q = para_first(strings2, count2);
  int foundmatch = 0;

  para* qlast = q;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_printcon(q, printrightcon, 1);
        q = para_next(q);
        qlast = q;
      }
      p = para_next(p);
      q = para_next(q);
    } else {
      para_printcon(p, printleftcon, 0);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_printcon(q, printrightcon, 1);
    q = para_next(q);
  }
  return 0;
}

int diffuni(int argc, const char * argv[]) {//NORMAL DIFF
  struct stat filestat;
  stat(files[0],&filestat);
  printf("--- %s %s", files[0], ctime(&filestat.st_mtime));
  stat(files[1],&filestat);
  printf("+++ %s %s", files[1], ctime(&filestat.st_mtime));
  
  para* p = para_first(strings1, count1);
  para* q = para_first(strings2, count2);
  int foundmatch = 0;

  para* qlast = q;
  while (p != NULL) {
    qlast = q;
    foundmatch = 0;
    while (q != NULL && (foundmatch = para_equal(p, q)) == 0) {
      q = para_next(q);
    }
    q = qlast;

    if (foundmatch) {
      while ((foundmatch = para_equal(p, q)) == 0) {
        para_printnor(q, printrightcon, 1);
        q = para_next(q);
        qlast = q;
      }
        para_printnor(q, printsame, 2);
      p = para_next(p);
      q = para_next(q);
    } else {
      para_printnor(p, printleftcon, 0);
      p = para_next(p);
    }
  }
  while (q != NULL) {
    para_printnor(q, printrightcon, 1);
    q = para_next(q);
  }
  return 0;
}

int main(int argc, const char * argv[]) {
  
  init_options_files(--argc, ++argv);

  //comparelines(argc, argv); //compares everyline but not by paragraph

  //printf("%s", *strings1);

  //para_printfile(strings1, count1, printleft);
  //para_printfile(strings2, count2, printright);

  //string1 is a string array that contains all the lines from the left file
  //string2 is a string array that contains all the lines from the right file
 
  if(showcontext) diffcon(argc,argv);
  else if(showunified) diffuni(argc,argv);
  else if(showsidebyside && showleftcolumn) diffLC(argc,argv);
  else if(showsidebyside && suppresscommon) diffSL(argc, argv);
  else if(showsidebyside) diffSBS(argc, argv);
  else diffnor(argc,argv);
 
  return 0;
}



