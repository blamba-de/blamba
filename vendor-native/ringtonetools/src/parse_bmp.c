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

struct _bitmap_file
{
  unsigned char bfType[2];
  unsigned int bfSize;
  unsigned short int reserved1;
  unsigned short int reserved2;
  unsigned int bfOffs;
};

struct _bitmap_info
{
  unsigned int biSize;
  unsigned int biWidth;
  unsigned int biHeight;
  unsigned short int biPlanes;
  unsigned short int biBitCount;
  unsigned int biCompression;
  unsigned int biSizeImage;
  unsigned int biXPelsPerMetre;
  unsigned int biYPelsPerMetre;
  unsigned int biClrUsed;
  unsigned int biClrImportant;
};

int colors[256];

void read_bitmap_file(FILE *in, struct _bitmap_file *bitmap_file)
{
  bitmap_file->bfType[0]=getc(in);
  bitmap_file->bfType[1]=getc(in);
  bitmap_file->bfSize=read_long(in);
  bitmap_file->reserved1=read_word(in);
  bitmap_file->reserved2=read_word(in);
  bitmap_file->bfOffs=read_long(in);
}

void read_bitmap_info(FILE *in, struct _bitmap_info *bitmap_info)
{
  bitmap_info->biSize=read_long(in);
  bitmap_info->biWidth=read_long(in);
  bitmap_info->biHeight=read_long(in);
  bitmap_info->biPlanes=read_word(in);
  bitmap_info->biBitCount=read_word(in);
  bitmap_info->biCompression=read_long(in);
  bitmap_info->biSizeImage=read_long(in);
  bitmap_info->biXPelsPerMetre=read_long(in);
  bitmap_info->biYPelsPerMetre=read_long(in);
  bitmap_info->biClrUsed=read_long(in);
  bitmap_info->biClrImportant=read_long(in);
}

void raw_uncompressed(FILE *in, struct rtt_info_t *rtt_info, int bits)
{
int x,y;
int c=0,t;
int byte_count;
int use_trans;

  use_trans=rtt_info->trans;

  for (y=rtt_info->height-1; y>=0; y--)
  {
    byte_count=0;

    for (x=0; x<rtt_info->width; x++)
    {
      if (bits==8 || bits==24 || bits==32)
      {
        if (bits==8) 
        {
          c=getc(in);
          c=colors[c];
/* printf("0x%06x\n",c); */
          byte_count++;
        }
          else
        if (bits==24)
        {
          c=getc(in)+(getc(in)<<8)+(getc(in)<<16);
          byte_count=byte_count+3;
        }
          else
        if (bits==32)
        {
          c=getc(in)+(getc(in)<<8)+(getc(in)<<16);
          t=getc(in);
          if (t==255 && rtt_info->trans==-2) rtt_info->trans=c;
/*
          if (use_trans==-2)
          { 
            rgb[0]=getc(in);
            rgb[1]=getc(in);
            rgb[2]=getc(in);

            t=getc(in);
            if (t==255 && rtt_info->trans==-2) rtt_info->trans=0x000000; 
            adj=(double)(255-t)/(double)255;
            rgb[0]=(unsigned char)((double)rgb[0]*adj);
            rgb[1]=(unsigned char)((double)rgb[1]*adj);
            rgb[2]=(unsigned char)((double)rgb[2]*adj);
            c=rgb[0]+(rgb[1]<<8)+(rgb[2]<<16);
            if (t==255 && rtt_info->trans==-2) rtt_info->trans=c;
          }
            else
          {
            c=getc(in)+(getc(in)<<8)+(getc(in)<<16);
            t=getc(in);
          }
*/

          byte_count=byte_count+4;
        }

        rtt_info->picture[x+(y*rtt_info->width)]=c;
      }
        else
      if (bits==4)
      {
        c=getc(in);
        byte_count++;

        rtt_info->picture[x+(y*rtt_info->width)]=colors[((c>>4)&15)];
        x++;

        if (x<rtt_info->width)
        {
          rtt_info->picture[x+(y*rtt_info->width)]=colors[(c&15)];
        }
      }
        else
      if (bits==1)
      {
        c=getc(in);
        byte_count++;

        for (t=7; t>=0; t--)
        {
          if (x<rtt_info->width)
          {
            if (((c>>t)&1)==0)
            { rtt_info->picture[x+(y*rtt_info->width)]=colors[0]; }
              else
            { rtt_info->picture[x+(y*rtt_info->width)]=colors[1]; }
          }
          x++;
        }
        x=x-1;
      }
    }

    c=(byte_count%4);
    if (c!=0)
    {
      for (t=c; t<4; t++)
      { getc(in); }
    }
  }
}

