/*
    ugrw1.c:

    Copyright (C) 1997 Robin Whittle

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

/* These files are based on Robin Whittle's
 *       ugrw1.c of 27 August 1996
 * and   ugrw1.h of 7 January 1995
 *
 * In February 1997, John Fitch reformatted the comments and
 * cleaned up some code in printksset() - which was working fine
 * but was inelegantly coded.
 * In February 1998, John Fitch modified the code wrt types so it
 * compiled with MicroSoft C without warnings.
 *
 *
 * Copyright notice - Robin Whittle  25 February 1997
 *
 * Documentation files, and the original .c and .h files, with more
 * spaced out comments, are available from http://www.firstpr.com.au
 *
 * The code in both ugrw1 and ugrw2 is copyright Robin Whittle.
 * Permission is granted to use this in whole or in part for any
 * purpose, provided this copyright notice remains intact and
 * any alterations to the source code, including comments, are
 * clearly indicated as such.
 */
#include "ugrw1.h"
#include <math.h>
#include <ctype.h>

/* Do List:
 *
 * Clean up code in zak - use arrays rather than messy pointer stuff.
 * Clean up doco file.
 * Add maxampr to ugrw1.doc
 */

/*      27 August 1996:
 *
 *      Offset bug in ftkrchkw() fixed, as per my 3 Feb bug report.
 *
 *      1 February 1996:
 *      Offset bugs fixed in init time table write - same as in table read.
 *      See itblchkw()
 *
 *      New ugen 7 January 1996 - peak.
 *
 *      Unit generators by Robin Whittle        8 September 1995
 *      UGRW1.H contains data structures.
 *
 *      See UGRW1.DOC for more details on using these ugens.  Essential
 *      functional documentation for them is included here to ensure it
 *      is never separated from the code.
 *
 * Changes required to other files:
 *---------------------------------
 *
 * 2 -  A new fgens.c is needed to provide ftfindp() - finding tables at
 *      performance time.
 *
 * 3 -  prototype.h has a new entry for ftfindp() - see start of fgens.c.
 *
 * Table write ugens
 *------------------
 *
 *      These table write ugens are adapted from similar code in my modified,
 *      "bug free" version of the table read code in UGENS2.H & .C.
 *
 *      Ugens:          Subroutines:    Data structure:
 *
 *      tablew          tblsetw()       TABLEW
 *                      ktablew()       "
 *                      tablew()        "
 *
 *      itablew         itablew()       "
 *
 *      tablewkt        tblsetwkt()     TABLEW
 *                      ktablewkt()     "
 *                      tablewkt()      "
 *--
 *
 *      These find out the length of a table, and write the guard point
 *      of a table.
 *
 *      Ugens:          Subroutines:    Data structure:
 *
 *      tableng         tableng()       TABLENG
 *      itableng        itableng()
 *
 *      tablegpw        tablegpw()      TABLEGPW
 *      itablegpw       itablegpw()
 *
 *      Two ugens to manipulate the entire contents, or part of the contents
 *      of a table in a single k cycle.
 *
 *      Ugens:          Subroutines:    Data structure:
 *
 *      tablemix        tablemix()      TABLEMIX
 *      itablemix       itablemix()
 *
 *      tablecopy       tablecopy()     TABLECOPY
 *      itablecopy      itablecopy()
 *
 *      Two ugens which enable a rate reading and writing to sequential
 *      locations in tables.  Useful of a rate manipulation of table contents
 *      and conversely for writing a rate data into a table, where k rate
 *      code can manipulate it before it is written back to an a rate variable.
 *
 *      Ugens:          Subroutines:    Data structure:
 *
 *      tablera         tableraset()    TABLERA
 *                      tablera()
 *
 *      tablewa         tablewaset()    TABLEWA
 *                      tablewa()
 *
 *------
 *
 *      The following ugens are fudges - it would be better to have arrays -
 *      like ablah[kx][ky].
 *
 *      Arrays should be local and global, multidimensional, a, k and i rate.
 *
 *      I will use these to get things done until arrays are implemented in
 *      the core of the language.
 *
 *      The zak system uses one area of memory as a global i or k rate
 *      patching area, and another for audio rate patching.
 *
 *      Ugens:          Subroutines:    Data structure:
 *
 *      zakinit         zakinit()       ZAKINIT
 *
 *      zir             zkset()         -
 *      zkr             zir()           ZKR
 *                      zkr()           ZKR
 *      ziw             ziw()           ZKW
 *      zkw             zkw()           ZKW
 *      ziwm            ziwm()          ZKWM
 *      zkwm            zkwm()          ZKWM
 *      zkmod           zkmod()         ZKMOD
 *      zkcl            zkcl()          ZKCL
 *
 *      zar             zaset()         -
 *      zaw             zar()           ZAR
 *                      zaw()           ZAW
 *      zarg            zarg()          ZARG
 *
 *      zawm            zawm()          ZAWM
 *      zamod           zamod()         ZAMOD
 *      zacl            zacl()          ZACL
 *
 *---
 *      Four ugens for reading the abosolute time since the start of the
 *      performance, and two for reading the time since the instrument was
 *      initialised:
 *
 *      Ugens:          Subroutines:    Data structure:
 *
 *      timek           timek()         RDTIME
 *      itimek          itimek()        "
 *
 *      times           timesr()        "
 *      itimes          itimes()        "
 *
 *      instimek        instimset()     "
 *      instimes        instimek()      "
 *                      instimes()      "
 *---
 *      Two ugens for printing a k rate variable during k rate processing.
 *
 *      Ugens:          Subroutines:    Data structure:
 *
 *      printk          printk()        PRINTK
 *      printks         printks()       PRINTKS
 *
 */


/*****************************************************************************
 *
 * Table write syntax
 *-------------------
 *
 * Use itablew when all inputs are init time variables or constants
 * and you only want to run it at the initialisation of the instrument.
 *
 * tablew is for writing at k or at a rates, with the table number being
 * specified at init time.
 *
 * tablewkt is the same, but uses a k rate variable for selecting the table
 * number.  The valid combinations of variable types are shown by the first
 * letter of the variable names:
 *
***     itablew   isig, indx, ifn [,ixmode] [,ixoff] [,iwgmode]
 *
***     tablew    ksig, kndx, ifn [,ixmode] [,ixoff] [,iwgmode]
***     tablew    asig, andx, ifn [,ixmode] [,ixoff] [,iwgmode]
 *
***     tablewkt  ksig, kndx, kfn [,ixmode] [,ixoff] [,iwgmode]
***     tablewkt  asig, andx, kfn [,ixmode] [,ixoff] [,iwgmode]
 *
 * isig, ksig,  The value to be written into the table.
 * asig
 *
 * indx, kndx,  Index into table, either a positive number range
 * andx         matching the table length (ixmode = 0) or a 0 to 1
 *              range (ixmode != 0)
 *
 * ifn, kfn     Table number. Must be >= 1. Floats are rounded down to
 *              an integer.  If a table number does not point to a
 *              valid table, or the table has not yet been loaded
 *              (gen01) then an error will result and the instrument
 *              will be de-activated.
 *
 * ixmode       Default 0  ==0  xndx and ixoff ranges match the length
 *                              of the table.
 *
 *                         !=0  xndx and ixoff have a 0 to 1 range.
 *
 *
 * ixoff        Default 0  ==0  Total index is controlled directly by
 *                              xndx.  ie. the indexing starts from the
 *                              start of the table.
 *
 *                         !=0  Start indexing from somewhere else in
 *                              the table. Value must be positive and
 *                              less than the table length (ixmode = 0)
 *                              or less than 1 (ixmode !=0
 *
 * iwgmode      Default 0  ==0  Limit mode      } See full explanation in
 *                         ==1  Wrap mode       } ugrw1.doc
 *                         ==2  Guardpoint mode }
 */

/* Known bugs in table write:
 *
 *      Watch out for giving a number (like "5" or p6) instead of a a rate
 *      variable to the index of table write ugens operating at a rate.
 *      The ugen gets a pointer to the number, but it is expecting a pointer
 *      to an a rate variable - which is an array of numbers.
 *      Upsamp the number (or k rate variable) to an a rate variable first.
 *
 *      This pitfall could be fixed with a revamp of how Csound processes the
 *      orchestra, or by making different named ugens for different types of
 *      variables.
 */



