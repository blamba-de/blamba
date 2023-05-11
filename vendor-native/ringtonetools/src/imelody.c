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

/* iMelody for newer Ericsson, Motorola, Siemens and others */

void write_imelody_header(FILE *out, struct rtt_info_t *rtt_info)
{
char header[2048];
char temp[128];

  header[0]=0;

  if (rtt_info->headers>=2)
  {
    strcat(header,"BEGIN:IMELODY\r\n");
  }

  if (rtt_info->headers>=5)
  {
    strcat(header,"VERSION:1.0\r\n");
    strcat(header,"FORMAT:CLASS 1.0\r\n");
  }

  if (rtt_info->headers>=3)
  {
    sprintf(temp,"NAME:%s\r\n",rtt_info->songname);
    strcat(header,temp);
    sprintf(temp,"BEAT:%d\r\n",rtt_info->bpm);
    strcat(header,temp);
  }

  if (rtt_info->headers>=4)
  {
    sprintf(temp,"STYLE:S%d\r\n",rtt_info->style);
    strcat(header,temp);
  }

  if (rtt_info->headers>=1)
  {
    strcat(header,"MELODY:");
  }

  if (rtt_info->ems==0)
  { fprintf(out,header); }
    else
  {
    write_ems(rtt_info,header);
  }

  rtt_info->prev_scale=1;
}

void write_imelody_note(FILE *out, struct rtt_info_t *rtt_info)
{
char outchars[16];
char temp[16];

  outchars[0]=0;

  /* if (rtt_info->prev_scale!=rtt_info->scale) */

  if (rtt_info->tone!=0)
  {
    sprintf(outchars,"*%d",rtt_info->scale+3);
    rtt_info->prev_scale=rtt_info->scale;
  }

  if (rtt_info->tone==0) strcat(outchars,"r");
    else
  if (rtt_info->tone==1) strcat(outchars,"c");
    else
  if (rtt_info->tone==2) strcat(outchars,"#c");
    else
  if (rtt_info->tone==3) strcat(outchars,"d");
    else
  if (rtt_info->tone==4) strcat(outchars,"#d");
    else
  if (rtt_info->tone==5) strcat(outchars,"e");
    else
  if (rtt_info->tone==6) strcat(outchars,"f");
    else
  if (rtt_info->tone==7) strcat(outchars,"#f");
    else
  if (rtt_info->tone==8) strcat(outchars,"g");
    else
  if (rtt_info->tone==9) strcat(outchars,"#g");
    else
  if (rtt_info->tone==10) strcat(outchars,"a");
    else
  if (rtt_info->tone==11) strcat(outchars,"#a");
    else
  if (rtt_info->tone==12) strcat(outchars,"b");
    else
  { strcat(outchars,"c"); }

  if (rtt_info->length<6)
  { sprintf(temp,"%d",rtt_info->length); }
    else
  { sprintf(temp,"5"); }
  strcat(outchars,temp);

  if (rtt_info->modifier==1) strcat(outchars,".");
    else
  if (rtt_info->modifier==2) strcat(outchars,":");
    else
  if (rtt_info->modifier==3) strcat(outchars,";");

  if (rtt_info->ems==0)
  { fprintf(out,"%s",outchars); }
    else
  { write_ems(rtt_info,outchars); }
}

void write_imelody_footer(FILE *out, struct rtt_info_t *rtt_info)
{
char header[64];

  header[0]=0;

  strcat(header,"\r\n");

  if (rtt_info->headers>=2)
  {
    strcat(header,"END:IMELODY\r\n");
  }

  if (rtt_info->ems==0)
  { fprintf(out,"%s",header); }
    else
  {
    write_ems(rtt_info,header);
    write_ems_footer(out,rtt_info);
  }
}


