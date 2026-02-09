/*
 *ABOUT: 
 * REMOVES COMMENTS FROM OTHER C/C++, PYTHON OR FORTRAN PROGRAMS.
 *
 *INTENDED USE: 
 * FROM THE COMMAND LINE, RUN THE COMPILED 
 *VERSION OF THIS FILE ALONG WITH THE INTENDED PROGRAM 
 *, AND THE INTENDED PROGRAM SHOULD BE RE-WRITTEN W/O 
 *COMMENTS IN A SEPARATE FILE WITH THE PREFIX "_NC"
 *E.G., IF THIS FILE IS COMPILED AS A.OUT AND THE FILE
 *YOU WANT TO REMOVE COMMENTS FROM IS FILE.C, THEN RUN 
 *FROM CLI : 
 *
 * ./A.OUT FILE.C
 *
 *AND THE OUTPUT SHOULD BE A FILE CALLED "_NCFILE.C".
 *
 *POTENTIAL IMPROVEMENTS:
 * MAYBE HAVE IT SO THAT YOU CAN PASS MULTIPLE FILES FROM 
 * THE COMMAND LINE TO REMOVE COMMENTS FROM EVERY FILE PASSED.
 * IT SHOULD REMOVE COMMENTS FROM THE FIRST FILES AND GO TO NEXT, 
 * AND SKIP THAT FILE IF ERROR OCCURS AND WARN USER.
 */

#include <stdio.h>
#include <stdlib.h>

#define MAXLINE 1024

int str_in(char *str1, char *str2);
void rmv_com(char *filename, char c);
char *concat(char *str1, char* str2);
char *rmv_inline(char *str, int idx);


int main(int argc, char **argv) {
  if (argc == 1) {
    printf("Please provide a file.\n");
    return 0;
  }

if (argc > 2) {
    printf("Too many files!\n");
    return 0;
  }

  char c;
  
  if (str_in(".c",argv[1]) >= 0)
    c = 'c'; // C/C++ file 
  else if (str_in(".py",argv[1]) >= 0)
    c = 'p'; // Python file
  else if (str_in(".f",argv[1]) >= 0)
    c = 'f'; // Fortran file 
  else {
    printf("Unsupported file extension.\n");
    return 0;
  }

  rmv_com(argv[1],c);
  return 0;

}

int str_in(char *str1, char *str2) {
  // returns index of first occurence of str1 in str2 if str1 is in str2 , else -1
  int i = 0;
  int j = 0;
  while (str2[i]) {
    if (str2[i] == str1[j]) {
      i++, j++;
      if (str1[j] == '\0')
        break;
    }
    else {
      if (j > 0) {
        if (*str1 == str2[i]) // next substring could be str1
          j = 1, i++;
        else 
          i++, j = 0;
      }
      else 
        i++;
    }
  }
    return (str1[j] == '\0') ? (i - j) : -1;
}

char *concat(char *str1, char* str2){
  // concatenates str1 and str2 

  // Discover total length of strings
  int len = 0;
  int i = 0;
  while (str1[i++])
    len++;
  i = 0 ;
  while (str2[i++])
    len++;

  char *newstr = malloc(len + 1);
  
  i = 0;
  char *p1 = str1;
  char *p2 = str2;
  char *p3 = newstr;
  while (*p3++ = *p1++) // copies str1 to newstr
    ;
  --p3; // nullspace character was added
  while(*p3++ = *p2++)
    ;
  *p3 = '\0';
  return newstr;
}

char *rmv_inline(char *str, int idx) {
  // replaces inline comment (at position idx) from a single line with blank spaces.
  char *p = str;
  while (*(p + idx))
    if (*(p + idx) != '\n')
      *(p + idx++) = ' ';
    else 
      idx++;

  return str;
}


void rmv_com(char *filename, char c) {
  /* Removes comments from file by checking from comment string 
   * (or char) until end of line or "*" + "/".
   */
  FILE *fp;
  FILE *nfp;
  char line[MAXLINE];
  int idx;
  int start = 0;
  
  if (c == 'c') {
    fp = fopen(filename,"r");
    nfp = fopen(concat("_NC",filename),"w");
    
    while (fgets(line,MAXLINE,fp)) {
      if ((idx = str_in("//",line)) >= 0) {
        fputs(rmv_inline(line,idx),nfp);
      }
      else if ((idx = str_in("/*",line)) >= 0) {
        start = idx;
        if ((idx = str_in("*/",line)) >= 0) {// inline comment 
          for (int i = start; i <= idx + 1; i++)
            *(line + i) = ' ';
          fputs(line,nfp);
        }
        else { // comment spans over multiple lines 
          fputs(rmv_inline(line,start),nfp);
          while (fgets(line,MAXLINE,fp)) {// search for end of comment 
            if ((idx = str_in("*/",line)) >= 0) {
              for (int i = 0; i <= idx + 1; i++)
                *(line + i) = ' ';
              fputs(line,nfp);

              break;
            }
            else // still in comment 
              fputs(rmv_inline(line,0),nfp);
          }
        }
      }
      else
        fputs(line,nfp);
    }

    fclose(fp);
    fclose(nfp);
  }
  else if (c == 'p') {
    // Doesn't remove docstrings, only '#'
    fp = fopen(filename,"r");
    nfp = fopen(concat("_NC",filename),"w");

    while(fgets(line,MAXLINE,fp))
      if ((idx = str_in("#",line)) >= 0)
        fputs(rmv_inline(line,idx),nfp);
      else 
        fputs(line,nfp);
  
    fclose(nfp);
    fclose(fp);
  }
  else if (c == 'f') {
    // removes lines that have 'c' or '*' in column 1 or lines with '!'
    fp = fopen(filename,"r");
    nfp = fopen(concat("_NC",filename),"w");

    while(fgets(line,MAXLINE,fp))
      if ((idx = str_in("!",line)) >= 0)
        fputs(rmv_inline(line,idx),nfp);
      else 
        fputs(line,nfp);
  
    fclose(nfp);
    fclose(fp);
  }
}