/*****************************************************************************
 *
 * Other table manipulation ugens
 * ------------------------------
 *
*** ir  itableng ifn
*** kr  tableng  kfn
 *
 * ifn  i rate number of function table
 * kfn  k rate number of function table
 *
 *      These return the length of the specified table.  This will be a power
 *      of two number in most circumstances - it will not show whether
 *      a table has a guardpoint or not - it seems this information is not
 *      available in the table's data structure. If table is not found, then
 *      0 will be returned.
 *
 *      Likely to be useful for setting up code for table manipulation
 *      operations, such as tablemix and tablecopy.
 *
 *
***     itablegpw ifn
***     tablegpw  kfn
 *
 *      For writing the table's guard point, with the value which is in
 *      location 0.  Does nothing if table does not exist.
 *
 *      Likely to be useful after manipulating a table with tablemix or
 *      tablecopy.
 *
 *------
 *
***     tablemix  kdft, kdoff, klen, ks1ft, ks1off, ks1g, ks2ft, ks2off, ks2g
***     itablemix idft, idoff, ilen, is1ft, is1off, is1g, is2ft, is2off, is2g
 *
 *      This ugen mixes from two tables, with separate gains into the
 *      destination table.  Writing is done for klen locations, usually
 *      stepping forward through the table - if klen was positive.
 *      If it is negative, then the writing and reading order is backwards -
 *      towards lower indexes in the tables.  This bidirectional option makes
 *      it easy to shift the contents of a table by reading from it and
 *      writing back to it.
 *
 *      If klen is 0, no writing occurs. Note that the internal integer value
 *      of klen is derived from the ANSI C floor() function - which returns
 *      the next most negative integer.  Hence a fractional negative klen
 *      value of -2.3 would create an internal length of 3, and cause
 *      the copying to start from the offset locations and proceed for
 *      two locations to the left.
 *
 *      The total index for table reading and writing is calculated from the
 *      starting offset for each table, plus the index value, which starts
 *      at 0 and then increments (or decrements) by 1 as mixing proceeds.
 *
 *      These total indexes can potentially be very large, since there is no
 *      restriction on the offset or the klen. However each total index for
 *      each table is ANDed with a length mask (such as 0000 0111 for a table
 *      of length 8) to form a final index which is actually used for
 *      reading or writing.  So no reading or writing can occur outside
 *      the tables.
 *
 *      This is the same as "wrap" mode in table read and write. This process
 *      does not read or write the guardpoint.
 *
 *      If a table has been rewritten with one of these, then if it has a
 *      guardpoint which is supposed to contain the same value as the
 *      location 0, then call tablegpw afterwards.
 *
 *      The indexes and offsets are all in table steps - they are not
 *      normalised to 0 - 1.  So for a table of length 256, klen would be
 *      set to 256 if all the table was to be read or written.
 *
 *      The tables do not need to be the same length - wrapping occurs
 *      individually for each table.
 *
 * kdft         Destination function table.
 *
 * kdoff        Offset to start writing from. Can be negative.
 *
 * klen         Number of write operations to perform. Negative means
 *              work backwards.
 *
 * ks1ft ks2ft  Source function tables. These can be the same as the
 *              destination table, if care is exercised about direction
 *              of copying data.
 *
 * ks1off ks2off Offsets to start reading from in source tables.
 *
 * ks1g ks2g    Gains to apply when reading from the source tables.  The
 *              results are added and the sum is written to the destination
 *              table.
 *
 *---
 *
***     tablecopy  kdft, ksft
***     itablecopy idft, isft
 *
 *      Simple, fast table copy ugens.  Takes the table length from the
 *      destination table, and reads from the start of the source table.
 *      For speed reasons, does not check the source length - just copies
 *      regardless - in "wrap" mode.  This may read through the source
 *      table several times.  A source table with length 1 will cause
 *      all values in the destination table to be written to its value.
 *
 *      Table copy cannot read or write the guardpoint.  To read it
 *      use table read, with ndx = the table length.  Likewise use
 *      table write to write it.
 *
 *      To write the guardpoint to the value in location 0, use tablegpw.
 *
 *      This is primarily to change function tables quickly in a real-time
 *      situation.
 *
 * kdft         Number of destination function table.
 *
 * ksft         Number of source function table.
 *
 *-----
 *
*** ar     tablera  kfn, kstart, koff
 *
*** kstart tablewa  kfn, asig, koff
 *
 * ar           a rate distination for reading ksmps values from a table.
 *
 * kfn          i oro k rate number of the table to read or write.
 *
 * kstart       Where in table to read or write.
 *
 * asig         a rate signal to read from when writing to the table.
 *
 * koff         k rate offset into table. Range unlimited - see explanation
 *              at end of this section.
 *
 *      In one application, these are intended to be used in pairs, or with
 *      several tablera ugens before a tablewa - all sharing the same kstart
 *      variable.
 *
 *      These read from and write to sequential locations in a table at audio
 *      rates, with ksmps floats being written and read each cycle.
 *
 *      tablera starts reading from location kstart.
 *      tablewa starts writing to location kstart, and then writes to kstart
 *      with the number of the location one more than the one it last wrote.
 *      (Note that for tablewa, kstart is both an input and output variable.)
 *      If the writing index reaches the end of the table, then the no further
 *      writing occurs and zero is written to kstart.
 *
 *      For instance, if the table's length was 16 (locations 0 to 15), and
 *      ksmps was 5. Then the following steps would occur with repetitive
 *      runs of the tablewa ugen, assuming that kstart started at 0.
 *
 *      Run no. Initial Final   locations written
 *              kstart  kstart
 *
 *      1       0       5       0  1  2  3  4
 *
 *      2       5      10       5  6  7  8  9
 *
 *      3      10      15      10 11 12 13 14
 *
 *      4      15       0      15
 *
 *      This is to facilitate processing table data using standard a rate
 *      orchestra code between the tablera and tablewa ugens:
 *
 * ;-----------------------------
 *                              ;
 *      kstart = 0              ;
 *                              ; Read 5 values from table into an a rate
 *                              ; variable.
 *
 * lab1: atemp  tablera ktabsource, kstart, 0
 *
 *                              ; Process the values using a rate code.
 * atemp = log(atemp)           ;
 *                              ; Write it back to the table
 *
 * kstart tablewa ktabdest, atemp, 0
 *
 *                              ; Loop until all table locations have been
 *                              ; processed.
 * if ktemp > 0 goto lab1       ;
 *                              ;
 * ;-----------------------------
 *
 *      This example shows a processing loop, which runs every k cycle,
 *      reading each location in the table ktabsource, and writing the log
 *      of those values into the same locations of table ktabdest.
 *
 *      This enables whole tables, parts of tables (with offsets and different
 *      control loops) and data from several tables at once to be manipulated
 *      with a rate code and written back to another (or the same) table.
 *      This is a bit of a fudge, but it is faster than doing it with
 *      k rate table read and write code.
 *
 *      Another application is:
 *
 * ;-----------------------------
 *
 * kzero = 0                    ;
 * kloop = 0                    ;
 *                              ;
 * kzero tablewa 23, asignal, 0 ; ksmps a rate samples written into
 *                              ; locations 0 to (ksmps -1) of table 23.
 *                              ;
 * lab1: ktemp table kloop, 23  ; Start a loop which runs ksmps times,
 *                              ; in which each cycle processes one of
 * [ Some code to manipulate ]  ; table 23's values with k rate orchestra
 * [ the value of ktemp.     ]  ; code.
 *                              ;
 *                              ;
 *      tablew ktemp, kloop, 23 ; Write the processed value to the table.
 *                              ;
 * kloop = kloop + 1            ; Increment the kloop, which is both the
 *                              ; pointer into the table and the loop
 * if kloop < ksmps goto lab1   ; counter.  Keep looping until all values
 *                              ; in the table have been processed.
 *                              ;
 * asignal tablera 23, 0, 0     ; Copy the table contents back to an a rate
 *                              ; variable.
 * ;-----------------------------
 *
 *
 * koff This is an offset which is added to the sum of kstart and the internal
 *      index variable which steps through the table.  The result is then
 *      ANDed with the lengthmask (000 0111 for a table of length 8 - or
 *      9 with guardpoint) and that final index is used to read or write to
 *      the table.  koff can be any value.  It is converted into a long using
 *      the ANSI floor() function so that -4.3 becomes -5.  This is what we
 *      would want when using wide rangeing offsets.
 *
 *      Ideally this would be an optional variable, defaulting to 0, however
 *      with the existing Csount orchestra read code, such default parameters
 *      must be init time only.  We want k rate here, so we cannot have a
 *      default.
 *
 * Notes on tablera and tablewa
 *-----------------------------
 *
 *      These are a fudge, but they allows all Csounds k rate operators to be
 *      used (with caution) on a rate variables - something that would only
 *      be possible otherwise by ksmps = 1, downsamp and upsamp.
 *
 *      Several cautions:
 *
 * 1 -  The k rate code in the processing loop is really running at a rate,
 *      so time dependant functions like port would not work normally.
 *
 * 2 -  This system will produce undesirable results unless the ksmps fits
 *      within the table length.  For instance a table of length 16 will
 *      accomodate 1 to 16 samples, so this example will work with
 *      ksmps = 1 to 16.
 *
 *      Both these ugens generate an error and deactivate the instrument if
 *      a table with length < ksmps is selected. Likewise an error occurs
 *      if kstart is below 0 or greater than the highest entry in the table -
 *      if kstart >= table length.
 *
 * 3 -  kstart is intended to contain integer values between 0 and (table
 *      length - 1).  Fractional values above this should not affect operation
 *      but do not achieve anything useful.
 *
 * 4 -  These ugens do not do interpolation and the kstart and koff parameters
 *      always have a range of 0 to (table length - 1) - not 0 to 1 as is
 *      available in other table read/write ugens.  koff can be outside this
 *      range but it is wrapped around by the final AND operation.
 *
 * 5 -  These ugnes are permanently in wrap mode.  When koff is 0, no wrapping
 *      needs to occur, since the kstart++ index will always be within the
 *      table's normal range.  koff != 0 can lead to wrapping.
 *
 * 6 -  The offset does not affect the number of read/write cycles performed,
 *      or the value written to kstart by tablewa.
 *
 * 7 -  These ugens cannot read or write the guardpoint.  Use tablegpw to
 *      write the guardpoint after manipulations have been done with tablewa.
 *
 *------
 *
 */



/*****************************************************************************
 *
 * The "zak" system
 * ----------------
 *
 *      The zak system uses one area of memory as a global i or k rate
 *      patching area, and another for audio rate patching. These are
 *      establised by a ugen which must be called once only.
 *
***     zakinit isizea, isizek
 *
 *      isizek is the number of locations we want to reserve for floats
 *      in the zk space.  These can be written and read at i and k rates.
 *
 *      isizea is the number of audio rate "locations" for a rate patching.
 *      Each "location" is actually an array which is ksmps long.
 *
 *      eg. zakinit 100 30 reserves memory for locations 0 to 100 of zk space
 *      and for locations 0 to 30 of a rate za space. With ksmps = 8, this
 *      would take 101 floats for zk and 248 floats for za space.
 *
 *      At least one location is always allocated for both zk and za spaces.
 *
 *      These patching locations can be referred to by number with the
 *      following ugens.
 *
 *      There are two short, simple, fast opcodes which read a location in
 *      zk space, at either i time or at the k rate.
 *
*** ir  zir     indx
*** kr  zkr     kndx
 *
 *      Likewise, two write to a location in zk space at i time or at the k
 *      rate.
 *
***     ziw     isig, indx
***     zkw     ksig, kndx
 *
 *      These are fast and always check that the index is within the
 *      range of zk space.  If it is out of range, an error is reported
 *      and 0 is returned, or no writing takes place.
 *
***     ziwm    isig, indx [,imix]
***     zkwm    ksig, kndx [,imix]
 *
 *      Like ziw and zkw above, except that they can mix - add the sig
 *      to the current value of the variable.  If no imix is specified,
 *      they mix, but if imix is used, then 0 will cause writing (like
 *      ziw and zkw) any other value will cause direct mixing.
 *
*** kr  zkmod   ksig, kzkmod
 *
 *      zkmod is a unit generator intended to facilitate the modulation
 *      of one signal by another, where the modulating signal comes from
 *      a zk variable.  Either additive or mulitiplicative modulation is
 *      provided.
 *
 *      ksig is the input signal, to be modulated and sent to the output
 *      of the zkmod unit generator.
 *
 *      kzkmod controls which zk variable is used for modulation.  A positive
 *      value means additive modulation, a negative value means multiplicative
 *      modulation.  A value of 0 means no change to ksig - it is transferred
 *      directly to the output.
 *
 *      For instance kzkmod = 23 will read from zk variable 23, and add the
 *      value it finds there to ksig.  If kzkmod = -402, then ksig is
 *      multiplied by the value read from zk location 402.
 *
 *      kskmod can be an init time constant, or a k rate value.
 *
 *
***     zkcl    kfirst, klast
 *
 *      This will clear to zero one or more variables in the zk space.
 *      Useful for those variables which are accumulators for mixing
 *      things during the processing for each cycle, but which must be
 *      cleared to zero before the next set of calculations.
 *
 *------
 *
 *      For a rate reading and writing, in the za space, we use similar
 *      opcodes:
 *
*** ar  zar     kndx
 *
 *      kndx points to which za variable to read.  This reads the number kndx
 *      array of floats which are the ksmps number of audio rate floats to be
 *      processed in a k cycle.
 *
*** ar  zarg    kndx, kgain
 *
 *      Similar to zar, but multiplies the a rate signal by a k rate value
 *      kgain.
 *
***     zaw     asig, kndx
 *
 *      Writes into the array specified by kndx.
 *
***     zawm    asig, kndx [,imix]
 *
 *      Like zaw above, except that it can mix - add the asig to the current
 *      value of the destination za variable.  If no imix is specified,
 *      it mixes, but if imix is used, then 0 will cause a simple write (like
 *      zaw) and any other value will cause mixing.
 *
*** ar  zamod   asig, kzamod
 *
 *      Modulation of one audio rate signal by a second one - which comes from
 *      a za variable.  The location of the modulating variable is controlled
 *      by the k rate variable kzamod.  This is the audio rate version of
 *      zkmod described above.
 *
***     zacl    kfirst, klast
 *
 *      This will clear to zero one or more variables in the za space.
 *      Useful for those variables which are accumulators for mixing
 *      things during the processing for each cycle, but which must be
 *      cleared to zero before the next set of calculations.
 *
 *------
 *
 * What types of input variables are used?
 *
 *                                      Runs at time
 * ir   zir     indx                    i
 * kr   zkr     kndx                            k
 *
 *      ziw     isig, indx              i
 *      zkw     ksig, kndx                      k
 *
 *      ziwm    isig, indx, imix        i
 *      zkwm    ksig, kndx, kmix                k
 *
 *      zkcl    kfirst, klast                   k
 *
 * ar   zar     kndx                            k but does arrays
 * ar   zarg    kndx, kgain                     k but does arrays
 *
 *      zaw     asig, kndx                      k but does arrays
 *
 *      zawm    asig, kndx, kmix                k but does arrays
 *
 *      zacl    kfirst, klast                   k but does arrays
 *
 *
 * isig         }
 * indx         } Known at init time
 * imix         }
 *
 * ksig         }
 * kndx         }
 * kmix         } k rate variables
 * kfirst       }
 * klast        }
 * kgain        }
 *
 * asig         } a rate variable - an array of floats.
 *
 */


/**************************************************************************
 *
 * Simple time reading ugens
 *--------------------------
 *
*** kr  timek
*** kr  timesr
 *
 *      timek returns a float containing an integer - the number of k cycles
 *      since the start of the performance of the orchestra.
 *
 *      timesr returns a float with the number of seconds.
 *
 *      They both expect a k rate variable as their output. There are no
 *      input parameters.
 *
*** ir  itimek
*** ir  itimes
 *
 *      itemek and itimes are similar ugens which only operate during the
 *      initialisation of an instance of the instrument.
 *
*** kr  instimek
*** kr  instimes
 *
 *      These are similar to timek and timesr, except they return the
 *      time since the start of this instance of the instrument.
 *
 */


