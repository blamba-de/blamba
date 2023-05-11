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

/* Siemens Older Models */

void write_siemens_header(FILE *out, struct rtt_info_t *rtt_info)
{
  rtt_info->bytes=0;
}

void write_siemens_note(FILE *out, struct rtt_info_t *rtt_info)
{
char outchars[20];
char temp[20];

  outchars[0]=0;

  if (rtt_info->tone==0) strcpy(outchars,"P");
    else
  if (rtt_info->tone==1) strcpy(outchars,"C");
    else
  if (rtt_info->tone==2) strcpy(outchars,"Cis");
    else
  if (rtt_info->tone==3) strcpy(outchars,"D");
    else
  if (rtt_info->tone==4) strcpy(outchars,"Dis");
    else
  if (rtt_info->tone==5) strcpy(outchars,"E");
    else
  if (rtt_info->tone==6) strcpy(outchars,"F");
    else
  if (rtt_info->tone==7) strcpy(outchars,"Fis");
    else
  if (rtt_info->tone==8) strcpy(outchars,"G");
    else
  if (rtt_info->tone==9) strcpy(outchars,"Gis");
    else
  if (rtt_info->tone==10) strcpy(outchars,"A");
    else
  if (rtt_info->tone==11) strcpy(outchars,"Ais");
    else
  if (rtt_info->tone==12) strcpy(outchars,"H");
    else
  { strcpy(outchars,"0"); }

  if (rtt_info->tone!=0)
  {
    sprintf(temp,"%d",(rtt_info->scale+1));
    strcat(outchars,temp);
  }

  sprintf(temp,"(1/%d) ",1<<rtt_info->length);
  strcat(outchars,temp);

  fprintf(out,"%s",outchars);

  rtt_info->bytes=rtt_info->bytes+strlen(outchars);

  if (rtt_info->bytes>70)
  {
    fprintf(out,"\n");
    rtt_info->bytes=0;
  }
}

void write_siemens_footer(FILE *out, struct rtt_info_t *rtt_info)
{
  fprintf(out,"\n");
}


