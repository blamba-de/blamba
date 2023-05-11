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

int parse_ems(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
unsigned char buffer[8192];
int s,t,ch,c,ptr,k;

  ptr=0;
  t=0; c=0;
  while((ch=getc(in))!=EOF)
  {
    ch=tolower(ch);
    if ((ch>='0' && ch<='9') || (ch>='a' && ch<='f')) 
    {
      if (ch>='0' && ch<='9')
      { t=(t<<4)+(ch-'0'); }
        else
      { t=(t<<4)+(ch-'a')+10; }
      c++;

      if ((c%2)==0)
      {
        buffer[ptr++]=t;
        t=0;
      }
    }
  }

  s=4;
  if (buffer[1]==0x11)
  {
    rtt_info->width=16;
    rtt_info->height=16;
  }
    else
  if (buffer[1]==0x10)
  {
    rtt_info->width=32;
    rtt_info->height=32;
  }
    else
  if (buffer[1]==0x12)
  {
    rtt_info->width=buffer[4]*8;
    rtt_info->height=buffer[5];
    s=s+2;
  }
    else
  {
    printf("Unknown or unsupported EMS type\n");
    return 1;
  }

  logo_header_route(out,rtt_info);

  k=0;
  for (c=s; c<ptr; c++)
  {
    /* ring_stack[stackptr++]=buffer[t]; */
    for (t=7; t>=0; t--)
    {
      if (((buffer[c]>>t)&1)==0)
      { rtt_info->picture[k++]=0; }
        else
      { rtt_info->picture[k++]=0xffffff; }
    }
  }

  logo_footer_route(out,rtt_info);

  return (0);
}



