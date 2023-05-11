#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "ringtonetools.h"
#include "fileoutput.h"

/*

Ringtone Tools - Copyright 2001-2006 Michael Kohn (mike@mikekohn.net)
This falls under the Kohnian license.  Please read
it at http://ringtonetools.mikekohn.net/

This program is NOT opensourced.  You may not use any part
of this program for your own software.

*/

int quiet=0;

int reverse_tempo(int l)
{
short int tempo_code[32]={ 25,28,31,35,40,45,50,56,63,70,80,90,100,
                           112,125,140,160,180,200,225,250,285,320,
                           355,400,450,500,565,635,715,800,900 };

  return tempo_code[l];
}

int get_tempo(int tempo)
{
int t;
short int tempo_code[32]={ 25,28,31,35,40,45,50,56,63,70,80,90,100,
                           112,125,140,160,180,200,225,250,285,320,
                           355,400,450,500,565,635,715,800,900 };

  if (tempo<25) tempo=25;
     else
  if (tempo>900) tempo=900;

  for (t=0; t<30; t++)
  {
    if (tempo_code[t]==tempo) return t;
    if (tempo_code[t]<tempo && tempo_code[t+1]>tempo)
    {
      if (tempo-tempo_code[t]<tempo_code[t+1]-tempo)
      { return t; }
        else
      { return t+1; }
    }
  }

  return t;
}

int push(int data, int size, struct rtt_info_t *rtt_info)
{
int d,t;

  d=data;
/*
#ifdef DEBUG
printf("%d: ",stackptr);
#endif
*/

  if (rtt_info->stackptr>=RINGSTACK_SIZE) return -1;

  for (t=size-1; t>=0; t--)
  {
    d=(data>>t)&1;
/*
#ifdef DEBUG
printf("%d",d);
#endif
*/

    rtt_info->ring_stack[rtt_info->stackptr]&=255-(1<<rtt_info->bitptr);
    rtt_info->ring_stack[rtt_info->stackptr]+=(d<<rtt_info->bitptr);

    rtt_info->bitptr--;

    if (rtt_info->bitptr==-1)
    {
      rtt_info->bitptr=7;
      rtt_info->stackptr++;
      if (rtt_info->stackptr<RINGSTACK_SIZE)
      { rtt_info->ring_stack[rtt_info->stackptr]=0; }
    }
  }

/*
#ifdef DEBUG
printf("\n");
#endif
*/

  return 0;
}

int push_addr(int data, int size, struct rtt_info_t *rtt_info)
{
int stackptr,bitptr;
int d,t;

  stackptr=rtt_info->marker;
  bitptr=rtt_info->mcc;

  if (stackptr>=RINGSTACK_SIZE) return -1;

  d=data;

  for (t=size-1; t>=0; t--)
  {
    d=(data>>t)&1;
    rtt_info->ring_stack[stackptr]&=255-(1<<bitptr);
    rtt_info->ring_stack[stackptr]+=(d<<bitptr);

    bitptr--;

    if (bitptr==-1)
    {
      bitptr=7;
      stackptr++;
    }
  }

  return 0;
}

/*
int push_addr(int data, int size, struct rtt_info_t *rtt_info)
{
int d,t;

  if (rtt_info->stackptr>=RINGSTACK_SIZE) return -1;

  d=data;

  for (t=size-1; t>=0; t--)
  {
    d=(data>>t)&1;
    rtt_info->ring_stack[rtt_info->marker]&=255-(1<<rtt_info->mcc);
    rtt_info->ring_stack[rtt_info->marker]+=(d<<rtt_info->mcc);

    rtt_info->bitptr--;

    if (rtt_info->bitptr==-1)
    {
      rtt_info->bitptr=7;
      rtt_info->stackptr++;
    }
  }

  return 0;
}
*/

