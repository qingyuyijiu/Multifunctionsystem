#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "speech.h"

int speech_arecord() // 录音
{
  system("arecord -d2 -c1 -r16000 -traw -fS16_LE hehe.pcm");
}

int speech_apply(char *filename) // 播放
{
  char wavfile[124] = {0};
  sprintf(wavfile, "aplay %s", filename);
  system("aplay xiaolao.wav");
}