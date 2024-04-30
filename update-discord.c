#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define PATH "/opt/discord/resources/build_info.json"
#define BUFF_SIZE 512
#define PATTERN "\"version\": \""
#define PATTERN_SIZE 12
#define COMMAND "sudo chmod a+w "

void print_help(){
  printf("Usage: update-discord [VERSION]\n");
  exit(1);
}

int open_file(){

  int fd;

  if((fd = open(PATH, O_RDWR)) == -1){
    if(errno != EACCES){
      perror("opening file");
      exit(1);
    }

    const int command_size = sizeof(char) * sizeof(PATH) + sizeof(char) * sizeof(COMMAND);

    char* command = malloc(command_size);
    memset(command, 0, command_size);
    strcpy(command, COMMAND);
    strcat(command, PATH);
    system(command);

    if((fd = open(PATH, O_RDWR)) == -1){
       perror("opening file");
       exit(1);
    }
  }

  return fd;
}

void update_version(const int fd, char* const buff, char* const version, int ver, char* new_version){

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
  int new_version_number; //= ((ver == -1) ? old_version_number+1 : ver);
  if(ver == -1) new_version_number = old_version_number+1;
  else new_version_number = ver;


  char new_version_string[10];
  memset(new_version_string, 0, 10);
  sprintf(new_version_string, "%d", new_version_number);

  char* output = malloc(sizeof(char) * BUFF_SIZE);
  memset(output, 0, BUFF_SIZE);
  strncpy(output, buff, strlen(buff) - strlen(version) + end-1);
  strcat(output, new_version_string);
  strcat(output, &buff[strlen(buff)-1- strlen(version) + end +strlen(version_string)]);

  lseek(fd, 0, SEEK_SET);
  write(fd, output, strlen(output));

  strcpy(new_version, new_version_string);
  free(version_string);

  return;
}

int main(int argn, char** args){

  int fd = open_file();

  char buff[BUFF_SIZE];
  int n = read(fd, buff, BUFF_SIZE-1);
  buff[n] = '\0';

  char* version;
  if((version = strstr(buff, PATTERN)) == NULL){
    perror("version not found");
    exit(1);
  }

  if(argn != 1 && argn != 2){
    print_help();
    exit(1);
  }

  int ver = -1;
  if(argn == 2){
    if(args[1] == "--help") print_help();
    else if((ver = atoi(args[1])) == 0) print_help();
  }

  char new_version[10];
  update_version(fd, buff, version+PATTERN_SIZE, ver, new_version); 
  printf("Updated discord to version 0.0.%s\n", new_version);

  close(fd);

  return 0;
}