void write_codes(FILE *out, char *port, struct rtt_info_t *rtt_info)
{
int t;
int count=0,part,total_parts;
int sms_size;

  if (rtt_info->out_type==RTT_TYPE_RNG)
  {
    for (t=0; t<rtt_info->stackptr; t++)
    {
      putc(rtt_info->ring_stack[t],out);
    }
  }
    else
  if (rtt_info->headers==RTT_HEADERS_DEFAULT)
  { 
    if (((rtt_info->stackptr*2)+strlen("//SCKL1581 "))<=rtt_info->full_sms_size)
    {
      count=strlen("//SCKL1581 ");
      rtt_info->full_sms_size=1000;
      rtt_info->headers=RTT_HEADERS_FULL;
    }
     else
    { count=strlen("//SCKL15810000010101 "); }
  }
    else
  if (rtt_info->headers==RTT_HEADERS_LESS)
  { count=strlen("//SCKL1582 0B0504158200000003010202"); }
    else
  if (rtt_info->headers==RTT_HEADERS_MOST)
  { 
    if (((rtt_info->stackptr*2)+14)<=rtt_info->full_sms_size)
    {
      count=strlen("06050415830000");
      rtt_info->headers=RTT_HEADERS_STD;
    }
      else
    { count=strlen("0B0504158200000003010202"); }
  }
    else
  { count=0; }

  sms_size=(rtt_info->full_sms_size-count)/2;

#ifdef DEBUG
printf("sms_size=%d\n",sms_size);
#endif

  count=1000;
  part=1;
  total_parts=(rtt_info->stackptr/sms_size)+1;
  if ((rtt_info->stackptr%sms_size)==0) total_parts--;

  for (t=0; t<rtt_info->stackptr; t++)
  {
    if (count>=sms_size)
    {
      /* GIMME HEADers */

      if (part!=1) fprintf(out,"\n\n");

      if (rtt_info->headers==RTT_HEADERS_DEFAULT)
      {
        fprintf(out,"//SCKL%s000001%02X%02X ",port,total_parts,part);
      }
        else
      if (rtt_info->headers==RTT_HEADERS_LESS)
      {
        fprintf(out,"//SCKL%s 0B0504%s0000000301%02X%02X",port,port,total_parts,part);
      }
        else
      if (rtt_info->headers==RTT_HEADERS_MOST)
      {
        fprintf(out,"0B0504%s0000000301%02X%02X",port,total_parts,part);
      }
        else
      if (rtt_info->headers==RTT_HEADERS_STD)
      {
        fprintf(out,"060504%s0000",port);
      }
        else
      if (rtt_info->headers==RTT_HEADERS_FULL)
      {
        fprintf(out,"//SCKL%s ",port);
      }

      count=0;
      part++;
    }

    fprintf(out,"%02X",rtt_info->ring_stack[t]); 
    count++;
  }

  fprintf(out,"\n\n");

  if (total_parts>3)
  {
    printf("\nWarning: Song exceeds 3 SMS messages.  This might cause a problem.\n");
  }
}

void check_songname(FILE *in, char *songname)
{
char buffer[1024];
int ch,ptr;

  ptr=0;
  while (1)
  {
    ch=getc(in);
    if (ch==EOF || ptr==1023)
    {
      ptr=0;
      fseek(in,0,SEEK_SET);
      break;
    }

    if (ch==':')
    { break; }

    buffer[ptr++]=ch;
  }

  buffer[ptr]=0;

  if (songname[0]==0) strncpy(songname,buffer,SONGNAME_LEN);
  songname[SONGNAME_LEN-1]=0;
}

int convcolor(char *s)
{
int t,c,b;

  if (s[0]!='0' || s[1]!='x' || strlen(s)!=8)
  {
    printf("\n%s is an invalid color. Colors are made by the format of \n"
           "0xrrggbb where rr,gg,bb are replaced with a hex number\n"
           "representing the color you want to use.  For example 0x000000\n"
           "would be black and 0xffffff would be white.\n\n",s);
    exit(0);
    return -1;
  }

  c=0;
  b=20;

  for (t=2; t<8; t++)
  {
    if (s[t]>='0' && s[t]<='9')
    { c=c+((s[t]-'0')<<b); }
      else
    if (tolower(s[t])>='a' && tolower(s[t])<='f')
    { c=c+((s[t]-'a'+10)<<b); }

    b=b-4;
  } 

  return c;
}

