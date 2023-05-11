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

int get_bits(unsigned char *buffer, int *ptr, int *bitptr, int bits)
{
unsigned int holding;
int i;

  holding=(buffer[*ptr]<<8)+buffer[*ptr+1];
  i=(holding>>(16-(bits+(*bitptr)))) & ((1<<bits)-1);

  *bitptr=*bitptr+bits;
  if (*bitptr>7)
  {
    *bitptr=*bitptr-8;
    (*ptr)++;
  }

/*
  printf("i=0x%x   ptr=%d   bitptr=%d    bits=%d\n",i,*ptr,*bitptr,bits);
*/

  return i;
}


int parse_ringtone(FILE *out, unsigned char *buffer, int ptr, struct rtt_info_t *rtt_info)
{
int bitptr;
int k,t,x;
int patterns,pattern_id,count;
int hflag;

/*
  for (t=0; t<ptr; t++)
  {
    printf("%x\n",buffer[t]);
  }
*/

  hflag=0;
  bitptr=0; t=0;

  k=get_bits(buffer,&t,&bitptr,8);
  get_bits(buffer,&t,&bitptr,8);
  k=get_bits(buffer,&t,&bitptr,7);
  k=get_bits(buffer,&t,&bitptr,3);

  if (k!=1 && k!=2)
  {
    printf("Unsupported songtype %d\n",k);
    return 0;
  }

  k=get_bits(buffer,&t,&bitptr,4);

  for (x=0; x<k; x++)
  {
    rtt_info->songname[x]=get_bits(buffer,&t,&bitptr,8);
  }
  rtt_info->songname[x]=0;

  patterns=get_bits(buffer,&t,&bitptr,8);

  while(t<ptr)
  {
    if (patterns==0) break;

    get_bits(buffer,&t,&bitptr,3);
    get_bits(buffer,&t,&bitptr,2);
    rtt_info->loop=get_bits(buffer,&t,&bitptr,4);
    count=get_bits(buffer,&t,&bitptr,8);

/*
printf("count=%d\n",count);
*/

    for (x=0; x<count; x++)
    {
      if (t>=ptr) break;

      k=get_bits(buffer,&t,&bitptr,3);
/*
printf("command=%d\n",k);
*/
      if (k==0)
      {
        pattern_id=get_bits(buffer,&t,&bitptr,2);
      }
        else
      if (k==1)
      {
        if (hflag==0)
        {
          header_route(out,rtt_info);
          hflag=1;
        }

        rtt_info->tone=get_bits(buffer,&t,&bitptr,4);
        rtt_info->length=get_bits(buffer,&t,&bitptr,3);
        rtt_info->modifier=get_bits(buffer,&t,&bitptr,2);

        note_route(out,rtt_info);
      }
        else
      if (k==2)
      {
        rtt_info->scale=get_bits(buffer,&t,&bitptr,2);
        if (rtt_info->scale>0) rtt_info->scale--;
      }
        else
      if (k==3)
      {
        rtt_info->style=get_bits(buffer,&t,&bitptr,2);
      }
        else
      if (k==4)
      {
        k=get_bits(buffer,&t,&bitptr,5);
        rtt_info->bpm=reverse_tempo(k);
        if (hflag==1) tempo_route(out,rtt_info);
      }
        else
      if (k==5)
      {
        rtt_info->volume=get_bits(buffer,&t,&bitptr,4);
      }
    }

    if (t>=ptr) break;

    patterns--;
  }

  footer_route(out,rtt_info);

  return 0;
}

int parse_group_icon(FILE *out,unsigned char *buffer,int ptr,struct rtt_info_t *rtt_info)
{
int t,k,p;

/*
#ifdef DEBUG
  for (t=0; t<ptr; t++)
  {
    printf("%x\n",buffer[t]);
  }
#endif
*/

  if (buffer[0]!=0x30 || buffer[1]!=0x00)
  {
    printf("\nSCKL: Illegal header %d %d\n",buffer[0],buffer[1]);
    return 0;
  }

  rtt_info->width=buffer[2];
  rtt_info->height=buffer[3];

  /* rtt_info->bitplanes=buffer[4]; */

  logo_header_route(out,rtt_info);

  p=0;
  for (k=5; k<ptr; k++)
  {
    /* ring_stack[stackptr++]=buffer[k]; */
    for (t=7; t>=0; t--)
    {
      if (((buffer[k]>>t)&1)==0)
      { rtt_info->picture[p++]=0; }
        else
      { rtt_info->picture[p++]=0xffffff; }
    }
  }

  logo_footer_route(out,rtt_info);

  return 0;
}

int parse_mixed_message(FILE *out,unsigned char *buffer,int ptr,struct rtt_info_t *rtt_info)
{
int t,k,p,len;
int ptr0;

  if (buffer[0]!=0x30)
  {
    printf("\nSCKL: Illegal header %d\n",buffer[0]);
    return 0;
  }

  ptr0=1;

  while(ptr0<ptr)
  {
    k=buffer[ptr0++];

    if (k==0)
    {
      len=buffer[ptr0++];
      len=(len<<8)+buffer[ptr0++];

/* printf("len=%d / %d\n",len,ptr); */

      printf("\nMultipart message: ");

      for (k=0; k<len; k++)
      {
        printf("%c",buffer[ptr0++]);
      }

      printf("\n\n");
    }
      else
    if (k==2)
    {
      len=buffer[ptr0++];
      len=(len<<8)+buffer[ptr0++];

      ptr0++;

      rtt_info->width=buffer[ptr0++];
      rtt_info->height=buffer[ptr0++];

      logo_header_route(out,rtt_info);

      p=0;
      for (k=ptr0; k<ptr0+len; k++)
      {
        for (t=7; t>=0; t--)
        {
          if (((buffer[k]>>t)&1)==0)
          { rtt_info->picture[p++]=0; }
            else
          { rtt_info->picture[p++]=0xffffff; }
        }
      }

      ptr0=ptr0+len;

      logo_footer_route(out,rtt_info);
    }
      else
    { break; }
  }

  return 0;
}


