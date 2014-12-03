
#ifndef __SOUND_INCLUDED__
#define __SOUND_INCLUDED__ 1

#include <windows.h>
#include <string>
#include <mmsystem.h>
// Project -> properties -> linker -> command line, add winmm.lib to additional options

struct WAVFileInfo
{ char codename1[4]; // should be 'RIFF'
  int total_size;
  char codename2[4]; // should be 'WAVE'
  char codename3[4]; // should be 'fmt '
  int header_size;
  short int format;
  short int channels;
  int samples_per_second;
  int average_bytes_per_second;
  short int block_size;
  short int bits_per_sample;
  char codename4[4]; // should be 'data'
  int data_size;
  char * filename;
  int num_samples; };

WAVFileInfo GetWAVFileInfo(std::string wavfilename);
void print(const WAVFileInfo & w);

struct WaveForm;

class Sound
{ protected:
    WaveForm *wf;
    WAVEFORMATEX *wx;
    int length, WAVE_SAMPLE_RATE, ok, justacopy;
  public:
    Sound(double seconds, double samplespersec=11025.0);
    Sound(std::string wavfilename, double start=0.0, double maxlen=999999999.9);
    Sound(const Sound & s);
	Sound(void);
    Sound & operator=(const Sound & s);
    ~Sound(void);
    int samplerate(void);
    int getlength(void);
    short int & operator[](int index);
    void save(std::string wavfilename);
    void record(void);
    void play(void); };

void play(Sound &s);
void record(Sound &s);
int number_of_samples(Sound &s);
int samples_per_second(Sound &s);

#endif

