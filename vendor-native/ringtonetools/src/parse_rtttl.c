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

int pushback=-1;

int gettoken(FILE *fp, char *token, int flag)
{
int tokentype;
int ptr,ch;

  tokentype=0;
  ptr=0;
  token[0]=0;

  while (1)
  {
    if (ptr>15) break;

    if (pushback!=-1)
    { ch=pushback; pushback=-1; }
      else
    { ch=getc(fp); }

    if (flag==0)
    { if ((ch==' ' || ch=='\t') || (ch=='\n' || ch=='\r')) continue; }
      else
    { if ((ch==' ' || ch=='\t' || ch=='\n' || ch=='\r') && ptr==0) continue; }

    if (flag==2 && (ch<'0' || ch>'9'))
    {
      ungetc(ch,fp);
      break;
    }

    if (ch==EOF && ptr==0) return -1;
      else
    if (ch==EOF) { pushback=ch; break; }

    if ((ch>='A' && ch<='Z') || (ch>='a' && ch<='z'))
    {
      if (tokentype==1) { token[ptr++]=ch; }
        else
      if (ptr==0) { token[ptr++]=ch; tokentype=1; }
        else
      { pushback=ch; break; }
    }
      else
    if (ch>='0' && ch<='9')
    {
      if (tokentype==2) { token[ptr++]=ch; }
        else
      if (ptr==0) { token[ptr++]=ch; tokentype=2; }
        else
      { pushback=ch; break; }
    }
      else
    {
      if (ptr==0)
      {
        tokentype=3;
        token[ptr++]=ch;
        break;
      }
        else
      {
        pushback=ch;
        break;
      }
    }

  }

  token[ptr]=0;
  return tokentype;
}

int parse_rtttl(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
char token[16];
int t,r,l,ch;
int d,o,scale;
int length,tone,octave,modifier;

  d=4;
  o=6;
  rtt_info->bpm=63;
  tone=0;

  scale=-1;

  l=0;
  t=0;
  if (rtt_info->songname[0]!=0) t=1;
  while(1)
  {
    ch=getc(in);
    if (ch==':') break;
    if (ch==EOF)
    {
      printf("Error: Illegal RTTTL format file\n");
      return -1;
    }

    if (l<SONGNAME_LEN-1 && t==0) rtt_info->songname[l++]=ch;
  }

  if (t!=0) rtt_info->songname[l]=0;

#ifdef DEBUG
  printf("\n       Song Name: %s\n",rtt_info->songname);
#endif

  r=0;
  while(1)
  {
    if (gettoken(in,token,0)==-1)
    {
      printf("Error: Illegal RTTTL format file\n");
      return -1;
    }

    if (r==0)
    {
      if (strcmp(token,",")==0) continue;
        else
      if (strcmp(token,":")==0) break;
        else
      if (strcasecmp(token,"d")==0) r=1;
        else
      if (strcasecmp(token,"o")==0) r=2;
        else
      if (strcasecmp(token,"b")==0) r=3;
        else
      if (strcasecmp(token,"s")==0) r=4;
        else
      if (strcasecmp(token,"l")==0) r=5;
    }
      else
    if (strcmp(token,"=")==0)
    {
      gettoken(in,token,0);

      if (r==1) d=atoi(token);
        else
      if (r==2) o=atoi(token);
        else
      if (r==3) rtt_info->bpm=atoi(token);
        else
      if (r==4)
      {
        if (strcasecmp(token,"n")==0)
        { rtt_info->style=0; }
          else
        if (strcasecmp(token,"c")==0)
        { rtt_info->style=1; }
          else
        if (strcasecmp(token,"s")==0)
        { rtt_info->style=2; }
      }
        else
      if (r==5) rtt_info->flats=atoi(token);
      r=0;
    }
      else
    {
      printf("Error: Illegal format\n");
      return -1;
    }
  }

  if (rtt_info->flats==15) rtt_info->flats=31;

#ifdef DEBUG
  printf("Default Duration: %d\n",d);
  printf("   Default Scale: %d\n",o);
  printf("Beats Per Minute: %d\n",rtt_info->bpm);
  printf("           Style: ");
  if (rtt_info->style==0) printf("Natural\n");
    else
  if (rtt_info->style==1) printf("Continuous\n");
    else
  if (rtt_info->style==2) printf("Staccato\n");
  printf("            Loop: %d\n",rtt_info->flats);
#endif

  header_route(out,rtt_info);

  while(1)
  {
    t=gettoken(in,token,0);
    if (t==-1) 
    { break; } 

#ifdef DEBUG
    printf("%s\n",token);
#endif
    if (strcmp(token,";")==0) break; 

    length=d;

    if (t==2) 
    {
      length=atoi(token);
      t=gettoken(in,token,0);
    }

#ifdef DEBUG
    printf("Length: %d\n",length);
#endif

    if (length==1) length=0;
      else
    if (length==2) length=1;
      else
    if (length==4) length=2;
      else
    if (length==8) length=3;
      else
    if (length==16) length=4;
      else
    if (length==32) length=5;

    token[0]=tolower(token[0]);

    if (token[0]=='p') tone=0;
      else
    if (token[0]=='a') tone=10;
      else
    if (token[0]=='h' || token[0]=='b') tone=12;
      else
    if (token[0]=='c') tone=1;
      else
    if (token[0]=='d') tone=3;
      else
    if (token[0]=='e') tone=5;
      else
    if (token[0]=='f') tone=6;
      else
    if (token[0]=='g') tone=8;
      else
    if (token[0]=='o') tone=1;
      else
    if (token[0]=='s') tone=2;

#ifdef DEBUG
  printf("tone: %d\n",tone);
#endif

    t=gettoken(in,token,0);

    if (token[0]=='=')
    {
#ifdef DEBUG
printf("equals\n");
#endif
      t=gettoken(in,token,0);
#ifdef DEBUG
printf("token: %s\n",token);
#endif

      if (tone==12)
      {
        rtt_info->bpm=atoi(token);
        tempo_route(out, rtt_info);
      }
        else
      if (tone==1)
      {
        o=atoi(token);
      }
        else
      if (tone==2)
      {
        if (strcasecmp(token,"n")==0)
        { rtt_info->style=0; }
          else
        if (strcasecmp(token,"c")==0)
        { rtt_info->style=1; }
          else
        if (strcasecmp(token,"s")==0)
        { rtt_info->style=2; }

        if (rtt_info->out_type==RTT_TYPE_SCKL || rtt_info->out_type==RTT_TYPE_RNG)
        { write_nokia_style(out, rtt_info); }
      }

      t=gettoken(in,token,0);
      if (strcmp(token,",")!=0)
      {
        printf("Error: expecting , and found %s\n",token);
      } 

      continue;
    }

    modifier=0;
    octave=-1;

    while(1)
    {
      if (token[0]==',' || token[0]==';' || t==-1) break;

      if (token[0]=='#')
      { tone=tone+1; }
        else
      if (t==2)
      { octave=atoi(token); }
        else
      if (strcmp(token,".")==0)
      { modifier++; }

      t=gettoken(in,token,0);
    }

    if (tone!=0)
    {
      if (octave==-1) octave=o;
      if (tone>12)
      {
        tone=tone-12;
        octave++;
      }

      if (scale!=octave)
      {
        scale=octave;
      }
    }

    rtt_info->tone=tone;
    rtt_info->length=length;
    rtt_info->modifier=modifier;
    rtt_info->scale=octave-rtt_info->a440;

    note_route(out,rtt_info);

    if (t==-1)
    { break; }
  }

  footer_route(out,rtt_info);

  return (0);
}



