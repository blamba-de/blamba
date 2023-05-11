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

int default_channel,default_track;
int header_flag;
int current_note;
int running_status=0x90;

struct _header_chunk
{
  int header_length;
  int format;
  int tracks;
  short int division;
};

struct _meta_events
{
  int sequence_number;
  int tempo;
  char smpte_offset[6];
  char time_signature[5];
  char key_signature[3];
  int quarter_note;
};

int read_string(FILE *in, char *s, int n)
{
int t,ch;

  for(t=0; t<n; t++)
  {
    ch=getc(in);
    if (ch==EOF) return -1;

    s[t]=ch;
  }

  s[t]=0;
  return 0;
}

int print_string(FILE *in, int n)
{
int t,ch;

  for(t=0; t<n; t++)
  {
    ch=getc(in);
    if (ch==EOF) return -1;

#ifdef DEBUG
    printf("%c",ch);
#endif
  }

#ifdef DEBUG
  printf("\n");
#endif

  return 0;
}

int read_count(FILE *in, int n)
{
int t,s;

  t=0;

  for (s=0; s<n; s++)
  { t=(t<<8)+getc(in); }

  return t;
}

int read_var(FILE *in)
{
int t,ch;

  t=0;

  while(1)
  {
    ch=getc(in);

    if (ch==EOF) return -1;

    t=(t<<7)+(ch&127);
    if ((ch&128)==0) break;
  }

  return t;
}

int MThd_parse(FILE *in, struct _header_chunk *header_chunk, int n)
{
  header_chunk->header_length=n;
  header_chunk->format=read_word_b(in);
  header_chunk->tracks=read_word_b(in);
  header_chunk->division=read_word_b(in);

  return 0;
}

int parse_meta_event(FILE *in, struct _meta_events *meta_events, struct _header_chunk *header_chunk)
{
int ch,v_length,t;

  ch=getc(in);
  v_length=read_var(in);

  /* printf("(%d) ",ch); */

  if (ch==0x00)
  { 
    printf("Sequence Number: "); 
    t=read_count(in,v_length);
    meta_events->sequence_number=t;
    printf("%d\n",t);
    return 0;
  }
    else
  if (ch==0x01)
  { 
#ifdef DEBUG
    printf("Text Event: "); 
#endif
    print_string(in, v_length);
    return 0;
  }
    else
  if (ch==0x02)
  { 
#ifdef DEBUG
    printf("Copyright Notice: "); 
#endif
    print_string(in, v_length);
    return 0;
  }
    else
  if (ch==0x03)
  { 
#ifdef DEBUG
    printf("Track Name: "); 
#endif
    print_string(in, v_length);
    return 0;
  }
    else
  if (ch==0x04)
  { 
#ifdef DEBUG
    printf("Instrument Name: "); 
#endif
    print_string(in, v_length);
    return 0;
  }
    else
  if (ch==0x05)
  { 
#ifdef DEBUG
    printf("Lyric Text: "); 
#endif
    print_string(in, v_length);
    return 0;
  }
    else
  if (ch==0x06)
  { 
#ifdef DEBUG
    printf("Marker Text: "); 
#endif
    print_string(in, v_length);
    return 0;
  }
    else
  if (ch==0x07)
  {
#ifdef DEBUG
    printf("Cue Point: ");
#endif
  }
    else
  if (ch==0x20)
  {
#ifdef DEBUG
    printf("MIDI Channel Prefix: ");
#endif
  }
    else
  if (ch==0x2f)
  { 
#ifdef DEBUG
    printf("End Of Track\n"); 
#endif
    return -1;
  }
    else
  if (ch==0x51)
  { 
    t=read_count(in,v_length);
    meta_events->tempo=t;
    meta_events->quarter_note=meta_events->tempo/header_chunk->division;
#ifdef DEBUG
    printf("Tempo Setting: "); 
    printf("%d\n",t);
#endif
    return 0;
  }
    else
  if (ch==0x54)
  { 
#ifdef DEBUG
    printf("SMPTE Offset: ");
#endif
    read_string(in,meta_events->smpte_offset,5);
    for (t=0; t<5; t++)
    {
#ifdef DEBUG
      printf("%d ",meta_events->smpte_offset[t]);
#endif
    }
#ifdef DEBUG
    printf("\n");
#endif
    return 0;
  }
    else
  if (ch==0x58)
  {
#ifdef DEBUG
    printf("Time Signature: ");
#endif
    read_string(in,meta_events->time_signature,4);
    for (t=0; t<4; t++)
    {
#ifdef DEBUG
      printf("%d ",meta_events->time_signature[t]);
#endif
    }
#ifdef DEBUG
    printf("\n");
#endif
    return 0;
  }
    else
  if (ch==0x59)
  {
#ifdef DEBUG
    printf("Key Signature: ");
#endif
    read_string(in,meta_events->key_signature,2);
    for (t=0; t<2; t++)
    {
#ifdef DEBUG
      printf("%d ",meta_events->key_signature[t]);
#endif
    }
#ifdef DEBUG
    printf("\n");
#endif
    return 0;
  }
    else
  if (ch==0x7F)
  {
#ifdef DEBUG
    printf("Sequencer Specific Event: ");
#endif
  }
    else
  {
#ifdef DEBUG
    printf("\nUnknown Event: ");
#endif
  }

  for (t=0; t<v_length; t++)
  {
    ch=getc(in);
#ifdef DEBUG
    printf("%c ",ch);
#endif
  }

#ifdef DEBUG
  printf("\n");
#endif

  return 0;
}

