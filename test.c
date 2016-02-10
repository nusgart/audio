#include "AudioInterface.h"
#include "stdio.h"

int main(int argc, char** argv){
  puts("Loading");
  initAudio();
  puts("inited audio");
  loadSound("s", "sh.mpcm");
  puts("loaded sounds");
  playSound("s", 1, 1, 1, 0,0,0, 1,1);
  puts("played sound");
  quitAudio();
  puts("quit audio");
}
