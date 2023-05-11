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

int write_long(FILE *out, int n)
{
  putc((n&255),out);
  putc(((n>>8)&255),out);
  putc(((n>>16)&255),out);
  putc(((n>>24)&255),out);

  return 0;
}

int write_word(FILE *out, int n)
{
  putc((n&255),out);
  putc(((n>>8)&255),out);

  return 0;
}

int read_long(FILE *in)
{
int c;

  c=getc(in);
  c=c|(getc(in)<<8);
  c=c|(getc(in)<<16);
  c=c|(getc(in)<<24);

  return c;
}

int read_word(FILE *in)
{
int c;

  c=getc(in);
  c=c|(getc(in)<<8);

  return c;
}

int write_long_b(FILE *out, int n)
{
  putc(((n>>24)&255),out);
  putc(((n>>16)&255),out);
  putc(((n>>8)&255),out);
  putc((n&255),out);

  return 0;
}

int write_word_b(FILE *out, int n)
{
  putc(((n>>8)&255),out);
  putc((n&255),out);

  return 0;
}

int read_long_b(FILE *in)
{
int c;

  c=getc(in);
  c=(c<<8)|getc(in);
  c=(c<<8)|getc(in);
  c=(c<<8)|getc(in);

  return c;
}

int read_word_b(FILE *in)
{
int c;

  c=getc(in);
  c=(c<<8)|getc(in);

  return c;
}

int read_chars(FILE *in, char *s, int count)
{
int t;

  for (t=0; t<count; t++)
  {
    s[t]=getc(in);
  }

  s[t]=0;

  return 0;
}

int write_chars(FILE *out, char *s)
{
int t;

  t=0;
  while(s[t]!=0 && t<255)
  {
    putc(s[t++],out);
  }

  return 0;
}

/* Routing */

