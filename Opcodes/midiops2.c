/*
    midiops2.c:

    Copyright (C) 1997 Gabriel Maldonado

    This file is part of Csound.

    The Csound Library is free software; you can redistribute it
    and/or modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    Csound is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Csound; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA
*/

/****************************************/
/** midicXX   UGs by Gabriel Maldonado **/
/****************************************/

#include "csdl.h"
#include "midiops2.h"
#ifndef TRUE
#define TRUE (1)
#define FALSE (0)
#endif

#define f7bit   (FL(127.0))
#define f14bit  (FL(16383.0))
#define f21bit  (FL(2097151.0))

#define oneTOf7bit       ((MYFLT)1./127.)
#define oneTOf14bit      ((MYFLT)1./16383.)
#define oneTOf21bit      ((MYFLT)1./2097151.)

/*------------------------------------------------------------------------*/
/* 7 bit midi control UGs */

int imidic7(ENVIRON *csound, MIDICTL2 *p)
{
    MYFLT value;
    FUNC  *ftp;
    long  ctlno;

    if ((ctlno = (long)*p->ictlno) < 0 || ctlno > 127)
      return initerror(Str("illegal controller number"));
    else {
      value = (MYFLT)(curip->m_chnbp->ctl_val[ctlno] * oneTOf7bit);
      if (*p->ifn > 0) {
        if ((ftp = ftfind(csound, p->ifn)) == NULL)
          return NOTOK; /* if valid ftable, use value as index   */
        value = *(ftp->ftable + (long)(value*ftp->flen)); /* no interpolation */
      }
      *p->r = value * (*p->imax - *p->imin) + *p->imin; /* scales the output*/
    }
    return OK;
}


int midic7set(ENVIRON *csound, MIDICTL2 *p)
{
    long  ctlno;
    if ((ctlno = (long)*p->ictlno) < 0 || ctlno > 127) {
      return initerror(Str("illegal controller number"));
    }
    else p->ctlno = ctlno;
    if (*p->ifn > 0) {
      if (((p->ftp = ftfind(csound, p->ifn)) == NULL))
        p->flag = FALSE;  /* invalid ftable */
      else p->flag= TRUE;
    }
    else p->flag= FALSE;
    return OK;
}

int midic7(ENVIRON *csound, MIDICTL2 *p)
{
    MYFLT value;
    INSDS *lcurip = p->h.insdshead;

    value = (MYFLT) (lcurip->m_chnbp->ctl_val[p->ctlno] * oneTOf7bit);
    if (p->flag)  {             /* if valid ftable,use value as index   */
      value = *(p->ftp->ftable +
                (long)(value*p->ftp->flen));            /* no interpolation */
    }
    *p->r = value * (*p->imax - *p->imin) + *p->imin;   /* scales the output */
    return OK;
}

/*------------------------------------------------------------------------*/
/* 14 bit midi control UGs */

int imidic14(ENVIRON *csound, MIDICTL3 *p)
{
    MYFLT value;
    FUNC  *ftp;
    long  ctlno1;
    long  ctlno2;

    if ((ctlno1 = (long)*p->ictlno1) < 0 || ctlno1 > 127 ||
        (ctlno2 = (long)*p->ictlno2) < 0 || ctlno2 > 127 )
      return initerror(Str("illegal controller number"));
    else {
      value = (MYFLT) ((curip->m_chnbp->ctl_val[ctlno1] * 128 +
                        curip->m_chnbp->ctl_val[ctlno2])
                       * oneTOf14bit);
      if (*p->ifn > 0) {
        /* linear interpolation routine */
        MYFLT phase;
        MYFLT *base_address;
        MYFLT *base;
        MYFLT *top;
        MYFLT diff;
        long length;

        if ((ftp = ftfind(csound, p->ifn)) == NULL)
          return NOTOK; /* if valid ftable,use value as index   */
        phase = value * (length = ftp->flen);
        diff = phase - (long) phase;
        base = (base_address = ftp->ftable) + (long)(phase);
        top  = base + 1 ;
        top = (top - base_address > length) ?  base_address : top;
        value = *base + (*top - *base) * diff;
      }
      *p->r = value * (*p->imax - *p->imin) + *p->imin;  /* scales the output*/
    }
    return OK;
}


