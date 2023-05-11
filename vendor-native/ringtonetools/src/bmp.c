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

/* BMP */

void write_bmp_info_header(FILE *out, struct rtt_info_t *rtt_info, int bits, int *palette, int palette_count)
{
int t;
/* int t,d; */

  /* d=ftell(out); */

  write_long(out,40);            /* biSize */
  write_long(out,rtt_info->width);
  write_long(out,rtt_info->height);
  write_word(out,1);
  write_word(out,bits);
  write_long(out,0);             /* compression */
  /* write_long(out,image_size); */
  write_long(out,0);
  write_long(out,0);             /* biXPelsperMetre */
  write_long(out,0);             /* biYPelsperMetre */
  write_long(out,palette_count); /* biClrUsed */
  write_long(out,palette_count); /* biClrImportant */

  for (t=0; t<palette_count; t++)
  { write_long(out,palette[t]); } 

/*
  t=ftell(out);
  fseek(out,d,SEEK_SET);
  write_long(out,t-d);
  fseek(out,t,SEEK_SET);
*/
}

void write_bmp_header(FILE *out, struct rtt_info_t *rtt_info)
{
  write_chars(out,"BM");
  write_long(out,0);
  write_word(out,0);
  write_word(out,0);
  write_long(out,0);   /* size */
}

int color_distance(int c1, int c2)
{
int d;

  d=abs((c1&255)-(c2&255))+(((c1>>8)&255)-((c2>>8)&255))+(((c1>>16)&255)-((c2>>16)&255));

  return d/3;
}

int compute_palette(struct rtt_info_t *rtt_info,int *palette,int *palette_count,int true_color)
{
int l,ptr,c,count;

  count=0;
  l=rtt_info->width*rtt_info->height;

  for (ptr=0; ptr<l; ptr++)
  {
    for (c=0; c<count; c++)
    {
      if (palette[c]==rtt_info->picture[ptr]) break;
    }

    if (c==count)
    {
      if (count>=256)
      {
        if (true_color==1) return 24;
        return 8;
      }
      palette[count++]=rtt_info->picture[ptr];
    }
  }

  *palette_count=count;
 
  if (count<=2) return 1;
    else 
  if (count<=16) return 4;
    else 
  if (count<=256) return 8;

  return 24;
}

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

void write_bmp_data(FILE *out, struct rtt_info_t *rtt_info, int bits, int *palette, int palette_count)
{
int x,y,c,b,k,padding;

  if (bits==1)
  {
    padding=rtt_info->width/8;
    if ((rtt_info->width%8)!=0) padding++;
    padding=padding%4;
    if (padding==0) padding=4;

    for (y=rtt_info->height-1; y>=0; y--)
    {
      x=0;
      while(x<rtt_info->width)
      {
        k=0;

        for (b=7; b>=0; b--)
        {
          if (x<rtt_info->width)
          {
            c=rtt_info->picture[x+(y*rtt_info->width)];
            c=((c&255)+((c>>8)&255)+((c>>16)&255))/3;

            if (c>COLOR_THRESHOLD)
            { k=k+(1<<b); }
          }
          x++;
        }
        putc(k,out);
      }

      for (x=padding; x<4; x++)
      { putc(0,out); }
    }
  }
    else
  if (bits==4)
  {
    padding=(rtt_info->width>>1)+(rtt_info->width&1);
    padding=(padding%4);
    if (padding==0) padding=4;

    for (y=rtt_info->height-1; y>=0; y--)
    {
      for (x=0; x<rtt_info->width; x++)
      {
        b=get_color(rtt_info->picture[x+(y*rtt_info->width)],palette,palette_count)<<4;
        x++;
        if (x<rtt_info->width);
        { b|=get_color(rtt_info->picture[x+(y*rtt_info->width)],palette,palette_count); }
        putc(b,out);
      }

      for (x=padding; x<4; x++)
      { putc(0,out); }
    }
  }
    else
  if (bits==8)
  {
    padding=(rtt_info->width)%4;
    if (padding==0) padding=4;

    for (y=rtt_info->height-1; y>=0; y--)
    {
      for (x=0; x<rtt_info->width; x++)
      {
        putc(get_color(rtt_info->picture[x+(y*rtt_info->width)],palette,palette_count),out);
      }

      for (x=padding; x<4; x++)
      { putc(0,out); }
    }
  }
    else
  if (bits==24)
  {
    padding=(rtt_info->width*3)%4;
    if (padding==0) padding=4;

    for (y=rtt_info->height-1; y>=0; y--)
    {
      for (x=0; x<rtt_info->width; x++)
      {
        putc(((rtt_info->picture[x+(y*rtt_info->width)])&255),out);
        putc(((rtt_info->picture[x+(y*rtt_info->width)]>>8)&255),out);
        putc(((rtt_info->picture[x+(y*rtt_info->width)]>>16)&255),out);
      }

      for (x=padding; x<4; x++)
      { putc(0,out); }
    }
  }

}

void write_bmp_footer(FILE *out, struct rtt_info_t *rtt_info)
{
int data_length,palette_count,bits;
int palette[256];
int t;

  palette_count=0;

  if ((rtt_info->bmp_flags&4)==0)
  {
    bits=1;
    palette[palette_count++]=0;
    palette[palette_count++]=0xffffff;
  }
    else
  { 
    bits=compute_palette(rtt_info,palette,&palette_count,1); 
  }

  write_bmp_info_header(out,rtt_info,bits,palette,palette_count);

  data_length=ftell(out);
  fseek(out,10,SEEK_SET);
  write_long(out,data_length);
  fseek(out,data_length,SEEK_SET);

  /* printf("pointer to data %d\n",data_length); */

  t=ftell(out);
  write_bmp_data(out,rtt_info,bits,palette,palette_count);

  data_length=ftell(out);
  fseek(out,2,SEEK_SET);
  write_long(out,data_length);
  fseek(out,34,SEEK_SET);
  write_long(out,data_length-t);
  fseek(out,data_length,SEEK_SET);
}

