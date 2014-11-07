/* Keyboard Layer/2
 *
 * Fullscreen monitors support
 * Copyright (C) 1999 by Dmitry A.Steklenev
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * $Id$
 */

#define  INCL_WIN
#define  INCL_DOS
#define  INCL_KBD
#define  INCL_ERRORS
#define  INCL_DOSDEVIOCTL
#include <os2.h>
#include <stdio.h>

#include "kl_monitors.h"
#include "kl_engine.h"
#include "kl_trigger.h"

static HMONITOR hkeyboard = NULLHANDLE; // Keyboard driver handle
static HEV      hdone     = NULLHANDLE; // Notifying about end of monitor
                                        // initialization

static TID   tids[16]; // Monitors threads identificators
static BOOL  extended  = FALSE;
static ULONG layer_id  = KL_NONE;

/*--------------------------------------------------
 * Translate scan code to ASCII
 *--------------------------------------------------*/
static void translate( KEYPACKET *key )
{
  static struct _DATAPACKET
  {
    UCHAR   XlatedChar;
    UCHAR   XlatedScan;
    UCHAR   DBCSStatus;
    UCHAR   DBCSShift;
    USHORT  ShiftState;
    ULONG   Milliseconds;
    USHORT  KbdDDFlagWord;
    USHORT  XlateFlags;
    USHORT  XlateState1;
    USHORT  XlateState2;

  } data = { 0,0,0,0,0,0,0,0,0,0 };

  struct _PARMPACKET
  {
    USHORT cp;
    USHORT reserved;

  } parm = { 0, 0 };

  data.XlatedChar    = key->XlatedChar;
  data.XlatedScan    = key->XlatedScan;
  data.DBCSStatus    = key->DBCSStatus;
  data.DBCSShift     = key->DBCSShift;
  data.ShiftState    = key->ShiftState;
  data.Milliseconds  = key->Milliseconds;
  data.KbdDDFlagWord = key->KbdDDFlagWord;
  data.XlateFlags    = 0;

  ULONG parm_len = sizeof(parm);
  ULONG data_len = sizeof(data);

  DosDevIOCtl( hkeyboard, IOCTL_KEYBOARD, KBD_XLATESCAN,
                          &parm, sizeof(parm), &parm_len,
                          &data, sizeof(data), &data_len  );

  if( data.XlateFlags & 0x0001 && key->XlatedScan != 0x37 )
  {
    key->XlatedChar = data.XlatedChar;
    key->XlatedScan = data.XlatedScan;
    key->ShiftState = data.ShiftState;
  }
}

/*--------------------------------------------------
 * Sets the keyboard layer for current FS session
 *--------------------------------------------------*/
static void set_fs_layer( ULONG layer )
{
  struct _PARMPACKET
  {
    USHORT cp;
    USHORT layer;
  };

  _PARMPACKET parm = { 0, 0 };
  _PARMPACKET data = { 0, 0 };

  // Get current codepage
  ULONG parm_len = sizeof(parm);
  ULONG data_len = sizeof(data);

  DosDevIOCtl( hkeyboard, IOCTL_KEYBOARD, KBD_GETCODEPAGEID,
                          &parm, sizeof(parm), &parm_len,
                          &data, sizeof(data), &data_len  );
  // Set keyboard layer
  parm_len   = sizeof(parm);
  data_len   = sizeof(data);
  parm.layer = layer;
  layer_id   = layer;

  DosDevIOCtl( hkeyboard, IOCTL_KEYBOARD, 0x58,
                          &parm, sizeof(parm), &parm_len,
                          &data, sizeof(data), &data_len  );
}

/*--------------------------------------------------
 * Keyboard monitor
 *--------------------------------------------------*/
