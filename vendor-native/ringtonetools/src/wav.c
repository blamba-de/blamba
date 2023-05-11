#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ringtonetools.h"
#include "fileoutput.h"

#ifdef DSP
#ifndef WINDOWS
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
int audio_fd;
#else
#include <windows.h>
#include <mmsystem.h>
unsigned long result;
HWAVEOUT      outHandle;
WAVEFORMATEX  waveFormat;
#endif
#endif

#define SAMPLES 700
#define CUTOFF 7
#define WAVES 2
#define ZEROS 1000

/*

Ringtone Tools - Copyright 2001-2006 Michael Kohn (mike@mikekohn.net)
This falls under the Kohnian license.  Please read
it at http://ringtonetools.mikekohn.net/

This program is NOT opensourced.  You may not use any part
of this program for your own software.

*/

int get_duration(float duration);

/* WAV */

void write_wav_header(FILE *out, struct rtt_info_t *rtt_info)
{
int t;

  rtt_info->marker=ftell(out)-4;

  /* Write RIFF header */

  fprintf(out,"RIFF");
  write_long(out,4);
  fprintf(out,"WAVE");

  /* Write fmt header */

  fprintf(out,"fmt ");
  write_long(out,16);
  write_word(out,1);
  write_word(out,1);
  write_long(out,rtt_info->sample_rate);
  write_long(out,rtt_info->sample_rate*rtt_info->bytes);
  if (rtt_info->bytes==1)
  { write_word(out,1); }
    else
  if (rtt_info->bytes==2)
  { write_word(out,2); }
  write_word(out,rtt_info->bytes*8);

  /* Write the start of the data header */

  fprintf(out,"data");
  write_long(out,0);

  rtt_info->samples_per_beat=(double)rtt_info->sample_rate*(60/(double)rtt_info->bpm);
  t=(rtt_info->samples_per_beat)*4;

  rtt_info->wav_buffer=malloc(rtt_info->bytes*(t+(t/2)+(t/4)));
}

