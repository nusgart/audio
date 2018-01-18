#include "AudioInterface.h"
#include "stdio.h"

int main(int argc, char** argv){
  puts("Loading");
  initAudio();
  puts("inited audio");
  loadSound("s", "sh.mpcm");
  puts("loaded sounds");
  unsigned long int j=0;
  while(1){
    playSound("s", 1, 1, 1, 0,0,0, 1,1);
//    j = 0;
    for(int i = 0; i < 100000000; i++)j+=i;
    if(j > 2l<<62l)break;
  }
  printf("%ld", j);
  puts("played sound");
  getchar();
  quitAudio();
  puts("quit audio");
  getchar();
}
