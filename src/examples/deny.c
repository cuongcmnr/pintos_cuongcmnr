#include <syscall.h>
int main (int argc, char **argv) {
  int handle = open (argv[0]); 
  if (handle != -1) { 
    char buffer[1] = {'A'}; 
    int bytes_written = write (handle, buffer, 1); 
    close (handle); 
    return bytes_written == 1 ? 0 : -1; 
  } 
  return -1; 
}
