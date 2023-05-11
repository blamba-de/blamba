#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ringtonetools.h"
#include "fileoutput.h"

/*

Ringtone Tools - Copyright 2001-2006 Michael Kohn (mike@mikekohn.net)
This falls under the Kohnian license.  Please read
it at http://ringtonetools.mikekohn.net/

This program is NOT opensourced.  You may not use any part
of this program for your own software.

*/

int get_duration(float duration);

int parse_kws(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
char notes[2048];
int duration[2048];
int file_length,count;
int t,a,b,ch;
int quarter_note=1;

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

  file_length=read_long(in);
  file_length=file_length^0xffffffff;
  count=read_word(in);

  fseek(in,0,SEEK_END);

  if (ftell(in)!=file_length)
  {
    printf("This is not a KWS file.\n");
    return -1;
  }
  fseek(in,6,SEEK_SET);

  if (count>2040)
  {
    printf("This song is too long.\n");
    return -1;
  }

  for (t=0; t<count; t++)
  {
    ch=getc(in);
    notes[t]=ch;
    ch=getc(in);
    if (ch!=4) notes[t]=0;

    a=getc(in);
    b=getc(in);

    a=a^(b<<1);

    duration[t]=(b<<8)+a;

    if (t==0) quarter_note=duration[t];
  }

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
    if (rtt_info->bpm>90) break;
    quarter_note=quarter_note/2;
    rtt_info->bpm=60000/quarter_note;

    t++;
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

    if (t!=count-1 || rtt_info->tone!=0)
    {
      note_route(out,rtt_info);
    }
  }

  footer_route(out,rtt_info);

  return (0);
}