int midic14set(ENVIRON *csound, MIDICTL3 *p)
{
    long   ctlno1;
    long   ctlno2;
    if ((ctlno1 = (long)*p->ictlno1) < 0 || ctlno1 > 127 ||
        (ctlno2 = (long)*p->ictlno2) < 0 || ctlno2 > 127 ) {
      return initerror(Str("illegal controller number"));
    }
    p->ctlno1 = ctlno1;
    p->ctlno2 = ctlno2;
    if (*p->ifn > 0) {
      if (((p->ftp = ftfind(csound, p->ifn)) == NULL))
        p->flag = FALSE;  /* invalid ftable */
      else p->flag= TRUE;
    }
    else
      p->flag= FALSE;
    return OK;
}

int midic14(ENVIRON *csound, MIDICTL3 *p)
{
    MYFLT value;
    INSDS *lcurip = p->h.insdshead;

    value =     (MYFLT) ((lcurip->m_chnbp->ctl_val[p->ctlno1] *128  +
                          lcurip->m_chnbp->ctl_val[p->ctlno2] )
                         * oneTOf14bit);
    if (p->flag)  {     /* if valid ftable,use value as index   */
      MYFLT phase = value * p->ftp->flen; /* gab-A1 */
      MYFLT *base = p->ftp->ftable + (long)(phase);
      value = *base + (*(base+1) - *base) * (phase - (long) phase);

      /* linear interpolation routine */
      /*
        MYFLT phase;
        MYFLT *base_address;
        MYFLT *base;
        MYFLT *top;
        MYFLT diff;
        long length;

        phase =  value * (length = p->ftp->flen);
        diff = phase - (long) phase;
        base = (base_address = p->ftp->ftable) + (long)(phase);
        top  = base + 1 ;
        top = (top - base_address > length) ?  base_address : top;
        value = *base + (*top - *base) * diff;
      */
    }
    *p->r = value * (*p->imax - *p->imin) + *p->imin;   /* scales the output */
    return OK;
}

/*-----------------------------------------------------------------------------*/
/* 21 bit midi control UGs */



int imidic21(ENVIRON *csound, MIDICTL4 *p)
{
    MYFLT value;
    long   ctlno1;
    long   ctlno2;
    long   ctlno3;

    if ((ctlno1 = (long)*p->ictlno1) < 0 || ctlno1 > 127 ||
        (ctlno2 = (long)*p->ictlno2) < 0 || ctlno2 > 127 ||
        (ctlno3 = (long)*p->ictlno3) < 0 || ctlno3 > 127)
      return initerror(Str("illegal controller number"));
    else {
      value = (MYFLT) ((curip->m_chnbp->ctl_val[ctlno1] * 16384 +
                        curip->m_chnbp->ctl_val[ctlno2] * 128   +
                        curip->m_chnbp->ctl_val[ctlno3])
                       * oneTOf21bit);
      if (*p->ifn > 0) {
        /* linear interpolation routine */
        FUNC *ftp = ftfind(csound, p->ifn); /* gab-A1 */
        MYFLT phase;
        MYFLT *base;
        if (ftp == NULL) {
          sprintf(errmsg, Str("Invalid ftable no. %f"), p->ifn);
          return initerror(errmsg);
        }
        phase = value * ftp->flen;
        base = ftp->ftable + (long)(phase);
        value = *base + (*(base+1) - *base) * (phase - (long)phase);
      }
      *p->r = value * (*p->imax - *p->imin) + *p->imin;  /* scales the output*/
    }
    return OK;
}


int midic21set(ENVIRON *csound, MIDICTL4 *p)
{
    long   ctlno1;
    long   ctlno2;
    long   ctlno3;
    if ((ctlno1 = (long)*p->ictlno1) < 0 || ctlno1 > 127 ||
        (ctlno2 = (long)*p->ictlno2) < 0 || ctlno2 > 127 ||
        (ctlno3 = (long)*p->ictlno3) < 0 || ctlno3 > 127) {
      return initerror(Str("illegal controller number"));
    }
    p->ctlno1 = ctlno1;
    p->ctlno2 = ctlno2;
    p->ctlno3 = ctlno3;
    if (*p->ifn > 0) {
      if (((p->ftp = ftfind(csound, p->ifn)) == NULL))
        p->flag = FALSE;  /* invalid ftable */
      else
        p->flag= TRUE;
    }
    else
      p->flag= FALSE;
    return OK;
}

