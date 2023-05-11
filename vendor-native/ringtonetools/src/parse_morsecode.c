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

int parse_morsecode(FILE *in, FILE *out, struct rtt_info_t *rtt_info)
{
int ch,s,t;
char *p;
char code_alpha[50][5]={".-","-...","-.-.","-..",".","..-.","--.", /* A - G */
                   "....","..",".---","-.-",".-..","--",      /* H - M */
                   "-.","---",".--.","--.-",".-.","...","-",  /* N - T */
                   "..-","...-",".--","-..-","-.--","--.." }; /* U - Z */
char code_numbers[14][7]={"-----",".----","..---","...--","....-", /* 0 - 4 */
                     ".....","-....","--...","---..","----.", /* 5 - 9 */
                     ".-.-.-","--..--","..--.."," " };        /* Stop , ? */

  header_route(out,rtt_info);
  rtt_info->scale=0;
  rtt_info->modifier=0;
  rtt_info->tone=1;

  while ((ch=getc(in))!=EOF)
  {
    ch=tolower(ch);
    if (ch>='a' && ch <='z') p=code_alpha[ch-'a'];
      else
    if (ch>='0' && ch <='9') p=code_numbers[ch-'0'];
      else
    if (ch==',') p=code_numbers[11];
      else
    if (ch=='?') p=code_numbers[12];
      else
    if (ch=='$') p=code_numbers[10];  /* Full Stop? */
      else
    if (ch==' ') p=code_numbers[13];
      else
    { continue; }

    s=strlen(p);

    for (t=0; t<s; t++)
    {
      if (p[t]=='-')
      {
        rtt_info->length=3;
        rtt_info->tone=9;
        rtt_info->modifier=1;
        note_route(out,rtt_info);
      }
        else
      if (p[t]=='.')
      {
        rtt_info->length=4;
        rtt_info->tone=9;
        rtt_info->modifier=0;
        note_route(out,rtt_info);
/*
        rtt_info->length=4;
        rtt_info->tone=0;
        note_route(out,rtt_info);
*/
      }
        else
      if (p[t]==' ')
      {
        rtt_info->length=3;
        rtt_info->tone=0;
        rtt_info->modifier=1;
        note_route(out,rtt_info);
      }
    }
  }

  footer_route(out,rtt_info);

  return (0);
}