void write_wav_note(FILE *out, struct rtt_info_t *rtt_info)
{
unsigned int samples_per_note;
double samples_per_wave;
double waveform;
double waveform_slope;
double actual_freq;
double waveform_x;
/*              P  C         C#       C        D#       E        F */
double freqs[]={ 0, 261.625, 277.175, 293.675, 311.125, 329.625, 349.225, 
                370, 392, 415.3, 440, 466.15, 493.883 };
/*              F#   G    G#     A    A#      B */
int t;
short int sample16;
char sample8;
int style_length;
/* unsigned char buffer[256000]; */
int ptr;


#ifdef DSP
#ifdef WINDOWS
WAVEHDR waveheader;
#else
  if (out==0 && audio_fd==-1) return;
#endif
#endif
  ptr=0;

  if (rtt_info->transpose>0) rtt_info->scale+=rtt_info->transpose;

  /* rtt_info->samples_per_beat=(double)rtt_info->sample_rate*(60/(double)rtt_info->bpm); */

  samples_per_note=(double)rtt_info->samples_per_beat*(double)((double)4/(double)(1<<(rtt_info->length)));

  if (rtt_info->modifier==1)
  { samples_per_note=samples_per_note+(samples_per_note/2); }
    else
  if (rtt_info->modifier==2)
  { samples_per_note=samples_per_note+(samples_per_note/2)+(samples_per_note/4); }

#ifdef DEBUG
printf("Samples per note %d\n",samples_per_note);
#endif

  if (rtt_info->tone==0)
  {
    for (t=0; t<samples_per_note; t++)
    {
      if (rtt_info->bytes==1)
      { rtt_info->wav_buffer[ptr++]=0; }
        else
      { rtt_info->wav_buffer[ptr++]=0; rtt_info->wav_buffer[ptr++]=0; }
    }
  }
    else
  {
    if (rtt_info->tone>12) rtt_info->tone=1;

    actual_freq=freqs[rtt_info->tone];
    actual_freq=actual_freq*(double)(1<<rtt_info->scale);

    samples_per_wave=(double)rtt_info->sample_rate/(double)actual_freq;
    waveform_x=0;

    if (rtt_info->style==0)
    { style_length=samples_per_note-(double)rtt_info->sample_rate/64; }
      else
    if (rtt_info->style==2)
    { style_length=samples_per_note/3; }
      else
    { style_length=samples_per_note; }

    if (rtt_info->bytes==1)
    {
      waveform_slope=255/(double)samples_per_wave;
      waveform=127;

      for (t=0; t<samples_per_note; t++)
      {
        if (t<style_length)
        {
          if (waveform_x>samples_per_wave)
          {
            waveform=127;
            waveform_x=waveform_x-samples_per_wave;
          }

          sample8=waveform;
          /* putc(sample8,out); */
          rtt_info->wav_buffer[ptr++]=sample8;
          waveform=waveform-waveform_slope;
          if (waveform<-127) waveform=-127;
          waveform_x=waveform_x+1;
        }
          else
        { rtt_info->wav_buffer[ptr++]=0; }
      }
    }
      else
    if (rtt_info->bytes==2)
    {
      waveform_slope=65534/(double)samples_per_wave;
      waveform=32767;

      for (t=0; t<samples_per_note; t++)
      {
        if (t<style_length)
        {
          if (waveform_x>samples_per_wave)
          {
            waveform=32767;
            waveform_x=waveform_x-samples_per_wave;
          }

          sample16=waveform;
          /* write_word(out,sample16); */
          rtt_info->wav_buffer[ptr++]=(sample16&255);
          rtt_info->wav_buffer[ptr++]=(sample16>>8);
          waveform=waveform-waveform_slope;
          if (waveform<-32767) waveform=-32767;
          waveform_x=waveform_x+1;
        }
          else
        { rtt_info->wav_buffer[ptr++]=0; rtt_info->wav_buffer[ptr++]=0; }
      }
    } 
  }

#ifdef DSP
#ifndef WINDOWS

  if (out==0) { write(audio_fd,rtt_info->wav_buffer,ptr); }
    else
#else
  if (out==0)
  {
    waveheader.lpData=rtt_info->wav_buffer;
    waveheader.dwBufferLength=ptr;
    waveheader.dwFlags=0;
    waveOutPrepareHeader(outHandle,&waveheader,sizeof(WAVEHDR));
    t=waveOutWrite(outHandle,&waveheader,sizeof(WAVEHDR));

    while(!(waveheader.dwFlags & WHDR_DONE));

    MMTIME mmtime;
    mmtime.wType=TIME_BYTES;

    waveOutUnprepareHeader(outHandle,&waveheader,sizeof(WAVEHDR));
  }
    else
#endif
#endif
  {
    fwrite(rtt_info->wav_buffer,1,ptr,out);
  }
}


void write_wav_footer(FILE *out, struct rtt_info_t *rtt_info)
{
unsigned int data_length,t;

  t=ftell(out);
  data_length=t-44;
  fseek(out,40,0);
  write_long(out,data_length);
  fseek(out,4,0);
  write_long(out,t-8);

  if (rtt_info->wav_buffer!=0)
  {
    free(rtt_info->wav_buffer);
    rtt_info->wav_buffer=0;
  }
}


#ifdef DSP

void write_dsp_header(FILE *out, struct rtt_info_t *rtt_info)
{
#ifndef WINDOWS
int format;

  rtt_info->bytes=2;
  rtt_info->sample_rate=44100; 

  audio_fd=open(rtt_info->outname, O_WRONLY, 0);
  if (audio_fd==-1)
  {
    printf("Problem opening %s\n",rtt_info->outname);
    return;
  }

  format=AFMT_S16_LE;
  if (ioctl(audio_fd, SNDCTL_DSP_SETFMT, &format)==-1)
  { goto dsperror; }

  if (format!=AFMT_S16_LE)
  {
    rtt_info->bytes=1;

    format=AFMT_S8;
    if (ioctl(audio_fd, SNDCTL_DSP_SETFMT, &format)==-1)
    {
      goto dsperror;
    }
    if (format!=AFMT_S8) goto dsperror;
  }

  format=0;
  if (ioctl(audio_fd, SNDCTL_DSP_STEREO, &format)==-1)
  {
    goto dsperror;
  }
  if (format==-1) goto dsperror;

  format=44100;
  if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &format)==-1)
  {
    goto dsperror;
  }

  if (format!=44100)
  {
    format=11025;
    if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &format)==-1) goto dsperror;
    if (format!=11025)
    {
      format=8000;
      if (ioctl(audio_fd, SNDCTL_DSP_SPEED, &format)==-1) goto dsperror;
      if (format!=8000) goto dsperror;
    }
  }

  rtt_info->sample_rate=format;