int parse_extras(FILE *in, int channel)
{
  if (channel==1)
  { getc(in); }
    else
  if (channel==2)
  { getc(in); getc(in); }
    else
  if (channel==3)
  { getc(in); }

  return 0;
}

int get_duration(float duration)
{
int t;
float duration_code[16]={ 1.5,1,        0.75,0.5,   0.37,0.25,
                        0.1875,0.125,  0.09375,0.0625,
                        0.046875,0.03125,  0 };

  if (duration>1.5) duration=1.5;

  for (t=0; t<13; t++)
  {
    if (duration_code[t]==duration) return t;

    if (duration_code[t]>duration && duration>duration_code[t+1])
    {
      if (duration_code[t]-duration<duration-duration_code[t+1])
      { return t; }
        else
      { return t+1; }
    }
  }

  return t-1;
}

int add_note(FILE *out, int tone, int v_time, struct rtt_info_t *rtt_info, struct _header_chunk *header_chunk)
{
int t,k,d;
float m;

#ifdef DEBUG
printf("In add note.. v_time=%d\n",v_time);
#endif

  if (v_time==0) return 0;

  m=(float)v_time/(float)(header_chunk->division*4);
  d=get_duration(m);

#ifdef DEBUG
if (d==12) printf("cancelling... note too short\n");
#endif

  if (d==12) return 0;

  k=d/2;
  t=1<<k;

  d=(d%2)^1;

  rtt_info->length=k;

#ifdef DEBUG
printf("> %d  %d < ",t,d%2);
#endif

  if (tone==0)
  {
    rtt_info->tone=0;
  }
    else
  {
    t=tone-60+(13*(rtt_info->transpose));

    if (t<0)
    {
      printf("Error: This song uses notes lower than middle C.\n");
      printf("Please use the -transpose option to raise the note to a higher scale.\n");
      t=0;
    }

    rtt_info->tone=t%12+1;
    rtt_info->scale=t/12;

    /* if (rtt_info->tone>12) { rtt_info->tone=rtt_info->tone-12; rtt_info->scale++; } */

#ifdef DEBUG
printf("***** TONE %d  ****** SCALE %d\n",rtt_info->tone,rtt_info->scale);
#endif
  }

  if (d==1)
  { rtt_info->modifier=1; }
    else
  { rtt_info->modifier=0; }

  note_route(out,rtt_info);

  return 0;
}

int midi_command(FILE *in, FILE *out, int n, struct rtt_info_t *rtt_info, struct _meta_events *meta_events, int v_time, struct _header_chunk *header_chunk)
{
int command,channel;
int ch,tone;
float duration;

  if (n<0x80)
  {
    ungetc(n,in);
    n=running_status;
  }

  command=n&0xf0;
  channel=n&0x0f;

  /* if (command==0x90 && v_time==0) command=0x80; */

  if (command==0x80)
  {
    /* Sound Off */
    tone=getc(in);
    ch=getc(in);

    if (v_time!=0)
    {
      duration=(float)v_time/((float)meta_events->quarter_note);
#ifdef DEBUG
printf("Sound Off [ %f %d %d ] ",duration,meta_events->quarter_note,v_time);
#endif
    }
      else
    { duration=0; }

    if (tone!=current_note)
    {
#ifdef DEBUG
      printf("Warning: Note to stop and previous note don't match.\n");
#endif
    }
      else
    {
      if (default_channel==-1 || channel==default_channel)
      { add_note(out, tone, v_time, rtt_info, header_chunk); }
    }
#ifdef DEBUG
printf("tone: %d %f\n",tone,duration);
#endif
  }
    else
  if (command==0x90)
  {
    /* Sound On */
    tone=getc(in);
    ch=getc(in);

    if (v_time!=0)
    {
      duration=(float)v_time/((float)meta_events->quarter_note);
#ifdef DEBUG
printf("Sound On [ %f %d %d ] ",duration,meta_events->quarter_note,v_time);
#endif
    }
      else
    { duration=0; }

    current_note=tone;

    if (default_channel==-1 || channel==default_channel)
    { if (v_time!=0) add_note(out, tone, v_time, rtt_info, header_chunk); }

#ifdef DEBUG
printf("tone: %d %f\n",tone,duration);
#endif
  }
    else
  if (command==0xa0)
  {
    /* Aftertouch */
    ch=getc(in);
    ch=getc(in);
  }
    else
  if (command==0xb0)
  {
    /* Controller */
    ch=getc(in);
    ch=getc(in);
  }
    else
  if (command==0xc0)
  {
    /* Program Change */
    ch=getc(in);
  }
    else
  if (command==0xd0)
  {
    /* Channel Pressure */
    ch=getc(in);
  }
    else
  if (command==0xe0)
  {
    /* Pitch Wheel */
    ch=getc(in);
    ch=getc(in);
  }
    else
  if (command==0xf0)
  {
    parse_extras(in,channel);
  }
/*
    else
  {
    printf("Unknown MIDI code.  Please email Michael Kohn (mike@mikekohn.net)\n");
    printf("Code: ");
    print_hex(n);
    printf("\n"); 
  }
*/

  running_status=n;

  return 0;
}