/*
void write_bmp_footer(FILE *out, struct rtt_info_t *rtt_info)
{
int x,y,k,b,c;
int padding=0;
int data_length;
int image_size;

  if ((rtt_info->bmp_flags&4)==0)
  {
    image_size=rtt_info->width/8;
    if (rtt_info->width%8>0) image_size++;
    if (image_size%4!=0)
    { image_size=image_size+(4-(image_size%4)); }
  }
    else
  {
    image_size=rtt_info->width*3;
    while ((image_size%4)!=0) image_size++;
    image_size=image_size*rtt_info->height;
  }


  data_length=ftell(out);
  fseek(out,2,SEEK_SET);
  write_long(out,data_length);
  fseek(out,data_length,SEEK_SET);
}

*/

void write_text_footer(FILE *out, struct rtt_info_t *rtt_info)
{
int x,y,c;

  for (y=0; y<rtt_info->height; y++)
  {
    for (x=0; x<rtt_info->width; x++)
    {
      c=rtt_info->picture[x+(y*rtt_info->width)];
      c=((c&255)+((c>>8)&255)+((c>>16)&255))/3;

      if (c>COLOR_THRESHOLD)
      { putc('*',out); }
        else
      { putc(' ',out); }
    }
    putc('\n',out);
  }
}

void write_wbmp_header(FILE *out, struct rtt_info_t *rtt_info)
{
  putc(0,out);
  putc(0,out);
  putc(rtt_info->width,out);
  putc(rtt_info->height,out);
}

void write_wbmp_footer(FILE *out, struct rtt_info_t *rtt_info)
{
int x,y,k,b,c;
int ptr;

  ptr=0;

  for (y=0; y<rtt_info->height; y++)
  {
    x=0;
    while (x<rtt_info->width)
    {
      k=0;

      for (b=7; b>=0; b--)
      {
        if (x<rtt_info->width)
        {
          c=rtt_info->picture[ptr++];
          c=((c&255)+((c>>8)&255)+((c>>16)&255))/3;

          if (c>COLOR_THRESHOLD)
          { k=k+(1<<b); }
        }
        x++;
      }
      putc(k,out);
    }
  }
}


void write_icon_header(FILE *out, struct rtt_info_t *rtt_info)
{
  if (rtt_info->width!=16 && rtt_info->width!=32 && rtt_info->width!=64)
  {
    rtt_info->width=0;
    printf("Invalid width for icon file\n");
    return;
  }

  if (rtt_info->height!=16 && rtt_info->height!=32 && rtt_info->height!=64)
  {
    rtt_info->height=0;
    printf("Invalid width for icon file\n");
    return;
  }

  write_word(out,0);
  write_word(out,1);
  write_word(out,1);

  putc(rtt_info->width,out);
  putc(rtt_info->height,out);
}

void write_icon_footer(FILE *out, struct rtt_info_t *rtt_info)
{
int data_length,palette_count,bits,bmp_start;
int palette[256],t,d,k,p,x,y;

  palette_count=0;

  if ((rtt_info->bmp_flags&4)==0)
  {
    bits=1;
    palette[palette_count++]=0;
    palette[palette_count++]=0xffffff;
  }
    else
  { 
    bits=compute_palette(rtt_info,palette,&palette_count,1); 
  }

  if (bits>=8)
  { putc(0,out); }
    else
  { putc((1<<bits),out); }

  putc(0,out);
  write_word(out,0);
  write_word(out,0);
  t=ftell(out);
  write_long(out,0);
  write_long(out,ftell(out)+4);

#ifdef DEBUG
printf("Palette count=%d  bits=%d\n",palette_count,bits);
#endif

  bmp_start=ftell(out);
  write_bmp_info_header(out,rtt_info,bits,palette,palette_count);
  if ((1<<bits)!=palette_count && bits<24)
  {
    d=1<<bits;
    for (k=palette_count; k<d; k++)
    {
      write_long(out,0);
    }
  }
  write_bmp_data(out,rtt_info,bits,palette,palette_count);

  for (y=rtt_info->height-1; y>=0; y--)
  {
    x=0;
    while(x<rtt_info->width)
    {
      k=0;
      if (rtt_info->trans>=0)
      {
        for (p=7; p>=0; p--)
        {
          if (x<rtt_info->width && rtt_info->picture[x+(y*rtt_info->width)]==rtt_info->trans)
          { k=k+(1<<p); }
          x++;
        }
      }
        else
      { x=x+8; }
      putc(k,out);
    }

    if ((x%32)!=0)
    {
      p=((x%32)/8);
      for(k=0; k<p; k++) putc(0,out);
    }
  }

  data_length=ftell(out);

  fseek(out,t,SEEK_SET);              /* Icon Offset */
  write_long(out,data_length-bmp_start);
  /* write_long(out,data_length-(t+8)); */

  fseek(out,t+16,SEEK_SET);           /* biHeight */
  write_long(out,rtt_info->height*2);

  fseek(out,t+28,SEEK_SET);           /* biSizeImage */
  /* write_long(out,data_length-d-40); */
  write_long(out,0);
  write_long(out,0);
  write_long(out,0);
  write_long(out,0);
  write_long(out,0);

  /* write_long(out,data_length-(t+8-40)); */
  fseek(out,data_length,SEEK_SET);

}