/**************************************************************************
 *
 * Two ugens for printing at k rate
 *---------------------------------
 *
 *      printk prints one k rate value on every k cycle, every second or at
 *      intervals specified.  First the instrument number is printed, then
 *      the absolute time in seconds, then a specified number of spaces, then
 *      the value.  The variable number of spaces enables different values to
 *      be spaced out across the screen - so they are easier to view.
 *      This is for debugging orchestra code.
 *
***     printk  itime, kval [, ispace]
 *
 * itime How much time in seconds is to elapse between printings.
 *
 * kval  The number to be printed.
 *
 * ispace How many spaces to insert before it is printed.  (Max 120.)
 *        Default = 0.
 *
 *      The first print is on the first k cycle of the instance of the
 *      instrument.
 *
 *------
 *
 *      printks prints numbers and text, with up to four printable numbers
 *      - which can be i or k rate values.
 *
***     printks "txtstring", itime, kval1, kval2, kval3, kval4
 *
 * txtstring    Text to be printed first - can be up to 130 characters at
 *              least.  _Must_ be in double quotes.
 *
 *              The string is printed as is, but standard printf %f etc.
 *              codes are interpreted to print the four parameters.
 *
 *              However the \n style of character codes are not interpreted
 *              by printf.  (They are converted by the C compiler in
 *              string literals found in the C program.) This ugen therefore
 *              provides certain specific codes which are expanded:
 *
 *              \n or \N        Newline
 *              \t or \T        Tab
 *
 *              ^               Escape character
 *              ^^              ^
 *
 *              ~               Escape and '[' These are the leadin codes for
 *                              MSDOS ANSI.SYS screen control characters.
 *              ~~              ~
 *
 *              An init error is generated if the first parameter is not
 *              a string of length > 0 enclosed in double quotes.
 *
 *              A special mode of operation allows this ugen to convert
 *              the first input parameter into a 0 to 255 character, and
 *              to use it as the first character to be printed.  This enables
 *              a Csound program to send arbitrary characters to the console -
 *              albeit with a little awkwardness.  printf() does not have a
 *              format specifier to read a float and turn it into a byte
 *              for direct output. We could add extra code to do this if
 *              we really wanted to put arbitrary characters out with ease.
 *
 *              To acheive this, make the first character of the string a
 *              # and then, if desired continue with normal text and format
 *              specifiers.  Three more format specifers may be used - they
 *              access kval2, kval3 and kval4.
 *
 * itime How much time in seconds is to elapse between printings.
 *
 * kvalx The k rate values to be printed. Use 0 for those which are not
 *       used.
 *
 *
 */



/**************************************************************************
 *
 * Two ugens for tracking peak signal levels
 *------------------------------------------
 *
 *      peakk takes k rate inputs and peak takes a rate inputs.
 *
 *      They maintain the output k rate variable as the peak absolute
 *      level so far received.
 *
***     kpeakout peakk  ksigin
***     kpeakout peaka  asigin
 *
 *
 * kpeakout     Output equal to the highest absolute value received
 *              so far.
 *              This is effectively an input to the ugen as well, since it
 *              reads kpeakout in order to decide whether to write
 *              something higher into it.
 *
 * ksigin       k rate input signal.
 * asigin       a rate input signal.
 *
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

/* Table write code
 *
 * The way that the k rate table numbers are handled by different
 * ugens and functions is analogous to the approach used in the new
 * ugens2.c.  */

/* itblchkw()
 *
 * Internal function itblchkw().  Called at init time by tblsetw() to
 * initialise some of the variables in TABLEW - which is pointed to by
 * p.
 *
 * Also called by itablew().
 *
 * A similar function ptblchkw() does the same job at performance time
 * - k processing cycles.
 *
 * Both these functions are virtually identical to those itblchk() and
 * ptblchk() in ugens2.c.  The differences are:
 *
 * 1 - They use TABLEW instead of TABLE.
 * 2 -  There is no iwrap parameter.
 * 3 -  There is an igwmode parameter.
*/

int itblchkw(ENVIRON *csound, TABLEW *p)
{
    /* Get pointer to the function table data structure of the table
     * number specified in xfn. Return 0 if it cannot be found.
     *
     * ftfind() generates an error message if the table cannot be
     * found. This works OK at init time.  It also deactivates the
     * instrument.
     *
     * It also checks for numbers < 0, and table 0 is never valid, so we
     * do not need to check here for the table number being < 1.  */

    if ((p->ftp = ftfind(csound, p->xfn)) == NULL)
      return NOTOK;
    /* Although TABLEW has an integer variable for the table number
     * (p->pfn) we do not need to * write it.  We know that the * k
     * and a rate functions * which will follow will not * be
     * expecting a changed * table number.
     *
     * p->pfn exists only for checking * table number changes for
     * functions * which are expecting a k rate * table number.  */

    /* Set denormalisation factor to 1 or table length, depending on
     * the state of ixmode.  1L means a 32 bit 1.  */
    if (*p->ixmode)
      p->xbmul = p->ftp->flen;
    else    p->xbmul = 1L;
    /* Multiply the ixoff value by the xbmul denormalisation
     * factor and then check it is between 0 and the table length.  */
    if ((p->offset = p->xbmul * *p->ixoff) < FL(0.0)
        || p->offset > p->ftp->flen) {
      sprintf(errmsg, Str("Table write offset %f < 0 or > tablelength"),
              p->offset);
      return initerror(errmsg);
    }
    p->iwgm   = (long)*p->iwgmode;
    return OK;
}

/*************************************/

/* ptblchkw()
 *
 * This is called at init time by tblsetwkt() to set up the TABLEW
 * data structure for subsequent k and a rate operations which are
 * expecting the table number to change at k rate.
 *
 * tblsetwkt() does very little - just setting up the iwgm variable in
 * TABLE. All the other variables depend on the table number. This is
 * not available at init time, so the following two functions must
 * look for the changed table number and set up the variables
 * accordingly - generating error messages in a way which works at
 * performance time.
 *
 * k rate    a rate
 *
 * ktablewkt tablewkt
 *
 */
int ptblchkw(ENVIRON *csound, TABLEW *p)
{
    /* TABLEW has an integer variable for the previous table number
     * (p->pfn).
     *
     * Now (at init time) we do not know the function table number which
     * will be provided at perf time, so set p->pfn to 0, so that the k or
     * a rate code will recognise that the first table number is different
     * from the "previous" one.
     */
    p->pfn = 0;         /* the only other thing to do is write the iwgmode
                           value into the immediate copy of it in TABLEW. */
    p->iwgm = (long)*p->iwgmode;
    return OK;
}

/*---------------------------------------------------------------------------*/

/* tblsetw()
 *
 * This is called at init time to set up TABLEW for the a and k rate
 * table read functions which are expecting the table number to be
 * fixed at i time.
 *
 * Call the itblchkw() function to do the work.
 */
int tblsetw(ENVIRON *csound, TABLEW *p)
{
    return itblchkw(csound,p);
}

/* tblsetwkt()
 *
 * This is called at init time to set up TABLEW for the a and k rate
 * table read functions which are expecting the table number to be a k
 * rate variable.
 *
 * Call the ptblchkw() function to do the work.  */
int tblsetwkt(ENVIRON *csound, TABLEW *p)
{
    return ptblchkw(csound,p);
}

/* itablew()
 *
 * Used to write to a table only at init time. It is called (via the
 * opcodlst in entry.c) for the itablew opcode.
 *
 * It sets up some variables in the TABLEW data structure for this
 * instance and calls ktablew() once to write to the table.  */
int itablew(ENVIRON *csound, TABLEW *p)
{
/*   printf("tableiw: p->xsig, p->xndx = %f, %f\n", *p->xsig, *p->xndx); */
    if (itblchkw(csound,p)==OK)
      return ktablew(csound,p);
    return NOTOK;
}

/*---------------------------------------------------------------------------*/

/* ktablew is called with p pointing to the TABLEW data structure -
 * which contains the input arguments.  */

int ktablew(ENVIRON *csound, TABLEW  *p)
{
/* Pointer to data structure for accessing the table we will be
 * writing to.
 */
    FUNC        *ftp;
        /* 32 bit integers for pointing into table and for the table
         * length - which is always a power of 2.  The table must
         * actually be one more longer than this if it has a guard
         * point.  */
    long        indx, length;
    MYFLT       ndx;            /*  for calculating index of read.  */
    MYFLT       *ptab;          /* Where we will write */
/*     printf("tablew: p->xsig, p->xndx = %f, %f\n", *p->xsig, *p->xndx); */
    /*-----------------------------------*/
    /* Assume that TABLEW has been set up correctly.  */

    ftp    = p->ftp;
    ndx    = *p->xndx;
    length = ftp->flen;
    /* Multiply ndx by denormalisation factor.  and add in the
     * offset - already denormalised - by tblchkw().
     * xbmul = 1 or table length depending on state of ixmode.  */

    ndx = (ndx * p->xbmul) + p->offset;

    /* ndx now includes the offset and is ready to address the table.
     * However we have three modes to consider:
     * igwm = 0     Limit mode.
     *        1     Wrap mode.
     *        2     Guardpoint mode.
     */
    if (p->iwgm == 0) {
      /* Limit mode - when igmode = 0.
       *
       * Limit the final index to 0 and the last location in the table.
       */
      indx = (long) floor(ndx); /* Limit to (table length - 1) */
      if (indx > length - 1)
        indx = length - 1;      /* Limit the high values. */
      else if (indx < 0L) indx = 0L; /* limit negative values to zero. */
    }
    /* Wrap and guard point mode.
     * In guard point mode only, add 0.5 to the index. */
    else {
      if (p->iwgm == 2) ndx += FL(0.5);
      indx = (long) floor(ndx);

      /* Both wrap and guard point mode.
       * The following code uses an AND with an integer like 0000 0111 to wrap
       * the current index within the range of the table. */
      indx &= ftp->lenmask;
    }
                                /* Calculate the address of where we
                                 * want to write to, from indx and the
                                 * starting address of the table.
                                 */
    ptab = ftp->ftable + indx;
    *ptab = *p->xsig;           /* Write the input value to the table. */
                                /* If this is guard point mode and we
                                 * have just written to location 0,
                                 * then also write to the guard point.
                                 */
    if ((p->iwgm == 2) && indx == 0) { /* Fix -- JPff 2000/1/5 */
      ptab += ftp->flen;
      *ptab = *p->xsig;
    }
    return OK;
}

/*---------------------------------------------------------------------------*/

/* tablew() is similar to ktablew()  above, except that it processes
 * two arrays of input values and indexes.  These arrays are ksmps long. */
int tablew(ENVIRON *csound, TABLEW *p)
{
    FUNC        *ftp;   /* Pointer to function table data structure. */
    MYFLT       *psig;  /* Array of input values to be written to table. */
    MYFLT       *pxndx; /* Array of input index values */
    MYFLT       *ptab;  /* Pointer to start of table we will write. */
    MYFLT       *pwrite;/* Pointer to location in table where we will write */
    long        indx;   /* Used to read table. */
    long        mask;   /* ANDed with indx to make it wrap within table.*/
    long        length; /* Length of table - always a power of two,
                         * even if the table has a guard point. */
    /* For instance length = 8, mask = 0000 0111, normal locations in table
     * are 0 to 7.  Location 8 is the guard point.  table() does not read
     * the guard point - tabli() does.*/
    long        liwgm;          /* Local copy of iwgm for speed */
    int         nsmps = ksmps;
    MYFLT       ndx, xbmul, offset;
                                /*-----------------------------------*/
/*     printf("atablew: p->xsig, p->xndx = %f, %f\n", *p->xsig, *p->xndx); */
    /* Assume that TABLEW has been set up correctly. */

    ftp    = p->ftp;
    psig   = p->xsig;
    pxndx  = p->xndx;
    ptab   = ftp->ftable;
    mask   = ftp->lenmask;
    length = ftp->flen;
    liwgm  = p->iwgm;
    xbmul  = (MYFLT)p->xbmul;
    offset = p->offset;
                /* Main loop - for the number of a samples in a k cycle. */
    do {
      /* Read in the next raw index and increment the pointer ready for the
         next cycle.  Then multiply the ndx by the denormalising factor and
         add in the offset.  */
      ndx = (*pxndx++ * xbmul) + offset;
      if (liwgm == 0) {         /* Limit mode - when igmode = 0. */
        indx = (long) floor(ndx);
        if (indx > length - 1) indx = length - 1;
        else if (indx < 0L) indx = 0L;
      }
      else {
        if (liwgm == 2) ndx += FL(0.5);
        indx = (long) floor(ndx);
        /* Both wrap and guard point mode.
         *
         * AND with an integer like 0000 0111 to wrap the index within the
         * range of the table.   */
        indx &= mask;
      }
      pwrite = ptab + indx;
      *pwrite = *psig++;
                                        /* If this is guard point mode and we
                                         * have just written to location 0,
                                         * then also write to the guard point.
                                         */
      if ((liwgm == 2) && indx == 0) {  /* Fix -- JPff 2000/1/5 */
                                        /* Note that since pwrite is a pointer
                                         * to a float, adding length to it
                                         * adds (4 * length) to its value since
                                         * the length of a float is 4 bytes.
                                         */
        pwrite += length;
                                        /* Decrement psig to make it point
                                         * to the same input value.
                                         * Write to guard point.
                                         */
        psig--;
        *pwrite = *psig++;
      }
    }
    while(--nsmps);
    return OK;
}

/*************************************/

/* ktablewkt() and tablewkt()
 *
 * These call ktablew() and tablew() above - after they have set up
 * TABLEW after the k rate table number changes.
 *
 * Prior to these running, we can assume that tblsetwkt() has been run
 * at init time.
 *
 * tblsetwkt() does very little - just setting up the iwgmode variable
 * in TABLEW. All the other variables depend on the table number. This
 * is not available at init time, so the following two functions must
 * look for the changed table number and set up the variables
 * accordingly - generating error messages in a way which works at
 * performance time.
 *
 * k rate    a rate
 *
 * ktablewkt tablewkt
 *
 * Since these perform identical operations, apart from the function
 * they call, create a common function to do this work:
 *
 * ftkrchkw() */

