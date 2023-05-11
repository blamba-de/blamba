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

/* eMelody for older Ericsson */

void write_emelody_header(FILE *out, struct rtt_info_t *rtt_info)
{

}

void write_emelody_note(FILE *out, struct rtt_info_t *rtt_info)
{
char outchars[5];

  if (rtt_info->scale==1)
  { fprintf(out,"+"); }
    else
  if (rtt_info->scale>=2)
  { fprintf(out,"++"); }

  if (rtt_info->tone==0) strcpy(outchars,"P");
    else
  if (rtt_info->tone==1) strcpy(outchars,"C");
    else
  if (rtt_info->tone==2) strcpy(outchars,"C#");
    else
  if (rtt_info->tone==3) strcpy(outchars,"D");
    else
  if (rtt_info->tone==4) strcpy(outchars,"D#");
    else
  if (rtt_info->tone==5) strcpy(outchars,"E");
    else
  if (rtt_info->tone==6) strcpy(outchars,"F");
    else
  if (rtt_info->tone==7) strcpy(outchars,"F#");
    else
  if (rtt_info->tone==8) strcpy(outchars,"G");
    else
  if (rtt_info->tone==9) strcpy(outchars,"G#");
    else
  if (rtt_info->tone==10) strcpy(outchars,"A");
    else
  if (rtt_info->tone==11) strcpy(outchars,"A#");
    else
  if (rtt_info->tone==12) strcpy(outchars,"B");
    else
  { strcpy(outchars,"C"); }

  if (rtt_info->length>=3) outchars[0]=tolower(outchars[0]);

  if (rtt_info->modifier>0) strcat(outchars,".");

  fprintf(out,"%s",outchars);
}

void write_emelody_footer(FILE *out, struct rtt_info_t *rtt_info)
{

}