int reorder(unsigned char *buffer,short int *messages, int curr, int ptr)
{
int data[8192];
int t,r;
int s,e,p;

  p=0;

  for (t=0; t<curr; t++)
  {
    s=messages[t*2];
    e=messages[t*2+1];
/*
    if (t==curr-1)
    { e=ptr; }
      else
    { e=messages[t+1]; }
*/

/* printf("in reorder %d %d\n",s,e); */
    for (r=s; r<e; r++)
    {
/* printf("%x\n",buffer[r]); */
      data[p++]=buffer[r];
    }
  }

  for(r=0; r<ptr; r++)
  {
    buffer[r]=data[r];
/* printf("%x\n",buffer[r]); */
  }

  return 0;
}

int read_sckl_header(FILE *in, int *port, int *id, int *count, int *mess)
{
unsigned char buffer[5];
int ch;

  *port=0;

  while((ch=getc(in))!=EOF)
  {
    if (ch=='\n' || ch=='\r' || ch==' ' || ch=='\t') continue;
    break;
  }

  if (ch==EOF) return -1;
  if (ch=='/')
  {
    getc(in);
    read_chars(in,buffer,4);
    if (strcasecmp(buffer,"SCKL")!=0) return 0;
    read_chars(in,buffer,4);
    /* *port=atoi(buffer); */

    for (ch=0; ch<4; ch++)
    {
      buffer[ch]=tolower(buffer[ch]);

      if (buffer[ch]>='0' && buffer[ch]<='9')
      {
        *port=((*port)<<4)+(buffer[ch]-'0');
      }
        else
      if (buffer[ch]>='a' && buffer[ch]<='f')
      {
        *port=((*port)<<4)+(buffer[ch]-'a'+10);
      }
    }

    read_chars(in,buffer,4);
    read_chars(in,buffer,2);
    *id=atoi(buffer);
    read_chars(in,buffer,2);
    *count=atoi(buffer);
    read_chars(in,buffer,2);
    *mess=atoi(buffer);
  }
    else
  {
    ungetc(ch,in);
    if (ch=='0')
    {
      *port=0x1581;
    }
      else
    if (ch=='3')
    {
      *port=0x1583;
    }

    (*id)=0;
    (*mess)++; 
    (*count)++; 
  }

  return *port;
}

int parse_sckl(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
unsigned char buffer[8192];
short int messages[512];
int ch;
int port,id,count,mess;
int i,p,c,m;
int curr,ptr;

  curr=0;
  ptr=0;

  port=0;
  id=0;
  count=0;

  while(1)
  {
    ch=read_sckl_header(in,&p,&i,&c,&m);
    if (ch==0)
    {
      printf("Unknown header\n");
      break;
    }
      else
    if (ch==-1)
    { break; }

    if (m>c)
    {
      printf("Error: More messages than maxiumum in header.\n");
      return 0;
    }

    if (port==0)
    {
      port=p;
      count=c;
      id=i;
    }
      else
    {
      if (port!=p)
      {
        printf("Ports don't match.  Exiting..\n");
        return 0;
      }

      if (id!=i)
      {
        printf("ID's don't match.  Exiting..\n");
        return 0;
      }

      if (count!=c)
      {
        printf("Message counts don't match.  Exiting..\n");
        return 0;
      }
    }

    mess=m;
    messages[(mess-1)*2]=ptr;
    curr++;

    while((ch=getc(in))!=EOF)
    {
      if (ch==' ' || ch=='\t' || ch=='\n' || ch=='\r') continue;
      if (ch=='/')
      {
        ungetc(ch,in);
        break;
      }

      p=2;
      m=0;

      while(p>0)
      {
        ch=tolower(ch);

        if (ch=='a') c=10;
          else
        if (ch=='b') c=11;
          else
        if (ch=='c') c=12;
          else
        if (ch=='d') c=13;
          else
        if (ch=='e') c=14;
          else
        if (ch=='f') c=15;
          else
        { c=ch-'0'; }

        m=(m<<4)+c;
        p--;
        if (p==1) ch=getc(in);
      }

      /* printf("%x\n",m); */
      buffer[ptr++]=m;
    }

    messages[(mess-1)*2+1]=ptr;
  }

  if (count!=curr)
  {
    printf("Missing messages!\n");
    return 0;
  }

  reorder(buffer,messages,curr,ptr); 

  if (port==0x1583)
  {
    parse_group_icon(out,buffer,ptr,rtt_info); 
  }
    else
  if (port==0x1581)
  {
    parse_ringtone(out,buffer,ptr,rtt_info); 
  }
    else
  if (port==0x158A)
  {
    parse_mixed_message(out,buffer,ptr,rtt_info);
  }
    else
  {
    printf("Unsupported port.  Exiting.\n");
    return 0;
  }

  return 0;
}


int parse_ott(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
unsigned char buffer[16738];
int ptr,ch;

  for (ptr=0; ptr<16738; ptr++)
  {
    ch=getc(in);
    if (ch==EOF) break;
    buffer[ptr]=ch;
  }

  parse_ringtone(out,buffer,ptr,rtt_info);

  return 0;
}