int ftkrchkw(ENVIRON *csound, TABLEW *p)
{
/* Check the table number is >= 1.  Print error and deactivate if it
 * is.  Return 0 to tell calling function not to proceed with a or k
 * rate operations.
 *
 * This was not necessary in the versions of ktablew() and tablew()
 * because ftfind() would catch a table number < 1.
 *
 * However, in this case, we only call ftfindp() if the table number
 * changes from p->pfn, so we want to generate an error message if the
 * ugen is ever called with a table number of 0.  While we are about
 * it, catch negative values too.  */
    if (*p->xfn < 1) {
      sprintf(errmsg,
              Str("Table write k rate function table no. %f < 1"), *p->xfn);
      return perferror(errmsg);
    }
    /* Check to see if table number has changed from previous value.
     *
     * On the first run through, the previous value will be 0.  */

    if (p->pfn != (long)*p->xfn) {
      /* If it is different, check to see if the table exists.  If it
       * doesn't, an error message should be produced by ftfindp()
       * which should also deactivate the instrument.  Return 0 to
       * tell calling function not to proceed with a or k rate
       * operations.
       *
       * ftfind is in a new version of fgens.c. A prototype for it
       * should be added to prototype.h.  */

      if ((p->ftp = ftfindp(csound, p->xfn)) == NULL) {
        return NOTOK;
      }

      /* p->ftp now points to the FUNC data structure of the newly
       * selected table.
       *
       * Now we set up some variables in TABLEW ready for the k or a
       * rate functions which follow.  */

      /* Write the integer version of the table number into pfn so we
       * can later decide whether subsequent calls to the k and a rate
       * functions occur with a table number value which points to a
       * different table.
       *
       * p->pfn is an integer.  */
      p->pfn = (long)*p->xfn;

      /* Set denormalisation factor to 1 or table length, depending on
       * the state of ixmode.  1L means a 32 bit 1.  */
      if (*p->ixmode) p->xbmul = p->ftp->flen;
      else            p->xbmul = 1L;

      /* Multiply the ixoff value by the xbmul denormalisation factor
       * and then check it is between 0 and the table length.  */

      if ((p->offset = p->xbmul * *p->ixoff) < FL(0.0) ||
          p->offset > p->ftp->flen) {
        sprintf(errmsg, Str("Table write offset %f < 0 or > tablelength"),
                p->offset);
        return perferror(errmsg);
      }
    }
    /* If all is well, return 1 to tell calling function to proceed
     * with a or k rate operations.  */
    return OK;
}

/* Now for the two functions, which are called as a result of being
 * listed in opcodlst in entry.c */

int    ktablewkt(ENVIRON *csound, TABLEW *p)
{
    if (ftkrchkw(csound,p)==OK) return ktablew(csound,p);
    return NOTOK;
}

int    tablewkt(ENVIRON *csound, TABLEW *p)
{
    if (ftkrchkw(csound,p)==OK) return tablew(csound,p);
    return NOTOK;
}


/*****************************************************************************/

/* Reading the table length                      */

/* tableng()
 *
 * At k rate - performance time.  See similar function to do it at
 * init time - itableng().
 *
 * The means of finding the table, and of reporting an error differ
 * between these i time and perf time.  */
int    tableng(ENVIRON *csound, TABLENG *p)
{
    /*  Pointer to data structure for accessing table. */
    FUNC        *ftp;
    /* Check to see we can find the table and find its location in
     * memory.  Returns zero if not found.  Report and error, which
     * will cause this instrument to be de-activated.  */

    if ((ftp = ftfindp(csound, p->xfn)) == NULL) {
        *p->kout = FL(0.0);
        sprintf(errmsg, Str("Table %f not found\n"), *(p->xfn));
        return perferror(errmsg);
      }
    /* Return length as a float if we do find the table. */
    else        *p->kout = (MYFLT) ftp->flen;
    return OK;
}
/*-----------------------------------*/

/* itableng()
 *
 * At init time.
 */
int    itableng(ENVIRON *csound, TABLENG *p)
{
    /*
     * Pointer to data structure for accessing table.  */
    FUNC        *ftp;
    /* Check to see we can find the table and find its location in
     * memory.  Returns zero if not found.  Report and error, which
     * will cause this instrument initialisation to fail.  */

    if ((ftp = ftfind(csound, p->xfn)) == NULL) {
      *p->kout = FL(0.0);
/*       sprintf(errmsg, Str("Table %f not found\n"), *(p->xfn)); */
/*       return initerror(errmsg); */
    }
    /* Return length as a float if we do find the table. */
    else
      *p->kout = (MYFLT) ftp->flen;
    return OK;
}

/*---------------------------------------------------------------------------*/

/* Writing the guardpoint */

/* tablegpw()
 *
 * At k rate - performance time.  See similar function to do it at
 * init time - itablegpw().
 *
 * The means of finding the table, and of reporting an error differ
 * between these i time and perf time.
 *
 * Read the value in location 0 and write it to the guard point.  */
int    tablegpw(ENVIRON *csound, TABLEGPW *p)
{
    /* Local variables
     *
     * Pointer to data structure for
     * accessing table.
     */
    FUNC        *ftp;

    MYFLT  *ptab;    /* Pointer to start of table. */

    MYFLT       val0;    /* Value read from location 0 in table. */

    long        length;         /* temporary storage for length -
                                 * in floats, not in bytes.
                                 */

    /* Check to see we can find the table
     * and find its location in memory.
     */

    if ((ftp = ftfindp(csound, p->xfn)) == NULL) {
/*       sprintf(errmsg, Str("Table %f not found\n"), *(p->xfn)); */
/*       return perferror(errmsg); */
    }
    /* Find length of table.
     */
    else {
      length = (long) ftp->flen;
      ptab   = ftp->ftable;
      /* Now write from location 0 to
       * the guard point which is at
       * location length.
       */
      val0 = *ptab;
      ptab = ptab + length;
      *ptab = val0;
    }
    return OK;
}
/*-----------------------------------*/

                                        /* itablegpw()
                                         *
                                         * At init time.
                                         */
int itablegpw(ENVIRON *csound, TABLEGPW *p)
{
    /* Local variables
     *
     * Pointer to data structure for
     * accessing table.
                                         */
    FUNC        *ftp;    /* Pointer to start of table.            */
    MYFLT  *ptab;        /* Value read from location 0 in table.  */
    MYFLT       val0;    /* Temporary storage for length in floats, not in bytes.*/
    long        length;

    /* Check to see we can find the table and find its location in memory. */
    if ((ftp = ftfind(csound, p->xfn)) == NULL) {
      sprintf(errmsg, Str("Table %f not found\n"), *(p->xfn));
      return initerror(errmsg);
    }
    /* Find length of table. */
    else {
      length = (long) ftp->flen;
      ptab   = ftp->ftable;
      /* Now write from location 0 to
       * the guard point which is at
       * location length.
       */
      val0 = *ptab;
      ptab = ptab + length;
      *ptab = val0;
    }
    return OK;
}


/*---------------------------------------------------------------------------*/

        /* tablemix functions */

/* tablemixset()
 *
 * Called at i time prior to the k rate function tablemix(). */

int tablemixset(ENVIRON *csound, TABLEMIX *p)
{
    /* There may be no input values now - they are typically k rate and so
     * are not present at i time.
     *
     * Set to zero the three variables with which we check to see if
     * the k rate table numbers have changed.  These are in the TABLEMIX
     * structure which is specific to this instance of the ugen.  However
     * its values have not been initialised, so they could be anything.  */
    p->pdft = 0;
    p->ps1ft = 0;
    p->ps2ft = 0;
    return OK;
}


/* tablemix()
 *
 * k rate version - see itablemix() for the init time version.
 *
 * These are similar but require two different approaches to
 * finding tables and reporting errors.
 *
 * This adventurous ugen uses nine parameters which are all assumed
 * to be k rate variables - which could change at any time.
 *
 * Six of these will used directly.
 *
 * However, three of them will be checked to see if they have changed
 * from last time - the three variables which point to the
 * destination and source tables.
 *
 * If they change, then the new values will be used to search for a new
 * table. Otherwise, existing pointers will be used to access the data
 * structures for the tables.
 *
 * Both the i and k rate operations have a lot in common, so use a
 * common function domix().
 *
 * Declare it here.
 */
static void domix(ENVIRON *csound, TABLEMIX *p);

int tablemix(ENVIRON *csound, TABLEMIX *p)
{
    /* Check the state of the three table number variables.
     *
     * Error message if any are < 1 and no further action.
     * We cannot leave it for ftfindp() to find 0 values, since it is only
     * called if the input value is different from the "previous" value
     * which is initially 0.
     */
    if ((*p->dft  < 1) || (*p->s1ft < 1) || (*p->s2ft < 1)) {
      sprintf(errmsg, Str("Table no. < 1 dft=%.2f  s1ft=%.2f  s2ft=%.2f\n"),
              *p->dft, *p->s1ft, *p->s2ft);
      return perferror(errmsg);
    }
    /* Check each table number in turn.  */

    /* Destination  */
    if (p->pdft != (int)*p->dft) {
      /* Get pointer to the function table data structure.
       *
       * ftfindp() for perf time.
       * ftfind() for init time.
       */

      if ((p->funcd = ftfindp(csound, p->dft)) == NULL) {
        sprintf(errmsg, Str("Destination dft table %.2f not found.\n"),
                *p->dft);
        return perferror(errmsg);
      }
      /* Table number is valid.
       *
       * Save the integer version of the table number for future reference.*/
      p->pdft = (int)*p->dft;
    }

    /* Source 1 */
    if (p->ps1ft != (int)*p->s1ft) {
      if ((p->funcs1 = ftfindp(csound, p->s1ft)) == NULL) {
        sprintf(errmsg, Str("Source 1 s1ft table %.2f not found.\n"),
                *p->s1ft);
        return perferror(errmsg);
      }
      p->ps1ft = (int)*p->s1ft;
    }

    /* Source 2 */
    if (p->ps2ft != (int)*p->s2ft) {
      if ((p->funcs2 = ftfindp(csound, p->s2ft)) == NULL) {
        sprintf(errmsg, Str("Source 2 s2ft table %.2f not found.\n"),
                *p->s2ft);
        return perferror(errmsg);
      }
      p->ps2ft = (int)*p->s2ft;
    }
    /* OK all tables present and the funcx pointers are pointing to
     * their data structures.
     *
     * The other parameters do not need checking - and the remaining
     * proceedures are common to the init time version, so call a
     * function to do the rest.  */
    domix(csound,p);
    return OK;
}

/*-----------------------------------*/

/* itablemix()
 *
 * identical to above, but we know it runs at init time, so we do not
 * check for changes, we look for the table with ftfind() instead of
 * ftfindp() and we use initerror() instead of perferror().      */
int itablemix(ENVIRON *csound, TABLEMIX *p)
{
    /* Check the state of the three table number variables.
     *
     * Error message if any are < 1 and no further action.
     *
     * Technically we do not need to check for values of 0 or negative
     * since they will all be fed to ftfind().
     * Do so anyway to be consistent with tablemix().
     *
     * This runs only once, so speed is not an issue.    */
    if ((*p->dft < 1) || (*p->s1ft < 1) || (*p->s2ft < 1)) {
      sprintf(errmsg,
              Str("Table number < 1 dft=%.2f  s1ft=%.2f  s2ft=%.2f\n"),
              *p->dft, *p->s1ft, *p->s2ft);
      return initerror(errmsg);
    }
    /* Check each table number in turn.   */

    /* Destination */

    /* Get pointer to the function table data structure.
     *
     * ftfind() for init time.
     */

    if ((p->funcd = ftfind(csound, p->dft)) == NULL) {
      sprintf(errmsg, Str("Destination dft table %.2f not found.\n"),
              *p->dft);
      return initerror(errmsg);
    }
    /* Table number is valid.
     *
     * Save the integer version of the table number for future reference.  */
    p->pdft = (int)*p->dft;

    /* Source 1 */
    if ((p->funcs1 = ftfind(csound, p->s1ft)) == NULL) {
      sprintf(errmsg, Str("Source 1 s1ft table %.2f not found.\n"),
              *p->s1ft);
      return initerror(errmsg);
    }
    p->ps1ft = (int)*p->s1ft;

    /* Source 2 */
    if ((p->funcs2 = ftfind(csound, p->s2ft)) == NULL) {
      sprintf(errmsg, Str("Source 2 s2ft table %.2f not found.\n"),
              *p->s2ft);
      return initerror(errmsg);
    }
    p->ps2ft = (int)*p->s2ft;
    domix(csound,p);
    return OK;
}

/*-----------------------------------*/

