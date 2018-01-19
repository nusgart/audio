#include "AudioInterface.h"
#include "stdio.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
//
#define Read            0
#define Write           1
#define ParentRead      read_pipe[0]
#define ParentWrite     write_pipe[1]
#define ChildRead       write_pipe[0]
#define ChildWrite      read_pipe[1]
// IN OUT
static FILE* input;
static FILE* output;
//signal magic
static int found;
static pid_t cpid;
#if 1
static void _sig_handler(int id){
  if(id == SIGUSR1){
    found = 0;
  }else if(id == SIGUSR2){
    found = 1;
  }else return;
}
#endif
int initAudio(){
  found = 0;
  int read_pipe[2];
  int write_pipe[2];
  int retI = pipe(read_pipe);
  int retO = pipe(write_pipe);
  cpid = fork();
  #if 1
  /// SIGNAL BLACK MAGIC
  struct sigaction sig, sav, sav2;
  sigfillset(&sig.sa_mask);
  sig.sa_handler = _sig_handler;
  sig.sa_flags = 0;
  sigaction(SIGUSR2, &sig, &sav2);
  #endif
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
    execlp("./audio/AudioEngine", "AudioEngine", (char*)NULL);
    printf("Could not find AudioEngine\n");
    fprintf(stderr, "Could not find AudioEngine\n");
    abort();
  }else{
    //parent
    close(ChildRead);
    close(ChildWrite);
    input = fdopen(ParentRead, "r");
    output = fdopen(ParentWrite, "w");
  }
  return 0;
}
int loadSound(char *soundName, char *filePath){
  //
  if(!found)return -1;
  fprintf(output, "load %s %s\n", soundName, filePath);
  fflush(output);
  return 0;
}
int playSnd(char *soundName, double x, double y, double z, float pitch, float gain){
  if(!found) return -1;
  fprintf(output, "playSound %s %f %f %f  %f %f %f  %f %f\n", soundName, x, y, z, 0., 0., 0., pitch, gain);
  fflush(output);
  return 0;
}
int playSound(char *soundName, double x, double y, double z, double vx, double vy, double vz, float pitch, float gain){
  if(!found) return -1;
  fprintf(output, "playSound %s %f %f %f  %f %f %f  %f %f\n", soundName, x, y, z, vx, vy, vz, pitch, gain);
  puts("Playing Sound");
  fflush(output);
  return 0;
}
int quitAudio(){
  if(!found){
    puts("Ignoring request to Quit Audio as AudioEngine was not found!");
  }
  fprintf(output, "quit\n");
  puts("Quitting Audio");
  int status;
  fflush(output);
  wait(&status);
  fclose(output);
  fclose(input);
  return 0;
}
