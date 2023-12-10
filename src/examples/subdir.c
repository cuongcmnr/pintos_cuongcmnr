#include <syscall.h> 
#include <stdio.h> 
int main (void) { 
  if (!mkdir ("/dir")) { 
    printf ("mkdir failed\n"); 
    return 1; 
  } 
  if (!chdir ("/dir")) { 
    printf ("chdir failed\n"); 
    return 1; 
  }
  return 0; 
} 