int header_route(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->out_type==RTT_TYPE_WAV)
  { write_wav_header(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_KWS)
  { write_kws_header(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_MOT)
  { write_mot_header(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_SCKL)
  { write_nokia_header(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_RTTTL || rtt_info->out_type==RTT_TYPE_RTX)
  { write_rtttl_header(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_SAMSUNG1 || rtt_info->out_type==RTT_TYPE_SAMSUNG2)
  { write_samsung_header(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_MIDI)
  { write_midi_header(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_SEO)
  { write_siemens_header(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_MOT_KEYPRESS)
  { write_mot_key_header(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_EMELODY)
  { write_emelody_header(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_IMELODY)
  { write_imelody_header(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_NOKIA3210)
  { write_3210_header(out,rtt_info); }
#ifdef DSP
    else
  if (rtt_info->out_type==RTT_TYPE_DSP)
  { write_dsp_header(out,rtt_info); }
#endif

  return 0;
}

int note_route(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->out_type==RTT_TYPE_WAV)
  { write_wav_note(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_KWS)
  { write_kws_note(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_MOT)
  { write_mot_note(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_SCKL)
  { write_nokia_note(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_RTTTL || rtt_info->out_type==RTT_TYPE_RTX)
  { write_rtttl_note(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_SAMSUNG1)
  { write_samsung1_note(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_SAMSUNG2)
  { write_samsung2_note(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_MIDI)
  { write_midi_note(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_SEO)
  { write_siemens_note(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_MOT_KEYPRESS)
  { write_mot_key_note(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_EMELODY)
  { write_emelody_note(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_IMELODY)
  { write_imelody_note(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_NOKIA3210)
  { write_3210_note(out,rtt_info); }
#ifdef DSP
    else
  if (rtt_info->out_type==RTT_TYPE_DSP)
  { write_dsp_note(out,rtt_info); }
#endif

  rtt_info->note_count++;

  return 0;
}

int footer_route(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->out_type==RTT_TYPE_WAV)
  { write_wav_footer(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_KWS)
  { write_kws_footer(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_MOT)
  { write_mot_footer(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_SCKL)
  { write_nokia_footer(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_RTTTL || rtt_info->out_type==RTT_TYPE_RTX)
  { write_rtttl_footer(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_SAMSUNG1 || rtt_info->out_type==RTT_TYPE_SAMSUNG2)
  { write_samsung_footer(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_MIDI)
  { write_midi_footer(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_SEO)
  { write_siemens_footer(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_MOT)
  { write_mot_key_footer(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_EMELODY)
  { write_emelody_footer(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_IMELODY)
  { write_imelody_footer(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_NOKIA3210)
  { write_3210_footer(out,rtt_info); }
#ifdef DSP
    else
  if (rtt_info->out_type==RTT_TYPE_DSP)
  { write_dsp_footer(out,rtt_info); }
#endif

  return 0;
}

int logo_header_route(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->width>MAX_WIDTH || rtt_info->height>MAX_HEIGHT)
  {
    if (quiet==0)
    { printf("Logos are limited to %d x %d.  Image is too big.\n",MAX_WIDTH,MAX_HEIGHT); }
    return -2;
  }

  rtt_info->picture=malloc(rtt_info->width*rtt_info->height*sizeof(int));
  memset(rtt_info->picture,0,rtt_info->width*rtt_info->height*sizeof(int));

  if (rtt_info->out_type==RTT_TYPE_SCKL || rtt_info->out_type==RTT_TYPE_OTT)
  { write_nokia_logo_header(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_EMS) write_ems_logo_header(out,rtt_info);
    else
  if (rtt_info->out_type==RTT_TYPE_BMP) write_bmp_header(out,rtt_info);
    else
  if (rtt_info->out_type==RTT_TYPE_TEXT) { }
    else
  if (rtt_info->out_type==RTT_TYPE_NGG || rtt_info->out_type==RTT_TYPE_NOL)
  { write_ngg_logo_header(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_WBMP) write_wbmp_header(out,rtt_info);
    else
  if (rtt_info->out_type==RTT_TYPE_ICO) write_icon_header(out,rtt_info);
    else
  if (rtt_info->out_type==RTT_TYPE_GIF) write_gif_header(out,rtt_info);
    else
  {
    if (quiet==0)
    { printf("This file cannot be converted to this output file type\n"); }

    return -1;
  }

  return 0;
}

int logo_footer_route(FILE *out, struct rtt_info_t *rtt_info)
{
int f,u,c;

  if ((rtt_info->bmp_flags&1)==1)
  {
    f=rtt_info->width*rtt_info->height;

    for(u=0; u<f; u++)
    {
      c=rtt_info->picture[u];
      c=(255-(c&255))+((255-((c>>8)&255))<<8)+((255-((c>>16)&255))<<16);
      rtt_info->picture[u]=c;
    }
  }

#ifdef DEBUG
debug_logo(rtt_info);
#endif

  if (rtt_info->out_type==RTT_TYPE_SCKL || rtt_info->out_type==RTT_TYPE_OTT)
  { write_nokia_logo_footer(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_EMS) write_ems_logo_footer(out,rtt_info);
    else
  if (rtt_info->out_type==RTT_TYPE_BMP) write_bmp_footer(out,rtt_info);
    else
  if (rtt_info->out_type==RTT_TYPE_TEXT) write_text_footer(out,rtt_info);
    else
  if (rtt_info->out_type==RTT_TYPE_NGG || rtt_info->out_type==RTT_TYPE_NOL)
  { write_ngg_logo_footer(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_WBMP) write_wbmp_footer(out,rtt_info);
    else
  if (rtt_info->out_type==RTT_TYPE_ICO) write_icon_footer(out,rtt_info);
    else
  if (rtt_info->out_type==RTT_TYPE_GIF) write_gif_footer(out,rtt_info);

  free(rtt_info->picture);

  return 0;
}

int tempo_route(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->out_type==RTT_TYPE_SCKL || rtt_info->out_type==RTT_TYPE_OTT)
  { write_nokia_bpm(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_MIDI)
  { write_midi_bpm(out,rtt_info); }
    else
  if (rtt_info->out_type==RTT_TYPE_RTX)
  { write_rtx_bpm(out,rtt_info); }

  return 0;
}

int volume_route(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->out_type==RTT_TYPE_SCKL || rtt_info->out_type==RTT_TYPE_OTT)
  { write_nokia_volume(out,rtt_info); }

  return 0;
}