/* domix()
 *
 * This is the business end of tablemix and itablemix.
 *
 * We could be called at either init or perf time. So we do not
 * make any error messages here.
 *
 * The three tables have been found and are known to be of greater
 * than zero length - ftfind() and ftfindp() check this.
 *
 * We will use the remaining input parameters no matter what their
 * values are.
 *
 * Length is converted from a float to a long, with floor()
 * so that -0.3 converts to -1.
 *
 * The resulting integer could be negative - this tells us to
 * work backwards.
 *
 * The offsets could be anything whatsoever - these will be added
 * to index to produce integers which are rounded by the lenmask
 * of each table. So we don't mind if the offsets are all over the place.
 *
 * Likewise the gain parameters for source tables 1 and 2, except
 * that if the gain of source 2 is 0, then we do not bother reading
 * it.  This is to save time when all that the user wants is a copy.
 */
static void domix(ENVIRON *csound, TABLEMIX *p)
{
    MYFLT       gains1, gains2; /* Gains for source tables 1 and 2. */
    long length;                /* from len input parameter */
    long loopcount;

    long offd, offs1, offs2;    /* Offsets for the three tables. */

    /* Index to be added to offsets as we step through the tables.
     * If length was positive, this will increase by one each cycle.
     * If length was negative, it will step backards from 0.
     */
    long indx = 0;
    MYFLT *based, *bases1, *bases2;    /* Base addresses of the three tables. */
    long maskd, masks1, masks2;
    MYFLT *pdest, *ps1, *ps2;

    gains1 = *p->s1g;
    gains2 = *p->s2g;

    /* Get the length and generate the loop count.
     * Return with no action if it is 0.    */

    if ((length = (long)floor(*p->len)) == 0L) return;

    if (length < 0L) loopcount = 0L - length;
    else             loopcount = length;

    /* Get the offsets for the three tables; Use floor to reduce negative floats
     * to the next most negative integer. This ensures that a sweeping
     * offset will wrap correctly into the table's address space.    */

    offd  = (long)floor(*p->doff);
    offs1 = (long)floor(*p->s1off);
    offs2 = (long)floor(*p->s2off);

    /* Now get the base addresses and length masks of the three tables.  */
    based  = p->funcd->ftable;
    maskd  = p->funcd->lenmask;

    bases1 = p->funcs1->ftable;
    masks1 = p->funcs1->lenmask;

    bases2 = p->funcs2->ftable;
    masks2 = p->funcs2->lenmask;

    /* Decide which of four loops to do based on:
     * Forwards or backwards?  Source 2 gain is zero or not?  */
    if (length > 0) {
      if (gains2 != 0) {    /* Forwards, full mix.  */
        do {
          /* Create pointers by adding index to offset, ANDing
           * with mask, and adding to base address.
           *
           * Mask, offset and index are all in units of 1 - not
           * the units of 4 bytes (typically) needed to step
           * through memory to find floats.
           *
           * So we make base a pointer to a float and the
           * compiler is smart enough to know that when we add
           * an integer to a float pointer, we want that
           * pointers address to change by sizeof(float) * the
           * value of the integer.  */

          pdest = based  + (maskd  & (offd  + indx));
          ps1   = bases1 + (masks1 & (offs1 + indx));
          ps2   = bases2 + (masks2 & (offs2 + indx));

          /* Mix from source1 and source 2. */
          *pdest = (*ps1 * gains1) + (*ps2 * gains2);
          indx++;
        } while (--loopcount);
      }
      else {
        /* Forwards, only read source 1  */
        do {
          pdest = based  + (maskd  & (offd  + indx));
          ps1   = bases1 + (masks1 & (offs1 + indx));
          /* Write fomr source1 to destination. */
          *pdest = (*ps1 * gains1);
          indx++;
        } while (--loopcount);
      }
    }
    else {      /* Negative length, so do things backwards. */
      if (gains2 != 0) {  /* Backwards, full mix. */
        do {
          pdest = based  + (maskd  & (offd  + indx));
          ps1   = bases1 + (masks1 & (offs1 + indx));
          ps2   = bases2 + (masks2 & (offs2 + indx));
          /* Mix from source1 and source 2. */
          *pdest = (*ps1 * gains1) + (*ps2 * gains2);
          indx--;
        } while (--loopcount);
      }
      else { /* Backwards, only read source 1 */
        do {
          pdest = based  + (maskd  & (offd  + indx));
          ps1   = bases1 + (masks1 & (offs1 + indx));
          /* Write from source1 to destination. */
          *pdest = (*ps1 * gains1);
          indx--;
        } while (--loopcount);
      }
    }
    return;
}

/*---------------------------------------------------------------------------*/

/* tablecopy functions
 */

/* tablecopyset()
 *
 * Called at i time prior to the k rate function tablemix().
 * Similar function to tablemixset().
 */

int tablecopyset(ENVIRON *csound, TABLECOPY *p)
{
    p->pdft = 0;
    p->psft = 0;
    return OK;
}

/*-----------------------------------*/
/* tablecopy()
 *
 * k rate version - see itablecopy() for the init time version.
 *
 * These two functions, and the docopy() function they share are
 * simpler, faster, cut-down versions of their equivalent in the
 * tablemix section above.  Read the comments there for a full
 * explanation.
 */
static int docopy(ENVIRON *csound, TABLECOPY *p);

int tablecopy(ENVIRON *csound, TABLECOPY *p)
{
    /* Check the state of the two table number variables.
     * Error message if any are < 1 and no further action.     */
    if ((*p->dft < 1) || (*p->sft < 1)) {
      sprintf(errmsg, Str("Table no. < 1 dft=%.2f  sft=%.2f\n"),
              *p->dft, *p->sft);
      return perferror(errmsg);
    }
    /* Check each table number in turn.   */

    /* Destination  */
    if (p->pdft != (int)*p->dft) {
      /* Get pointer to the function table data structure.
       * ftfindp() for perf time. ftfind() for init time.
       */
      if ((p->funcd = ftfindp(csound, p->dft)) == NULL) {
        sprintf(errmsg, Str("Destination dft table %.2f not found.\n"),
                *p->dft);
        return perferror(errmsg);
      }
      /* Table number is valid.
       * Save the integer version of the table number for future reference.*/
      p->pdft = (int)*p->dft;
    }
    /* Source  */
    if (p->psft != (int)*p->sft) {
      if ((p->funcs = ftfindp(csound, p->sft)) == NULL) {
        sprintf(errmsg, Str("Source sft table %.2f not found.\n"),
                *p->sft);
        return perferror(errmsg);
      }
      p->psft = (int)*p->sft;
    }
    /* OK both tables present and the funcx pointers are pointing to
     * their data structures.    */
    docopy(csound,p);
    return OK;
}

/*-----------------------------------*/

int itablecopy(ENVIRON *csound, TABLECOPY *p)
{
    /* Check the state of the two table number variables.
     * Error message if any are < 1 and no further action. */
    if ((*p->dft < 1) || (*p->sft < 1)) {
      sprintf(errmsg, Str("Table no. < 1 dft=%.2f  sft=%.2f\n"),
              *p->dft, *p->sft);
      return initerror(errmsg);
    }
    /* Check each table number in turn.  */

    /* Destination */
    if (p->pdft != (int)*p->dft) {
      /* Get pointer to the function table data structure.
       * ftfindp() for perf time. ftfind() for init time. */
      if ((p->funcd = ftfind(csound, p->dft)) == NULL) {
        sprintf(errmsg, Str("Destination dft table %.2f not found.\n"),
                *p->dft);
        return initerror(errmsg);
      }
      /* Table number is valid.
       * Save the integer version of the table number for future reference. */
      p->pdft = (int)*p->dft;
    }
    /* Source  */
    if (p->psft != (int)*p->sft) {
      if ((p->funcs = ftfind(csound, p->sft)) == NULL) {
        sprintf(errmsg, Str("Source sft table %.2f not found.\n"),
                *p->sft);
        return initerror(errmsg);
      }
      p->psft = (int)*p->sft;
    }
    /* OK both tables present and the funcx pointers are pointing to
     * their data structures.    */
    docopy(csound,p);
    return OK;
}

/*-----------------------------------*/

/* docopy()
 *
 * This is the business end oF tablecopy and itablecopy.
 *
 * See domix for full explanation. make any error messages here.
 */
static int docopy(ENVIRON *csound, TABLECOPY *p)
{
    long loopcount;/* Loop counter. Set by the length of the destination table.*/
    long indx = 0;              /* Index to be added to offsets */
    MYFLT *based, *bases;       /* Base addresses of the two tables.*/
    long masks;                 /* Binary masks for the source table */
    MYFLT *pdest, *ps;

    loopcount = p->funcd->flen;

    /* Now get the base addresses and length masks of the tables. */
    based  = p->funcd->ftable;
    bases = p->funcs->ftable;
    masks = p->funcs->lenmask;

    do {
      /* Create source pointers by ANDing index with mask, and adding to base
       * address. This causes source  addresses to wrap around if the
       * destination table is longer.
       * Destination address is simply the index plus the base address since
       * we know we will be writing within the table.          */

      pdest = based  + indx;
      ps    = bases  + (masks & indx);
      *pdest = *ps;
      indx++;
    } while (--loopcount);
    return OK;
}

/*---------------------------------------------------------------------------*/

        /* tablera functions */

/* tableraset()
 *
 * Called at i time prior to the k rate function tablemix().
 * Similar function to tablemixset().  Set the "previous table number"
 * to 0, so that at the first k cycle, a positive table number
 * will be recognised as a new value.
 */
int tableraset(ENVIRON *csound, TABLERA *p)
{
    p->pfn = 0;
    return OK;
}

/*-----------------------------------*/

/* tablera()
 *
 * Read ksmps values from a table, starting at location kstart.
 * Has an offset and wrap-around index calculation.
 *
 * Write them to an a rate destination.
 *
 * Table number is k rate, so check for it changing and for it being 0.
 */
int tablera(ENVIRON *csound, TABLERA *p)
{
    MYFLT       *writeloc, *readloc;
    long        kstart;
    /* Local variable to hold integer version of offset, and the length
     * mask for ANDing the total index - wrapping it into the table length. */
    long        kioff, mask;
    int         loopcount;

    /* Check the state of the table number variable.
     * Error message if it is < 1 and no further action. */
    if (*p->kfn < 1) {
      sprintf(errmsg, Str("Table kfn=%.2f < 1\n"), *p->kfn);
      return perferror(errmsg);
    }

    if (p->pfn != (int)*p->kfn) { /* Check if the table number has changed. */
      /* Get pointer to the function table data structure.
       * ftfindp() for perf time.                              */

      if ((p->ftp = ftfindp(csound, p->kfn)) == NULL) {
        sprintf(errmsg, Str("kfn table %.2f not found\n"), *p->kfn);
        return perferror(errmsg);
      }
      /* Table number is valid.
       * Save the integer version of the table number for future reference. */
      p->pfn = (int)*p->kfn;

      /* Check that the table length is equal to or greater than ksmps.
       * Create error message if this is not so.  We must ensure that
       * the ksmps number of reads can fit within the length of the table. */

      if (p->ftp->flen < ksmps) {
        sprintf(errmsg,
                Str("Table kfn=%.2f length %ld shorter than ksmps %d\n"),
                *p->kfn, p->ftp->flen, ksmps);
        return perferror(errmsg);
      }
    }
    /* Check that kstart is within the range of the table. */

    if (((kstart = (long)*p->kstart) < 0L) || (kstart >= p->ftp->flen)) {
      sprintf(errmsg,
              Str("kstart %.2f is outside table %.2f range 0 to %ld\n"),
              *p->kstart, *p->kfn, p->ftp->flen - 1);
      return perferror(errmsg);
    }
    /* Set up the offset integer rounding float input argument to the next
     * more negative integer. Also read the mask from the FUNC data structure.
     */
    kioff = (long)floor(*p->koff);
    mask = p->ftp->lenmask;

    /* We are almost ready to go, but first check to see whether
     * ksmps loops from the starting point of kstart, will take us
     * beyond the length of the table.
     *
     * Therefore calculate the number of loop cycles to perform.
     * Eg. if kstart = 14, ksmps = 8 and table length = 16, then we only
     * want to read 2 locations.
     *
     * koff is not considered here. It changes the read location - wrapped
     * around the length of the table. It does not change the number of
     * cycles of read/write.
     */

    if ((loopcount = p->ftp->flen - kstart) > ksmps) {
      /* If we are not going to exceed the length of the table,
       * then loopcount = ksmps. */
      loopcount = ksmps;
    }
    /* Otherwise it is the number of locations between kstart and
     * the end of the table - as calculated above.     */

    /* Main loop:
     *
     * Write sequentially into the a rate variable.
     *
     * Read from masked totalindex in the table, where the total index
     * is (kstart++ + kioff).     */

    /* Initialise write location to start of a rate destination array.  */
    writeloc = p->adest;
    /* Transfer the data, whilst updating pointers and masking to get
     * final read address.     */
    do {
      readloc = p->ftp->ftable + ((kstart++ + kioff) & mask);
      *writeloc++ = *readloc;
    } while (--loopcount);
    return OK;
}

