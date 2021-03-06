/* AudioInterface -- an interface to the cli AudioEngine.
 * Copyright (C) 2016 Nicholas Nusgart
 */
#ifndef _AUDIO_INTERFACE_H
#define _AUDIO_INTERFACE_H
#ifdef __cplusplus
extern "C"{
#endif
int initAudio(void);
int loadSound(const char *soundName, const char *filePath);
int playSnd(const char *soundName, double x, double y, double z, float pitch, float gain);
int playSound(const char *soundName, double x, double y, double z, double vx, double vy, double vz, float pitch, float gain);
int quitAudio(void);

#ifdef __cplusplus
}
#endif

#endif  
