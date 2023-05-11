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

void write_nokia_header(FILE *out, struct rtt_info_t *rtt_info)
{
int l,t;

  rtt_info->bitptr=7;
  rtt_info->bytes=0;

  push(2,8,rtt_info);   /* ring tone commands */
  push(74,8,rtt_info);
  push(29,7,rtt_info);

  l=strlen(rtt_info->songname);
  push(1,3,rtt_info);   /* song type */
  push(l,4,rtt_info);   /* length of song name */

  for (t=0; t<l; t++)
  { push(rtt_info->songname[t],8,rtt_info); }

  push(1,8,rtt_info);   /* 1 song pattern */
  push(0,3,rtt_info);   /* pattern header id */
  push(0,2,rtt_info);   /* A-part */

  push(rtt_info->flats,4,rtt_info); /* looping  */

#ifdef DEBUG
  printf("stackptr=%d   bitptr=%d\n",rtt_info->stackptr,rtt_info->bitptr);
#endif

  rtt_info->marker=rtt_info->stackptr;
  rtt_info->mcc=rtt_info->bitptr;
  push(0,8,rtt_info);

  l=get_tempo(rtt_info->bpm);
  t=reverse_tempo(l);

  if (t!=rtt_info->bpm)
  {
    if (quiet==0)
    { printf("Warning:  Tempo has been adjusted to %d bpm due to Nokia limitations\n",t); }
  }
  push(4,3,rtt_info);       /* set tempo */
  push(l,5,rtt_info);
  rtt_info->bytes++;

  push(3,3,rtt_info);       /* set style */
  push(rtt_info->style,2,rtt_info);
  rtt_info->bytes++;

  push(5,3,rtt_info);       /* set volume */
  push(rtt_info->volume,4,rtt_info);
  rtt_info->bytes++;
}

void write_nokia_note(FILE *out, struct rtt_info_t *rtt_info)
{
  rtt_info->scale+=rtt_info->transpose;
  if (rtt_info->scale<0) rtt_info->scale=0;

  if (rtt_info->prev_scale!=rtt_info->scale)
  {
    rtt_info->prev_scale=rtt_info->scale;
    push(2,3,rtt_info);
    push(rtt_info->scale+1,2,rtt_info);
    rtt_info->bytes++;
  }

  push(1,3,rtt_info);
  push(rtt_info->tone,4,rtt_info);
  push(rtt_info->length,3,rtt_info);
  push(rtt_info->modifier,2,rtt_info);
  rtt_info->bytes++;
}

void write_nokia_style(FILE *out, struct rtt_info_t *rtt_info)
{
  push(3,3,rtt_info);       /* set style */
  push(rtt_info->style,2,rtt_info);
  rtt_info->bytes++;
}

void write_nokia_footer(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->bitptr!=7) rtt_info->stackptr++;
  push(0,8,rtt_info);

#ifdef DEBUG
  printf("commands: %d    a0=%d   b0=%d\n",rtt_info->bytes,rtt_info->marker,rtt_info->mcc);
#endif

  /* push_addr(rtt_info->bytes,8,rtt_info->marker,rtt_info->mcc); */
  push_addr(rtt_info->bytes,8,rtt_info);
  write_codes(out,"1581",rtt_info);

}