static void _System monitor( ULONG ses_id )
{
  MONBUF    inp_buf   = { sizeof(MONBUF), 0 };
  MONBUF    out_buf   = { sizeof(MONBUF), 0 };
  ULONG     ses_layer = KL_LATIN;
  ULONG     detected  = KL_LATIN;
  KEYPACKET key;
  BYTE      scan;

  DosSetPriority( PRTYS_THREAD, PRTYC_TIMECRITICAL, PRTYD_MAXIMUM, 0 );

  // Register keyboard monitor for specified session
  if( DosMonReg( hkeyboard, &inp_buf, &out_buf,
                 MONITOR_BEGIN, (USHORT)ses_id ) != NO_ERROR )
  {
    tids[ses_id] = 0;
    DosPostEventSem( hdone );
    DosExit( EXIT_THREAD, 0 );
  }
  else
    DosPostEventSem( hdone );

  // Dispatch keyboard packets
  for(;;)
  {
    USHORT len = sizeof(key);
    DosMonRead((PBYTE)&inp_buf, 0, (PBYTE)&key, &len );

    if( get_trigger_hotkeys() == XS_DONTTOUCH )
    {
      DosMonWrite((PBYTE)&out_buf, (PBYTE)&key, len );
      continue;
    }

    /* debug
    printf( "key MonFlagWord   : %02X\n"
            "    OriginScan    : %02X\n"
            "    XlatedChar    : %02X\n"
            "    XlatedScan    : %02X\n"
            "    DBCSStatus    : %02X\n"
            "    DBCSShift     : %02X\n"
            "    ShiftState    : %04X\n"
            "    Milliseconds  : %08X\n"
            "    KbdDDFlagWord : %04X\n",

            key.MonFlagWord  ,
            key.OriginScan   ,
            key.XlatedChar   ,
            key.XlatedScan   ,
            key.DBCSStatus   ,
            key.DBCSShift    ,
            key.ShiftState   ,
            key.Milliseconds ,
            key.KbdDDFlagWord
          );

    fflush( stdout );
    */

    // Skip open, close and flush packets. Also, skip key was repeated
    // make of a toggle key
    if(( key.MonFlagWord   & ( MF_FLUSH | MF_OPEN | MF_CLOSE )) ||
       ( key.KbdDDFlagWord & KF_MULTIMAKE ))
    {
      DosMonWrite((PBYTE)&out_buf, (PBYTE)&key, len );
      continue;
    }

    // Handle Multi-Layer NLS packet.
    // Layout index: 0,1,2 - latin, 3,4 - national
    // (from kbdbase.sys source):
    if(( key.KbdDDFlagWord & KF_KEYTYPEMASK ) == KF_MLNOTIFICATION )
    {
      ses_layer = key.LayerIndex <= 2 ? KL_LATIN : KL_NATIONAL;
      DosMonWrite((PBYTE)&out_buf, (PBYTE)&key, len );
      continue;
    }

    if( layer_id != ses_layer )
      set_fs_layer( ses_layer );

    // Translate other keys
    if(!(key.KbdDDFlagWord & KF_KEYTYPEMASK ) &&
       !(key.DBCSStatus & SF_NOTCHAR))
    {
      translate( &key );
    }

    // Checking hotkeys
    if( !hkey_empty())
    {
      int scancode = key.OriginScan;
      int state    = 0;

      if( key.ShiftState & RS_CONTROL ) state |= KLHotkey::control;
      if( key.ShiftState & RS_ALT     ) state |= KLHotkey::alt;
      if( key.ShiftState & RS_RSHIFT  ) state |= KLHotkey::shift;
      if( key.ShiftState & RS_LSHIFT  ) state |= KLHotkey::shift;

      const char* name = hkey_find( KLHotkey( scancode, state ));

      if( name )
      {
        set_layout( name );

        if( get_layouts_layer() != KL_NONE && is_multilayered())
        {
          set_fs_layer( ses_layer = get_layouts_layer());

          if( is_beeps_enabled())
              DosBeep( ses_layer == KL_LATIN ? BEEP_LAT : BEEP_NAT, BEEP_LEN );
        }
        continue;
      }
    }

    // Checking switch sequence
    scan = key.OriginScan;

    if( scan == 0xE0 )
      // Extended scan code
      extended = TRUE;
    else
    {
      // Convert extended key to PM scan code
      if( extended )
      {
        switch( scan & 0x7F )
        {
          case CS_LALT     : scan = CS_RALT;     break;
          case CS_LCONTROL : scan = CS_RCONTROL; break;
          case 0x5B        : scan = CS_LWIN;     break;
          case 0x5C        : scan = CS_RWIN;     break;
          case 0x5D        : scan = CS_WINMENU;  break;
        }

        scan |= ( key.OriginScan & 0x80 );
        extended = FALSE;
      }

      detected = KLTrigger::dispatch( scan, ses_layer, key.Milliseconds/100 );

      if( detected != KL_NONE && is_multilayered())
      {
        set_fs_layer( ses_layer = detected );

        if( is_beeps_enabled())
            DosBeep( ses_layer == KL_LATIN ? BEEP_LAT : BEEP_NAT, BEEP_LEN );
      }
    }

    DosMonWrite((PBYTE)&out_buf, (PBYTE)&key, len );
  }
}

/*--------------------------------------------------
 * Start full screen sessions monitors
 *--------------------------------------------------*/
void monitors_start()
{
  if( !hkeyboard )
  {
    // Open keyboard driver
    if( DosMonOpen( "KBD$", &hkeyboard ))
      return;

    if( DosCreateEventSem( NULL, &hdone, 0, FALSE ))
      return;

    // Register monitors for all OS/2 fullscreen sessions
    for( ULONG ses_id = 0; ses_id <= 15; ses_id++ )
    {
      ULONG pulPostCt;

      DosCreateThread ( tids+ses_id, (PFNTHREAD)&monitor, ses_id, CREATE_READY, 12000L );
      DosWaitEventSem ( hdone, (ULONG)SEM_INDEFINITE_WAIT );
      DosResetEventSem( hdone, &pulPostCt );

      /* debug
      printf( "install monitor for session %02d, TID=%08X\n", ses_id, tids[ses_id] );
      fflush( stdout );
      */
    }

    DosCloseEventSem( hdone );
  }
}

/*--------------------------------------------------
 * Stop monitors
 *--------------------------------------------------*/
void monitors_stop()
{
  // Kill all threads with monitors
  for( ULONG id = 0; id <= 15; id++ )
  {
    if( tids[id] )
        DosKillThread( tids[id] );
  }

  // Close keyboard driver
  if( hkeyboard )
  {
     DosMonClose(  hkeyboard );
     hkeyboard = NULLHANDLE;
  }
}