void raw_compressed(FILE *in, struct rtt_info_t *rtt_info, int bits)
{
int x,y;
int c,t,r;

  y=rtt_info->height-1;
  x=0;

  while (1)
  {
#ifdef DEBUG
if (x==0) printf("reading line %d\n",y);
#endif

    c=getc(in);

    if (c==EOF) return;

    if (c!=0)
    {
      r=getc(in);
#ifdef DEBUG
printf("repeat same %d %d times\n",r,c);
#endif

      for (t=0; t<c; t++)
      {
        if (bits==4)
        {
          if ((t%2)==0)
          { rtt_info->picture[x+(y*rtt_info->width)]=colors[(r>>4)]; }
            else
          { rtt_info->picture[x+(y*rtt_info->width)]=colors[(r&15)]; }
        }
          else
        if (bits==8)
        { rtt_info->picture[x+(y*rtt_info->width)]=colors[r]; }

        x++;
      }
    }
      else
    {
      r=getc(in);
#ifdef DEBUG
printf("repeat not the same %d times\n",r);
#endif

      if (r==0)
      {
        x=0;
        y--;
        continue;
      }
        else
      if (r==1)
      { break; }
        else
      if (r==2)
      {
        x=x+getc(in);
        y=y-getc(in);
        return;
      }

      for (t=0; t<r; t++)
      {
        c=getc(in);
        if (bits==8)
        {
          rtt_info->picture[x+(y*rtt_info->width)]=colors[c];
        }
          else
        if (bits==4)
        {
          rtt_info->picture[x+(y*rtt_info->width)]=colors[c>>4];
          t++;
          if (t<r)
          {
            x++;
            rtt_info->picture[x+(y*rtt_info->width)]=colors[(c&15)];
          }
        }

        x++;
      }

      if (bits==8)
      { c=r%2; }
        else
      if (bits==4)
      {
        t=(r/2)+(r%2);
        c=t%2;
      }

      if (c!=0)
      { getc(in); }
    }
  }
}

int parse_bmp(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
struct _bitmap_file bitmap_file;
struct _bitmap_info bitmap_info;
int t;
/* int t,c; */

  read_bitmap_file(in, &bitmap_file);

  if (bitmap_file.bfType[0]!='B' || bitmap_file.bfType[1]!='M')
  {
    printf("Not a bitmap.\n");
    return -1;
  }

  read_bitmap_info(in, &bitmap_info);

#ifdef DEBUG
  printf("Bitmap File Header\n");
  printf("----------------------------------------------\n");
  printf("         bfType: %c%c\n",bitmap_file.bfType[0],bitmap_file.bfType[1]);
  printf("         bfSize: %d\n",bitmap_file.bfSize);
  printf("      reserved1: %d\n",bitmap_file.reserved1);
  printf("      reserved2: %d\n",bitmap_file.reserved2);
  printf("         bfOffs: %d\n",bitmap_file.bfOffs);
  printf("----------------------------------------------\n");
  printf("Bitmap Info Header\n");
  printf("----------------------------------------------\n");
  printf("         biSize: %d\n",bitmap_info.biSize);
  printf("        biWidth: %d\n",bitmap_info.biWidth);
  printf("       biHeight: %d\n",bitmap_info.biHeight);
  printf("       biPlanes: %d\n",bitmap_info.biPlanes);
  printf("     biBitCount: %d\n",bitmap_info.biBitCount);
  printf("  biCompression: %d\n",bitmap_info.biCompression);
  printf("    biSizeImage: %d\n",bitmap_info.biSizeImage);
  printf("biXPelsPerMetre: %d\n",bitmap_info.biXPelsPerMetre);
  printf("biYPelsPerMetre: %d\n",bitmap_info.biYPelsPerMetre);
  printf("      biClrUsed: %d\n",bitmap_info.biClrUsed);
  printf(" biClrImportant: %d\n",bitmap_info.biClrImportant);
  printf("----------------------------------------------\n");
#endif

  colors[0]=0;
  colors[1]=0xffffff;
  colors[255]=0xffffff;

  if (bitmap_info.biClrImportant==0 && bitmap_info.biBitCount==8)
  { bitmap_info.biClrImportant=256; }
  
  for (t=0; t<bitmap_info.biClrImportant; t++)
  {
    colors[t]=read_long(in);
/*
    c=(getc(in)+(getc(in)<<8)+(getc(in)<<16));
    getc(in);
    colors[t]=c;
*/
  }

  rtt_info->width=bitmap_info.biWidth;
  rtt_info->height=bitmap_info.biHeight;

  if (logo_header_route(out,rtt_info)==-1) return 0;

  fseek(in,bitmap_file.bfOffs,0);

  if (bitmap_info.biCompression==0)
  {
    raw_uncompressed(in,rtt_info,bitmap_info.biBitCount);
  }
    else
  if (bitmap_info.biCompression==1)
  {
    raw_compressed(in,rtt_info,8);
  }
    else
  if (bitmap_info.biCompression==2)
  {
    raw_compressed(in,rtt_info,4);
  }
    else
  if (bitmap_info.biCompression==3)
  {
    raw_uncompressed(in,rtt_info,bitmap_info.biBitCount);
  }
    else
  {
    printf("This type of compression is not supported at this time.\n");
    return 0;
  }

  logo_footer_route(out,rtt_info);

  return 0;
}

