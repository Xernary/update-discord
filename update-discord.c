#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define PATH "/opt/discord/resources/build_info.json"
#define BUFF_SIZE 512
#define PATTERN "\"version\": \""
#define PATTERN_SIZE 12

void increase_version(const int fd, char* const buff, char* const version){

  char* const version_string = malloc(sizeof(char) * 5);
  memset(version_string, 0, 5);

  int dots = 0;
  int end = -1;

  for(int i = 0; i < strlen(version) - 1; i++){
    char ch = version[i];
    if(ch == '"') break;
    if(dots == 2) {end = i; strncpy(&version_string[strlen(version_string)], &ch, 1);}
    if(ch == '.') dots++;
  }


  int old_version_number = atoi(version_string);

  char* new_version_string;
  sprintf(new_version_string, "%d", old_version_number+1);

  char* output = malloc(sizeof(char) * BUFF_SIZE);
  memset(output, 0, BUFF_SIZE);
  strncpy(output, buff, strlen(buff) - strlen(version) + end-1);
  printf("\n%s\n", output);
  strcat(output, new_version_string);
  printf("\n%s\n", output);
  strcat(output, &buff[strlen(buff)-1- strlen(version) + end +strlen(version_string)]);

  printf("\noutput = %s\n", output);

  lseek(fd, 0, SEEK_SET);
  write(fd, output, strlen(output));

  return;
}

int main(){

  const int fd = open(PATH, O_RDWR); 
  char buff[BUFF_SIZE];

  int n = read(fd, buff, BUFF_SIZE-1);
  buff[n] = '\0';

  char* version;
  if((version = strstr(buff, PATTERN)) == NULL){
    perror("version not found");
    exit(1);
  }

  printf("WORKED\n");

  increase_version(fd, buff, version+PATTERN_SIZE); 

  close(fd);

  return 0;
}