int MTrk_parse(FILE *in, FILE *out, struct _header_chunk *header_chunk, int n, struct _meta_events *meta_events, struct rtt_info_t *rtt_info)
{
int ch;
int v_time;
int t;
#ifdef DEBUG
int x=0;
#endif

#ifdef DEBUG
  printf("***Length: %d\n",n);
#endif

  while (1)
  {
    v_time=read_var(in);

    ch=getc(in);

#ifdef DEBUG
    printf("\nMTrk_parse read in: %x ",ch);
#endif

    if (ch==EOF)
    {
      printf("EOF found... oops.\n");
      return 0;
    }

    if (ch==0xff)
    {
      /* META Event */

#ifdef DEBUG
      printf("META Event\n");
      if (x!=0) { x=0; printf("\n"); }
#endif
      if (parse_meta_event(in,meta_events,header_chunk)==-1) break;
    }
      else
    if (ch==0xf0 || ch==0xf7)
    {
      /* System Exclusive Event */

#ifdef DEBUG
printf("System Exclusive 0x%x v_time=%d\n",ch,v_time);

      if (x!=0) { x=0; printf("\n"); }
#endif
      for (t=0; t<v_time; t++)
      {
        ch=getc(in);
        printf("%c(0x%x)",ch,ch);
        if (ch==0xf7) break;
        if (ch==EOF) { printf("Premature end of file\n"); return -1; }
      }
    }
      else
    {
      /* MIDI DATA */

      if (header_flag==0)
      {
        rtt_info->bpm=60000000/meta_events->tempo;

        header_route(out,rtt_info);

        header_flag=1;
      }

#ifdef DEBUG
      printf("(%d) %02X ",v_time,ch);
/*
      print_hex(ch);
      printf(" ");
*/
#endif

      midi_command(in,out,ch,rtt_info,meta_events,v_time,header_chunk);

#ifdef DEBUG
      x++;
      if (x==15) { x=0; printf("\n"); }
#endif
    }
  }

#ifdef DEBUG
  printf("%d\n",meta_events->tempo);
#endif
  return 0;
}

int parse_midi(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
char header[5];
int n,t,track;
struct _header_chunk header_chunk;
struct _meta_events meta_events;

  default_channel=0;
  default_track=-1;
  header_flag=0;

  default_channel=rtt_info->mcc;
  default_track=rtt_info->mnc;

  if (rtt_info->songname[0]==0)
  {
    t=strlen(rtt_info->outname);
    while(t>0 && rtt_info->outname[t]!='/' && rtt_info->outname[t]!='\\') t--;
    if (rtt_info->outname[t]=='/' || rtt_info->outname[t]=='\\') t++;
    for (n=0; n<SONGNAME_LEN-1; n++)
    {
      if (rtt_info->outname[t]=='.' || rtt_info->outname[t]==0) break;
      rtt_info->songname[n]=rtt_info->outname[t++];
    }
    rtt_info->songname[n]=0; 
  }

  if (quiet==0)
  {
    printf("Song Name: %s\n",rtt_info->songname);
  }

  track=0;

  meta_events.sequence_number=track;
  meta_events.tempo=500000;
  meta_events.time_signature[0]=4;
  meta_events.time_signature[1]=2;
  meta_events.time_signature[2]=0;
  meta_events.time_signature[3]=0;
  meta_events.key_signature[0]=0;
  meta_events.key_signature[1]=0;
  meta_events.quarter_note=5208;

  current_note=-1;

  while(1)
  {
    if (read_string(in,header,4)==-1)
    { break; }

    n=read_long_b(in);

    if (strcasecmp(header,"MThd")==0) 
    {
      MThd_parse(in,&header_chunk,n);
      if (quiet==0)
      {
        printf("\n--------------------------------------------\n");
        printf("   Header length: %d\n",header_chunk.header_length);
        printf("          Format: %d\n",header_chunk.format);
        printf("Number of Tracks: %d\n",header_chunk.tracks);
        printf("        Division: %d\n",header_chunk.division);
        printf("--------------------------------------------\n");
      }
      meta_events.quarter_note=meta_events.tempo/header_chunk.division;
    }
      else
    if (strcasecmp(header,"MTrk")==0) 
    {
      if (default_track>-1)
      {
        if (track>default_track) break;
        if (track!=default_track)
        {
          track++;
          fseek(in,n,SEEK_CUR);
          continue;
        }
      }

      if (quiet==0)
      {
        printf("\n--------------------------------------------\n");
        printf("Track: %d",track++);
        printf("\n--------------------------------------------\n");
      }

      MTrk_parse(in, out, &header_chunk, n, &meta_events, rtt_info);
    }
  }

  footer_route(out,rtt_info);

  return 0;
}



