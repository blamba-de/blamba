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

int rtt_set_filenames(struct rtt_info_t *rtt_info, char *input_filename, char *output_filename)
{

  strcpy(rtt_info->inname,input_filename);
  strcpy(rtt_info->outname,output_filename);

  return 0;
}

/* struct rtt_info_t *rtt_info note_alloc() */
int rtt_info_alloc()
{
  /* return (struct rtt_info_t*)malloc(sizeof(struct rtt_info_t)); */
  return (int)malloc(sizeof(struct rtt_info_t));
}

int rtt_info_free(struct rtt_info_t *rtt_info)
{

  free(rtt_info);

  return 0;
}

int rtt_set_logo_dims(struct rtt_info_t *rtt_info, int width, int height)
{
  rtt_info->width=width;
  rtt_info->height=height;

  return 0;
}

int rtt_set_types(struct rtt_info_t *rtt_info, int in_type, int out_type)
{
  rtt_info->in_type=in_type;
  rtt_info->out_type=out_type;

  return 0;
}

int rtt_set_tempo(struct rtt_info_t *rtt_info, int bpm)
{
  rtt_info->bpm=bpm;

  return 0;
}

int rtt_set_name(struct rtt_info_t *rtt_info, char *songname)
{
  strncpy(rtt_info->songname,songname,SONGNAME_LEN);
  rtt_info->songname[SONGNAME_LEN-1]=0;

  return 0;
}

int rtt_set_pause(struct rtt_info_t *rtt_info, int pause)
{
  rtt_info->pause=pause;

  return 0;
}

int rtt_set_wav_options(struct rtt_info_t *rtt_info, int bits_per_sample, int sample_rate)
{
  rtt_info->bytes=bits_per_sample/8;
  rtt_info->sample_rate=sample_rate;

  return 0;
}

int rtt_set_mcc_mnc(struct rtt_info_t *rtt_info, int mcc, int mnc)
{
  rtt_info->mcc=mcc;
  rtt_info->mnc=mnc;

  return 0;
}

int rtt_transpose(struct rtt_info_t *rtt_info, int transpose)
{
  rtt_info->trans=transpose;

  return 0;
}

int rtt_set_sms_size(struct rtt_info_t *rtt_info, int sms_size)
{
  rtt_info->full_sms_size=sms_size;

  return 0;
}

int rtt_set_volume(struct rtt_info_t *rtt_info, int volume)
{
  rtt_info->volume=volume;

  return 0;
}

int rtt_set_midi_options(struct rtt_info_t *rtt_info, int channel, int track)
{
  rtt_info->mcc=channel;
  rtt_info->mnc=track;

  return 0;
}

int rtt_set_header_options(struct rtt_info_t *rtt_info, int header_option)
{
  rtt_info->headers=header_option;

  return 0;
}

int rtt_get_version(char *version_str)
{
  sprintf(version_str,"%s - %s",VERSION,DATE_RELEASED);

  return 0;
}