/*---------------------------------------------------------------------------*/

/* tablewa functions
 */

/* tablewaset()
 *
 * Called at i time prior to the k rate function tablemix().
 * Same function to tablerset().
 */
int tablewaset(ENVIRON *csound, TABLEWA *p)
{
    p->pfn = 0;
    return OK;
}

/*-----------------------------------*/

/* tablewa()
 *
 * Read ksmps values from an a rate variable and write them into a
 * table, starting at location kstart.
 *
 * Similar to tablera() above, except that writing is from a rate to table
 * and we rewrite kstart.
 */

int tablewa(ENVIRON *csound, TABLEWA *p)
{
    MYFLT *writeloc, *readloc;
    long        kstart;
    long        kioff;          /* integer version of offset */
    long        mask;           /* length mask for ANDing the total index */
    long        loopcount;

    /* From here the main loop, (except where noted "!!") this code is
     * the same as tablera above.  It is not in a common subroutine
     * for speed reasons and because there are several local variables for
     * tablewa() that need to be written.     */

    /* Check the state of the table number variable.
     * Error message if it is < 1 and no further action.     */
    if (*p->kfn < 1) {
        sprintf(errmsg, Str("Table kfn=%.2f < 1\n"), *p->kfn);
        return perferror(errmsg);
    }

    if (p->pfn != (int)*p->kfn) { /* Check if the table number has changed. */
        /* Get pointer to the function table data structure.
         * ftfindp() for perf time. */

        if ((p->ftp = ftfindp(csound, p->kfn)) == NULL) {
            sprintf(errmsg, Str("kfn table %.2f not found\n"), *p->kfn);
            return perferror(errmsg);
        }
        /* Table number is valid.
         * Save the integer version of the table number for future reference.
         */
        p->pfn = (int)*p->kfn;

        /* Check that the table length is equal to or greater than ksmps.
         * Create error message if this is not so.  We must ensure that
         * the ksmps number of reads can fit within the length of the table. */

        if (p->ftp->flen < ksmps) {
          sprintf(errmsg,
                  Str("Table kfn=%.2f length %ld shorter than ksmps %d\n"),
                  *p->kfn, p->ftp->flen, ksmps);
          return perferror(errmsg);
        }
    }

    /* Check that kstart is within the range of the table. */
    if (((kstart = (long)*p->kstart) < 0L) || (kstart >= p->ftp->flen)) {
        sprintf(errmsg,
                Str("kstart %.2f is outside table %.2f range 0 to %ld\n"),
                *p->kstart, *p->kfn, p->ftp->flen - 1);
        return perferror(errmsg);
      }
    /* Set up the offset integer rounding float input argument to the next
     * more negative integer.  Also read the mask from the FUNC data structure.
     */
    kioff = (long)floor(*p->koff);
    mask = p->ftp->lenmask;
    /* !! end of code identical to tablera.  */

    /* We are almost ready to go, but first check to see whether
     * ksmps loops from the starting point of kstart, will take us
     * beyond the length of the table.
     *
     * Therefore calculate the number of loop cycles to perform.
     * Eg. if kstart = 14, ksmps = 8 and table length = 16, then we only
     * want to read 2 locations.
     *
     * koff is not considered here. It changes the read location - wrapped
     * around the length of the table.  It does not change the number of
     * cycles of read/write.
     */

    if ((p->ftp->flen - kstart) > ksmps) {
        /* If we are not going to exceed the length of the table, then
         * loopcount = ksmps.    */
        loopcount = ksmps;

        /* Write the kstart i/o variable to be ksmps higher than before. */
        *p->kstart += ksmps;
    }
    else {
      loopcount = p->ftp->flen - kstart;

        /* Otherwise loopcount is the number of locations between kstart and
         * the end of the table - as calculated above.
         *
         * We have hit the end of the process of stepping kstart up by ksmps.
         * Set it to 0 so the next time this ugen is run, with the same variable
         * the cycle will start from the start of the table.
         */
      *p->kstart = FL(0.0);
    }

    /* Main loop:
     *
     * Read sequentially from the a rate variable.
     *
     * Write to masked total index in the table, where the total index
     * is (kstart++ + kioff).
     */

    /* Initialise read location to start of a rate source array.
     */
    readloc = p->asig;
    /* Transfer the data, whilst updating pointers and masking to get
     * final write address.
     */
    do {
      writeloc = p->ftp->ftable + ((kstart++ + kioff) & mask);
      *writeloc = *readloc++;
    } while (--loopcount);
    return OK;
}



/*****************************************************************************/
/*****************************************************************************/

/* The zak system - patching i, k and a rate signals in a global set of
 * patch points - one set for i and k,* the other for a rate.
 * See doco at the start of this file.
 */

/* There are four global variables which are used by these ugens. */

/* Starting addresses of zk and za spaces */
/* MYFLT   *zkstart = NULL, *zastart = NULL;  */
/* Number of the last location in zk/za space */
/* long    zklast = 0, zalast = 0; */
/* There are currently no limits on the size of these spaces.  */

/* zakinit is an opcode which must be called once to reserve the memory
 * for zk and za spaces.
 */
int zakinit(ENVIRON *csound, ZAKINIT *p)
{
    long        length;

    /* Check to see this is the first time zakinit() has been called.
     * Global variables will be zero if it has not been called.     */

    if ((zkstart != NULL) || (zastart != NULL)) {
      return initerror(Str("zakinit should only be called once."));
    }

    if ((*p->isizea <= 0) || (*p->isizek <= 0)) {
      return initerror(Str("zakinit: both isizea and isizek should be > 0."));
    }
    /* Allocate memory for zk space.
     * This is all set to 0 and there will be an error report if the
     * memory cannot be allocated. */

    length = (long)((*p->isizek + 1) * sizeof(MYFLT));
    zkstart = (MYFLT*) mcalloc(length);
    zklast = (long) *p->isizek;
    /* Likewise, allocate memory for za space, but do it in arrays of
     * length ksmps.
     * This is all set to 0 and there will be an error report if the
     * memory cannot be allocated.       */
    length = (long)((*p->isizea + 1) * sizeof(MYFLT) * ksmps);
    zastart = (MYFLT*) mcalloc(length);
    zalast = (long) *p->isizea;
    return OK;
}

/*---------------------------------------------------------------------------*/

/* I and K rate zak code. */

/* zkset() is called at the init time of the instance of the zir, zkr
 * zir and ziw ugens.  It complains if zk space has not been allocated yet.
 */
int zkset(ENVIRON *csound, ZKR *p)
{
    if  (zkstart == NULL) {
      return initerror(Str("No zk space: zakinit has not been called yet."));
    }
    else
      return OK;
}

/*-----------------------------------*/

/* k rate READ code. */

/* zkr reads from zk space at k rate. */
int zkr(ENVIRON *csound, ZKR *p)
{
    MYFLT       *readloc;
    long indx;

    /* Check to see this index is within the limits of zk space. */
    indx = (long) *p->ndx;
    if (indx > zklast) {
      *p->rslt = FL(0.0);
      if (O.msglevel & WARNMSG)
        printf(Str("WARNING: zkr index > isizek. Returning 0.\n"));
    }
    else if (indx < 0) {
      *p->rslt = FL(0.0);
      if (O.msglevel & WARNMSG)
        printf(Str("WARNING: zkr index < 0. Returning 0.\n"));
    }
    else {
        /* Now read from the zk space and write to the destination. */
        readloc = zkstart + indx;
        *p->rslt = *readloc;
    }
    return OK;
}

/*-----------------------------------*/

/* zir reads from zk space, but only  at init time.
 *
 * Call zkset() to check that zk space has been allocated, then do
 * similar code to zkr() above, except with initerror() instead of
 * perferror(). */
int zir(ENVIRON *csound, ZKR *p)
{
    /* See zkr() for more comments.  */
    MYFLT       *readloc;
    long indx;

    if (zkset(csound,p) == 0) return OK;
    /* Check to see this index is within the limits of zk space. */

    indx = (long) *p->ndx;
    if (indx > zklast) {
      if (O.msglevel & WARNMSG)
        printf(Str("WARNING: zir index > isizek. Returning 0.\n"));
      *p->rslt = FL(0.0);
    }
    else if (indx < 0) {
      if (O.msglevel & WARNMSG)
        printf(Str("WARNING: zir index < 0. Returning 0.\n"));
      *p->rslt = FL(0.0);
    }
    else {
        /* Now read from the zk space. */
        readloc = zkstart + indx;
        *p->rslt = *readloc;
    }
    return OK;
}

/*-----------------------------------*/

/* Now the i and k rate WRITE code.  zkw writes to zk space at k rate. */
int zkw(ENVIRON *csound, ZKW *p)
{
    MYFLT       *writeloc;
    long indx;

    /* Check to see this index is within the limits of zk space. */
    indx = (long) *p->ndx;
    if (indx > zklast) {
      return perferror(Str("zkw index > isizek. Not writing."));
    }
    else if (indx < 0) {
      return perferror(Str("zkw index < 0. Not writing."));
    }
    else {
      /* Now write to the appropriate location in zk space.  */
      writeloc = zkstart + indx;
      *writeloc = *p->sig;
    }
    return OK;
}

/*-----------------------------------*/

/* ziw writes to zk space, but only at init time.
 *
 * Call zkset() to check that zk space has been allocated, then use
 * same code as zkw() except that errors go to initerror().  */
int ziw(ENVIRON *csound, ZKW *p)
{
    MYFLT       *writeloc;
    long indx;

    if (zkset(csound,(ZKR*)p) == 0) return NOTOK;
    indx = (long) *p->ndx;
    if (indx > zklast) {
      return perferror(Str("zkw index > isizek. Not writing."));
    }
    else if (indx < 0) {
      return perferror(Str("zkw index < 0. Not writing."));
    }
    else {
        /* Now write to the appropriate location in zk space. */
        writeloc = zkstart + indx;
        *writeloc = *p->sig;
    }
    return OK;
}

/*-----------------------------------*/

/* i and k rate zk WRITE code, with a mix option. */


/* zkwm writes to zk space at k rate. */
int zkwm(ENVIRON *csound, ZKWM *p)
{
    MYFLT       *writeloc;
    long indx;

    /* Check to see this index is within the limits of zk space.   */
    indx = (long) *p->ndx;
    if (indx > zklast) {
      return perferror(Str("zkwm index > isizek. Not writing."));
    }
    else if (indx < 0) {
       return  perferror(Str("zkwm index < 0. Not writing."));
    }
    else {
        /* Now write to the appropriate location in zk space.  */
        writeloc = zkstart + indx;
        /* If mix parameter is 0, then overwrite the data in the
         * zk space variable, otherwise read the old value, and write
         * the sum of it and the input sig.    */
        if (*p->mix == 0)
          *writeloc = *p->sig;
        else
          *writeloc += *p->sig;
    }
    return OK;
}

/*-----------------------------------*/

/* ziwm writes to zk space, but only at init time - with a mix option.
 *
 * Call zkset() to check that zk space has been allocated, then run
 * similar code to zkwm() to do the work - but with errors to initerror().
 */
int ziwm(ENVIRON *csound, ZKWM *p)
{
    MYFLT       *writeloc;
    long indx;

    if (zkset(csound,(ZKR*)p) == 0) return OK;
    indx = (long) *p->ndx;
    if (indx > zklast) {
      return initerror(Str("ziwm index > isizek. Not writing."));
    }
    else if (indx < 0) {
      return initerror(Str("ziwm index < 0. Not writing."));
    }
    else {
      writeloc = zkstart + indx;
      if (*p->mix == 0)
        *writeloc = *p->sig;
      else
        *writeloc += *p->sig;
    }
    return OK;
}

/*-----------------------------------*/

/* k rate ZKMOD subroutine.      */
int zkmod(ENVIRON *csound, ZKMOD *p)
{
    MYFLT *readloc;
    long indx;
    int mflag = 0;    /* set to true if should do the modulation with
                         multiplication rather than addition.    */

    /* If zkmod = 0, then just copy input to output. We want to make
     * this as fast as possible, because in many instances, this will be
     * the case.
     *
     * Note that in converting the zkmod index into a long, we want
     * the normal conversion rules to apply to negative numbers -
     * so -2.3 is converted to -2.                               */

    if ((indx = (long)*p->zkmod) == 0) {
      *p->rslt = *p->sig;
      return OK;
    }
    /* Decide whether index is positive or negative. Make it postive. */
    if (indx < 0) {
      indx = - indx;
      mflag = 1;
    }
    /* Check to see this index is within the limits of zk space. */

    if (indx > zklast) {
      return perferror(Str("zkmod kzkmod > isizek. Not writing."));
    }
    else {
      /* Now read the value from zk space. */
      readloc = zkstart + indx;
      /* If mflag is 0, then add the modulation factor. Otherwise multiply it.*/
      if (mflag == 0)
        *p->rslt = *p->sig + *readloc;
      else
        *p->rslt = *p->sig * *readloc;
    }
    return OK;
}

