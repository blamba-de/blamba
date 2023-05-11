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

/* EMS */

void write_ems_header(FILE *out, struct rtt_info_t *rtt_info)
{

}

void write_ems(struct rtt_info_t *rtt_info, char *s)
{
int l,t;

  l=strlen(s);

  for (t=0; t<l; t++)
  {
    rtt_info->ring_stack[rtt_info->stackptr++]=s[t];
  }
}

void write_ems_footer(FILE *out, struct rtt_info_t *rtt_info)
{
int t;

  if (rtt_info->stackptr>128)
  {
    if (quiet==0)
    {
      printf("Error: EMS data exceeds 128 octets.  Message is truncated.\n");
      rtt_info->stackptr=128;
    }
  }

  fprintf(out,"%02X0C%02X00",rtt_info->stackptr+3,rtt_info->stackptr+1);

  for (t=0; t<rtt_info->stackptr; t++)
  {
    fprintf(out,"%02X",rtt_info->ring_stack[t]);
  }
}

void write_ems_logo_header(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->width==16 && rtt_info->height==16)
  {
    rtt_info->width=16;
    rtt_info->height=16;
    rtt_info->ems=0x11;
  }
    else
  if (rtt_info->width==32 && rtt_info->height==32)
  {
    rtt_info->width=32;
    rtt_info->height=32;
    rtt_info->ems=0x10;
  }
    else
  {
    rtt_info->ems=0x12;
  }
}

void write_ems_logo_footer(FILE *out, struct rtt_info_t *rtt_info)
{
int x,y,b,c,ptr;
int octets,k;

  ptr=0;
  octets=0;

  for (y=0; y<rtt_info->height; y++)
  {
    x=0;
    while (x<rtt_info->width)
    {
      k=0;

      for (b=7; b>=0; b--)
      {
        if (x<rtt_info->width)
        {
          c=rtt_info->picture[ptr++];
          c=((c&255)+((c>>8)&255)+((c>>16)&255))/3;

          if (c>COLOR_THRESHOLD)
          { k=k+(1<<b); }
        }
        x++;
      }

      rtt_info->picture[octets++]=k;
    }
  }

  if (octets>128)
  {
    if (quiet==0)
    {
      printf("Warning: EMS data exceeds 128 octets.  Message has been tructated.\n");
    }
    octets=128;
  }

  if (rtt_info->ems==0x12)
  {
    if ((rtt_info->width%8)!=0)
    {
      printf("Warning:  Width needs to be a multiple of 8 pixels.\n");
      rtt_info->width=rtt_info->width+(8-(rtt_info->width%8));
    }
    fprintf(out,"%02X%02X%02X00",octets+5,rtt_info->ems,octets+3);
    fprintf(out,"%02X%02X",rtt_info->width/8,rtt_info->height);
  }
    else
  { fprintf(out,"%02X%02X%02X00",octets+3,rtt_info->ems,octets+1); }

  for (k=0; k<octets; k++)
  {
    fprintf(out,"%02X",rtt_info->picture[k]);
  }
}



