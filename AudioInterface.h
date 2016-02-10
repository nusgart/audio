/* AudioInterface -- an interface to the cli AudioEngine.
 * Copyright (C) 2016 Nicholas Nusgart
 */
#ifndef _AUDIO_INTERFACE_H
#define _AUDIO_INTERFACE_H
int initAudio(void);
int loadSound(char *soundName, char *filePath);
int playSnd(char *soundName, double x, double y, double z, float pitch, float gain);
int playSound(char *soundName, double x, double y, double z, double vx, double vy, double vz, float pitch, float gain);
int quitAudio(void);
#endif  
