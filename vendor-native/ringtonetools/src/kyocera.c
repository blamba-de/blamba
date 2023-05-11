#include <stdio.h>
#include <stdlib.h>

#include "ringtonetools.h"
#include "fileoutput.h"

/*

Ringtone Tools - Copyright 2001-2006 Michael Kohn (mike@mikekohn.net)
This falls under the Kohnian license.  Please read
it at http://ringtonetools.mikekohn.net/

This program is NOT opensourced.  You may not use any part
of this program for your own software.

*/

/* Kyocera KWS */

void write_kws_header(FILE *out, struct rtt_info_t *rtt_info)
{

}

void write_kws_note(FILE *out, struct rtt_info_t *rtt_info)
{
int d,a,b;

  rtt_info->ring_stack[rtt_info->stackptr++]=rtt_info->tone+(rtt_info->scale*12);

  if (rtt_info->tone!=0)
  { rtt_info->ring_stack[rtt_info->stackptr++]=0x04; }
    else
  { rtt_info->ring_stack[rtt_info->stackptr++]=0x00; }

  d=(int)(((float)60000/(float)rtt_info->bpm)*(float)((float)4/(float)(1<<rtt_info->length)));

  if (rtt_info->modifier==1)
  { d=d+(d/2); }
    else
  if (rtt_info->modifier==2)
  { d=d+(d/2)+(d/4); }

  a=d&255;
  b=(d>>8)&255;

  a=a^(b<<1);

  rtt_info->ring_stack[rtt_info->stackptr++]=a;
  rtt_info->ring_stack[rtt_info->stackptr++]=b;
}

void write_kws_footer(FILE *out, struct rtt_info_t *rtt_info)
{
int data_length;
int a,b,d;

  if (rtt_info->pause!=0)
  {
    rtt_info->ring_stack[rtt_info->stackptr++]=0;
    rtt_info->ring_stack[rtt_info->stackptr++]=0;

    d=(int)(((float)60000/(float)rtt_info->bpm)*(float)((float)4/(float)(1<<2)));
    d=d*rtt_info->pause;

    a=d&255;
    b=(d>>8)&255;

    a=a^(b<<1);

    rtt_info->ring_stack[rtt_info->stackptr++]=a;
    rtt_info->ring_stack[rtt_info->stackptr++]=b;

    rtt_info->note_count++;
  }

  data_length=rtt_info->stackptr+10;
  write_long(out,(data_length^0xffffffff));
  write_word(out,rtt_info->note_count);

  for (d=0; d<rtt_info->stackptr; d++) putc(rtt_info->ring_stack[d],out);

  write_long(out,(data_length^0xffffffff));
}