int rtt_init_info(struct rtt_info_t *rtt_info)
{
  memset(rtt_info, 0, sizeof(struct rtt_info_t));

  rtt_info->sample_rate=44100;
  rtt_info->bytes=2;
  rtt_info->bpm=140;
  rtt_info->a440=5;
  rtt_info->ats=1;
  rtt_info->volume=7;
  /* rtt_info->loop=0; */

  rtt_info->width=72;
  rtt_info->height=14;
  rtt_info->mcc=-1;
  rtt_info->mnc=-1;
  rtt_info->trans=-1;
  rtt_info->full_sms_size=SMS_SIZE;
  rtt_info->headers=RTT_HEADERS_DEFAULT;

  rtt_info->pause=2;
/*
  rtt_info->picture=0;
  rtt_info->message=0;
  rtt_info->bmp_flags=0;

  rtt_info->outname[0]=0;
  rtt_info->inname[0]=0;
  rtt_info->stackptr=0;
  rtt_info->bitptr=0;
*/

  return 0;
}

int rtt_add_extension_to_file(struct rtt_info_t *rtt_info)
{
  if (rtt_info->out_type==RTT_TYPE_WAV)
  { strcat(rtt_info->outname,"wav"); }
    else
  if (rtt_info->out_type==RTT_TYPE_KWS)
  { strcat(rtt_info->outname,"kws"); }
    else
  if (rtt_info->out_type==RTT_TYPE_MOT)
  { strcat(rtt_info->outname,"mot"); }
    else
  if (rtt_info->out_type==RTT_TYPE_PDB)
  { strcat(rtt_info->outname,"pdb"); }
    else
  if (rtt_info->out_type==RTT_TYPE_SCKL)
  { strcat(rtt_info->outname,"sckl"); }
    else
  if (rtt_info->out_type==RTT_TYPE_RNG)
  { strcat(rtt_info->outname,"rng"); }
    else
  if (rtt_info->out_type==RTT_TYPE_RTTTL)
  { strcat(rtt_info->outname,"rtttl"); }
    else
  if (rtt_info->out_type==RTT_TYPE_SAMSUNG1)
  { strcat(rtt_info->outname,"samsung1"); }
    else
  if (rtt_info->out_type==RTT_TYPE_SAMSUNG2)
  { strcat(rtt_info->outname,"samsung2"); }
    else
  if (rtt_info->out_type==RTT_TYPE_MIDI)
  { strcat(rtt_info->outname,"midi"); }
    else
  if (rtt_info->out_type==RTT_TYPE_SEO)
  { strcat(rtt_info->outname,"siemens"); }
    else
  if (rtt_info->out_type==RTT_TYPE_EMELODY)
  { strcat(rtt_info->outname,"emelody"); }
    else
  if (rtt_info->out_type==RTT_TYPE_IMELODY)
  { strcat(rtt_info->outname,"imy"); }
    else
  if (rtt_info->out_type==RTT_TYPE_RTX)
  { strcat(rtt_info->outname,"rtx"); }
    else
  if (rtt_info->out_type==RTT_TYPE_EMS)
  { strcat(rtt_info->outname,"ems"); }
    else
  if (rtt_info->out_type==RTT_TYPE_NOKIA3210)
  { strcat(rtt_info->outname,"3210"); }
    else
  if (rtt_info->out_type==RTT_TYPE_SEO)
  { strcat(rtt_info->outname,"seo"); }
    else
  if (rtt_info->out_type==RTT_TYPE_BMP)
  { strcat(rtt_info->outname,"bmp"); }
    else
  if (rtt_info->out_type==RTT_TYPE_TEXT)
  { strcat(rtt_info->outname,"txt"); }
    else
  if (rtt_info->out_type==RTT_TYPE_NOL)
  { strcat(rtt_info->outname,"nol"); }
    else
  if (rtt_info->out_type==RTT_TYPE_NGG)
  { strcat(rtt_info->outname,"ngg"); }
    else
  if (rtt_info->out_type==RTT_TYPE_WBMP)
  { strcat(rtt_info->outname,"wbmp"); }
    else
  if (rtt_info->out_type==RTT_TYPE_ICO)
  { strcat(rtt_info->outname,"ico"); }
    else
  if (rtt_info->out_type==RTT_TYPE_GIF)
  { strcat(rtt_info->outname,"gif"); }

  return RTT_UNSUPPORTED_OUTTYPE;
}