int midic21(ENVIRON *csound, MIDICTL4 *p)
{
    MYFLT value;
    INSDS *lcurip = p->h.insdshead;

    value = (MYFLT)((lcurip->m_chnbp->ctl_val[p->ctlno1] * 16384 +
                     lcurip->m_chnbp->ctl_val[p->ctlno2] * 128   +
                     lcurip->m_chnbp->ctl_val[p->ctlno3] )  * oneTOf21bit);
    if (p->flag)  {     /* if valid ftable,use value as index   */
      /* linear interpolation routine */
      MYFLT phase = value * p->ftp->flen;
      MYFLT *base = p->ftp->ftable + (long)(phase);
      value = *base + (*(base+1) - *base) * (phase - (long) phase);

      /*
        MYFLT phase;
        MYFLT *base_address;
        MYFLT *base;
        MYFLT *top;
        MYFLT diff;
        long length;

        phase = value * (length = p->ftp->flen);
        diff = phase - (long) phase;
        base = (base_address = p->ftp->ftable) + (long)(phase);
        top  = base + 1 ;
        top = (top - base_address > length) ?  base_address : top;
        value = *base + (*top - *base) * diff;
      */
    }
    *p->r = value * (*p->imax - *p->imin) + *p->imin;   /* scales the output */
    return OK;
}


/*-----------------------------------------------------------------*/
/* GLOBAL MIDI IN CONTROLS activable by score-oriented instruments*/
/*-----------------------------------------------------------------*/

int ictrl7(ENVIRON *csound, CTRL7 *p)
{
    MYFLT value;
    FUNC *ftp;
    long  ctlno;

    if ((ctlno = (long)*p->ictlno) < 0 || ctlno > 127)
      return initerror(Str("illegal controller number"));
    else {
      value = (MYFLT)(M_CHNBP[(int) *p->ichan-1]->ctl_val[ctlno]* oneTOf7bit);
      if (*p->ifn > 0) {
        if ((ftp = ftfind(csound, p->ifn)) == NULL)
          return NOTOK;               /* if valid ftable,use value as index   */
        value = *(ftp->ftable + (long)(value*ftp->flen)); /* no interpolation */
      }
      *p->r = value * (*p->imax - *p->imin) + *p->imin;  /* scales the output*/
    }
    return OK;
}


int ctrl7set(ENVIRON *csound, CTRL7 *p)
{
    long  ctlno;
    int chan;
    if ((ctlno = (long) *p->ictlno) < 0 || ctlno > 127) {
      return initerror(Str("illegal controller number"));
    }
    else if ((chan=(int) *p->ichan-1) < 0 || chan > 15) {
      return initerror(Str("illegal midi channel")); /* gab-A2 (chan number fix)*/
    }
    /*else if (midi_in_p_num < 0) midi_in_error("ctrl7");*/
    else p->ctlno = ctlno;
    if (*p->ifn > 0) {
      if (((p->ftp = ftfind(csound, p->ifn)) == NULL))
        p->flag = FALSE;  /* invalid ftable */
      else p->flag= TRUE;
    }
    else p->flag= FALSE;
    return OK;
}

int ctrl7(ENVIRON *csound, CTRL7 *p)
{
    MYFLT value = (MYFLT) (M_CHNBP[(int) *p->ichan-1]->ctl_val[p->ctlno] *
                           oneTOf7bit);
    if (p->flag)  {             /* if valid ftable,use value as index   */
      value =
        *(p->ftp->ftable + (long)(value*p->ftp->flen)); /* no interpolation */
    }
    *p->r = value * (*p->imax - *p->imin) + *p->imin;   /* scales the output */
    return OK;
}


/* 14 bit midi control UGs */