void write_nokia_logo_header(FILE *out, struct rtt_info_t *rtt_info)
{
int t,k,r;

  rtt_info->stackptr=0;
  rtt_info->bitptr=7;

  push(0x30,8,rtt_info);   /* Identifier for version */

  if (rtt_info->mcc!=-1 && rtt_info->mnc!=-1)
  {
    t=rtt_info->mcc%10;
    k=(rtt_info->mcc/10)%10;
    r=(rtt_info->mcc/100)%10;
    push((k<<4)+r,8,rtt_info);   /* MCC Mobile Country Code */
    push(0xf0+t,8,rtt_info);     /* MCC Mobile Country Code */
    t=rtt_info->mnc%10;
    k=(rtt_info->mnc/10)%10;
    push((t<<4)+k,8,rtt_info);   /* MNC Mobile Network Code */
    push(0x0A,8,rtt_info);       /* Line feed */
  }
    else
  if ((rtt_info->bmp_flags&2)==2)
  {
    push(0x06,8,rtt_info);
    k=((rtt_info->height*rtt_info->width)/8)+4;
    if (((rtt_info->height*rtt_info->width)%8)!=0) k++;
    push((k>>8),8,rtt_info);
    push((k&255),8,rtt_info);
  }
    else
  if (rtt_info->message!=0)
  {
    push(0x00,8,rtt_info);
    k=strlen(rtt_info->message);
    push((k>>8),8,rtt_info);
    push((k&255),8,rtt_info);

    for(t=0; t<k; t++)
    { push(rtt_info->message[t],8,rtt_info); }

    push(0x02,8,rtt_info);
    k=((rtt_info->height*rtt_info->width)/8)+4;
    if (((rtt_info->height*rtt_info->width)%8)!=0) k++;
    push((k>>8),8,rtt_info);
    push((k&255),8,rtt_info);
  }

  push(0x00,8,rtt_info);          /* Info Field? */
  push(rtt_info->width,8,rtt_info);   /* height and width DUH */
  push(rtt_info->height,8,rtt_info);
  push(0x01,8,rtt_info);          /* picture depth */
}

void write_nokia_logo_footer(FILE *out, struct rtt_info_t *rtt_info)
{
int x,y,c,ptr;

  ptr=0;
  for (y=0; y<rtt_info->height; y++)
  {
    for (x=0; x<rtt_info->width; x++)
    {
      c=rtt_info->picture[ptr++];
      c=((c&255)+((c>>8)&255)+((c>>16)&255))/3;
      if (c>COLOR_THRESHOLD)
      { push(1,1,rtt_info); }
        else
      { push(0,1,rtt_info); }
    }
  }

  rtt_info->stackptr++;

  if (rtt_info->mcc!=-1 && rtt_info->mnc!=-1)
  { write_codes(out,"1582",rtt_info); }
    else
  if ((rtt_info->bmp_flags&2)==2)
  { write_codes(out,"158A",rtt_info); }
    else
  { write_codes(out,"1583",rtt_info); }
}

void write_nokia_bpm(FILE *out, struct rtt_info_t *rtt_info)
{
int t;

  t=get_tempo(rtt_info->bpm);
  push(4,3,rtt_info);
  push(t,5,rtt_info);
  rtt_info->bytes++;
}

void write_nokia_volume(FILE *out, struct rtt_info_t *rtt_info)
{
  push(5,3,rtt_info);       /* set volume */
  push(rtt_info->volume,4,rtt_info);
  rtt_info->bytes++;
}

void write_ngg_logo_header(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->ems==0)
  {
    fprintf(out,"NOL%c%c%c",0,1,0);
    write_word(out,rtt_info->mcc);
    write_word(out,rtt_info->mnc);
  }
    else
  {
    fprintf(out,"NGG%c%c%c",0,1,0);
  }

  write_word(out,rtt_info->width);
  write_word(out,rtt_info->height);
  write_word(out,1);
  write_word(out,1);
  write_word(out,0x53);
}

void write_ngg_logo_footer(FILE *out, struct rtt_info_t *rtt_info)
{
int x,y,c,ptr;

  ptr=0;
  for (y=0; y<rtt_info->height; y++)
  {
    for (x=0; x<rtt_info->width; x++)
    {
      c=rtt_info->picture[ptr++];
      c=((c&255)+((c>>8)&255)+((c>>16)&255))/3;
      if (c>COLOR_THRESHOLD)
      { putc('1',out); }
        else
      { putc('0',out); }
    }
  }

  fprintf(out,"Created by Michael Kohn's Ringtone Tools - http://ringtonetools.mikekohn.net/");
}