int rtt_get_intype(struct rtt_info_t *rtt_info, char *param)
{
  if (strcasecmp(param,"rtttl")==0 || strcasecmp(param,"nokring")==0)
  { return RTT_TYPE_RTTTL; }
    else
  if (strcasecmp(param,"rtx")==0)
  { return RTT_TYPE_RTX; }
    else
  if (strcasecmp(param,"imelody")==0 || strcasecmp(param,"imy")==0)
  { return RTT_TYPE_IMELODY; }
    else
  if (strcasecmp(param,"wav")==0)
  { return RTT_TYPE_WAV; }
    else
  if (strcasecmp(param,"bmp")==0)
  { return RTT_TYPE_BMP; }
    else
  if (strcasecmp(param,"text")==0 || strcasecmp(param,"txt")==0)
  { return RTT_TYPE_TEXT; }
    else
  if (strcasecmp(param,"midi")==0 || strcasecmp(param,"mid")==0)
  { return RTT_TYPE_MIDI; }
    else
  if (strcasecmp(param,"kws")==0)
  { return RTT_TYPE_KWS; }
    else
  if (strcasecmp(param,"siemens")==0 || strcasecmp(param,"seo")==0)
  { return RTT_TYPE_SEO; }
    else
  if (strcasecmp(param,"emelody")==0 || strcasecmp(param,"emy")==0)
  { return RTT_TYPE_EMELODY; }
    else
  if (strcasecmp(param,"3210")==0)
  { return RTT_TYPE_NOKIA3210; }
    else
  if (strcasecmp(param,"nok")==0 || strcasecmp(param,"nokia")==0 || strcasecmp(param,"sckl")==0)
  { return RTT_TYPE_SCKL; }
    else
  if (strcasecmp(param,"morse")==0)
  { return RTT_TYPE_MORSECODE; }
    else
  if (strcasecmp(param,"ems")==0)
  { return RTT_TYPE_EMS; }
    else
  if (strcasecmp(param,"nol")==0 || strcasecmp(param,"ngg")==0)
  { return RTT_TYPE_NOL; }
    else
  if (strcasecmp(param,"wbmp")==0)
  { return RTT_TYPE_WBMP; }
    else
  if (strcasecmp(param,"ico")==0 || strcasecmp(param,"icon")==0)
  { return RTT_TYPE_ICO; }
    else
  if (strcasecmp(param,"gif")==0)
  { return RTT_TYPE_GIF; }
    else
  if (strcasecmp(param,"ott")==0 || strcasecmp(param,"rng")==0)
  { return RTT_TYPE_OTT; }

  return RTT_UNSUPPORTED_INTYPE;
}

