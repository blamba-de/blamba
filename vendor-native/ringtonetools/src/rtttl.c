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

/* RTTTL and RTX */

#define DEFAULT_LENGTH 2

void write_rtttl_header(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->songname[0]==0)
  { strcpy(rtt_info->songname,"unamed"); }

  if (rtt_info->out_type==RTT_TYPE_RTTTL)
  { fprintf(out,"%s:d=%d,o=5,b=%d:",rtt_info->songname,1<<DEFAULT_LENGTH,rtt_info->bpm); }
    else
  {
    if (rtt_info->loop==0)
    { fprintf(out,"%s:d=%d,o=4,b=%d:",rtt_info->songname,1<<DEFAULT_LENGTH,rtt_info->bpm); }
      else
    { fprintf(out,"%s:d=%d,o=4,b=%d,l=%d:",rtt_info->songname,1<<DEFAULT_LENGTH,rtt_info->bpm,rtt_info->loop); }
  }
}

void write_rtttl_note(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->note_count>0) fprintf(out,",");

  if (rtt_info->length!=DEFAULT_LENGTH) fprintf(out,"%d",(1<<(rtt_info->length)));

  if (rtt_info->tone==0) fprintf(out,"p");
    else
  if (rtt_info->tone==1) fprintf(out,"c");
    else
  if (rtt_info->tone==2) fprintf(out,"c#");
    else
  if (rtt_info->tone==3) fprintf(out,"d");
    else
  if (rtt_info->tone==4) fprintf(out,"d#");
    else
  if (rtt_info->tone==5) fprintf(out,"e");
    else
  if (rtt_info->tone==6) fprintf(out,"f");
    else
  if (rtt_info->tone==7) fprintf(out,"f#");
    else
  if (rtt_info->tone==8) fprintf(out,"g");
    else
  if (rtt_info->tone==9) fprintf(out,"g#");
    else
  if (rtt_info->tone==10) fprintf(out,"a");
    else
  if (rtt_info->tone==11) fprintf(out,"a#");
    else
  if (rtt_info->tone==12) fprintf(out,"h");

  if (rtt_info->tone!=0)
  {
    if (rtt_info->out_type==RTT_TYPE_RTTTL)
    { if (rtt_info->scale!=0) fprintf(out,"%d",rtt_info->scale+5); }
      else
    { if (rtt_info->scale!=0) fprintf(out,"%d",rtt_info->scale+4); }
  }

  if (rtt_info->modifier!=0) fprintf(out,".");
}

void write_rtttl_footer(FILE *out, struct rtt_info_t *rtt_info)
{
  /* fprintf(out,";"); */
}

void write_rtx_bpm(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->note_count>0) fprintf(out,",");
  fprintf(out,"b=%d",rtt_info->bpm);
}


