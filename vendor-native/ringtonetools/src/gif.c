#include <stdio.h>
#include <stdlib.h>

#include "ringtonetools.h"
#include "fileoutput.h"
#include "lzw.h"

/*

Ringtone Tools - This code is copyright 2004-2006 by Michael Kohn
You may not use any of this program in your own programs
without permission from Michael Kohn <mike@mikekohn.net>

*/

int compute_palette_gif(int length,unsigned int *picture,unsigned int *palette,unsigned int *palette_count)
{
int ptr,c,count;

  count=0;

  for (ptr=0; ptr<length; ptr++)
  {
    for (c=0; c<count; c++)
    {
      if (palette[c]==picture[ptr]) break;
    }

    if (c==count)
    {
      if (count>=256)
      {
       *palette_count=count;
        return 8;
      }
      palette[count++]=picture[ptr];
    }
  }

  *palette_count=count;

  c=1;

  while(c<=8)
  {
    if ((count>>c)==0) return c;
    c++;
  }

  return 8;
}

/*
int get_color(int col, int *palette, int palette_count)
{
int d,c;

  for (c=0; c<palette_count; c++)
  {
    if (col==palette[c]) return c;
  }

  for (d=0; d<255; d++)
  {
    for (c=0; c<palette_count; c++)
    {
      if (color_distance(col,palette[c])<=d) return c;
    }
  }

  return 0;
}
*/

void write_gif_header(FILE *out, struct rtt_info_t *rtt_info)
{

}

void write_gif_footer(FILE *out, struct rtt_info_t *rtt_info)
{
struct gif_header_t gif_header;
struct compress_node_t node[4097];
unsigned char data_stream[256];
int curr_code_size,next_code;
int clear_code,eof_code,curr_code,data_size;
int c,max_colors,length,code_size,table_start_size,last_code;
unsigned int holding,ptr;
int bitptr,t;
unsigned char color;

  length=rtt_info->width*rtt_info->height;

  c=compute_palette_gif(length,rtt_info->picture,gif_header.color_map,(unsigned int *)&gif_header.colors);

  if (c<2) c=2; /* check this later */

  max_colors=1<<c;
  code_size=c;

/*
printf("colors=%d  bits=%d\n",gif_header.colors,c);
for (t=0; t<gif_header.colors; t++)
{
  printf("%d: %6x\n",t,gif_header.color_map[t]);
}
*/

#ifdef DEBUG
printf("colors=%d  bits=%d\n",gif_header.colors,c);
#endif

  /* WRITE GIF HEADER */

  write_chars(out,"GIF87a");
  write_word(out,rtt_info->width);
  write_word(out,rtt_info->height);
  putc(128+((code_size-1)<<4)+(code_size-1),out);
  putc(0,out);
  putc(0,out);

  for (c=0; c<max_colors; c++)
  {
    putc((gif_header.color_map[c]>>16),out);
    putc(((gif_header.color_map[c]>>8)&255),out);
    putc((gif_header.color_map[c]&255),out);
  } 

  putc(',',out);
  write_word(out,0);
  write_word(out,0);
  write_word(out,rtt_info->width);
  write_word(out,rtt_info->height);
  putc((code_size-1),out);

  putc(code_size,out);

  for (c=0; c<max_colors; c++)
  {
    node[c].color=c;
    node[c].right=c+1;
    node[c].down=-1;
  }

  /* node[c-1].right=-1; */

  clear_code=max_colors;
  eof_code=max_colors+1;

#ifdef DEBUG
printf("clear_code=%d\n",clear_code);
printf("eof_code=%d\n",eof_code);
#endif

  table_start_size=max_colors+2;
  next_code=table_start_size;
  curr_code_size=code_size+1;
  curr_code=-1;
  data_size=0;

#ifdef DEBUG
printf("curr_code_size=%d\n",curr_code_size);
#endif

  holding=clear_code;
  bitptr=curr_code_size;
  ptr=0;

#ifdef DEBUG
printf("length=%d\n",length);
#endif

  color=get_color(rtt_info->picture[ptr++],gif_header.color_map,gif_header.colors);
  curr_code=color;
  last_code=curr_code;

#ifdef DEBUG
printf("curr_code=%d color=%d\n",curr_code,color);
#endif

  while (ptr<length)
  {
    color=get_color(rtt_info->picture[ptr++],gif_header.color_map,gif_header.colors);

#ifdef DEBUG
printf("curr_code=%d color=%d\n",curr_code,color);
#endif

    if (node[curr_code].down==-1)
    {
      last_code=curr_code;
      node[curr_code].down=next_code;
    }
      else
    {
      last_code=curr_code;
      curr_code=node[curr_code].down;

      while(node[curr_code].color!=color && node[curr_code].right!=-1)
      { curr_code=node[curr_code].right; }

      if (node[curr_code].color!=color && node[curr_code].right==-1)
      { node[curr_code].right=next_code; }
        else
      { continue; }
    }

#ifdef DEBUG
printf("OUTPUT CODE %d  next_code=%d  color=%d\n",last_code,next_code,color);
#endif

    holding=holding+(last_code<<bitptr);
    bitptr=bitptr+curr_code_size;

    node[next_code].right=-1;
    node[next_code].down=-1;
    node[next_code].color=color;
    curr_code=color;


    if ((next_code>>curr_code_size)!=0)
    {
      if (curr_code_size>=12)
      {
        holding=holding+(clear_code<<bitptr);
        bitptr=bitptr+curr_code_size;

        for (c=0; c<max_colors; c++)
        { node[c].down=-1; }

        next_code=table_start_size-1;
        curr_code_size=code_size;
        /* curr_code=0; */
      }
      curr_code_size++;
    }

    next_code++;

#ifdef DEBUG
printf("holding=0x%x   bitptr=%d  code_size=%d\n",holding,bitptr,curr_code_size);
#endif

    while (bitptr>=8)
    {
      data_stream[data_size++]=holding&255;
      holding=holding>>8;
      bitptr=bitptr-8;
      if (data_size>=255)
      {
        putc(data_size,out);
        t=0;
        while(t!=data_size)
        { t=t+fwrite(data_stream,1,data_size-t,out); }
        data_size=0;
      }
    }
  }

#ifdef DEBUG
printf("Finishing up %d\n",curr_code);
#endif

  holding=holding+(curr_code<<bitptr);
  bitptr=bitptr+curr_code_size;
  holding=holding+(eof_code<<bitptr);
  bitptr=bitptr+curr_code_size;


  while (bitptr>0)
  {
    data_stream[data_size++]=holding&255;
    holding=holding>>8;
    bitptr=bitptr-8;
    if (data_size>=255)
    {
      putc(data_size,out);
      t=0;
      while(t!=data_size)
      { t=t+fwrite(data_stream,1,data_size-t,out); }
      data_size=0;
    }
  }

#ifdef DEBUG
printf("data_size=%d bitptr=%d\n",data_size,bitptr);
#endif

  if (data_size>0)
  {
    putc(data_size,out);
    t=0;
    while(t!=data_size)
    { t=t+fwrite(data_stream,1,data_size-t,out); }
  }

  putc(0,out);
  putc(';',out);

}


