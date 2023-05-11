#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ringtonetools.h"
#include "fileoutput.h"

#define MAX_EMS_LEN 2048

/*

Ringtone Tools - Copyright 2001-2006 Michael Kohn (mike@mikekohn.net)
This falls under the Kohnian license.  Please read
it at http://ringtonetools.mikekohn.net/

This program is NOT opensourced.  You may not use any part
of this program for your own software.

*/

int parse_emelody(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
char song[MAX_EMS_LEN];
int t,l,ptr,ch;
int scale;
int length=0,tone=0,octave,modifier;
int tone_modifier;
int default_octave;

  scale=-1;
  default_octave=0;
  rtt_info->bpm=120;

  check_songname(in,rtt_info->songname);

  ptr=0;
  while ((ch=getc(in))!=EOF)
  {
    song[ptr++]=ch;
    if (ptr>MAX_EMS_LEN-8) break;
  }

  song[ptr]=0;

#ifdef DEBUG
  printf("\n       Song Name: %s\n",rtt_info->songname);
  printf("Beats Per Minute: %d\n",rtt_info->bpm);
#endif

  header_route(out,rtt_info);

  for (t=0; t<ptr; t++)
  {
    if (song[t]=='r') song[t]='p';
    if (song[t]=='R') song[t]='P';
  }

  l=0; 
  while (l<ptr)
  {
    if (song[l]==' ' || song[l]=='\t' || song[l]=='\r' || song[l]=='\n')
    { l++; continue; }

    if (song[l]=='@' && rtt_info->ats==0)
    { l++; continue; }

    if (song[l]=='*')
    {
      l++;
      default_octave=(song[l++]-'0')-3;
    }

    if (song[l]=='+' && song[l+1]=='+') { octave=2+default_octave; l=l+2; }
      else
    if (song[l]=='+') { octave=1+default_octave; l++; }
      else
    { octave=default_octave; }

    tone_modifier=0;

    if (song[l]=='#')
    {
      tone_modifier=tone_modifier+1;
      l++;
    }

    if (song[l]=='&' || song[l]=='@')
    {
      tone_modifier=tone_modifier-1;
      l++;
    }

    if (song[l]=='(' && song[l+1]=='b' && song[l+2]==')')
    {
      tone_modifier=tone_modifier-1;
      l=l+3;
    }

    if (tolower(song[l])=='a') tone=10; 
      else
    if (tolower(song[l])=='h' || tolower(song[l])=='b') tone=12;
      else
    if (tolower(song[l])=='c') tone=1;
      else
    if (tolower(song[l])=='d') tone=3;
      else
    if (tolower(song[l])=='e') tone=5;
      else
    if (tolower(song[l])=='f') tone=6;
      else
    if (tolower(song[l])=='g') tone=8;
      else
    if (tolower(song[l])=='p') tone=0;

    if (song[l]>='a' && song[l]<='z') length=3;
      else
    if (song[l]>='A' && song[l]<='Z') length=2;

    if (song[l]=='p')
    {
      if (song[l+1]=='p' && song[l+2]=='p' && song[l+3]=='p')
      { length=1; l=l+4; }
        else
      if (song[l+1]=='p')
      { length=2; l=l+2; }
        else
      { l++; }
    }
      else
    { l++; }

    if (song[l]=='.')
    {
      modifier=1;
      l++;
    }
      else
    if (song[l]==':')
    {
      modifier=2;
      l++;
    }

    if (song[l]>='0' && song[l]<='3')
    {
      if (song[l]=='0') 
      { modifier=1; length=2; } 
        else
      if (song[l]=='1') 
      { modifier=0; length=2; } 
        else
      if (song[l]=='2') 
      { modifier=1; length=3; } 
        else
      if (song[l]=='3') 
      { modifier=0; length=3; } 

      l++;
    }

    tone=tone+tone_modifier;

    if (tone>12)
    {
      tone=tone-12;
      octave++;
    }

    if (scale!=octave)
    {
      scale=octave;
    }

    modifier=0;

    if (song[l]=='.')
    {
      modifier=1;
      l++;
    }

    rtt_info->tone=tone;
    rtt_info->length=length;
    rtt_info->modifier=modifier;
    rtt_info->scale=scale;

    note_route(out,rtt_info);
  }

  footer_route(out,rtt_info);

  return (0);
}



