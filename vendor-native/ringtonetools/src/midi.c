#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ringtonetools.h"
#include "fileoutput.h"

/*

Ringtone Tools - Copyright 2001-2006 Michael Kohn (mike@mikekohn.net)
This falls under the Kohnian license.  Please read
it at http://ringtonetools.mikekohn.net/

This program is NOT opensourced.  You may not use any part
of this program for your own software.

*/

/* MIDI */

#define DIVISIONS 240

void write_var(FILE *out, int i)
{
int t,k;

  t=7;
  while((i>>t)!=0)
  {
    t=t+7;
  }

  t=t-7;
  for (k=t; k>=0; k=k-7)
  {
    if (k!=0)
    { putc(((i>>k)&127)+128,out); }
      else
    { putc(((i>>k)&127),out); }
  }
}

void write_midi_header(FILE *out, struct rtt_info_t *rtt_info)
{
char *i={ "Created by Ringtone Tools (http://ringtonetools.mikekohn.net/)." };
int d;

  fprintf(out,"MThd");
  write_long_b(out,6);
  write_word_b(out,0);
  write_word_b(out,1);
  write_word_b(out,DIVISIONS);

  fprintf(out,"MTrk");
  rtt_info->marker=ftell(out);
  write_long_b(out,0);

  if (rtt_info->songname[0]!=0)
  {
    write_var(out,0);
    putc(0xff,out);
    putc(0x03,out);
    write_var(out,strlen(rtt_info->songname));
    fprintf(out,"%s",rtt_info->songname);
  }

  write_var(out,0);
  putc(0xff,out);
  putc(0x01,out);
  write_var(out,strlen(i));
  fprintf(out,"%s",i);

  write_var(out,0);
  putc(0xff,out);
  putc(0x51,out);
  putc(0x03,out);
  d=60000000/rtt_info->bpm;
  putc(d>>16,out);
  putc((d>>8)&255,out);
  putc(d&255,out);
}

void write_midi_note(FILE *out, struct rtt_info_t *rtt_info)
{
int d,i;

  d=(int)((float)DIVISIONS*((float)4/(float)(1<<rtt_info->length)));

  if (rtt_info->modifier==1) d=d+(d/2);
    else
  if (rtt_info->modifier==2) d=d+(d/2)+(d/4);

  write_var(out,0);
  putc(0x90,out);
  if (rtt_info->tone==0)
  { putc(0,out); }
    else
  { putc(60+(rtt_info->tone-1)+(rtt_info->scale*12),out); }
  putc(64,out);

  write_var(out,d);
  putc(0x80,out);
  if (rtt_info->tone==0)
  {
    putc(0,out);
    putc(0,out);
  }
    else
  {
    putc(60+(rtt_info->tone-1)+(rtt_info->scale*12),out);
    i=(int)((float)255*((float)rtt_info->volume/(float)14));
    if (i>255) i=255;
    putc(i,out);
  }
}

void write_midi_footer(FILE *out, struct rtt_info_t *rtt_info)
{
int i;

  write_var(out,0);
  putc(0xff,out);
  putc(0x2f,out);
  putc(0x00,out);

  i=ftell(out);
  fseek(out,rtt_info->marker,0);
  write_long_b(out,(i-rtt_info->marker)-4);
  fseek(out,i,0);
}

void write_midi_bpm(FILE *out, struct rtt_info_t *rtt_info)
{
int d;

  write_var(out,0);
  putc(0xff,out);
  putc(0x51,out);
  putc(0x03,out);
  d=60000000/rtt_info->bpm;
  putc(d>>16,out);
  putc((d>>8)&255,out);
  putc(d&255,out);
}

