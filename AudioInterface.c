#include "AudioInterface.h"
#include "stdio.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
//
#define Read            0
#define Write           1
#define ParentRead      read_pipe[0]
#define ParentWrite     write_pipe[1]
#define ChildRead       write_pipe[0]
#define ChildWrite      read_pipe[1]
static FILE* input;
static FILE* output;
int initAudio(){
  //
  int read_pipe[2];
  int write_pipe[2];
  int retI = pipe(read_pipe);
  int retO = pipe(write_pipe);
  pid_t cpid = fork();
  if(cpid == -1){
    //error
    fprintf(stderr, "NO FORKING\n");
  }else if(cpid == 0){
    //child
    close(Read);
    close(Write);
    close(ParentRead);
    close(ParentWrite);
    dup2(ChildRead, 0);
    dup2(ChildWrite, 1);
    execlp("./AudioEngine", "AudioEngine", (char*)NULL);
  }else{
    //parent
    close(ChildRead);
    close(ChildWrite);
    input = fdopen(ParentRead, "r");
    output = fdopen(ParentWrite, "w");
  }
}
int loadSound(char *soundName, char *filePath){
  //
  fprintf(output, "load %s %s\n", soundName, filePath);
  fflush(output);
  return 0;
}
int playSnd(char *soundName, double x, double y, double z, float pitch, float gain){
  fprintf(output, "playSound %s %f %f %f  %f %f %f  %f %f\n", soundName, x, y, z, 0., 0., 0., pitch, gain);
  fflush(output);
  return 0;
}
int playSound(char *soundName, double x, double y, double z, double vx, double vy, double vz, float pitch, float gain){
  fprintf(output, "playSound %s %f %f %f  %f %f %f  %f %f\n", soundName, x, y, z, vx, vy, vz, pitch, gain);
  puts("Playing Sound");
  fflush(output);
  return 0;
}
int quitAudio(){
  fprintf(output, "quit\n");
  puts("Quitting Audio");
  int status;
  fflush(output);
  wait(&status);
  fclose(output);
  fclose(input);
  return 0;
}