/*-----------------------------------*/

/* zkcl clears a range of variables in zk space at k rate.       */
int zkcl(ENVIRON *csound, ZKCL *p)
{
    MYFLT       *writeloc;
    long first = (long) *p->first, last = (long) *p->last, loopcount;

    /* Check to see both kfirst and klast are within the limits of zk space
     * and that last is >= first.                */
    if ((first > zklast) || (last > zklast))
      return perferror(Str("zkcl first or last > isizek. Not clearing."));
    else if ((first < 0) || (last < 0)) {
      return perferror(Str("zkcl first or last < 0. Not clearing."));
    }
    else if (first > last) {
      return perferror(Str("zkcl first > last. Not clearing."));
    }
    else {
          /* Now clear the appropriate locations in zk space. */
          loopcount = last - first + 1;
          writeloc = zkstart + first;
          do {
            *writeloc++ = FL(0.0);
          } while (--loopcount);
    }
    return OK;
}

/*---------------------------------------------------------------------------*/

/* AUDIO rate zak code.
 */

/* zaset() is called at the init time of the instance of the zar or zaw ugens.
 * All it has to do is spit the dummy if za space has not been allocated yet.
 */
int zaset(ENVIRON *csound, ZAR *p)
{
    if  (zastart == NULL) {
      return initerror(Str("No za space: zakinit has not been called yet."));
    }
    else
      return (OK);
}

/*-----------------------------------*/

/* a rate READ code. */

/* zar reads from za space at a rate. */
int zar(ENVIRON *csound, ZAR *p)
{
    MYFLT       *readloc, *writeloc;
    long indx;
    int nsmps = ksmps;

    /*-----------------------------------*/

    writeloc = p->rslt;

    /* Check to see this index is within the limits of za space.    */
    indx = (long) *p->ndx;
    if (indx > zalast) {
      do {
        *writeloc++ = FL(0.0);
      } while(--nsmps);
      return perferror(Str("zar index > isizea. Returning 0."));
    }
    else if (indx < 0) {
      do {
        *writeloc++ = FL(0.0);
      } while(--nsmps);
      return perferror(Str("zar index < 0. Returning 0."));
    }
    else {
      /* Now read from the array in za space and write to the destination.
       * See notes in zkr() on pointer arithmetic.     */
      readloc = zastart + (indx * ksmps);
      do {
        *writeloc++ = *readloc++;
      } while(--nsmps);
    }
    return OK;
}

/*-----------------------------------*/

/* zarg() reads from za space at audio rate, with gain controlled by a
 * k rate variable. Code is almost identical to zar() above. */
int zarg(ENVIRON *csound, ZARG *p)
{
    MYFLT       *readloc, *writeloc;
    MYFLT       kgain;          /* Gain control */
    long        indx;
    int nsmps = ksmps;

    /*-----------------------------------*/

    writeloc = p->rslt;
    kgain = *p->kgain;

    /* Check to see this index is within the limits of za space.    */
    indx = (long) *p->ndx;
    if (indx > zalast) {
      do {
        *writeloc++ = FL(0.0);
      } while(--nsmps);
      return perferror(Str("zarg index > isizea. Returning 0."));
    }
    else {
      if (indx < 0) {
        do {
          *writeloc++ = FL(0.0);
        } while(--nsmps);
        return perferror(Str("zarg index < 0. Returning 0."));
      }
      else {
        /* Now read from the array in za space multiply by kgain and write
         * to the destination.       */
        readloc = zastart + (indx * ksmps);
        do {
          *writeloc++ = *readloc++ * kgain;
        } while(--nsmps);
      }
    }
    return OK;
}


/*-----------------------------------*/

/* a rate WRITE code. */

/* zaw writes to za space at a rate. */
int zaw(ENVIRON *csound, ZAW *p)
{
    MYFLT       *readloc, *writeloc;
    long indx;
    int nsmps = ksmps;

    /* Set up the pointer for the source of data to write.    */
    readloc = p->sig;
    /* Check to see this index is within the limits of za space.     */
    indx = (long) *p->ndx;
    if (indx > zalast) {
        return perferror(Str("zaw index > isizea. Not writing."));
    }
    else if (indx < 0) {
        return perferror(Str("zaw index < 0. Not writing."));
    }
    else {
        /* Now write to the array in za space pointed to by indx.    */
      writeloc = zastart + (indx * ksmps);
      do {
        *writeloc++ = *readloc++;
      } while(--nsmps);
    }
    return OK;
}



/*-----------------------------------*/

/* a rate WRITE code with mix facility. */

/* zawm writes to za space at a rate. */
int zawm(ENVIRON *csound, ZAWM *p)
{
    MYFLT       *readloc, *writeloc;
    long indx;
    int nsmps = ksmps;
    /*-----------------------------------*/

    /* Set up the pointer for the source of data to write. */

    readloc = p->sig;
    /* Check to see this index is within the limits of za space.    */
    indx = (long) *p->ndx;
    if (indx > zalast) {
      return perferror(Str("zaw index > isizea. Not writing."));
    }
    else if (indx < 0) {
      return perferror(Str("zaw index < 0. Not writing."));
    }
    else {
      /* Now write to the array in za space pointed to by indx.    */
      writeloc = zastart + (indx * ksmps);
      if (*p->mix == 0) {
        /* Normal write mode.  */
        do {
          *writeloc++ = *readloc++;
        } while(--nsmps);
      }
      else {
        /* Mix mode - add to the existing value.   */
        do {
          MYFLT x = *readloc++ + *writeloc;
          *writeloc++ = x;
        } while(--nsmps);
      }
    }
    return OK;
}

/*-----------------------------------*/

/* audio rate ZAMOD subroutine.
 *
 * See zkmod() for fuller explanation of code.
 */
int zamod(ENVIRON *csound, ZAMOD *p)
{
    MYFLT       *writeloc, *readloc;
    MYFLT       *readsig;       /* Array of input floats */
    long indx;
    int mflag = 0;             /* non zero if modulation with multiplication  */
    int nsmps = ksmps;

    /* Make a local copy of the pointer to the input signal, so we can auto-
     * increment it. Likewise the location to write the result to.     */
    readsig = p->sig;
    writeloc = p->rslt;
    /* If zkmod = 0, then just copy input to output.    */
    if ((indx = (long) *p->zamod) == 0) {
      do {
        *writeloc++ = *readsig++;
      } while (--nsmps);
      return OK;
    }
    /* Decide whether index is positive or negative.  Make it postive.    */
    if (indx < 0) {
      indx = - indx;
      mflag = 1;
    }
    /* Check to see this index is within the limits of za space.    */
    if (indx > zalast) {
      return perferror(Str("zamod kzamod > isizea. Not writing."));
    }
    else {                      /* Now read the values from za space.    */
      readloc = zastart + (indx * ksmps);
      if (mflag == 0) {
        do {
          *writeloc++ = *readsig++ + *readloc++;
        } while (--nsmps);
      }
      else {
        do {
          *writeloc++ = *readsig++ * *readloc++;
        } while (--nsmps);
      }
    }
    return OK;
}

/*-----------------------------------*/

/* zacl clears a range of variables in za space at k rate. */
int zacl(ENVIRON *csound, ZACL *p)
{
    MYFLT       *writeloc;
    long first, last, loopcount;

    first = (long) *p->first;
    last  = (long) *p->last;
    /* Check to see both kfirst and klast are within the limits of za space
     * and that last is >= first.    */
    if ((first > zalast) || (last > zalast))
      return perferror(Str("zacl first or last > isizea. Not clearing."));
    else {
      if ((first < 0) || (last < 0)) {
        return perferror(Str("zacl first or last < 0. Not clearing."));
      }
      else {
        if (first > last) {
          return perferror(Str("zacl first > last. Not clearing."));
        }
        else {  /* Now clear the appropriate locations in za space. */
          loopcount = (last - first + 1) * ksmps;
          writeloc = zastart + (first * ksmps);
          do {
            *writeloc++ = FL(0.0);
          } while (--loopcount);
        }
      }
    }
    return OK;
}


/*****************************************************************************/
/*****************************************************************************/

/* Subroutines for reading absolute time. */

/* timek()
 *
 * Called at i rate or k rate, by timek, itimek, timesr or itemes.
 *
 * This is based on global variable kcounter in insert.c.
 * Since is apparently is not declared in a header file, we must declare it
 * an external.
 * Actually moved to the glob structure -- JPff march 2002
 */

int timek(ENVIRON *csound, RDTIME *p)
{
    /* Read the global variable kcounter and turn it into a float.   */
    *p->rslt = (MYFLT) kcounter;
    return OK;
}

/* timesr() */
int timesr(ENVIRON *csound, RDTIME *p)
{
    /* Read the global variable kcounter divide it by the k rate.    */

    *p->rslt = (MYFLT) kcounter * onedkr ;
    return OK;
}

/*-----------------------------------*/

/* Subroutines to read time for this instance of the instrument. */


/* instimset() runs at init time and keeps a record of the time then
 * in the RDTIME data structure.
 * Returns 0.
 */
int instimset(ENVIRON *csound, RDTIME *p)
{
    p->instartk = kcounter;
    *p->rslt = FL(0.0);
    return OK;
}

/*  void instimset1(RDTIME *p) */
/*  { */
/*      printf(Str("Deprecated opcode instime[ks].  Use timeinst[ks]\n")); */
/*      p->instartk = kcounter; */
/*      *p->rslt = FL(0.0); */
/*  } */

/* instimek()
 *
 * Read difference between the global variable kcounter and the starting
 * time of this instance. Return it as a float.
 */
int instimek(ENVIRON *csound, RDTIME *p)
{
    *p->rslt = (MYFLT) (kcounter - p->instartk);
    return OK;
}

/* insttimes()
 *
 * Read difference between the global variable kcounter and the starting
 * time of this instance.  Return it as a float in seconds.
 */
int instimes(ENVIRON *csound, RDTIME *p)
{
    *p->rslt = (MYFLT) (kcounter - p->instartk) * onedkr ;
    return OK;
}

/*****************************************************************************/
/*****************************************************************************/

/* Printing at k rate - printk. */

/* printkset is called when the instance of the instrument is initiallised. */

int printkset(ENVIRON *csound, PRINTK *p)
{
    /* Set up ctime so that if it was 0 or negative, it is set to a low value
     * to ensure that the print cycle happens every k cycle.  This low value is
     * 1 / ekr     */
    if (*p->ptime < FL(1.0) / ekr)
      p->ctime = FL(1.0) / ekr;
    else        p->ctime = *p->ptime;

    /* Set up the number of spaces.
       Limit to 120 for people with big screens or printers.
     */
    if (*p->space < FL(0.0))       p->pspace = 0L;
    else if (*p->space > FL(120.0))p->pspace = 120L;
    else                           p->pspace = (long)*p->space;

    /* Set the initime variable - how many seconds in absolute time
     * when this instance of the instrument was initialised.     */

    p->initime = (MYFLT) kcounter * onedkr;

    /* Set cysofar to - 1 so that on the first call to printk - when
     * cycle = 0, then there will be a print cycle.     */
    p->cysofar = -1;
    return OK;
}
/*************************************/

/* printk
 *
 * Called on every k cycle. It must decide when to do a print operation.
 */
int printk(ENVIRON *csound, PRINTK *p)
{
    MYFLT       timel;          /* Time in seconds since initialised */
    long        cycles;         /* What print cycle */
    long        spcount;        /* How many spaces to print */

    /*-----------------------------------*/

    /* Initialise variables.   */

    timel =     ((MYFLT) kcounter * onedkr) - p->initime;

    /* Divide the current elapsed time by the cycle time and round down to
     * an integer.
     */
    cycles = (long)(timel / p->ctime);

    /* Now test if the cycle number we arein is higher than the one in which
     * we last printed. If so, update cysofar and print.     */
    if (p->cysofar < cycles) {
        p->cysofar = cycles;
        /* Do the print cycle.
         * Print instrument number and time. Instrument number stuff from
         * printv() in disprep.c.
         */
        printf(" i%4d ", p->h.insdshead->insno);
        printf(Str("time %11.5f:"), (MYFLT) kcounter * onedkr);
        /* Print spaces and then the value we want to read.      */
        spcount = p->pspace + 1;
        do {
          printf(" ");
        } while (--spcount);
        printf("%11.5f\n", *p->val);
      }
    return OK;
}

/*---------------------------------------------------------------------------*/

/* printks() and printksset() */

/* Printing at k rate with a string * and up to four variables - printks. */

#define ESC (0x1B)

