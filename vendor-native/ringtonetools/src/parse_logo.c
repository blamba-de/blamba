#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ringtonetools.h"
#include "fileoutput.h"

#define COLOR_BMP 1

/*

Ringtone Tools - Copyright 2001-2006 Michael Kohn (mike@mikekohn.net)
This falls under the Kohnian license.  Please read
it at http://ringtonetools.mikekohn.net/

This program is NOT opensourced.  You may not use any part
of this program for your own software.

*/

int parse_text(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
int x,y,lastch,ch;

  logo_header_route(out,rtt_info);

  x=0; y=0; lastch=0;
  while ((ch=getc(in))!=EOF)
  {
    if (ch=='\n' || ch=='\r')
    {
      if ((lastch=='\r' || lastch=='\n') && (ch!=lastch && lastch!=0))
      { continue; }

      x=0; y++;
      lastch=ch;
      continue;
    }

    if (x<rtt_info->width)
    {
      if (ch==' ')
      { rtt_info->picture[x+(y*rtt_info->width)]=0; }
        else
      { rtt_info->picture[x+(y*rtt_info->width)]=0xffffff; }
    }

    x++;
  }

  logo_footer_route(out,rtt_info);

  return 0;
}

int parse_ngg(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
char temp[7];
int t,ptr,count,ch;

  read_chars(in,temp,6);
  temp[3]=0;

  if (strcasecmp(temp,"nol")==0)
  {
    rtt_info->mcc=read_word(in);
    rtt_info->mnc=read_word(in);
  }

  rtt_info->width=read_word(in);
  rtt_info->height=read_word(in);
  read_chars(in,temp,6);

  count=rtt_info->width*rtt_info->height;

  logo_header_route(out,rtt_info);

  ptr=0;
  for (t=0; t<count; t++)
  {
    ch=(getc(in));
    if (ch=='0')
    { rtt_info->picture[ptr++]=0; }
      else
    { rtt_info->picture[ptr++]=0xffffff; }
  }

  logo_footer_route(out,rtt_info);

  return 0;
}

#ifdef DEBUG
void debug_logo(struct rtt_info_t *rtt_info)
{
int x,y,c,ptr;

  printf("%d x %d\n",rtt_info->width,rtt_info->height);

  ptr=0;
  for (y=0; y<rtt_info->height; y++)
  {
    for (x=0; x<rtt_info->width; x++)
    {
      c=rtt_info->picture[ptr++];
      c=((c&255)+((c>>8)&255)+((c>>16)&255))/3;

      if (COLOR_BMP==1)
      {
        if (c<20)
        { printf(" "); }
          else
        if (c<40)
        { printf("."); }
          else
        if (c<60)
        { printf(","); }
          else
        if (c<80)
        { printf(":"); }
          else
        if (c<100)
        { printf("|"); }
          else
        if (c<120)
        { printf("&"); }
          else
        if (c<180)
        { printf("@"); }
          else
        if (c<210)
        { printf("*"); }
          else
        { printf("#"); }
      }
        else
      {
        if (c>COLOR_THRESHOLD)
        { printf("*"); }
          else
        { printf(" "); }
      }
    }
    printf("|\n");
  }

}
#endif