int parse_wbmp(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
int x,y,ch;
int b,ptr;

  if (getc(in)!=0)
  {
    printf("WBMP: Unknown compression type\n");
    return 0;
  }

  getc(in);

  rtt_info->width=getc(in);
  rtt_info->height=getc(in);

  logo_header_route(out,rtt_info);

  ptr=0;
  for (y=0; y<rtt_info->height; y++)
  {
    x=0;
    while(x<rtt_info->width)
    {
      ch=getc(in);
      for (b=7; b>=0; b--)
      {
        if (x<rtt_info->width)
        {
          if ((ch&(1<<b))==0)
          { rtt_info->picture[ptr++]=0; }
            else
          { rtt_info->picture[ptr++]=0xffffff; }
        }
        x++;
      }
    }
  }

  logo_footer_route(out,rtt_info);

  return 0;
}

int parse_icon(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
struct _bitmap_info bitmap_info;
int x,c,t,num_pics,num_colors;

  t=read_word(in);
  t=read_word(in);
  if (t!=1)
  {
    printf("Not a Windows Icon file.\n");
    return 0;
  }

  num_pics=read_word(in);
#ifdef DEBUG
printf("num_pics %d\n",num_pics);
#endif

  /* Icon Directory */

  rtt_info->width=getc(in);
  rtt_info->height=getc(in);

  /* printf("%dx%d\n",rtt_info->width,rtt_info->height); */

  num_colors=getc(in);
  if (num_colors==0) num_colors=256;
#ifdef DEBUG
printf("color count %d\n",num_colors);
#endif
  getc(in);
  t=read_word(in);
  t=read_word(in);

  t=read_long(in);
#ifdef DEBUG
printf("size of pixel array %d\n",t);
#endif
  t=read_long(in);
  x=read_long(in);
#ifdef DEBUG
printf("Offset %d  %d\n",t,(int)ftell(in));
#endif
  fseek(in,t,SEEK_SET);
#ifdef DEBUG
printf("Data Offset %d\n",x);
#endif

  read_bitmap_info(in, &bitmap_info);
#ifdef DEBUG
  printf("Bitmap Info Header\n");
  printf("----------------------------------------------\n");
  printf("         biSize: %d\n",bitmap_info.biSize);
  printf("        biWidth: %d\n",bitmap_info.biWidth);
  printf("       biHeight: %d\n",bitmap_info.biHeight);
  printf("       biPlanes: %d\n",bitmap_info.biPlanes);
  printf("     biBitCount: %d\n",bitmap_info.biBitCount);
  printf("  biCompression: %d\n",bitmap_info.biCompression);
  printf("    biSizeImage: %d\n",bitmap_info.biSizeImage);
  printf("biXPelsPerMetre: %d\n",bitmap_info.biXPelsPerMetre);
  printf("biYPelsPerMetre: %d\n",bitmap_info.biYPelsPerMetre);
  printf("      biClrUsed: %d\n",bitmap_info.biClrUsed);
  printf(" biClrImportant: %d\n",bitmap_info.biClrImportant);
  printf("----------------------------------------------\n");
#endif

  /* rtt_info->width=bitmap_info.biWidth; */
  /* rtt_info->height=bitmap_info.biHeight; */

  colors[0]=0;
  colors[1]=0xffffff;
  colors[255]=0xffffff;

  if (bitmap_info.biBitCount<24) num_colors=1<<bitmap_info.biBitCount;

  for (t=0; t<num_colors; t++)
  {
    c=(getc(in)+(getc(in)<<8)+(getc(in)<<16));
    getc(in);
    colors[t]=c;
  }

  logo_header_route(out,rtt_info);

  bitmap_info.biHeight=rtt_info->height;

  raw_uncompressed(in,rtt_info,bitmap_info.biBitCount);

/*
  for (y=rtt_info->height-1; y>=0; y--)
  {
    x=0;
    while(x<rtt_info->width)
    {
      c=getc(in);
      for (t=7; t>=0; t--)
      {
        if (((c>>t)&1)==1) rtt_info->picture[x+(y*rtt_info->width)]=0;
        x++;
      }
    }
  }
*/

  logo_footer_route(out,rtt_info);

  return 0;
}