#ifdef DEBUG
printf("%d\n",format);
#endif

  /* out=fdopen(audio_fd,"w"); */
  /* if (out==0) goto dsperror; */

  return;

dsperror:
  printf("Error: Problem setting up DSP device.\n");
  close(audio_fd);
  audio_fd=-1;
  out=0;
  return;
#else

  waveFormat.wFormatTag = WAVE_FORMAT_PCM;
  waveFormat.nChannels = 1;
  waveFormat.nSamplesPerSec = 44100;
  waveFormat.wBitsPerSample = 16;
  waveFormat.nBlockAlign = waveFormat.nChannels * (waveFormat.wBitsPerSample/8);
  waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
  waveFormat.cbSize = 0;

  /* int result = waveOutOpen(&outHandle, WAVE_MAPPER, &waveFormat, (DWORD)myWindow, 0, CALLBACK_WINDOW); */
  /* int result = waveOutOpen(&outHandle, WAVE_MAPPER, &waveFormat, (DWORD)NULL, 0, CALLBACK_WINDOW); */
  int result = waveOutOpen(&outHandle, WAVE_MAPPER, &waveFormat, (DWORD)NULL, 0,CALLBACK_NULL  );

  if (result)
  {
     printf("Error: Problem setting up sound device.\r\n");
     waveFormat.nChannels=0;
     return;
  }

  rtt_info->bytes=2;
  rtt_info->sample_rate=44100; 

#endif
}

void write_dsp_note(FILE *out, struct rtt_info_t *rtt_info)
{
#ifndef WINDOWS
  if (audio_fd==-1) return;
#endif

  write_wav_note(out,rtt_info);
}

void write_dsp_footer(FILE *out, struct rtt_info_t *rtt_info)
{
#ifndef WINDOWS
  if (ioctl(audio_fd, SNDCTL_DSP_SYNC, 0)==-1) {}
  close(audio_fd);
#else
  /* waveOutUnprepareHeader(outHandle,&waveheader,sizeof(WAVEHDR)); */
#endif

}

#endif

int get_note_from_freq(float freq, struct rtt_info_t *rtt_info)
{
double freqs[13];
/*                 P  C        C#       C        D#       E        F */
double freqs_t[]={ 0, 261.625, 277.175, 293.675, 311.125, 329.625, 349.225,
                370, 392, 415.3, 440, 466.15, 493.883 };
/*              F#   G    G#     A    A#      B */
int t,m;

  if (freq<freqs_t[1]-5) return 0;

  m=0;
  if (freq>freqs_t[12]+12) m=1;
  if (freq>(freqs_t[12]*2)+24) m=2;

  for(t=0; t<12; t++)
  { freqs[t]=freqs_t[t+1]*(1<<m); }

  if (freq<freqs[0]) freq=freqs[0];

  for (t=0; t<12; t++)
  {
    if (freqs[t]==freq) return (t+(m*12))+1;

    if (freqs[t]<freq && freq<freqs[t+1])
    {
      if (freq-freqs[t]<freqs[t+1]-freq)
      { return (t+(m*12))+1; }
        else
      { return (t+(m*12))+2; }
    }
  }

  return t;
}

