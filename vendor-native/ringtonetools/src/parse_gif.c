#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ringtonetools.h"
#include "fileoutput.h"
#include "lzw.h"

/*

Ringtone Tools - Copyright 2004-2006 by Michael Kohn
You may not use any of this program in your own programs
without permission from Michael Kohn <mike@mikekohn.net>

*/

int parse_gif(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
unsigned char temp_buffer[4096];
struct gif_header_t gif_header;
struct node_t node[4096];
int ch,t,r,x,y;
int code_size,curr_code_size,data_count;
unsigned int holding,bitptr;
int code,old_code,next_code,start_table_size;
int clear_code,eof_code,mask;
int ptr;

  read_chars(in,temp_buffer,6);

  if (strcmp(temp_buffer,"GIF89a")==0)
  { gif_header.version=89; }
    else
  if (strcmp(temp_buffer,"GIF87a")==0)
  { gif_header.version=87; }
    else
  {
    printf("This is not a GIF.\n");
    return -1;
  }

  /* READ GIF HEADER */

  rtt_info->width=read_word(in);
  rtt_info->height=read_word(in);
  /* gif_header.width=read_word(in); */
  /* gif_header.height=read_word(in); */
  gif_header.resolution=getc(in);
  gif_header.bgcolor=getc(in);
  gif_header.aspect=getc(in);

  gif_header.colors=1<<((gif_header.resolution&7)+1);

#ifdef DEBUG
printf("GIF HEADER\n");
printf("-----------------------------------------\n");
printf("  Signature: %s\n",temp_buffer);
printf(" Image size: %d x %d\n",rtt_info->width,rtt_info->height);
printf(" Resolution: 0x%02x colors=%d\n",gif_header.resolution,gif_header.colors);
printf("    BgColor: 0x%02x\n",gif_header.bgcolor);
printf("     Aspect: 0x%02x\n\n",gif_header.aspect);
#endif

  /* IF THERE IS A GLOBAL COLOR MAP, READ IT */

  if ((gif_header.resolution&128)==128)
  {
    for (t=0; t<gif_header.colors; t++)
    {
      gif_header.color_map[t]=(getc(in)<<16)+(getc(in)<<8)+getc(in);
    }
  }

  /* READ EXTENSION BLOCKS */

  while(1)
  {
    ch=getc(in);
    if (ch=='!')
    {
      ch=getc(in);
      while(1)
      {
        ch=getc(in);
        if (ch==0) break;
        for (t=0; t<ch; t++) getc(in);
      }
    }
      else
    { ungetc(ch,in); break; }
  }

  logo_header_route(out,rtt_info);

  /* READ IN IMAGE DESCRIPTOR BLOCK */

  t=getc(in);

  if (t!=',')
  {
    printf("Expected ',' got %c at 0x%06x\n",t,(int)ftell(in));
    return -1;
  }

  x=read_word(in);
  y=read_word(in);
  rtt_info->width=read_word(in);
  rtt_info->height=read_word(in);
  gif_header.flags=getc(in);

#ifdef DEBUG
printf("GIF IMAGE DESCRIPTION\n");
printf("-----------------------------------------\n");
printf("Coordinates: (%d,%d)\n",x,y);
printf(" Image size: %d x %d\n",rtt_info->width,rtt_info->height);
printf("      Flags: 0x%02x\n\n",gif_header.flags);
#endif

  /* IF THERE IS A LOCAL COLOR MAP, READ IT */

  if ((gif_header.flags&128)==128)
  {
    /* gif_header.colors=1<<((gif_header.flags&7)+1); */
    x=1<<((gif_header.flags&7)+1);

    for (t=0; t<x; t++)
    {
      gif_header.color_map[t]=(getc(in)<<16)+(getc(in)<<8)+getc(in);
    }
  }

  code_size=getc(in);

#ifdef DEBUG
printf("code_size=%d\n",code_size);
#endif

  start_table_size=(1<<code_size);

  for (t=0; t<start_table_size; t++)
  {
    node[t].prev=-1;
    node[t].color=t; 
  }

  clear_code=t;
  eof_code=t+1;
  next_code=t+2;

  code_size++;

  x=0; y=0;
  holding=0;
  bitptr=0;
  curr_code_size=code_size;
  old_code=-1;

  mask=(1<<curr_code_size)-1;
  data_count=0;

  ptr=0;

  while(1)
  {
/*
    if (next_code>=4096)
    {
      //curr_code_size=code_size;
      //next_code=start_table_size+2;
      //old_code=-1;
      //mask=(1<<curr_code_size)-1;
      curr_code_size--;
      mask=mask>>1;
printf("resetting\n");
    }
*/

    while (bitptr<curr_code_size)
    {
      if (data_count==0)
      {
        if ((data_count=getc(in))==EOF) break;
        if (data_count==0) break;
      }
      t=getc(in);
      holding=holding+(t<<bitptr);
      data_count--;
      bitptr=bitptr+8;
    }

if (t==EOF || data_count==EOF)
{
  printf("EOF! %d\n",ptr);
  break;
}
    code=holding&mask;

#ifdef DEBUG
printf("holding=%d bitptr=%d mask=%d curr_code_size=%d\n",holding,bitptr,mask,curr_code_size);
printf(">> code=%d   next_code=%d\n",code,next_code);
#endif

    holding=holding>>curr_code_size;
    bitptr=bitptr-curr_code_size;

    if (code==clear_code)
    {
      curr_code_size=code_size;
      next_code=start_table_size+2;
      old_code=-1;
      mask=(1<<curr_code_size)-1;

      continue;
    }
      else
    if (old_code==-1)
    {
      /* printf("0x%06x\n",gif_header.color_map[node[code].color]); */
      rtt_info->picture[ptr++]=gif_header.color_map[node[code].color];
    }
      else
    if (code==eof_code)
    {
      break;
    }
      else
    {
      if (code<next_code)
      {
        t=0;
        r=code;
        while(1)
        {
          temp_buffer[t++]=node[r].color;
          if (node[r].prev==-1) break;
          r=node[r].prev;
        }

        for (r=t-1; r>=0; r--)
        {
          /* printf("0x%06x\n",gif_header.color_map[temp_buffer[r]]); */
          rtt_info->picture[ptr++]=gif_header.color_map[temp_buffer[r]];
        }

        node[next_code].color=temp_buffer[t-1];
        node[next_code].prev=old_code;

        if (next_code==mask && mask!=4095)
        {
          curr_code_size++;
          mask=(1<<curr_code_size)-1;
        }
        next_code++;
      }
        else
      {
        t=0;
        r=old_code;
        while(1)
        {
          temp_buffer[t++]=node[r].color;
          if (node[r].prev==-1) break;
          r=node[r].prev;
        }

        node[next_code].color=temp_buffer[t-1];
        node[next_code].prev=old_code;

#ifdef DEBUG
printf("adding %d to %d\n",temp_buffer[t],old_code);
#endif

        if (next_code==mask && mask!=4095)
        {
          curr_code_size++;
          mask=(1<<curr_code_size)-1;
        }

        next_code++;

        for (r=t-1; r>=0; r--)
        {
          /* printf("0x%06x\n",gif_header.color_map[temp_buffer[r]]); */
          rtt_info->picture[ptr++]=gif_header.color_map[temp_buffer[r]];
#ifdef DEBUG
/* printf("outputting %d\n",temp_buffer[r]); */
#endif
        }

        rtt_info->picture[ptr++]=gif_header.color_map[temp_buffer[t-1]];
      }
    }

    old_code=code;
  }

  x=rtt_info->width*rtt_info->height;

  for (t=ptr; t<x; t++)
  {
    rtt_info->picture[t]=gif_header.color_map[gif_header.bgcolor];
  }

  logo_footer_route(out,rtt_info);

  return 0;
}

