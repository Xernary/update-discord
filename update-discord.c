#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define PATH "/home/nick/code/update-discord/test"//"/opt/discord/resources/build_info.json"
#define BUFF_SIZE 512
#define PATTERN "\"version\": \""
#define PATTERN_SIZE 12

void increase_version(const int fd, char* const buff, char* const version){

  char* version_string = malloc(sizeof(char) * 5);
  memset(version_string, 0, 5);

  int dots = 0;
  int end = -1;
  for(int i = 0; i < strlen(version) - 1; i++){
    if(version[i] == '"') break;
    if(dots == 2) {end = i; strcat(version_string, &version[i]);}
    if(version[i] == '.') dots++;
  }

  int old_version_number = atoi(version_string);
  printf("New version: %d\n", old_version_number+1);

  char* new_version_string;
  sprintf(new_version_string, "%d", old_version_number+1);

  char* output = malloc(sizeof(char) * BUFF_SIZE);
  memset(output, 0, BUFF_SIZE);
  strncpy(output, buff, strlen(buff) - strlen(version) + end-1);
  strcat(output, new_version_string);
  strcat(output, &buff[strlen(buff)-2- strlen(version) + end +strlen(version_string)]);

  lseek(fd, 0, SEEK_SET);
  write(fd, output, BUFF_SIZE-1);

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
