#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ringtonetools.h"
#include "fileoutput.h"

/*

Ringtone Tools - Copyright 2001-2006 Michael Kohn (mike@mikekohn.net)
This falls under the Kohnian license.  Please read
it at http://ringtonetools.mikekohn.net/

This program is NOT opensourced.  You may not use any part
of this program for your own software.

*/

/* Siemans supporting SEO (bmp and midi) */

int write_seo(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
FILE *fp;
int file_size;
int packets=0,n=0,ptr=0,datasize,headsize,num=1;
int max,i;
char filename[1024];
char objectname[1024];
unsigned int referenceid;

  fseek(in,0,SEEK_END);
  file_size=ftell(in);
  rewind(in);

  referenceid=time(NULL);

  if (file_size==0) return 0;

  /* n=strlen(rtt_info->outname); */
  n=strlen(rtt_info->inname);
  while(n>=0 && (rtt_info->inname[n]!='/' && rtt_info->inname[n]!='\\')) n--;
  if (n<0) n=0;
  strcpy(objectname,&rtt_info->inname[n]);

  datasize=rtt_info->full_sms_size-24-strlen(objectname)-1;
  packets=file_size/datasize;
  if (file_size%datasize!=0) packets++;

  headsize=rtt_info->full_sms_size-datasize;

  n=strlen(rtt_info->outname);
  while(n>0 && rtt_info->outname[n]!='.') n--;
  rtt_info->outname[n]=0;
  max=0;

  ptr=0;
  while(ptr<file_size)
  {
    sprintf(filename,"%s%d.seo",rtt_info->outname,num);
    fprintf(out,"%s\n",filename);
    fp=fopen(filename,"wb");

    if (datasize+ptr>file_size) datasize=file_size-ptr;

    fprintf(fp,"//SEO%c",1);
    if (max==0)
    { write_word(fp,datasize); }
      else
    { write_word(fp,max); }
    write_long(fp,referenceid);
    write_word(fp,num);
    write_word(fp,packets);
    write_long(fp,file_size);
    if (rtt_info->in_type==RTT_TYPE_BMP)
    { fprintf(fp,"%cbmp",3); }
      else
    if (rtt_info->in_type==RTT_TYPE_MIDI)
    { fprintf(fp,"%cmid",3); }
    putc(strlen(objectname),fp);
    fprintf(fp,"%s",objectname);

    for (n=0; n<datasize; n++)
    {
      putc(getc(in),fp);
    }

    if (max==0) max=datasize;
      else
    {
      for (i=n; n<max; n++) putc(0,fp);
    }

    fclose(fp);
    ptr=ptr+datasize;
    num++;
  }

  return 0;
}


