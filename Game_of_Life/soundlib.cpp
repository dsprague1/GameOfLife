//#include "library.h"
#include "soundlib.h"
#include "library.h"
#include <string>
#include <assert.h>

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

WAVFileInfo GetWAVFileInfo(std::string wavfilename)
{ char *nm=_strdup(wavfilename.c_str());
  WAVFileInfo r;
  r.total_size=0;
  r.header_size=0;
  r.channels=0;
  r.samples_per_second=0;
  r.average_bytes_per_second=0;
  r.block_size=0;
  r.bits_per_sample=0;
  r.data_size=0;
  r.num_samples=0;
  int f=_open(nm, _O_RDONLY | _O_BINARY);
  if (f<0)
  { std::string e="Can not open file \"" + wavfilename + "\"";
    error_pop_up(e,e);
    r.filename=nm;
    return r; }
  int n=_read(f, &r, sizeof(r)-8);
  if (n!=sizeof(r)-8)
  { std::string e="\"" + wavfilename + "\" is not a proper WAV file\n This error is on line 31.";
    cerr<<e;
    r.filename=nm;
    return r; }
  _close(f);
  r.filename=nm;
  r.num_samples=r.data_size/r.channels/(r.bits_per_sample/8);
  return r; }

void print(const WAVFileInfo & w)
{ print("Information from ");
  if (w.filename==NULL)
    print("(anonymous)\n");
  else
  { print(w.filename);
    print("\n"); }
  int big=(strcmp("(memory-resident sound descriptor)", w.filename)!=0);
  if (big)
  { printf("  codename1=\"%c%c%c%c\"\n",
          w.codename1[0], w.codename1[1], w.codename1[2], w.codename1[3]);
    print("  total_size="); print(w.total_size); print("\n");
    printf("  codename2=\"%c%c%c%c\"\n",
          w.codename2[0], w.codename2[1], w.codename2[2], w.codename2[3]);
    printf("  codename3=\"%c%c%c%c\"\n",
          w.codename3[0], w.codename3[1], w.codename3[2], w.codename3[3]);
    print("  header_size="); print(w.header_size); print("\n"); }
  print("  format="); print(w.format); print("\n");
  print("  channels="); print(w.channels); print("\n");
  print("  samples_per_second="); print(w.samples_per_second); print("\n");
  print("  average_bytes_per_second="); print(w.average_bytes_per_second); print("\n");
  print("  block_size="); print(w.block_size); print("\n");
  print("  bits_per_sample="); print(w.bits_per_sample); print("\n");
  if (big)
  { printf("  codename4=\"%c%c%c%c\"\n",
          w.codename4[0], w.codename4[1], w.codename4[2], w.codename4[3]);
    print("  data_size="); print(w.data_size); print("\n");
    print("  num_samples="); print(w.num_samples); print("\n");
    print("  playing time should be ");
      print(((double)w.num_samples)/w.samples_per_second);
      print(" seconds\n"); } }

struct WaveForm
{ char codename1[4];
  int totalsize;
  char codename2[4];
  char codename3[4];
  int headersize;
  PCMWAVEFORMAT header;
  char codename4[4];
  int datasize;
  short int sample[1]; };

void Sound::save(std::string wavfilename)
{ int f=_open(wavfilename.c_str(), _O_WRONLY | _O_BINARY | _O_CREAT, _S_IREAD | _S_IWRITE);
  if (f<0)
  { printf("Error saving\n");
    return; }
  WaveForm twf=*wf;
  int hsz=sizeof(WaveForm)-1;
  twf.totalsize=(twf.totalsize-hsz)*2+hsz;
  twf.header.wf.nAvgBytesPerSec*=2;
  twf.header.wf.nBlockAlign*=2;
  twf.header.wf.nChannels=2;
  twf.datasize*=2;
  _write(f, &twf, hsz);
  char * base=((char *)wf)+hsz;
  int remaining=wf->datasize;
  while (remaining>0)
  { char buff[1024];
    int cur=remaining;
    if (cur>512) cur=512;
    for (int i=0; i<cur; i+=2)
    { int ib=i*2;
      buff[ib]=base[i];
      buff[ib+1]=base[i+1];
      buff[ib+2]=base[i];
      buff[ib+3]=base[i+1]; }
    _write(f, buff, cur*2);
    base+=512;
    remaining-=512; }
  _close(f); }

