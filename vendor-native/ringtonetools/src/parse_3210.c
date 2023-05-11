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

extern int pushback;
extern int gettoken(FILE *fp, char *token, int flag);

int get_3210_tempo(FILE *fp)
{
int marker;
char token[100];
int t,ch;

  marker=ftell(fp);
  gettoken(fp,token,1);

  if (strcasecmp(token,"tempo")!=0)
  {
    pushback=-1;
    fseek(fp,marker,0);
    return 63;
  }

  gettoken(fp,token,1);
  if (strcmp(token,"=")!=0) printf("Expecting '=' and got '%s'\n",token);
  gettoken(fp,token,1);

  t=atoi(token);
  if (t<1) return 63;

  while (1)
  {
    ch=getc(fp);
    if (ch=='\n' || ch=='\r' || ch==':' || ch=='\t') continue;
    if (ch==EOF) { printf("Premature end of file\n"); break; }
      else
    {
      ungetc(ch,fp);
      break;
    }
  }

  return t;
}

int parse_3210(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
char token[16];
int t,l,ch,f;
int tone_modifier;

  l=0; t=0; f=0;
  if (rtt_info->songname[0]!=0) t=1;
  while(1)
  {
    ch=getc(in);
    if (f==0 && (ch=='\n' || ch=='\r')) continue;
    if (ch==':' || ch=='\n' || ch=='\r') break;
    f=1;
    if (ch==EOF)
    {
      printf("Error: Illegal keypress format file\n");
      return -1;
    }

    if (l<SONGNAME_LEN-1 && t==0) rtt_info->songname[l++]=ch;
  }

  if (t!=0) rtt_info->songname[l]=0;

  rtt_info->bpm=get_3210_tempo(in);

#ifdef DEBUG
  printf("\n       Song Name: %s\n",rtt_info->songname);
  printf("Beats Per Minute: %d\n",rtt_info->bpm);
#endif

  header_route(out,rtt_info);

  while(1)
  {
    tone_modifier=0;
    rtt_info->modifier=0;

    t=gettoken(in,token,1);
    if (t==-1) 
    { break; } 

    if (token[0]==':') continue;

    rtt_info->length=atoi(token);

    if (rtt_info->length==1) rtt_info->length=0;
      else
    if (rtt_info->length==2) rtt_info->length=1;
      else
    if (rtt_info->length==4) rtt_info->length=2;
      else
    if (rtt_info->length==8) rtt_info->length=3;
      else
    if (rtt_info->length==16) rtt_info->length=4;
      else
    if (rtt_info->length==32) rtt_info->length=5;

    while(1)
    {
      t=gettoken(in,token,1);

      if (strcmp(token,".")==0)
      { rtt_info->modifier++; }
        else
      if (strcmp(token,"#")==0)
      { tone_modifier++; }
        else
      { break; }
    }

    token[0]=tolower(token[0]);

    if (token[0]=='p' || token[0]=='-') rtt_info->tone=0;
      else
    if (token[0]=='a') rtt_info->tone=10;
      else
    if (token[0]=='h' || token[0]=='b') rtt_info->tone=12;
      else
    if (token[0]=='c') rtt_info->tone=1;
      else
    if (token[0]=='d') rtt_info->tone=3;
      else
    if (token[0]=='e') rtt_info->tone=5;
      else
    if (token[0]=='f') rtt_info->tone=6;
      else
    if (token[0]=='g') rtt_info->tone=8;

    rtt_info->tone=rtt_info->tone+tone_modifier;

    if (rtt_info->tone!=0)
    {
      t=gettoken(in,token,1);
      rtt_info->scale=atoi(token)-1;
    }

    if (rtt_info->tone>12)
    {
      rtt_info->tone=rtt_info->tone-12;
      rtt_info->scale++;
    }

    note_route(out,rtt_info);
  }

  footer_route(out,rtt_info);

  return (0);
}



