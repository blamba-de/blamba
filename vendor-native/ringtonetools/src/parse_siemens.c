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

int getnext(FILE *in)
{
int ch;

  while(1)
  {
    ch=getc(in);

    if (ch==' ' || ch=='\t' || ch=='\n' || ch=='\r')
    { continue; }
      else
    { return tolower(ch); }
  }

  return 0;
}

int parse_siemens(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
char token[16];
int t,ch,ptr;

  check_songname(in,rtt_info->songname);

#ifdef DEBUG
  printf("\n       Song Name: %s\n",rtt_info->songname);
#endif

  header_route(out,rtt_info);
  rtt_info->modifier=0;

  while(1)
  {
    rtt_info->tone=-1;
    rtt_info->scale=0;
    rtt_info->length=2;

    ch=getnext(in);
    if (ch==EOF) break;

    if (ch=='p') rtt_info->tone=0;
      else
    if (ch=='c') rtt_info->tone=1;
      else
    if (ch=='d') rtt_info->tone=3;
      else
    if (ch=='e') rtt_info->tone=5;
      else
    if (ch=='f') rtt_info->tone=6;
      else
    if (ch=='g') rtt_info->tone=8;
      else
    if (ch=='a') rtt_info->tone=10;
      else
    if (ch=='b' || ch=='h') rtt_info->tone=12;

    ch=getnext(in);
    if (rtt_info->tone!=0)
    {
      if (ch==EOF) break;

      if (ch=='i')
      {
        ch=getnext(in);
        if (ch==EOF) break;
        if (ch!='s') printf("Error:  expecting 's'\n"); 
        rtt_info->tone++;

        ch=getnext(in);
        if (ch==EOF) break;
      }

      if (ch>='0' && ch<='9')
      {
        rtt_info->scale=(ch-'0');
        ch=getnext(in);
        if (ch==EOF) break;
      }
        else
      { rtt_info->scale=0; }
    }

    if (ch=='(')
    {
      ptr=0;

      while(1)
      {
        ch=getnext(in);
        if (ch==EOF) break;
        if (ch=='/' || ch==')') break;
      }
      if (ch==EOF) break;

      ptr=0;
      while(1)
      {
        ch=getnext(in);
        if (ch==EOF) break;
        if (ch==')') break;
        if (ch>='0' && ch<='9') token[ptr++]=ch;
        if (ptr>2) break;
      }
      token[ptr]=0;

      t=atoi(token);
      if (t>32) t=32;

      if (t==1) rtt_info->length=0;
        else
      if (t==2) rtt_info->length=1;
        else
      if (t==4) rtt_info->length=2;
        else
      if (t==8) rtt_info->length=3;
        else
      if (t==16) rtt_info->length=4;
        else
      if (t==32) rtt_info->length=5;

      if (ch==EOF) break;
    }
      else
    {
      ungetc(ch,in);
    }

    note_route(out,rtt_info);
  }

  if (rtt_info->tone!=-1) note_route(out,rtt_info);

  footer_route(out,rtt_info);

  return (0);
}