WAVFileInfo GetWAVFileInfo(const WaveForm & w)
{ WAVFileInfo r;
  WaveForm * s = (WaveForm *)&r;
  *s=w;
  r.filename="(memory-resident sound)";
  r.num_samples=r.data_size/r.channels/(r.bits_per_sample/8);
  return r; }

WAVFileInfo GetWAVFileInfo(const WaveForm * w)
{ WAVFileInfo r;
  WaveForm * s = (WaveForm *)&r;
  *s=*w;
  r.filename="(memory-resident sound)";
  r.num_samples=r.data_size/r.channels/(r.bits_per_sample/8);
  return r; }

static WaveForm *MakeWaveForm(int nsamples, int WAVE_SAMPLE_RATE)
{ WaveForm *w=(WaveForm*)malloc(sizeof(WaveForm)+2*nsamples+1024);
  w->codename1[0]='R';  w->codename1[1]='I';  w->codename1[2]='F';  w->codename1[3]='F';
  w->totalsize=36+2*nsamples;
  w->codename2[0]='W';  w->codename2[1]='A';  w->codename2[2]='V';  w->codename2[3]='E';
  w->codename3[0]='f';  w->codename3[1]='m';  w->codename3[2]='t';  w->codename3[3]=' ';
  w->headersize=sizeof(PCMWAVEFORMAT);
  w->header.wf.wFormatTag=1;
  w->header.wf.nChannels=1;
  w->header.wf.nSamplesPerSec=WAVE_SAMPLE_RATE;
  w->header.wf.nAvgBytesPerSec=WAVE_SAMPLE_RATE*2;
  w->header.wf.nBlockAlign=2;
  w->header.wBitsPerSample=16;
  w->codename4[0]='d';  w->codename4[1]='a';  w->codename4[2]='t';  w->codename4[3]='a';
  w->datasize=2*nsamples;
  return w; }

static WAVEFORMATEX *MakeWaveFormEx(int nsamples, int WAVE_SAMPLE_RATE)
{ WAVEFORMATEX *w=(WAVEFORMATEX *)malloc(sizeof(WAVEFORMATEX));
  w->wFormatTag=1;
  w->nChannels=1;
  w->nSamplesPerSec=WAVE_SAMPLE_RATE;
  w->nAvgBytesPerSec=WAVE_SAMPLE_RATE*2;
  w->nBlockAlign=2;
  w->wBitsPerSample=16;
  w->cbSize=0;
  return w; }

WAVFileInfo GetWAVFileInfo(const WAVEFORMATEX * w)
{ WAVFileInfo r;
  r.codename1[0]='-'; r.codename1[1]='-'; r.codename1[2]='-'; r.codename1[3]='-';
  r.total_size=0;
  r.codename2[0]='-'; r.codename2[1]='-'; r.codename2[2]='-'; r.codename2[3]='-';
  r.codename3[0]='-'; r.codename3[1]='-'; r.codename3[2]='-'; r.codename3[3]='-';
  r.header_size=0;
  r.format=w->wFormatTag;
  r.channels=w->nChannels;
  r.samples_per_second=w->nSamplesPerSec;
  r.average_bytes_per_second=w->nAvgBytesPerSec;
  r.block_size=w->nBlockAlign;
  r.bits_per_sample=w->wBitsPerSample;
  r.codename4[0]='-'; r.codename4[1]='-'; r.codename4[2]='-'; r.codename4[3]='-';
  r.data_size=0;
  r.filename="(memory-resident sound descriptor)";
  r.num_samples=0;
  return r; }

