#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "ringtonetools.h"
#include "fileoutput.h"

#define RECORD_SIZE 60000

/*

Ringtone Tools - Copyright 2001-2006 Michael Kohn (mike@mikekohn.net)
This falls under the Kohnian license.  Please read
it at http://ringtonetools.mikekohn.net/

This program is NOT opensourced.  You may not use any part
of this program for your own software.

*/

int wav2pdb(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
char name[32],riff_type[5];
int ch,ch1,t,l,r;
int channels,sample_rate,bytes_per_sample;
int data_length,records;

  read_chars(in,name,4);
  l=read_long(in);
  read_chars(in,riff_type,4);

  if (strcasecmp(name,"RIFF")!=0 || strcasecmp(riff_type,"wave")!=0)
  {
    printf("Invalid WAV file: %s\n",rtt_info->inname);
    return 1;
  }

  if (rtt_info->songname[0]==0)
  { strcpy(name,"Ringtone Tools"); }
    else
  { strncpy(name,rtt_info->songname,32); }

  l=strlen(name); 
  if (l>31) l=31;

  for (t=0; t<l; t++)
  { putc(name[t],out); }
  for (r=t; r<32; r++)
  { putc(0,out); }

  write_word_b(out,0x18);
  write_word_b(out,0x1);

  write_long_b(out,time(NULL));
  write_long_b(out,time(NULL));

  write_long_b(out,0);

  write_long_b(out,0);
  write_long_b(out,0);
  write_long_b(out,0);

  putc('r',out);
  putc('i',out);
  putc('n',out);
  putc('g',out);

  putc('Q',out);
  putc('C',out);
  putc('B',out);
  putc('A',out);

  write_long_b(out,0);
  write_long_b(out,0);

  read_chars(in,name,4);
  read_long(in);
  read_word(in);
  channels=read_word(in);
  sample_rate=read_long(in);
  read_long(in);
  bytes_per_sample=read_word(in);
  read_word(in);

  read_chars(in,name,4);
  data_length=read_long(in);
  data_length=data_length+6;

  r=ftell(in);
  fseek(in,0,SEEK_END);
  l=ftell(in);
  fseek(in,r,SEEK_SET);

  if ((l-r)<data_length) data_length=(l-r);

  l=data_length/bytes_per_sample;

  records=(l+12)/RECORD_SIZE;

  if ((l%RECORD_SIZE)!=0) records++;

  r=78+(records*8);

  write_word_b(out,records);

  for (t=0; t<records; t++)
  {
    write_long_b(out,r);
    putc(0,out);
    putc(0,out);
    putc(0,out);
    putc(0,out);
    r=r+RECORD_SIZE;
  }

  if (channels==2)
  {
    if (quiet==0) printf("Warning:  This WAV is stereo.  Mixing channels.\n");
  }

  if ((bytes_per_sample==2 && channels==2) || bytes_per_sample==4)
  {
    if (quiet==0) printf("Warning:  This WAV is 16 bit.  Downsampling to 8 bit.\n");
  }

  if (sample_rate!=11025 && sample_rate!=16000)
  {
    if (quiet==0)
    {
      printf("\nWarning: This WAV is sampled at %d Hz.  The Kyocera 6035 plays back\n",sample_rate);
      printf("either 11025Hz or 16000Hz.  Your ringtone could sound awkward.\n");
      printf("You should resample this WAV with a program such as SOX\n");
      printf("before using Ringtonetools on it.\n\n");
    }
  }

  write_long_b(out,1);
  write_word_b(out,records-1);

  write_word_b(out,l);
  write_word_b(out,0);
  write_word_b(out,sample_rate);

  for (t=0; t<l; t++)
  {
    if (bytes_per_sample==1)
    {
      ch=getc(in);
      putc(ch,out);
    }
      else
    if (bytes_per_sample==2 && channels==2)
    {
      ch=getc(in)+getc(in);
      ch=ch/2;
      putc(ch,out);
    }
      else
    if (bytes_per_sample==2 && channels==1)
    {
      ch=read_word(in);
      ch=(int)((float)ch*(float)((float)255/(float)65535));
      putc(ch,out);
    }
      else
    if (bytes_per_sample==4 && channels==2)
    {
      ch=read_word(in);
      ch=(int)((float)ch*(float)((float)255/(float)65535));

      ch1=read_word(in);
      ch1=(int)((float)ch1*(float)((float)255/(float)65535));

      ch=(ch+ch1)/2;

      putc(ch,out);
    }
      else
   { printf("unknown format\n"); return -1; }
  }

  return 0;
}



