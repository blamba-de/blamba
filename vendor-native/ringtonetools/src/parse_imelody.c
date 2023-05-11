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

int get_next(FILE *in)
{
int ch;

  while(1)
  {
    ch=getc(in);

    if (ch==' ' || ch=='\t')
    { continue; }
      else
    if (ch=='\n' || ch=='\r')
    {
      ch=getc(in);
      if (ch!='\r' && ch!='\n') ungetc(ch,in);
      return '\n';
    }
      else
    if (ch==EOF)
    { return EOF; }
      else
    { return tolower(ch); }
  }

  return 0;
}

int get_field(FILE *in, char *field_name, char *field_value)
{
int ch,ptr,p;
char *buffer;
int marker;

  buffer=field_name;
  marker=ftell(in);

  field_name[0]=0;
  field_value[0]=0;

  ptr=0; p=0;
  while(1)
  {
    ch=getc(in);

    if (ptr==0 && p==0)
    {
      if (ch=='\r' || ch=='\n') continue;
      if (ch==EOF) return -1;
    }

    if (ch==':' && p==0)
    {
      buffer[ptr]=0;
      if (strcasecmp(buffer,"melody")==0) return 2;

      buffer=field_value;
      ptr=0;
      p++;
      continue;
    }

    if (ch=='\n' || ch=='\r' || ch==EOF || ptr==1023)
    {
      if (p==0)
      {
        fseek(in,marker,SEEK_SET);
        return 1;
      }

      buffer[ptr]=0;
      return 0;
    }

    buffer[ptr++]=ch;
  }

  return 0;
}

int parse_num(FILE *in)
{
int ch,i;

  i=0;

  while(1)
  {
    ch=getc(in);
    if (ch>='0' && ch<='9')
    {
      i=(i*10)+(ch-'0');
    }
      else
    { ungetc(ch,in); return i; }
  }
}

void parse_melody(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
int ch,t;
int tone_modifier;
int repeat,repeat_count=0;
unsigned char default_octave;
char token[255];

  default_octave=4;
  tone_modifier=0;
  repeat=ftell(in);

  header_route(out,rtt_info);

  while(1)
  {
    ch=get_next(in);
    if (ch==EOF || ch=='\n') break;

    if (ch=='*')
    {
      ch=get_next(in);
      if (ch==EOF || ch=='\n') break;

      default_octave=ch-'0';
      if (default_octave>8) default_octave=4;
      continue;
    }

    if (ch=='{')
    {
      repeat=ftell(in);

      continue;
    }

    if (ch=='}')
    {
      ch=get_next(in);
      if (ch==EOF || ch=='\n') break;
      if (ch=='@')
      {
        gettoken(in,token,2);
        t=atoi(token);
#ifdef DEBUG
        printf("%d\n",t);
#endif
        if (repeat_count<t)
        { fseek(in,repeat,SEEK_SET); }

        repeat_count++;

        continue;
      }

      break;
    }

    if (ch=='v')
    {
      ch=get_next(in);
      if (ch==EOF || ch=='\n') break;

      if (ch=='-') { rtt_info->volume--; }
        else
      if (ch=='+') { rtt_info->volume++; }
        else
      if (ch>='0' && ch<='9') rtt_info->volume=parse_num(in);

      if (rtt_info->volume<0) rtt_info->volume=0;
        else
      if (rtt_info->volume>15) rtt_info->volume=15;

      volume_route(out,rtt_info);

      continue;
    }

    if (ch=='#')
    {
      tone_modifier=1;
      continue;
    }

    if (ch=='&')
    {
      tone_modifier=-1;
      continue;
    }

    if (ch=='a') rtt_info->tone=10;
      else
    if (ch=='h' || ch=='b') rtt_info->tone=12;
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
    if (ch=='p' || ch=='r') rtt_info->tone=0;

    rtt_info->tone=rtt_info->tone+tone_modifier;
    if (rtt_info->tone<1 && ch!='r' && ch!='p')
    {
      rtt_info->tone=12;
      rtt_info->scale--;
    }
      else
    if (rtt_info->tone>12)
    {
      rtt_info->tone=1;
      rtt_info->scale++;
    }

    ch=get_next(in);
    /* if (ch==EOF || ch=='\n') break; */

    rtt_info->length=2;
    if (ch>='0' && ch<='5')
    {
      rtt_info->length=ch-'0';
      ch=get_next(in);
    }

    if (ch=='.') rtt_info->modifier=1;
      else
    if (ch==':') rtt_info->modifier=2;
      else
    if (ch==';') rtt_info->modifier=3;
      else
    {
      ungetc(ch,in);
      rtt_info->modifier=0;
    }

    rtt_info->scale=default_octave-3;
    if (rtt_info->scale<0) rtt_info->scale=0;

    note_route(out,rtt_info);

    if (ch==EOF || ch=='\n') break;

    tone_modifier=0;
  }

  footer_route(out,rtt_info);
}

int parse_imelody(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
char field_name[1024],field_value[1024];
int i;

  rtt_info->bpm=120;

  while(1)
  {
    i=get_field(in,field_name,field_value);

    if (i==1 || i==2)
    {
      parse_melody(in,out,rtt_info);
      break;
    }
      else
    if (i==-1)
    { break; }

    if (strcasecmp(field_name,"name")==0)
    {
      strncpy(rtt_info->songname,field_value,SONGNAME_LEN);
      rtt_info->songname[SONGNAME_LEN-1]=0;
    }
      else
    if (strcasecmp(field_name,"beat")==0)
    {
      rtt_info->bpm=atoi(field_value);
      if (rtt_info->bpm==0) rtt_info->bpm=120;
    }
      else
    if (strcasecmp(field_name,"volume")==0)
    {
      rtt_info->volume=atoi(field_value);
      if (rtt_info->volume<=0 || rtt_info->volume>15) rtt_info->volume=7;
    }
      else
    if (strcasecmp(field_name,"style")==0 && strlen(field_value)==2)
    {
      rtt_info->style=field_value[1]-'0';
      if (rtt_info->style<0 || rtt_info->style>2) rtt_info->style=0;
    }
  }

  return (0);
}