Sound::Sound(void)
{
	WAVE_SAMPLE_RATE = 0;
	length = 0.0;
	ok = 0;
	justacopy = 0;
}

Sound::Sound(double seconds, double samplespersec)
{ WAVE_SAMPLE_RATE=(int)samplespersec;
  length=(int)(seconds*samplespersec);
  wf=MakeWaveForm(length, WAVE_SAMPLE_RATE);
  wx=MakeWaveFormEx(length, WAVE_SAMPLE_RATE);
  ok=1;
  justacopy=0; }

Sound::Sound(std::string wavfilename, double start, double maxlen)
{ const char *nm=wavfilename.c_str();
  int f=_open(nm, _O_RDONLY | _O_BINARY);
  wf=NULL; wx=NULL;
  if (f<0)
  { std::string e="Can not open file \"" + wavfilename + "\"";
    cerr<<e;
    ok=0;
    return; }
  WaveForm wfs;
  int n=_read(f, &wfs, sizeof(wfs)-2);
  ok=1;
  if (wfs.codename1[0]!='R' || wfs.codename1[1]!='I' || wfs.codename1[2]!='F' || wfs.codename1[3]!='F')
    ok=0;
  if (wfs.codename2[0]!='W' || wfs.codename2[1]!='A' || wfs.codename2[2]!='V' || wfs.codename2[3]!='E')
    ok=0;
  if (wfs.codename3[0]!='f' || wfs.codename3[1]!='m' || wfs.codename3[2]!='t' || wfs.codename3[3]!=' ')
    ok=0;
  if (wfs.codename4[0]!='d' || wfs.codename4[1]!='a' || wfs.codename4[2]!='t' || wfs.codename4[3]!='a')
    ok=0;
  if (!ok)
  { std::string e="\"" + wavfilename + "\" is not a proper .WAV file.\nThis error is on line 205";
    cerr<<e;
    ok=0;
    return; }
  int bytes=0, chans=0;
  if (wfs.header.wBitsPerSample==8)
    bytes=1;
  else if (wfs.header.wBitsPerSample==16)
    bytes=2;
  if (wfs.header.wf.nChannels==1)
    chans=1;
  else if (wfs.header.wf.nChannels==2)
    chans=2;
  if (wfs.header.wf.wFormatTag!=1 || chans==0 || bytes==0)
  { char em[200];
    sprintf(em, "%d channel, %d bit, format %d\nNot Understood",
            wfs.header.wf.nChannels, wfs.header.wBitsPerSample, wfs.header.wf.wFormatTag);
    std::string e="\"" + wavfilename + "\"\n" + em;
    cerr<<e;
    ok=0;
    return; }
  WAVE_SAMPLE_RATE=wfs.header.wf.nSamplesPerSec;
  int maxdsz=wfs.datasize;
  if (maxlen<9999999)
  { maxdsz=(int)(maxlen*chans*bytes*WAVE_SAMPLE_RATE);
    maxdsz=(maxdsz+1)&0xFFFFFFFE;
    if (maxdsz>wfs.datasize) maxdsz=wfs.datasize;
    wfs.datasize=maxdsz; }
  length=wfs.datasize/chans/bytes;
  wf=MakeWaveForm(length, WAVE_SAMPLE_RATE);
  wx=MakeWaveFormEx(length, WAVE_SAMPLE_RATE);
  *wf=wfs;
  int skip=(int)(start*chans*bytes*WAVE_SAMPLE_RATE);
  skip=(skip+1)&0xFFFFFFFE;
  _lseek(f, skip, SEEK_CUR);
  if (chans==1 && bytes==1)
  { unsigned char buff[1024];
    int total=0;
    while (total<length)
    { n=_read(f, buff, 1024);
      if (n<=0) break;
      for (int i=0; i<1024; i+=1)
        wf->sample[total+i]=(((int)buff[i])-128)<<8;
      total+=1024; } }
  else if (chans==1 && bytes==2)
    n=_read(f, wf->sample, length);
  else if (chans==2 && bytes==1)
  { unsigned char buff[1024];
    int total=0;
    while (total<length)
    { n=_read(f, buff, 1024);
      if (n<=0) break;
      for (int i=0, j=0; i<512; i+=1, j+=2)
        wf->sample[total+i]=(((int)buff[j])-128)<<8;
      total+=512; } }
  else if (chans==2 && bytes==2)
  { short int buff[1024];
    int total=0;
    while (total<length)
    { n=_read(f, buff, 2048);
      if (n<=0) break;
      for (int i=0, j=0; i<512; i+=1, j+=2)
        wf->sample[total+i]=buff[j];
      total+=512; } }
  wfs.header.wf.nChannels=1;
  wfs.header.wBitsPerSample=16;
  wfs.header.wf.nSamplesPerSec=WAVE_SAMPLE_RATE;
  wfs.header.wf.nAvgBytesPerSec=WAVE_SAMPLE_RATE*2;
  wfs.header.wf.nBlockAlign=2;
  wfs.datasize=length*2;
  wf->header.wf.nChannels=wfs.header.wf.nChannels;
  wf->header.wBitsPerSample=wfs.header.wBitsPerSample;
  wf->header.wf.nAvgBytesPerSec=wfs.header.wf.nAvgBytesPerSec;
  wf->header.wf.nSamplesPerSec=wfs.header.wf.nSamplesPerSec;
  wf->header.wf.nBlockAlign=wfs.header.wf.nBlockAlign;
  wf->datasize=wfs.datasize;
  wx->nChannels=wfs.header.wf.nChannels;
  wx->wBitsPerSample=wfs.header.wBitsPerSample;
  wx->nAvgBytesPerSec=wfs.header.wf.nAvgBytesPerSec;
  wx->nSamplesPerSec=wfs.header.wf.nSamplesPerSec;
  wx->nBlockAlign=wfs.header.wf.nBlockAlign;
  _close(f);
  justacopy=0; }

