#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

bool isAnOperator(char* s);
bool validVariable(char* s);
bool legalDigit(char *s);
bool canOptimize(int argc, char *a[]);
void printAssembly(char *a[], int argc);
void optimizedAssembly(char *a[], int argc);
void optimizedPush(char *s);
void optimizedExpression(char* num, char* s);
void printExpression(char* s, int numsOnStack);
void push(char* s);
char* getString(char* num);

int main (int argc, char *argv[]) {
  int i, varCount = 0, opCount = 0;
  if(argc == 0 || argc == 1 || argc == 3) {
    fprintf(stderr, "Sorry, that is not a valid RPN function.\n");
    exit(EXIT_FAILURE);
  }
  for(i = 1; i < argc; i++) {
    // printf("%s\n", argv[i]);
    char *temp = *argv;
    if(!isAnOperator(argv[i]) && !validVariable(argv[i]) && !legalDigit(argv[i])) {
      fprintf(stderr, "You can only input any digit, x, y, z, +, -, or \"*\".\n");
      exit(EXIT_FAILURE);
    }
    if(isAnOperator(argv[i]))
      opCount++;
    else if(legalDigit(argv[i]) || validVariable(argv[i]))
      varCount++;
    //printf("op: %d var: %d\n", opCount, varCount);
    if(opCount == varCount) {
      fprintf(stderr, "You did not input a proper expression.\n");
      exit(EXIT_FAILURE);
    }
    temp++;
  }
  if(!((varCount - opCount) == 1)) {
    fprintf(stderr, "You have too few or too many operators.\n");
    exit(EXIT_FAILURE);
  }
  printAssembly(argv, argc);
  exit(EXIT_SUCCESS);
}

bool legalDigit(char *str) {
  char* temp; int num;
  num = strtol(str, &temp, 10);
  if(temp == NULL)
    return false;
  else if(*temp == 0)
    return true;
  else
    return false;
}

bool isAnOperator(char* s) {
  //int count = 0;
  //while(*s) {
    //count++;
    //*s++;
  //}
  //printf("count: %d\n", count);
  return ((strcmp(s, "+") == 0 || strcmp(s, "-") == 0 || strcmp(s, "*") == 0));
}

bool validVariable(char* s) {
  char *x, *y, *z;
  x = "x";
  y = "y";
  z = "z";
  return ((strcmp(s, x) == 0) || (strcmp(s, y) == 0) || (strcmp(s, z) == 0));
}

void printAssembly(char *a[], int argc) {
  int i = 2; char *temp = *a; int numsOnStack = 1;
  printf("  .globl compute\n");
  printf("compute:\n");
  if(canOptimize(argc, a))
    optimizedAssembly(a, argc);
  else {
    push(a[1]);
    while(a[i]) {
      if(isAnOperator(a[i])) {
        printExpression(a[i], numsOnStack);
        numsOnStack--;
      }
      else {
        push(a[i]);
        numsOnStack++;
      }
      temp++;
      i++;
    }
    printf("  popq  %%rax\n");
    printf("  retq\n");
  }
}

bool canOptimize(int argc, char *a[]) {
  int i;
  for(i = 3; i < argc; i += 2)
    if(!((i % 2 == 1) && isAnOperator(a[i])))
      return false;
  return true;
}

void printExpression(char* s, int nos) {
  char *plus, *minus, *mul;
  plus = "+";
  minus = "-";
  mul = "*";
  printf("  popq  %%r10\n");
  printf("  popq  %%r11\n");
  if(strcmp(s, plus) == 0)
    printf("  addq  %%r10, %%r11\n");
  else if(strcmp(s, minus) == 0)
    printf("  subq  %%r10, %%r11\n");
  else if(strcmp(s, mul) == 0) {
    printf("  imulq %%r10, %%r11\n");
  }
  printf("  pushq %%r11\n");
}

void push(char* s) {
  char *x, *y, *z;
  x = "x";
  y = "y";
  z = "z";
  if(strcmp(s, x) == 0)
    printf("  pushq %%rdi\n");
  else if(strcmp(s, y) == 0)
    printf("  pushq %%rsi\n");
  else if(strcmp(s, z) == 0)
    printf("  pushq %%rdx\n");
  else
    printf("  pushq $%s\n", s);
}

void optimizedAssembly(char *a[], int argc) {
  char *x, *y, *z; char* pass; int i = 3; char *str;
  x = "x";
  y = "y";
  z = "z";
  optimizedPush(a[1]);
  while(a[i]) {
    optimizedExpression(a[i - 1], a[i]);
    i += 2;
    if(a[i - 1] == NULL || a[i] == NULL)
      break;
  }
  printf("  retq\n");
}

void optimizedPush(char *s) {
  char *x, *y, *z;
  x = "x";
  y = "y";
  z = "z";
  if(strcmp(s, x) == 0)
    printf("  movq  %%rdi, %%rax\n");
  else if(strcmp(s, y) == 0)
    printf("  movq  %%rsi, %%rax\n");
  else if(strcmp(s, z) == 0)
    printf("  movq  %%rdx, %%rax\n");
  else
    printf("  movq  $%s, %%rax\n", s);
}

void optimizedExpression(char* num, char* s) {
  char *plus, *minus, *mul; char *x, *y, *z; char str[80];
  plus = "+"; minus = "-"; mul = "*";
  x = "x"; y = "y"; z = "z";
  //str = getString(num);
  //str = "4";
  //printf("%s\n", str);
  //malloc(80 * sizeof(str));
  if(strcmp(num, x) == 0)
    strcpy(str, "%rdi");
  else if(strcmp(num, y) == 0)
    strcpy(str, "%rsi");
  else if(strcmp(num, z) == 0)
    strcpy(str, "%rdx");
  else {
    strcpy(str, "$");
    strcat(str, num);
  }
  if(strcmp(s, plus) == 0)
    printf("  addq  %s, %%rax\n", str);
  else if(strcmp(s, minus) == 0)
    printf("  subq  %s, %%rax\n", str);
  else if(strcmp(s, mul) == 0)
    printf("  imulq %s, %%rax\n", str);
}

char* getString(char* var) {
  char *x, *y, *z; char str[80];
  x = "x"; y = "y"; z = "z";
  malloc(sizeof str);
  if(strcmp(var, x) == 0)
    return "%rdi";
  else if(strcmp(var, y) == 0)
    return "%rsi";
  else if(strcmp(var, z) == 0)
    return "%rdx";
  else {
    strcpy(str, "$");
    return strcat(str, var);
  }
}