int ictrl14(ENVIRON *csound, CTRL14 *p)
{
    MYFLT value;
    long  ctlno1;
    long  ctlno2;
    int chan;

    if ((ctlno1 = (long)*p->ictlno1) < 0 || ctlno1 > 127 ||
        (ctlno2 = (long)*p->ictlno2) < 0 || ctlno2 > 127 )
      return initerror(Str("illegal controller number"));
    else if ((chan=(int) *p->ichan-1) < 0 || chan > 15)
      return initerror(Str("illegal midi channel"));
    else {
      value = (MYFLT)((M_CHNBP[chan]->ctl_val[ctlno1] * 128 +
                       M_CHNBP[chan]->ctl_val[ctlno2]) * oneTOf14bit);

      if (*p->ifn > 0) {
        /* linear interpolation routine */
        FUNC *ftp = ftfind(csound, p->ifn);
        MYFLT phase;
        MYFLT *base;
        if (ftp == NULL) {
          sprintf(errmsg, Str("Invalid ftable no. %f"), p->ifn);
          return initerror(errmsg);
        }
        phase = value * ftp->flen;
        base = ftp->ftable + (long)(phase);
        value = *base + (*(base+1) - *base) * (phase - (long)phase);
      }
      *p->r = value * (*p->imax - *p->imin) + *p->imin;  /* scales the output*/
    }
    return OK;
}


int ctrl14set(ENVIRON *csound, CTRL14 *p)
{
    long   ctlno1;
    long   ctlno2;
    int chan;
    if ((ctlno1 = (long)*p->ictlno1) < 0 || ctlno1 > 127 ||
        (ctlno2 = (long)*p->ictlno2) < 0 || ctlno2 > 127 ) {
      return initerror(Str("illegal controller number"));
    }
    else if ((chan=(int) *p->ichan-1) < 0 || chan > 15) {
      return initerror(Str("illegal midi channel"));
    }
    p->ctlno1 = ctlno1;
    p->ctlno2 = ctlno2;
    if (*p->ifn > 0) {
      if (((p->ftp = ftfind(csound, p->ifn)) == NULL))
        p->flag = FALSE;  /* invalid ftable */
      else p->flag= TRUE;
    }
    else
      p->flag= FALSE;
    return OK;
}

int ctrl14(ENVIRON *csound, CTRL14 *p)
{
    MYFLT value;
    int chan=(int) *p->ichan-1;

    value = (MYFLT)((M_CHNBP[chan]->ctl_val[p->ctlno1] * 128 +
                     M_CHNBP[chan]->ctl_val[p->ctlno2]) * oneTOf14bit);

    if (p->flag)  {             /* if valid ftable,use value as index   */
                                /* linear interpolation routine */
       MYFLT phase = value * p->ftp->flen;
       MYFLT *base = p->ftp->ftable + (long)(phase);
       value = *base + (*(base+1) - *base) * (phase - (long) phase);

    }
    *p->r = value * (*p->imax - *p->imin) + *p->imin;   /* scales the output */
    return OK;
}

/*-----------------------------------------------------------------------------*/
/* 21 bit midi control UGs */

int ictrl21(ENVIRON *csound, CTRL21 *p)
{
    MYFLT  value;
    long   ctlno1;
    long   ctlno2;
    long   ctlno3;
    int chan;

    if ((ctlno1 = (long)*p->ictlno1) < 0 || ctlno1 > 127 ||
        (ctlno2 = (long)*p->ictlno2) < 0 || ctlno2 > 127 ||
        (ctlno3 = (long)*p->ictlno3) < 0 || ctlno3 > 127)
      return initerror(Str("illegal controller number"));
    else if ((chan=(int) *p->ichan-1) < 0 || chan > 15)
      return initerror(Str("illegal midi channel"));
    else {
      value = (MYFLT)((M_CHNBP[chan]->ctl_val[ctlno1] * 16384 +
                       M_CHNBP[chan]->ctl_val[ctlno2] * 128   +
                       M_CHNBP[chan]->ctl_val[ctlno3]) * oneTOf21bit);

      if (*p->ifn > 0) {
        /* linear interpolation routine */
        FUNC *ftp = ftfind(csound, p->ifn);
        MYFLT phase;
        MYFLT *base;
        if (ftp == NULL) {
          sprintf(errmsg, Str("Invalid ftable no. %f"), p->ifn);
          return initerror(errmsg);
        }
        phase = value * ftp->flen;
        base = ftp->ftable + (long)(phase);
        value = *base + (*(base+1) - *base) * (phase - (long)phase);
        }
      *p->r = value * (*p->imax - *p->imin) + *p->imin;  /* scales the output*/
    }
    return OK;
}