int rtt_get_outtype(struct rtt_info_t *rtt_info, char *param)
{
  if (strcasecmp(param,"wav")==0)
  { return RTT_TYPE_WAV; }
    else
  if (strcasecmp(param,"kws")==0)
  { return RTT_TYPE_KWS; }
    else
  if (strcasecmp(param,"mot")==0)
  { return RTT_TYPE_MOT; }
    else
  if (strcasecmp(param,"pdb")==0)
  { return RTT_TYPE_PDB; }
    else
  if (strcasecmp(param,"nok")==0 || strcasecmp(param,"nokia")==0 || strcasecmp(param,"sckl")==0)
  { return RTT_TYPE_SCKL; }
    else
  if (strcasecmp(param,"rtttl")==0 || strcasecmp(param,"nokring")==0)
  { return RTT_TYPE_RTTTL; }
    else
  if (strcasecmp(param,"samsung1")==0)
  { return RTT_TYPE_SAMSUNG1; }
    else
  if (strcasecmp(param,"samsung2")==0)
  { return RTT_TYPE_SAMSUNG2; }
    else
  if (strcasecmp(param,"midi")==0 || strcasecmp(param,"mid")==0)
  { return RTT_TYPE_MIDI; }
    else
  if (strcasecmp(param,"siemens")==0 || strcasecmp(param,"seo")==0)
  { return RTT_TYPE_SEO; }
    else
  if (strcasecmp(param,"emelody")==0 || strcasecmp(param,"emy")==0)
  { return RTT_TYPE_EMELODY; }
    else
  if (strcasecmp(param,"imelody")==0 || strcasecmp(param,"imy")==0)
  { return RTT_TYPE_IMELODY; }
    else
  if (strcasecmp(param,"rtx")==0)
  { return RTT_TYPE_RTX; }
    else
  if (strcasecmp(param,"ems")==0)
  { return RTT_TYPE_EMS; }
    else
  if (strcasecmp(param,"3210")==0)
  { return RTT_TYPE_NOKIA3210; }
    else
  if (strcasecmp(param,"seo")==0)
  { return RTT_TYPE_SEO; }
    else
  if (strcasecmp(param,"bmp")==0)
  { return RTT_TYPE_BMP; }
    else
  if (strcasecmp(param,"text")==0 || strcasecmp(param,"txt")==0)
  { return RTT_TYPE_TEXT; }
    else
  if (strcasecmp(param,"dsp")==0)
  { return RTT_TYPE_DSP; }
    else
  if (strcasecmp(param,"nol")==0)
  { return RTT_TYPE_NOL; }
    else
  if (strcasecmp(param,"ngg")==0)
  { return RTT_TYPE_NGG; }
    else
  if (strcasecmp(param,"wbmp")==0)
  { return RTT_TYPE_WBMP; }
    else
  if (strcasecmp(param,"ico")==0 || strcasecmp(param,"icon")==0)
  { return RTT_TYPE_ICO; }
    else
  if (strcasecmp(param,"gif")==0)
  { return RTT_TYPE_GIF; }
    else
  if (strcasecmp(param,"ott")==0 || strcasecmp(param,"rng")==0)
  {
    rtt_info->full_sms_size=5000;
    return RTT_TYPE_RNG;
  }

  return RTT_UNSUPPORTED_OUTTYPE;
}

