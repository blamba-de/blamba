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

int main (int argc, char *argv[])
{
char message[1024];
int t,n,flag=0;
struct rtt_info_t rtt_info;
int out_type,in_type;

  rtt_init_info(&rtt_info);

  message[0]=0;

  out_type=RTT_TYPE_UNDEFINED;
  in_type=RTT_TYPE_UNDEFINED;

  if (argc<2)
  {
    printf("\nRingtonetools "VERSION" ("DATE_RELEASED")\n");
    printf("Convert and create ringtones/logos for mobile phones.\n");
    printf(COPYRIGHT);
    printf("Usage: ringtonetools [ options ] <input filename> <output filename>\n");
    printf("       -intype <input type> [ 3210/bmp/emelody,emy/ems/gif/ico,icon/imelody,imy/kws/midi,mid/morse/nol,ngg/nokia,sckl,ott,rng/rtttl/rtx/siemens/text,txt/wav/wbmp ]\n");
    printf("       -outtype <output type> [ 3210/bmp/dsp/emelody,emy/ems/gif/ico,icon/imelody,imy/kws/midi,mid/mot/nol,ngg/nokia,sckl,ott,rng/pdb/rtttl/rtx/samsung1/samsung2/siemens/seo/text,txt/wav/wbmp ]\n");
    printf("       -name <song name>\n");
    printf("       -t <tempo [default: %d]>\n",rtt_info.bpm);
    printf("       -quiet\n");
    printf("   iMelody options:\n");
    printf("       -noheaders [ no headers ]\n");
    printf("       -melheaders [ melody header only <default> ]\n");
    printf("       -lessheaders [ begin,melody,end headers only ]\n");
    printf("       -mostheaders [ begin,melody,end,name,tempo headers on iMelody ]\n");
    printf("       -stdheaders [ begin,melody,end,name,tempo,style headers on iMelody ]\n");
    printf("       -fullheaders [ full headers on iMelody ]\n");
    printf("       -ems [ force EMS ]\n");
    printf("   Kyeocera KWS options:\n");
    printf("       -pause <number of quarter notes added to end of song [ default: 2 ]>\n");
    printf("   Motorola options:\n");
    printf("       -keypress [ Make keypress sequence for phones like v60t ]\n");
    printf("   wav output options:\n");
    printf("       -b <bits 8 or 16 for wav only [default: 16]>\n");
    printf("       -f <sampling frequency for wav only [default: 44100]>\n");
    printf("   graphics options:\n");
    printf("       -h <height> [ Height of Logo (default %d) ]\n",rtt_info.height);
    printf("       -w <width> [ Width of Logo (default %d) ]\n",rtt_info.width);
    printf("       -l <MCC> <MNC> [ Operator Logo (requires MCC and MNC) ]\n");
    printf("       -m <message> [ Send a picture message ]\n");
    printf("       -r [ reverse black n white ]\n");
    printf("       -c [ use full color if possible ]\n");
    printf("       -trans <0xrrggbb> [ make this hex code a transparent color  ]\n");
    printf("       -bmptrans [ Use BMP's transparency ]\n");
    printf("   nokia options:\n");
    printf("       -x [ exclude //SCKL header ]\n");
    printf("       -u [ only user-data-header (8bit) ]\n");
    printf("       -k [ include user-data-header and //SCKL ]\n");
    printf("       -s <sms size> [ Maximum size of SMS message (default %d)\n",SMS_SIZE);
    printf("       -transpose <pos or neg num > [ raise or lower song by n octaves ]\n");
    printf("       -ss Send as screensaver\n");
    printf("   midi options:\n");
    printf("       -channel [ channel ]\n");
    printf("       -track [ track ]\n");
#ifdef DSP
    printf("   sound output:\n");
    printf("       -dsp (write out to the sound device)\n");
#endif

    exit(0);
  }

  n=0;

  for (t=1; t<argc; t++)
  {
    if (strcmp(argv[t],"-a")==0)
    { rtt_info.ats=0; }
      else
    if (strcmp(argv[t],"-t")==0)
    { rtt_info.bpm=atoi(argv[++t]); }
      else
    if (strcmp(argv[t],"-f")==0)
    { rtt_info.sample_rate=atoi(argv[++t]); }
      else
    if (strcmp(argv[t],"-r")==0)
    { rtt_info.bmp_flags^=1; }
      else
    if (strcmp(argv[t],"-c")==0)
    { rtt_info.bmp_flags^=4; }
      else
    if (strcmp(argv[t],"-trans")==0)
    { rtt_info.trans=convcolor(argv[++t]); }
      else
    if (strcmp(argv[t],"-bmptrans")==0)
    { rtt_info.trans=-2; }
      else
    if (strcmp(argv[t],"-h")==0)
    {
      if (t+1<argc)
      {
        rtt_info.height=atoi(argv[++t]);
        if (rtt_info.height>MAX_WIDTH) rtt_info.height=MAX_WIDTH;
      }
    }
      else
    if (strcmp(argv[t],"-w")==0)
    {
      rtt_info.width=atoi(argv[++t]);
      if (rtt_info.width>MAX_WIDTH) rtt_info.width=MAX_WIDTH;
    }
      else
    if (strcmp(argv[t],"-ems")==0)
    { rtt_info.ems=1; }
      else
    if (strcmp(argv[t],"-noheaders")==0)
    { rtt_info.headers=RTT_HEADERS_NONE; }
      else
    if (strcmp(argv[t],"-melheaders")==0)
    {
      printf("Warning: -melheaders is the default\n");
      rtt_info.headers=RTT_HEADERS_DEFAULT;
    }
      else
    if (strcmp(argv[t],"-lessheaders")==0)
    { rtt_info.headers=RTT_HEADERS_LESS; }
      else
    if (strcmp(argv[t],"-mostheaders")==0)
    { rtt_info.headers=RTT_HEADERS_MOST; }
      else
    if (strcmp(argv[t],"-stdheaders")==0)
    { rtt_info.headers=RTT_HEADERS_STD; }
      else
    if (strcmp(argv[t],"-fullheaders")==0)
    { rtt_info.headers=RTT_HEADERS_FULL; }
      else
    if (strcmp(argv[t],"-quiet")==0)
    { quiet=1; }
      else
    if (strcmp(argv[t],"-x")==0)
    { rtt_info.headers=RTT_HEADERS_NONE; }
      else
    if (strcmp(argv[t],"-k")==0)
    { rtt_info.headers=RTT_HEADERS_LESS; }
      else
    if (strcmp(argv[t],"-u")==0)
    { rtt_info.headers=RTT_HEADERS_MOST; }
      else
    if (strcmp(argv[t],"-ss")==0)
    { rtt_info.bmp_flags^=2; }
      else
    if (strcmp(argv[t],"-pause")==0)
    { rtt_info.pause=atoi(argv[++t]); }
      else
    if (strcmp(argv[t],"-keypress")==0)
    { flag=1; }
      else
    if (strcmp(argv[t],"-s")==0)
    { rtt_info.full_sms_size=atoi(argv[++t]); }
      else
    if (strcmp(argv[t],"-channel")==0)
    { rtt_info.mcc=atoi(argv[++t]); }
      else
    if (strcmp(argv[t],"-track")==0)
    { rtt_info.mnc=atoi(argv[++t]); }
      else
    if (strcmp(argv[t],"-b")==0)
    { 
      rtt_info.bytes=atoi(argv[++t]);
      if (rtt_info.bytes==16)
      { rtt_info.bytes=2; }
        else
      { rtt_info.bytes=1; }
    }
      else
    if (strcmp(argv[t],"-l")==0)
    {
      rtt_info.mcc=atoi(argv[++t]);
      rtt_info.mnc=atoi(argv[++t]);
    }
      else
    if (strcmp(argv[t],"-transpose")==0)
    {
      rtt_info.transpose=atoi(argv[++t]);
    }
      else
    if (strcmp(argv[t],"-m")==0)
    { 
      strcpy(message,argv[++t]);
      rtt_info.message=message;
    }
      else
    if (strcmp(argv[t],"-name")==0)
    { 
      strncpy(rtt_info.songname,argv[++t],SONGNAME_LEN);
      rtt_info.songname[SONGNAME_LEN-1]=0;
    }
      else
#ifdef DSP
    if (strcmp(argv[t],"-dsp")==0)
    {
      out_type=RTT_TYPE_DSP;
      strcpy(rtt_info.outname,"/dev/dsp");
    }
      else
#endif
    if (strcmp(argv[t],"-outtype")==0)
    {
      t++;
      out_type=rtt_get_outtype(&rtt_info,argv[t]);
    }
      else
    if (strcmp(argv[t],"-intype")==0)
    {
      t++;
      in_type=rtt_get_intype(&rtt_info,argv[t]);
    }
      else
    if (argv[t][0]=='-' && !(argv[t][1]=='.' || argv[t][1]==0))
    {
      printf("Unknown option %s.  Exiting...\n",argv[t]);
      exit(1);
    }
      else
    if (n==0)
    { strncpy(rtt_info.inname,argv[t],1023); n++; }
      else
    { strcpy(rtt_info.outname,argv[t]); }
  }

  if (out_type==RTT_TYPE_UNDEFINED && rtt_info.outname[0]!=0)
  {
    t=strlen(rtt_info.outname);
    while(t>0 && rtt_info.outname[t]!='.') t--;

    if (rtt_info.outname[t]=='.')
    {
      t++;
      out_type=rtt_get_outtype(&rtt_info,&rtt_info.outname[t]);
    }
  }

  if (out_type==RTT_TYPE_UNDEFINED) out_type=RTT_TYPE_RTTTL;
  if (out_type==RTT_TYPE_MOT && (flag&1)==1) out_type=RTT_TYPE_MOT_KEYPRESS;

#ifdef DSP
  if (strncmp(rtt_info.outname,"/dev/dsp",sizeof("/dev/dsp"))==0)
  { out_type=RTT_TYPE_DSP; }
#endif

#ifndef DSP
  if (out_type==RTT_TYPE_DSP)
  { printf("No DSP support compiled in\n"); exit(1); }
#endif

  rtt_info.in_type=in_type;
  rtt_info.out_type=out_type;

  if (rtt_info.outname[0]==0)
  {
    strcpy(rtt_info.outname,rtt_info.inname);

    t=strlen(rtt_info.outname);

    while(t>0)
    {
      if (rtt_info.outname[t]=='.')
      {
        rtt_info.outname[t+1]=0;
        break;
      }
      t--;
    }

    rtt_add_extension_to_file(&rtt_info);
  }

  if (quiet==0)
  {
    printf("\nRingtonetools "VERSION" ("DATE_RELEASED")\n");
    printf(COPYRIGHT);

    printf(" Infile: %s\n",rtt_info.inname);
    printf("Outfile: %s\n",rtt_info.outname);
  }

  rtt_convert(&rtt_info);

  return (0);
}