int ctrl21set(ENVIRON *csound, CTRL21 *p)
{
    long   ctlno1;
    long   ctlno2;
    long   ctlno3;
    int chan;
    if ((ctlno1 = (long)*p->ictlno1) < 0 || ctlno1 > 127 ||
        (ctlno2 = (long)*p->ictlno2) < 0 || ctlno2 > 127 ||
        (ctlno3 = (long)*p->ictlno3) < 0 || ctlno3 > 127) {
      return initerror(Str("illegal controller number"));
    }
    else if ((chan=(int) *p->ichan-1) < 0 || chan > 15) {
      return initerror(Str("illegal midi channel"));
    }
    p->ctlno1 = ctlno1;
    p->ctlno2 = ctlno2;
    p->ctlno3 = ctlno3;
    if (*p->ifn > 0) {
      if (((p->ftp = ftfind(csound, p->ifn)) == NULL))
        p->flag = FALSE;  /* invalid ftable */
      else
        p->flag= TRUE;
    }
    else  p->flag= FALSE;
    return OK;
}

int ctrl21(ENVIRON *csound, CTRL21 *p)
{
    MYFLT value;
    int chan=(int) *p->ichan-1;
    value = (M_CHNBP[chan]->ctl_val[p->ctlno1] * 16384 +
             M_CHNBP[chan]->ctl_val[p->ctlno2] * 128   +
             M_CHNBP[chan]->ctl_val[p->ctlno3]) / f21bit;

    if (p->flag)  {     /* if valid ftable,use value as index   */
        /* linear interpolation routine */
       MYFLT phase = value * p->ftp->flen;
       MYFLT *base = p->ftp->ftable + (long)(phase);
       value = *base + (*(base+1) - *base) * (phase - (long) phase);
    }
    *p->r = value * (*p->imax - *p->imin) + *p->imin;   /* scales the output */
    return OK;
}


int initc7(ENVIRON *csound, INITC7 *p) /* for setting a precise value use the following formula:*/
{                      /* (value - min) / (max - min) */
    MYFLT fvalue;
    int chan;
    if ((fvalue = *p->ivalue) < 0. || fvalue > 1. )
      return initerror(Str("value out of range"));
    else if ((chan=(int) *p->ichan-1) < 0 || chan > 15 || !M_CHNBP[chan])
      return initerror(Str("illegal midi channel"));
    else M_CHNBP[chan]->ctl_val[(int) *p->ictlno] = fvalue * f7bit + FL(0.5);
    return OK;
}

int initc14(ENVIRON *csound, INITC14 *p)
{
    MYFLT fvalue;
    int value, msb, lsb, chan;
    if ((fvalue = *p->ivalue) < FL(0.0) || fvalue > FL(1.0) )
      return initerror(Str("value out of range"));
    else if ((chan=(int) *p->ichan-1) < 0 || chan > 15 || !M_CHNBP[chan])
      return initerror(Str("illegal midi channel"));
    else {
      value = (int)(fvalue * f14bit +FL(0.5));
      msb = value >> 7;
      lsb = value & 0x7F;
      M_CHNBP[chan]->ctl_val[(int) *p->ictlno1] = (MYFLT)msb;
      M_CHNBP[chan]->ctl_val[(int) *p->ictlno2] = (MYFLT)lsb;
    }
    return OK;
}