Sound::Sound(const Sound & s)
{ wf=s.wf;
  wx=s.wx;
  length=s.length;
  WAVE_SAMPLE_RATE=s.WAVE_SAMPLE_RATE;
  ok=s.ok;
  justacopy=1; }

Sound & Sound::operator =(const Sound & s)
{ wf=s.wf;
  wx=s.wx;
  length=s.length;
  WAVE_SAMPLE_RATE=s.WAVE_SAMPLE_RATE;
  ok=s.ok;
  justacopy=1;
  return *this; }

Sound::~Sound(void)
{ if (justacopy) return;
  if (wf!=NULL) free(wf);
  if (wx!=NULL) free(wx);
  ok=0; }

int Sound::samplerate(void)
{ if (!ok) return 0;
  return WAVE_SAMPLE_RATE; }

int Sound::getlength(void)
{ if (!ok) return 0;
  return length; }

short int & Sound::operator[](int index)
{ return wf->sample[index]; }

/*
void Sound::play(void)
{ if (!ok) return;
  int ok=PlaySound((char *)wf, NULL, SND_MEMORY); }
*/

void Sound::play(void)
{ if (!ok) return;
  WAVE_SAMPLE_RATE=wf->header.wf.nSamplesPerSec; // 44k for CD
  int atonce=WAVE_SAMPLE_RATE*2*30; // bytes in 5 seconds from a CD
  WaveForm orig=*wf;
  if (wf->datasize<atonce)
  { PlaySoundA((char *)wf, NULL, SND_MEMORY);
    return; }
  int remaining=wf->datasize, skipped=0;
  while (remaining>0)
  { WaveForm *place=(WaveForm *)(((char *)wf)+skipped);
    WaveForm safe=*place;
    *place=orig;
    int toplay=atonce;
    if (toplay>remaining) toplay=remaining;
    place->totalsize=36+toplay;
    place->datasize=toplay;
    PlaySoundA((char *)place, NULL, SND_MEMORY);
    putchar('.');
    *place=safe;
    skipped+=atonce;
    remaining-=atonce; } }

