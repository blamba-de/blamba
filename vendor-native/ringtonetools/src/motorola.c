#include <stdio.h>
#include <stdlib.h>

#include "ringtonetools.h"
#include "fileoutput.h"

/*

Ringtone Tools - Copyright 2001-2006 Michael Kohn (mike@mikekohn.net)
This falls under the Kohnian license.  Please read
it at http://ringtonetools.mikekohn.net/

This program is NOT opensourced.  You may not use any part
of this program for your own software.

*/

/* Motorola */

void write_mot_header(FILE *out, struct rtt_info_t *rtt_info)
{
char outchars[10];
int tempo;

  if (rtt_info->bpm<=110)
  { tempo=3; }
    else
  { tempo=4; }

  sprintf(outchars,"L35&%d ",tempo);
  write_chars(out,outchars);

  rtt_info->mcc=0;
}

void write_mot_note(FILE *out, struct rtt_info_t *rtt_info)
{
char outnote[10];
char notes[]={ 'R','C','C','D','D','E','F','F','G','G','A','A','B' };
char flats[]={  0 , 0,  1,  0,  1, -1,  0,  1,  0,  1,  0,  1, -1  };
char flmap[]={  0 , 1,  1,  2,  2, -1,  4,  4,  8,  8, 16, 16, -1  };
int t,d;

  t=0;

  if (rtt_info->note_count<35)
  {
    outnote[t++]=notes[rtt_info->tone];
    if (flats[rtt_info->tone]!=-1)
    {
      if (((rtt_info->flats&flmap[rtt_info->tone])==0 && flats[rtt_info->tone]==1) ||
          ((rtt_info->flats&flmap[rtt_info->tone])!=0 && flats[rtt_info->tone]==0))
      {
        outnote[t++]='#';
        rtt_info->flats^=flmap[rtt_info->tone];
      }
    }

    if (rtt_info->scale!=0)
    { outnote[t++]='+'; }

    d=5-rtt_info->length;
    outnote[t++]=d+'0';

    outnote[t]=0;

#ifdef DEBUG
printf("%s",outnote);
#endif

    fprintf(out,"%s",outnote);
    for (d=0; d<t; d++) rtt_info->mcc=rtt_info->mcc^outnote[d];
  }
}

void write_mot_footer(FILE *out, struct rtt_info_t *rtt_info)
{
char outchars[20];

  sprintf(outchars,"&&%c%c",((rtt_info->mcc>>4)+0x30),((rtt_info->mcc&15)+0x30));
  write_chars(out,outchars);

  if (rtt_info->note_count>35)
  {
    if (quiet==0)
    {
      printf("Warning: Motorola's format only supports 35 notes.\n");
      printf("         This song has been truncated.\n");
    }
  }
}

void write_mot_key_header(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->songname[0]!=0)
  {
#ifdef DEBUG
    fprintf(out,"%s:",rtt_info->songname);
#endif
  }
}

void write_mot_key_note(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->note_count!=0) fprintf(out," ");

  if (rtt_info->tone!=0)
  {
    if (rtt_info->scale==0)
    { fprintf(out,"1"); }
      else
    if (rtt_info->scale==1)
    { fprintf(out,"11"); }
      else
    { fprintf(out,"111"); }
  }

  if (rtt_info->tone==0) fprintf(out,"44");
    else
  if (rtt_info->tone==1) fprintf(out,"222");
    else
  if (rtt_info->tone==2) fprintf(out,"2227");
    else
  if (rtt_info->tone==3) fprintf(out,"3");
    else
  if (rtt_info->tone==4) fprintf(out,"37");
    else
  if (rtt_info->tone==5) fprintf(out,"33");
    else
  if (rtt_info->tone==6) fprintf(out,"333");
    else
  if (rtt_info->tone==7) fprintf(out,"3337");
    else
  if (rtt_info->tone==8) fprintf(out,"4");
    else
  if (rtt_info->tone==9) fprintf(out,"47");
    else
  if (rtt_info->tone==10) fprintf(out,"2");
    else
  if (rtt_info->tone==11) fprintf(out,"27");
    else
  if (rtt_info->tone==12) fprintf(out,"22");

  if (rtt_info->length<2) fprintf(out,"w");
    else
  if (rtt_info->length==2) fprintf(out,"h");
    else
  { fprintf(out,"q"); }
}

void write_mot_key_footer(FILE *out, struct rtt_info_t *rtt_info)
{

}