int initc21(ENVIRON *csound, INITC21 *p)
{
    MYFLT fvalue;
    int value, msb, xsb, lsb, chan;
    if ((fvalue = *p->ivalue) < FL(0.0) || fvalue > FL(1.0) )
      initerror(Str("value out of range"));
    else if ((chan=(int) *p->ichan-1) < 0 || chan > 15 || !M_CHNBP[chan])
      return initerror(Str("illegal midi channel"));
    else {
      value = (int)(fvalue * f21bit +FL(0.5));
      msb = value >> 14;
      xsb = (value >> 7) & 0x7F;
      lsb = value & 0x7F;
      M_CHNBP[chan]->ctl_val[(int) *p->ictlno1] = (MYFLT)msb;
      M_CHNBP[chan]->ctl_val[(int) *p->ictlno2] = (MYFLT)xsb;
      M_CHNBP[chan]->ctl_val[(int) *p->ictlno3] = (MYFLT)lsb;
    }
    return OK;
}

int midiin_set(ENVIRON *csound, MIDIIN *p)
{
    p->local_buf_index = MIDIINbufIndex & MIDIINBUFMSK;
    return OK;
}

int midiin(ENVIRON *csound, MIDIIN *p)
{
    unsigned char *temp;                        /* IV - Nov 30 2002 */
    if  (p->local_buf_index != MIDIINbufIndex) {
      temp = &(MIDIINbuffer2[p->local_buf_index++].bData[0]);
      p->local_buf_index &= MIDIINBUFMSK;
      *p->status = (MYFLT) (*temp & (unsigned char) 0xf0);
      *p->chan   = (MYFLT) ((*temp & 0x0f) + 1);
      *p->data1  = (MYFLT) *++temp;
      *p->data2  = (MYFLT) *++temp;
    }
    else *p->status = FL(0.0);
    return OK;
}

#define S       sizeof

static OENTRY localops[] = {
{ "ctrl14", 0xffff,                                                     },
{ "ctrl21", 0xffff,                                                     },
{ "ctrl7", 0xffff,                                                      },
{ "midic14", 0xffff,                                                    },
{ "midic21", 0xffff,                                                    },
{ "midic7", 0xffff,                                                     },
{ "midic7.i",S(MIDICTL2),1,  "i", "iiio", (SUBR)imidic7,   NULL,     NULL },
{ "midic7.k", S(MIDICTL2),3, "k", "ikko", (SUBR)midic7set, (SUBR)midic7,  NULL },
{ "midic14.i", S(MIDICTL3), 1,"i", "iiiio",(SUBR)imidic14,   NULL,     NULL },
{ "midic14.k", S(MIDICTL3), 3,"k", "iikko",(SUBR)midic14set, (SUBR)midic14, NULL },
{ "midic21.i", S(MIDICTL4),1,"i", "iiiiio",(SUBR)imidic21,   NULL,     NULL },
{ "midic21.k", S(MIDICTL4), 3,"k", "iiikko",(SUBR)midic21set,(SUBR)midic21, NULL },
{ "ctrl7.i", S(CTRL7), 1,    "i", "iiiio", (SUBR)ictrl7,     NULL,     NULL },
{ "ctrl7.k", S(CTRL7),  3,   "k", "iikko", (SUBR)ctrl7set,  (SUBR)ctrl7, NULL },
{ "ctrl14.i", S(CTRL14),1,   "i", "iiiiio",(SUBR)ictrl14,    NULL,     NULL },
{ "ctrl14.k", S(CTRL14), 3,  "k", "iiikko",(SUBR)ctrl14set, (SUBR)ctrl14,  NULL },
{ "ctrl21.i", S(CTRL21),1,   "i", "iiiiiio", (SUBR)ictrl21,  NULL,     NULL },
{ "ctrl21.k", S(CTRL21), 3,  "k", "iiiikko", (SUBR)ctrl21set, (SUBR)ctrl21, NULL },
{ "initc7", S(INITC7), 1,     "",  "iii",  (SUBR)initc7,     NULL,     NULL },
{ "initc14", S(INITC14), 1,   "",  "iiii", (SUBR)initc14,    NULL,     NULL },
{ "initc21", S(INITC21), 1,   "",  "iiiii",(SUBR)initc21,    NULL,     NULL },
{ "midiin", S(MIDIIN),   2,   "kkkk", "",     NULL, (SUBR)midiin,   NULL    },
};

LINKAGE
