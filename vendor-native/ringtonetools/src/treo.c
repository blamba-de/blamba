#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "ringtonetools.h"
#include "fileoutput.h"

#define RECORD_SIZE 60000

/*

Ringtone Tools - Copyright 2001-2006 Michael Kohn (mike@mikekohn.net)
This falls under the Kohnian license.  Please read
it at http://ringtonetools.mikekohn.net/

This program is NOT opensourced.  You may not use any part
of this program for your own software.

*/

/* Handspring Treo output */

int write_treo_pdb(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
char name[32];
int ch,t,l,r;
int records;

  read_chars(in,name,4);

  if (strcasecmp(name,"MThd")!=0)
  {
    printf("Invalid MIDI file: %s\n",rtt_info->inname);
    return 1;
  }

  if (rtt_info->songname[0]==0)
  { strcpy(name,"Ringtone Tools"); }
    else
  { strcpy(name,rtt_info->songname); }

  l=strlen(name); 
  if (l>31) l=31;

  for (t=0; t<l; t++)
  { putc(name[t],out); }
  for (r=t; r<32; r++)
  { putc(0,out); }

  write_word_b(out,0x00);
  write_word_b(out,0x00);

  write_long_b(out,time(NULL));
  write_long_b(out,time(NULL));
  write_long_b(out,0);

  write_long_b(out,0);
  write_long_b(out,0);
  write_long_b(out,0);

  write_chars(out,"smfr");
  write_chars(out,"Kohn");

  write_long_b(out,0);
  write_long_b(out,0);

  fseek(in,0,SEEK_END);
  l=ftell(in);
  fseek(in,0,SEEK_SET);

  /* l=l+10; */
  l=l+6+strlen(rtt_info->songname);

  /* if (l<data_length) data_length=l; */

  records=l/RECORD_SIZE;

  if ((l%RECORD_SIZE)!=0) records++;

  r=78+(records*8);

  write_word_b(out,records);

  for (t=0; t<records; t++)
  {
    write_long_b(out,r);
    putc(0,out);
    putc(0,out);
    putc(0,out);
    putc(0,out);
    r=r+RECORD_SIZE;
  }

  write_chars(out,"PMrc");
  write_word_b(out,0x0b00);

/*
  rtt_info->songname[4]=0;
  for (t=0; t<4; t++)
  { if (rtt_info->songname[t]==0) rtt_info->songname[t]=' '; }
*/

  /* write_chars(out,"Kohn"); */
  write_chars(out,rtt_info->songname);
  putc(0,out);

  for (t=0; t<l; t++)
  {
    ch=getc(in);
    putc(ch,out);
  }

  return 0;
}



