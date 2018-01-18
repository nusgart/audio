#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <map>
using namespace std;

struct Point3D{
  double x,y,z;
};
const unsigned int MPCM=0x504d4d43;
struct Sound{
  unsigned int alID;
  unsigned int format;
  unsigned int frequency;
  //maximum length 4GB
  unsigned int length;
  unsigned char* data;
};

struct Source{
  unsigned int sourceID;
  bool isPlaying() {
    int al_state;
    alGetSourcei(sourceID, AL_SOURCE_STATE, &al_state);
    return al_state == AL_PLAYING;
  }
  
  bool isLooping(){
    int al_looping;
    alGetSourcei(sourceID, AL_LOOPING, &al_looping);
    return al_looping == AL_TRUE;
  }

  void setData(Sound s, float x, float y, float z, float vx, float vy, float vz,float gain, float pitch, bool loop){
    alSourcei(sourceID, AL_BUFFER, s.alID);
    alSource3f(sourceID, AL_POSITION, x, y, z);
    alSource3f(sourceID, AL_VELOCITY, vx, vy, vz);
    alSourcef(sourceID, AL_GAIN, gain);
    alSourcef(sourceID, AL_PITCH, pitch);
    alSourcei(sourceID, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
  }
     
  void play() {
    alSourcePlay(sourceID);
  }

  void pause() {
    alSourcePause(sourceID);
  }

  void dispose() {
    alDeleteSources(1, &sourceID);
  }
};


map<string, Sound> soundMap;

Sound loadPCMSound(FILE* soundFile){
  unsigned int data[4];
  Sound s;
  //read header
  fread(data, sizeof(int), 4, soundFile);
  if(data[0] != MPCM){
    //TODO FIXME: Handle error
    cerr << "Bad Magic Number: " << data[0] << endl;
  }
  //setup sound
  s.format = data[1];
  s.frequency = data[2];
  s.length = data[3];
  cerr << "Magic: "<<data[0] << " frequency: " << s.frequency << " length: " << s.length ;//<< endl;
  //read sound data
  s.data = new unsigned char[s.length];
  fread(s.data, sizeof(char), s.length, soundFile);
  alGenBuffers(1, &(s.alID));
  cerr << " Id: " << s.alID << endl;
  alBufferData(s.alID, s.format, s.data, s.length, s.frequency);
  return s;
}

bool loadSound(string name, string file){
  FILE* fl = fopen(file.c_str(), "rb");
  if(!fl){
    return false;
  }
  soundMap[name] = loadPCMSound(fl);
  fclose(fl);
  return true;
}

void onExit(int numSources, Source* sources){
  bool exit = true;
  do{
    exit = true;
    for(int i = 0; i < numSources; i++){
      //ensure that all sounds have finished playing
      if(sources[i].isPlaying())// && !sources[i].isLooping())
	exit = false;
    }
  }while(!exit);
  cout << "ALENGINE EXIT" << endl;
}

int main(int argc, char* argv[]){
  ALCcontext *context;
  ALCdevice *device;
  const ALCchar *default_device;
  default_device = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
  device = alcOpenDevice(default_device);//alcOpenDevice(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
  if (device == NULL){
    // Handle Exception
    cout << "ERROR 0: Could Not Open ALDevice" << endl;
  }
  //Create a context
  context=alcCreateContext(device,NULL);
  //Make the context current
  alcMakeContextCurrent(context);
  // Clear Error Code
  cerr << "AL error: " << alGetError() << endl;
  //Allocate sources
  int stereo, mono;
  alcGetIntegerv(device, ALC_STEREO_SOURCES, 1, &stereo);
  alcGetIntegerv(device, ALC_MONO_SOURCES,1, &mono);
  //use one fourth of the availible sources
  int numSources = (stereo + mono) / 4;
  Source sources[numSources];
  cerr << "Num Sources: "<<numSources << " num mono: " << mono << " num stereo: " << stereo <<endl;
  cerr << "AL error: " << alGetError() << endl;
  for(int i=0; i<numSources; i++){
    //generate a source
    alGenSources(1, &(sources[i].sourceID));
    //cerr << "SourceID: " << sources[i].sourceID << endl;
    //set initial pitch and gain to 1
    alSourcef(sources[i].sourceID, AL_PITCH, 1);
    alSourcef(sources[i].sourceID, AL_GAIN, 1);
  }
  cerr << numSources << " Sources Loaded - AL error: " << alGetError() << endl;
  //command loop
  bool quit = false;
  while(!quit){
    string in;
    cin >> in;
    if(in.find("load") != string::npos){
      //load file
      string name; string fname;
      cin >> name >> fname;
      bool ret = loadSound(name, fname);
      if(!ret){
	cout << "ERROR 2: Sound file does not exist or is not valid file" << endl;
      }
    }else if(in.find("setpos") != string::npos){
      //set listner position
      float x, y, z;
      cin >> x >> y >> z;
      alListener3f(AL_POSITION, x, y, z);
    }else if(in.find("playSound") != string::npos){
      //play a sound
      string name;
      float x, y, z, vx, vy, vz, pitch, gain;
      cin >> name >> x >> y >> z >> vx >> vy >> vz>> pitch >> gain;
      int i = 0;
      Sound s = soundMap[name];
      cerr << "Sound : " << name << " id: " << s.alID << endl;
      //find a free source
      for(; i<numSources; i++){
	if(!sources[i].isPlaying()) break;
      }
      if(i == numSources){
	//give out of sources error
	cout << "ERROR 1: Out of Sources" << endl;
      }else{
	//use the source to play the sound
	sources[i].setData(s, x, y, z, vx, vy, vz, pitch, gain, false);
	sources[i].play();
	cout << "PLAYING SOUND " << i << endl;
      }
    }else if(in.find("quit") != string::npos){
      //quit
      onExit(numSources, sources);
      alcMakeContextCurrent(NULL);
      alcDestroyContext(context);
      alcCloseDevice(device);
      exit(0);
    }else{
    }
    cerr << "AL error: " << alGetError() << endl;
  }
}
