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

     void setData(Sound s, Point3D pos, Point3D vel, bool loop) {
	  alSourcei(sourceID, AL_BUFFER, s.alID);
	  alSource3f(sourceID, AL_POSITION, (float) pos.x, (float) pos.y, (float)pos.z);
	  alSource3f(sourceID, AL_VELOCITY, (float) vel.x, (float) vel.y, (float)vel.z);
	  alSourcef(sourceID, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
     }

     void setData(Sound s, Point3D pos, bool loop) {
	  alSourcei(sourceID, AL_BUFFER, s.alID);
	  alSource3f(sourceID, AL_POSITION, (float) pos.x, (float) pos.y, (float)pos.z);
//	  alSource3f(sourceID, AL_VELOCITY, (float) vel.x, (float) vel.y, (float)vel.z);
	  alSourcef(sourceID, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
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
     fread(data, sizeof(int), 4, soundFile);
     if(data[0] != MPCM){
	  //TODO FIXME: Handle error
     }
     s.format = data[1];
     s.frequency = data[2];
     s.length = data[3];
     cerr << "Magic: "<<data << " frequency: " << s.frequency << " length: " << s.length ;//<< endl;
     s.data = new unsigned char[s.length];
     alGenBuffers(1, &(s.alID));
     cerr << " Id: " << s.alID << endl;
     alBufferData(s.alID, s.format, s.data, s.length, s.frequency);
     return s;
}

Sound loadSound(string name, string file){
     soundMap[name] = loadPCMSound(fopen(file.c_str(), "r"));
     return soundMap[name];
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
     int stereo, mono;
     alcGetIntegerv(device, ALC_STEREO_SOURCES, 1, &stereo);
     alcGetIntegerv(device, ALC_MONO_SOURCES,1, &mono);
     int numSources = (stereo + mono) / 4;
     Source sources[numSources];
     cerr << "Num Sources: "<<numSources<<endl;
     cerr << "AL error: " << alGetError() << endl;
     for(int i=0; i<numSources; i++){
	  //generate a source
	  alGenSources(1, &(sources[i].sourceID));
	  cerr << "SourceID: " << sources[i].sourceID << endl;
	  alSourcef(sources[i].sourceID, AL_PITCH, 1);
	  alSourcef(sources[i].sourceID, AL_GAIN, 1);
     }
     cerr << "Sources Loaded - AL error: " << alGetError() << endl;
     bool quit = false;
     while(!quit){
	  string in;
	  cin >> in;
	  if(in.find("load") != string::npos){
	       //load file
	       string name; string fname;
	       cin >> name >> fname;
	       loadSound(name, fname);
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
	       //find a free source
	       for(; i<numSources; i++){
		    if(!sources[i].isPlaying()) break;
	       }
	       if(i == numSources){
		    cout << "ERROR 1: Out of Sources" << endl;
	       }else{
		    //use the source to play the sound
		    sources[i].setData(s, x, y, z, vx, vy, vz, pitch, gain, false);
		    sources[i].play();
		    cout << "PLAYING SOUND " << i << endl;
	       }
	  }else if(in.find("quit") != string::npos){
	       //quit
	       alcMakeContextCurrent(NULL);
	       alcDestroyContext(context);
	       alcCloseDevice(device);
	       exit(0);
	  }else{
	  }
	  cerr << "AL error: " << alGetError() << endl;
     }
}