int rtt_convert(struct rtt_info_t *rtt_info)
{
FILE *in,*out;
int t;

  if (rtt_info->inname[0]!='-')
  {
    if (rtt_info->in_type==RTT_TYPE_UNDEFINED)
    {
      t=strlen(rtt_info->inname);
      while(t>0 && rtt_info->inname[t]!='.') t--;

      if (rtt_info->inname[t]=='.')
      {
        t++;
        rtt_info->in_type=rtt_get_intype(rtt_info,&rtt_info->inname[t]);
      }
    }

    in=fopen(rtt_info->inname,"rb");
    if (in==0)
    {
      printf("Could not open file for reading: %s\n",rtt_info->inname);
      return -1;
    }
  }
    else
  { in=fdopen(fileno(stdin),"r"); }

  if (rtt_info->outname[0]!='-')
  {
    if (rtt_info->out_type!=RTT_TYPE_DSP)
    {
      out=fopen(rtt_info->outname,"wb+");
      if (out==0)
      {
        printf("Could not open file for writing: %s\n",rtt_info->outname);
        fclose(in);
        return -2;
      }
    }
      else
    { out=0; }
  }
    else
  { out=fdopen(fileno(stdout),"w"); }

  if (!(rtt_info->in_type==RTT_TYPE_BMP || rtt_info->in_type==RTT_TYPE_TEXT) && rtt_info->out_type==RTT_TYPE_EMS)
  {
    rtt_info->out_type=RTT_TYPE_IMELODY;
    rtt_info->ems=1;
  }

  if (rtt_info->out_type==RTT_TYPE_NGG) rtt_info->ems=1;

  if (rtt_info->out_type==RTT_TYPE_PDB && rtt_info->in_type==RTT_TYPE_MIDI)
  { write_treo_pdb(in,out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_SEO)
  {
    if (rtt_info->in_type==RTT_TYPE_BMP || rtt_info->in_type==RTT_TYPE_MIDI)
    { write_seo(in,out,rtt_info); }
      else
    { if (quiet==0) printf("SEO format only supports BMP and MIDI\n"); }
  }
    else
  if (rtt_info->in_type==RTT_TYPE_RTTTL || rtt_info->in_type==RTT_TYPE_RTX)
  {
    if (rtt_info->in_type==RTT_TYPE_RTTTL)
    { rtt_info->a440=5; }
      else
    { rtt_info->a440=4; }

    parse_rtttl(in,out,rtt_info);
  }
    else
  if (rtt_info->in_type==RTT_TYPE_IMELODY)
  { parse_imelody(in,out,rtt_info); }
    else
  if (rtt_info->in_type==RTT_TYPE_WAV)
  {
    if (rtt_info->out_type==RTT_TYPE_PDB)
    { wav2pdb(in,out,rtt_info); }
      else
    { 
      if (quiet==0)
      { printf("Wav's can only be converted to pdb's.\n"); }
      parse_wav(in,out,rtt_info);
    }
  }
    else
  if (rtt_info->in_type==RTT_TYPE_BMP)
  { parse_bmp(in,out,rtt_info); }
    else
  if (rtt_info->in_type==RTT_TYPE_TEXT)
  { parse_text(in,out,rtt_info); }
    else
  if (rtt_info->in_type==RTT_TYPE_NGG || rtt_info->in_type==RTT_TYPE_NOL)
  { parse_ngg(in,out,rtt_info); }
    else
  if (rtt_info->in_type==RTT_TYPE_WBMP)
  { parse_wbmp(in,out,rtt_info); }
    else
  if (rtt_info->in_type==RTT_TYPE_ICO)
  { parse_icon(in,out,rtt_info); }
    else
  if (rtt_info->in_type==RTT_TYPE_GIF)
  { parse_gif(in,out,rtt_info); }
    else
  if (rtt_info->in_type==RTT_TYPE_MIDI)
  { parse_midi(in,out,rtt_info); }
    else
  if (rtt_info->in_type==RTT_TYPE_KWS)
  { parse_kws(in,out,rtt_info); }
    else
  if (rtt_info->in_type==RTT_TYPE_SEO)
  { parse_siemens(in,out,rtt_info); }
    else
  if (rtt_info->in_type==RTT_TYPE_EMELODY)
  { parse_emelody(in,out,rtt_info); }
    else
  if (rtt_info->in_type==RTT_TYPE_NOKIA3210)
  { parse_3210(in,out,rtt_info); }
    else
  if (rtt_info->in_type==RTT_TYPE_SCKL)
  { parse_sckl(in,out,rtt_info); }
    else
  if (rtt_info->in_type==RTT_TYPE_MORSECODE)
  { parse_morsecode(in,out,rtt_info); }
    else
  if (rtt_info->in_type==RTT_TYPE_EMS)
  { parse_ems(in,out,rtt_info); }
    else
  if (rtt_info->in_type==RTT_TYPE_OTT)
  { parse_ott(in,out,rtt_info); }
    else
  {
    fclose(in);
    if (out!=0) fclose(out);
    return RTT_UNSUPPORTED_INTYPE;
  }

  if (rtt_info->outname[0]=='-') fprintf(out,"\n");

  fclose(in);
  if (out!=0) fclose(out);

  return 0;
}



