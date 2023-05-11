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

/* 3210 */

void write_3210_header(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->songname[0]==0)
  { strcpy(rtt_info->songname,"unamed"); }

  fprintf(out,"%s:Tempo=%d\n",rtt_info->songname,rtt_info->bpm);
}

void write_3210_note(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->note_count>0) fprintf(out," ");

  fprintf(out,"%d",(1<<(rtt_info->length)));

  if (rtt_info->modifier!=0) fprintf(out,".");

  if (rtt_info->tone==0) fprintf(out,"-");
    else
  if (rtt_info->tone==1) fprintf(out,"c");
    else
  if (rtt_info->tone==2) fprintf(out,"#c");
    else
  if (rtt_info->tone==3) fprintf(out,"d");
    else
  if (rtt_info->tone==4) fprintf(out,"#d");
    else
  if (rtt_info->tone==5) fprintf(out,"e");
    else
  if (rtt_info->tone==6) fprintf(out,"f");
    else
  if (rtt_info->tone==7) fprintf(out,"#f");
    else
  if (rtt_info->tone==8) fprintf(out,"g");
    else
  if (rtt_info->tone==9) fprintf(out,"#g");
    else
  if (rtt_info->tone==10) fprintf(out,"a");
    else
  if (rtt_info->tone==11) fprintf(out,"#a");
    else
  if (rtt_info->tone==12) fprintf(out,"b");
 /* if (rtt_info->tone==12) fprintf(out,"h"); */

  if (rtt_info->tone!=0)
  {
    fprintf(out,"%d",rtt_info->scale+1);
  }
}

void write_3210_footer(FILE *out, struct rtt_info_t *rtt_info)
{
  fprintf(out,"\n");
}