int parse_wav(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
char notes[2048];
char name[5],riff_type[5];
int duration[2048],sample_rate,channels,bytes;
int count,quarter_note,curr_note,last_note;
short int curr_sample;
int waves,samples,zeros;
int peak=1,length,t,a,ch,zero;
int datalength;
double freq;

  read_chars(in,name,4);
  t=read_long(in);
  read_chars(in,riff_type,4);

  zero=0;

  if (strcasecmp(name,"RIFF")!=0 || strcasecmp(riff_type,"wave")!=0)
  {
    printf("Invalid WAV format.\n");
    return -1;
  }

  read_chars(in,name,4);
  read_long(in);
  read_word(in);
  channels=read_word(in);
  sample_rate=read_long(in);
  read_long(in);
  bytes=read_word(in);
  read_word(in);

  read_chars(in,name,4);
  datalength=read_long(in);

  curr_sample=0;
  samples=0;
  waves=0;
  zeros=0;
  t=0;
  count=0;
  last_note=-1;
  length=0;

#ifdef DEBUG
printf("going to read %d bytes\n",datalength);
printf("bytes_per_sample=%d   channels=%d\n",bytes,channels);
#endif

  while (t<datalength)
  {
    if (bytes==1)
    {
      ch=getc(in);
      curr_sample=ch*65535/255;
    }
      else
    if (bytes==2 && channels==2)
    {
      ch=getc(in)+getc(in);
      ch=ch/2;
      curr_sample=ch*65535/255;
    }
      else
    if (bytes==2 && channels==1)
    {
      curr_sample=read_word(in);
    }
      else
    if (bytes==4 && channels==2)
    {
      read_word(in);
      curr_sample=(short int)read_word(in);
    }

#ifdef DEBUG
/* printf("curr_sample=%d   t=%d\n",curr_sample,t); */
#endif

    t=t+rtt_info->bytes;

    if (curr_sample<CUTOFF && curr_sample>-CUTOFF)
    { curr_sample=0; }

    if (curr_sample==0)
    {
      zero++;
      if (zero>=ZEROS)
      {
        if (count!=0)
        {
          notes[count]=0;
          duration[count]=((double)zeros/(double)sample_rate)*1000;
          count++;
        }
        zero=0;
        samples=0;
        waves=0;
      }
    }
      else
    {
      zero=0;

      if (curr_sample<0 && peak==1)
      {
        peak=-1;
      }
        else
      if (curr_sample>0 && peak==-1)
      {
        peak=1;
        waves++;
      }
    }

    samples++;

    /* if (samples>=SAMPLES) */
    if (waves>=WAVES)
    {
      freq=(double)waves/((double)samples/(double)sample_rate);
      curr_note=get_note_from_freq(freq,rtt_info);

#ifdef DEBUG
printf("waves: %d  freq: %f    curr_note: %d\n",waves,freq,curr_note);
#endif

      if (curr_note!=last_note)
      {
        if (last_note!=-1)
        {
          if (!(count==0 && last_note==0))
          {
            notes[count]=last_note;
            /* duration[count]=((double)length/(double)SAMPLES)*1000; */
            duration[count]=((double)length/(double)sample_rate)*1000;
#ifdef DEBUG
printf("Note %d  Dur %d  sam %d\n",last_note,duration[count],samples);
#endif
            if (duration[count]>10)
            { count++; }
          }
          length=0;
        }
      }

      length=length+samples;

      last_note=curr_note;
      waves=0;
      samples=0;
    }
  }


  if (count==0)
  {
    printf("No notes were processed.  Aborting!\n");
    return -1;
  }

#ifdef DEBUG
printf("count: %d\n",count);
#endif

  quarter_note=duration[0];

  t=0;
  while(t<8)
  {
    rtt_info->bpm=60000/quarter_note;
    if (rtt_info->bpm<250) break;

    quarter_note=quarter_note*2;
    t++;
  }

  t=0;
  while(t<8)
  {
    if (rtt_info->bpm>100) break;
    quarter_note=quarter_note/2;
    rtt_info->bpm=60000/quarter_note;

    t++;
  }

#ifdef DEBUG
printf("Quarter note: %d ms\n",quarter_note);
#endif

  t=0;
  if (rtt_info->songname[0]==0)
  {
    while(rtt_info->inname[t]!='.' && rtt_info->inname[t]!=0 && t<14)
    {
      rtt_info->songname[t]=rtt_info->inname[t];
      t++;
    }
    rtt_info->songname[t]=0;
  }

  header_route(out,rtt_info);

  for (t=0; t<count; t++)
  {
    if (notes[t]==0)
    {
      rtt_info->tone=0;
    }
      else
    {
      rtt_info->tone=(notes[t]-1)%12+1;
      rtt_info->scale=(notes[t]-1)/12;
    }

    a=get_duration((float)duration[t]/((float)quarter_note*4));
    rtt_info->length=a/2;
    rtt_info->modifier=(a%2)^1;

    if (rtt_info->length>5) continue;

    note_route(out,rtt_info);
  }

  footer_route(out,rtt_info);

  return (0);
}