/* printksset is called when the instance of the instrument is initiallised. */
int printksset(ENVIRON *csound, PRINTKS *p)
{
    char        *sarg;
    char        *sdest;
    char        temp, tempn;

    if (*p->ptime < FL(1.0) / ekr)
      p->ctime = FL(1.0) / ekr;
    else        p->ctime = *p->ptime;

    /* Set the initime variable - how many seconds in absolute time
     * when this instance of the instrument was initialised.     */

    p->initime = (MYFLT) kcounter * onedkr;

    /* Set cysofar to - 1 so that on the first call to printk - when
     * cycle = 0, then there will be a print cycle.    */
    p->cysofar = -1;

    /* Set up the string to print.  printf() will take care of the
     * %f format specifiers, but we need to decode any other special
     * codes we may want to put in here.
     */

    /* We get the string via the same mechanism used in ugens3.c for
     * adsyn().  I have not checked everything which stands behind this.
     */

    /* If it was not valid string, then complain.
     * Also complain if the string has nothing in it. However the
     * program (under DJGPP at least) seems to crash elsewhere if
     * the first parameter is "".     */

    if (*p->ifilcod != SSTRCOD && p->STRARG==NULL && currevent->strarg==NULL) {
        sprintf(errmsg,
                Str("printks parm 1 was not a \"quoted string\"\n"));
        return initerror(errmsg);
    }
    else {
      sarg = p->STRARG;
      if (sarg==NULL) sarg = currevent->strarg;
      memset(p->txtstring, 0, 8192);   /* This line from matt ingalls */
      sdest = p->txtstring;
      /* Copy the string to the storage place in PRINTKS.
       *
       * We will look out for certain special codes and write special
       * bytes directly to the string.
       *
       * There is probably a more elegant way of doing this, then using
       * the look flag.  I could use goto - but I would rather not.      */
                                /* This is really a if then else if...
                                 * construct and is currently grotty -- JPff */
      do {
        temp  = *sarg++;
        tempn = *sarg--;
        /* Look for a single caret and insert an escape char.  */
        if ((temp  == '^') && (tempn != '^')) {
          *sdest++ = ESC;
        }
/* Look for a double caret and insert a single caret - stepping forward  one */
        else if ((temp  == '^') && (tempn == '^')) {
          *sdest++ = '^';
          sarg++;
        }
/* Look for a single tilde and insert an escape followed by a '['.
 * ESC[ is the escape sequence for ANSI consoles */
        else if ((temp  == '~') && (tempn != '~')) {
          *sdest++ = ESC;
          *sdest++ = '[';
        }
/* Look for a double tilde and insert a tilde caret - stepping forward one.  */
        else if ((temp  == '~') && (tempn == '~')) {
          *sdest++ = '~';
          sarg++;
        }
        /* Look for \n, \N etc */
        else if (temp == '\\') {
          switch (tempn) {
          case 'r': case 'R':
            *sdest++ = '\r';
            sarg++;
            break;
          case 'n': case 'N':
            *sdest++ = '\n';
            sarg++;
            break;
          case 't': case 'T':
            *sdest++ = '\t';
            sarg++;
            break;
          case 'a': case 'A':
            *sdest++ = '\a';
            sarg++;
            break;
          case 'b': case 'B':
            *sdest++ = '\b';
            sarg++;
            break;
          case '\\':
            *sdest++ = '\\';
            sarg++;
            break;
          default:
            *sdest++ = tempn;
            sarg++;
            break;
          }
        }
        /* This case is from matt ingalls */
        else if (temp == '%') { /* an extra option to specify tab and
                                   return as %t and %r*/
          switch (tempn) {
          case 'r': case 'R':
            *sdest++ = '\r';
            sarg++;
            break;
          case 'n': case 'N':
            *sdest++ = '\n';
            sarg++;
            break;
          case 't': case 'T':
            *sdest++ = '\t';
            sarg++;
            break;
          case '!':     /* and a ';' */
            *sdest++ = ';';
            sarg++;
            break;
          default:
            *sdest++ = temp;
            break;
          }
        }
        else {
          /* If none of these match, then copy the character directly
           * and try again.      */
          *sdest++ = temp;
        }
      /* Increment pointer and process next character until end of string.  */
      } while (*++sarg != 0);
    }
    return OK;
}


/* perform a sprintf-style format  -- matt ingalls */
void sprints(char *outstring, char *fmt, MYFLT **kvals, long numVals)
{
    char strseg[8192];
    int i = 0, j = 0;
    char *segwaiting = 0;

    while (*fmt) {
      if (*fmt == '%') {
        /* if already a segment waiting, then lets print it */
        if (segwaiting) {
          strseg[i] = '\0';

          switch (*segwaiting) {
          case 'd':
          case 'i':
          case 'o':
          case 'x':
          case 'X':
          case 'u':
          case 'c':
            sprintf(outstring, strseg, (int)(*kvals[j]+.5));
            break;
          case 'h':
            sprintf(outstring, strseg, (short)(*kvals[j]+.5));
            break;
          case 'l':
            sprintf(outstring, strseg, (long)(*kvals[j]+.5));
            break;

          default:
            sprintf(outstring, strseg, *kvals[j]);
            break;
          }
          outstring += strlen(outstring);

          i = 0;
          segwaiting = 0;

          /* prevent potential problems if user didnt give enough input params */
          if (j < numVals-1)
            j++;
        }

        /* copy the '%' */
        strseg[i++] = *fmt++;

        /* find the format code */
        segwaiting = fmt;
        while (*segwaiting && !isalpha(*segwaiting))
          segwaiting++;
      }
      else
        strseg[i++] = *fmt++;
    }

    if (i) {
      strseg[i] = '\0';
      if (segwaiting) {
        switch (*segwaiting) {
        case 'd':
        case 'i':
        case 'o':
        case 'x':
        case 'X':
        case 'u':
        case 'c':
          sprintf(outstring, strseg, (int)(*kvals[j]+FL(0.5)));
          break;
        case 'h':
          sprintf(outstring, strseg, (short)(*kvals[j]+FL(0.5)));
          break;
        case 'l':
          sprintf(outstring, strseg, (long)(*kvals[j]+FL(0.5)));
          break;

        default:
          sprintf(outstring, strseg, *kvals[j]);
          break;
        }
      }
      else
        sprintf(outstring, strseg);
    }
}

/*************************************/

/* printks is called on every k cycle
 * It must decide when to do a
 * print operation.
 */
int printks(ENVIRON *csound, PRINTKS *p)
{
    MYFLT       timel;
    long        cycles;
    char        string[8192]; /* matt ingals replacement */

    /*-----------------------------------*/

    timel =     ((MYFLT) kcounter * onedkr) - p->initime;

    /* Divide the current elapsed time by the cycle time and round down to
     * an integer.     */
    cycles = (long)(timel / p->ctime);

    /* Now test if the cycle number we are in is higher than the one in which
     * we last printed.  If so, update cysofar and print.     */
    if (p->cysofar < cycles)  {
        p->cysofar = cycles;
        /* Do the print cycle. */
        sprints(string, p->txtstring, p->kvals, p->INOCOUNT-2);
        printf(string);

#ifdef REMOVE_WE_CAN_NOW_USE_PRINTF_CHAR_CODE_INSTEAD
        char *txtptr = p->txtstring;
        txtptr = p->txtstring;
        /* Special code to allow us to print the direct byte value of the
         * first parameter - only if the first character of the string is
         * a #.  Then go on and do a printf() with the three other
         * input variables as floats.
         *
         * This enables us to output any value to the output if we really
         * want to.
         */

        if (*txtptr == '#') {
          /* print the 0 to 255 value of the float - this gives wrap-around
           * for out of range values.      */
          printf("%c", 255 & (int)floor(*p->kval1));
          /* printf the rest of the string, if any.   */
          printf(++txtptr, *p->kval2, *p->kval3, *p->kval4, 0, 0, 0);
                                /* What on earth are these zeros for!!! */
        }
        else {
          /* Otherwise do the normal print cycle of printf()ing the string
           * with four float input variables.
           *
           * Put a few dummy variables on the end for Justin - just in case we
           * put too many % format specifiers in the string.
           */
          printf(txtptr, *p->kval1, *p->kval2, *p->kval3, *p->kval4, 0, 0);
        }
#endif
      }
    return OK;
}


/* matt ingalls --  i-rate prints */
int printsset(ENVIRON *csound, PRINTS *p)
{
    PRINTKS pk;
    char        string[8192];
    MYFLT ptime = 1;
    pk.h = p->h;
    pk.ifilcod = p->ifilcod;
    pk.ptime = &ptime;
    printksset(csound,&pk);
    sprints(string, pk.txtstring, p->kvals, p->INOCOUNT-1);
    printf(string);
    return OK;
}

/*****************************************************************************/


/* peakk and peak ugens */

/* peakk()
 *
 * Write the absolute value of the input argument to the output if the former
 is higher. */
int peakk(ENVIRON *csound, PEAK *p)
{
    if (*p->kpeakout < (MYFLT)fabs(*p->xsigin)) {
      *p->kpeakout = (MYFLT)fabs(*p->xsigin);
    }
    return OK;
}

/* peaka()
 *
 * Similar to peakk, but looks at an a rate input variable. */
int peaka(ENVIRON *csound, PEAK *p)
{
    int         loop;
    int change = 0;
    MYFLT       peak;
    MYFLT       *asigin;

    loop = ksmps;
    asigin = p->xsigin;
    peak = *p->kpeakout;

    do {
      if (peak < (MYFLT)fabs(*asigin++)) {
        peak = (MYFLT)fabs(*--asigin);
        asigin++;
        change = 1;
      }
    } while (--loop);

    if (change) {
      *p->kpeakout = peak;
    }
    return OK;
}

/*****************************************************************************/

/* Gab 21-8-97 */
/* print a k variable each time it changes (useful for MIDI control sliders) */

int printk2set(ENVIRON *csound, PRINTK2 *p)
{
    if (*p->space < 0)        p->pspace = 0;
    else if (*p->space > 120) p->pspace = 120;
    else                      p->pspace = (int)*p->space;
    return OK;
}

/* Gab 21-8-97 */
/* print a k variable each time it changes (useful for MIDI control sliders) */

int printk2(ENVIRON *csound, PRINTK2 *p)
{
    MYFLT   value = *p->val;
    int     spcount;
    if (p->oldvalue != value) {
      printf(" i%d", p->h.insdshead->insno);
      spcount = p->pspace + 1;
      do {
        printf(" ");
      } while (--spcount);
      printf("%11.5f\n", *p->val);
      p->oldvalue = value;
    }
    return OK;
}

/* inz writes to za space at a rate as many channels as can. */
int inz(ENVIRON *csound, IOZ *p)
{
    MYFLT       *readloc, *writeloc;
    long indx, i;
    int nsmps = ksmps;

    readloc = spin;
    /* Check to see this index is within the limits of za space.     */
    indx = (long) *p->ndx;
    if (indx > zalast) {
      return perferror(Str("inz index > isizea. Not writing."));
    }
    else if (indx < 0) {
      return perferror(Str("inz index < 0. Not writing."));
    }
    else {
      /* Now write to the array in za space pointed to by indx.    */
      writeloc = zastart + (indx * ksmps);
      for (i=0; i<nchnls; i++)
        for (nsmps=0; nsmps<ksmps; nsmps++)
          *writeloc++ = spin[i*ksmps+nsmps];
    }
    return OK;
}

/* outz reads from za space at a rate to output. */
int outz(ENVIRON *csound, IOZ *p)
{
    MYFLT       *readloc, *writeloc;
    long indx;
    int i;
    int nsmps = ksmps;

    /*-----------------------------------*/

    writeloc = spout;

    /* Check to see this index is within the limits of za space.    */
    indx = (long) *p->ndx;
    if (indx > zalast) {
      return perferror(Str("outz index > isizea. No output"));
    }
    else if (indx < 0) {
      return perferror(Str("outz index < 0. No output."));
    }
    /* Now read from the array in za space and write to the output. */
    readloc = zastart + (indx * ksmps);
    if (!spoutactive) {
      for (i=0; i<nchnls; i++)
        for (nsmps=0; nsmps<ksmps; nsmps++)
          spout[i*ksmps+nsmps] = *readloc++;
      spoutactive = 1;
    }
    else {
      for (i=0; i<nchnls; i++)
        for (nsmps=0; nsmps<ksmps; nsmps++)
          spout[i*ksmps+nsmps] += *readloc++;
    }
    return OK;
}


void RESET(struct ENVIRON_ *csound)        /* gab d7*/
  /* zakRESET originally */
{
    if (csound->zkstart_ != NULL) {
      csound->mfree_(csound->zkstart_);
      csound->zkstart_ = NULL;
    }
    if (csound->zastart_ != NULL) {
      csound->mfree_(csound->zastart_);
      csound->zastart_ = NULL;
    }
}


