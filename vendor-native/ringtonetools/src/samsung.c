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

/* Samsung Older Models */

void write_samsung_header(FILE *out, struct rtt_info_t *rtt_info)
{
  rtt_info->bytes=0;
}

void write_samsung1_note(FILE *out, struct rtt_info_t *rtt_info)
{
char outchars[20];

  outchars[0]=0;

  if (rtt_info->tone==0) strcpy(outchars,"0");
    else
  if (rtt_info->tone==1) strcpy(outchars,"1");
    else
  if (rtt_info->tone==2) strcpy(outchars,"1#");
    else
  if (rtt_info->tone==3) strcpy(outchars,"2");
    else
  if (rtt_info->tone==4) strcpy(outchars,"2#");
    else
  if (rtt_info->tone==5) strcpy(outchars,"3");
    else
  if (rtt_info->tone==6) strcpy(outchars,"4");
    else
  if (rtt_info->tone==7) strcpy(outchars,"4#");
    else
  if (rtt_info->tone==8) strcpy(outchars,"5");
    else
  if (rtt_info->tone==9) strcpy(outchars,"5#");
    else
  if (rtt_info->tone==10) strcpy(outchars,"6");
    else
  if (rtt_info->tone==11) strcpy(outchars,"6#");
    else
  if (rtt_info->tone==12) strcpy(outchars,"7");
    else
  { strcpy(outchars,"0"); }

  if (rtt_info->tone!=0)
  {
    if (rtt_info->transpose==-1)
    {
      if (rtt_info->scale==0) strcat(outchars,"88");
        else
      if (rtt_info->scale==2) strcat(outchars,"8");
    }
      else
    {
      if (rtt_info->scale==1) strcat(outchars,"8");
    }
  }

  if (rtt_info->length==0) strcat(outchars,"**");
    else
  if (rtt_info->length==1) strcat(outchars,"*");
    else
  if (rtt_info->length==3) strcat(outchars,"****");
    else
  if (rtt_info->length==4) strcat(outchars,"***");
    else
  if (rtt_info->length==5) strcat(outchars,"***");

  if (rtt_info->modifier!=0)
  {
    if (rtt_info->length==0) strcat(outchars," 0*");
      else
    if (rtt_info->length==1) strcat(outchars," 0");
      else
    if (rtt_info->length==2) strcat(outchars," 0****");
      else
    if (rtt_info->length==3) strcat(outchars," 0***");
      else
    if (rtt_info->length==4) strcat(outchars," 0***");
      else
    if (rtt_info->length==5) strcat(outchars," 0***");
  }

  strcat(outchars," ");
  fprintf(out,"%s",outchars);

  rtt_info->bytes=rtt_info->bytes+strlen(outchars);

  if (rtt_info->bytes>70)
  {
    fprintf(out,"\n");
    rtt_info->bytes=0;
  }
}

void write_samsung2_note(FILE *out, struct rtt_info_t *rtt_info)
{
char outchars[20];

  outchars[0]=0;

  if (rtt_info->tone==1) strcpy(outchars,"1");
    else
  if (rtt_info->tone==2) strcpy(outchars,"1^");
    else
  if (rtt_info->tone==3) strcpy(outchars,"2");
    else
  if (rtt_info->tone==4) strcpy(outchars,"2^");
    else
  if (rtt_info->tone==5) strcpy(outchars,"3");
    else
  if (rtt_info->tone==6) strcpy(outchars,"4");
    else
  if (rtt_info->tone==7) strcpy(outchars,"4^");
    else
  if (rtt_info->tone==8) strcpy(outchars,"5");
    else
  if (rtt_info->tone==9) strcpy(outchars,"5^");
    else
  if (rtt_info->tone==10) strcpy(outchars,"6");
    else
  if (rtt_info->tone==11) strcpy(outchars,"6^");
    else
  if (rtt_info->tone==12) strcpy(outchars,"7");
    else
  if (rtt_info->tone!=0)
  { strcpy(outchars,"1"); rtt_info->tone=1; }

  if (rtt_info->tone!=0)
  {
    if (rtt_info->transpose==-1)
    {
      if (rtt_info->scale==0) strcat(outchars,"88");
        else
      if (rtt_info->scale==2) strcat(outchars,"8");
    }
      else
    {
      if (rtt_info->scale==1) strcat(outchars,"8");
    }

    if (rtt_info->length==0) strcat(outchars,"<<<<");
      else
    if (rtt_info->length==1) strcat(outchars,"<<<");
      else
    if (rtt_info->length==3) strcat(outchars,"<");
      else
    if (rtt_info->length==4) strcat(outchars,"<<");
      else
    if (rtt_info->length==5) strcat(outchars,"<<");

    if (rtt_info->modifier!=0)
    {
      if (rtt_info->length==0) strcat(outchars," >>>");
        else
      if (rtt_info->length==1) strcat(outchars," 0");
        else
      if (rtt_info->length==2) strcat(outchars," >");
        else
      if (rtt_info->length==3) strcat(outchars," >>");
        else
      if (rtt_info->length==4) strcat(outchars," >>");
        else
      if (rtt_info->length==5) strcat(outchars," >>");
    }
  }
    else
  {
    if (rtt_info->length==0) strcat(outchars,">>>>");
      else
    if (rtt_info->length==1) strcat(outchars,">>>");
      else
    if (rtt_info->length==2) strcat(outchars,"0");
      else
    if (rtt_info->length==3) strcat(outchars,">");
      else
    if (rtt_info->length==4) strcat(outchars,">>");
      else
    if (rtt_info->length==5) strcat(outchars,">>");
  }
  strcat(outchars," ");
  fprintf(out,"%s",outchars);

  rtt_info->bytes=rtt_info->bytes+strlen(outchars);

  if (rtt_info->bytes>70)
  {
    fprintf(out,"\n");
    rtt_info->bytes=0;
  }
}

void write_samsung_footer(FILE *out, struct rtt_info_t *rtt_info)
{
  fprintf(out,"\n");
}