void Sound::record(void)
{ if (!ok) return;
  HWAVEIN h;
  MMRESULT r=waveInOpen(&h, WAVE_MAPPER, wx, 0, 0, CALLBACK_NULL);
  if (r!=0)
  { std::string s="1 Sound Recording Error";
    if (r==MMSYSERR_ALLOCATED)
      s="1 MMSYSERR_ALLOCATED Specified resource is already allocated.";
    else if (r==MMSYSERR_BADDEVICEID)
      s="1 MMSYSERR_BADDEVICEID Specified device identifier is out of range.";
    else if (r==MMSYSERR_NODRIVER)
      s="1 MMSYSERR_NODRIVER No device driver is present.";
    else if (r==MMSYSERR_NOMEM)
      s="1 MMSYSERR_NOMEM Unable to allocate or lock memory.";
    else if (r==WAVERR_BADFORMAT)
      s="1 WAVERR_BADFORMAT.";
	error_pop_up(s);
    return; }

  WAVEHDR wh;
  wh.lpData=(char *)(wf->sample);
  wh.dwBufferLength=length*2;
  wh.dwBytesRecorded=0;
  wh.dwUser=0;
  wh.dwFlags=0;
  wh.dwLoops=1;
  wh.lpNext=NULL;
  wh.reserved=0;
  r=waveInPrepareHeader(h, &wh, sizeof(wh));
  if (r!=0)
  { std::string s="2 Sound Recording Error";
    if (r==MMSYSERR_INVALHANDLE)
      s="2 MMSYSERR_INVALHANDLE Specified device handle is invalid.";
    else if (r==MMSYSERR_NODRIVER)
      s="2 MMSYSERR_NODRIVER No device driver is present.";
    else if (r==MMSYSERR_NOMEM)
      s="2 MMSYSERR_NOMEM Unable to allocate or lock memory.";
	error_pop_up(s);
    return; }

  r=waveInAddBuffer(h, &wh, sizeof(wh));
  if (r!=0)
  { std::string s="3 Sound Recording Error";
    if (r==MMSYSERR_INVALHANDLE)
      s="3 MMSYSERR_INVALHANDLE Specified device handle is invalid.";
    else if (r==MMSYSERR_NODRIVER)
      s="3 MMSYSERR_NODRIVER No device driver is present.";
    else if (r==MMSYSERR_NOMEM)
      s="3 MMSYSERR_NOMEM Unable to allocate or lock memory.";
    else if (r==WAVERR_UNPREPARED)
      s="3 WAVERR_UNPREPARED The buffer pointed to by the pwh parameter hasn't been prepared.";
	error_pop_up(s,s);
    return; }

  r=waveInStart(h);
  while (1)
  { MMTIME m;
    m.wType=TIME_BYTES;
    m.u.cb=0;
    r=waveInGetPosition(h, &m, sizeof(m));
    if (m.u.cb>=(unsigned int)length*2) break;
    sleep(100); }

  r=waveInClose(h);
  if (r!=0)
  { std::string s="4 Sound Recording Error";
    if (r==MMSYSERR_INVALHANDLE)
      s="4 MMSYSERR_INVALHANDLE Specified device handle is invalid.";
    else if (r==MMSYSERR_NODRIVER)
      s="4 MMSYSERR_NODRIVER No device driver is present.";
    else if (r==MMSYSERR_NOMEM)
      s="4 MMSYSERR_NOMEM Unable to allocate or lock memory.";
    else if (r==WAVERR_STILLPLAYING)
      s="4 WAVERR_STILLPLAYING There are still buffers in the queue.";
	error_pop_up(s);
    return; } }

void play(Sound &s)
{ s.play(); }

void record(Sound &s)
{ s.record(); }

int number_of_samples(Sound &s)
{ return s.getlength(); }

int samples_per_second(Sound &s)
{ return s.samplerate(); }




