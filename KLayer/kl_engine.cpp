/* Keyboard Layer/2
 *
 * Engine
 * Copyright (C) 1998 by Dmitry A.Steklenev
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
 * $Id: kl_engine.cpp,v 1.43 2006/02/13 11:50:56 glass Exp $
 */

#include "kl_engine.h"
#include "kl_monitors.h"
#include "kl_trigger.h"
#include "kl_broker.h"
#include "kl_vdd.h"

#define  INCL_DOS
#define  INCL_WIN
#define  INCL_ERRORS
#include <os2.h>
#include <stdio.h>

static HWND    hviewer = NULLHANDLE;  // Layer window handle
static HMODULE hdll    = NULLHANDLE;  // DLL handle
static HWND    hlockup = NULLHANDLE;  // Handle of lockup window
static HVDD    hvdd    = NULLHANDLE;  // VDD handle
static TID     tvdd    = 0;           // VDM thread identifier

static ULONG layer_id               = KL_LATIN;
static ULONG layer_default_id       = KL_NONE;
static ULONG layer_layouts_id       = KL_NONE;
static BOOL  enable_vdm_synchronize = FALSE;
static BOOL  enable_beeps           = FALSE;
static BOOL  enable_changer         = FALSE;
static BOOL  enable_float_on_top    = FALSE;

struct hkey_pair
{
  KLHotkey hotkey;
  char layout[_MAX_LNAME];
};

static hkey_pair   hkey_list[_MAX_HKEYS];
static int         hkey_size  = 0;
static int         hkey_head  = 0;
static HWND        hkey_excl  = NULLHANDLE;

#define LSET_LAYOUT          0
#define LSET_IS_MULTILAYERED 1

static const void* lset_param = NULL;
static ULONG       lset_rc    = 0;
static TID         lset_tid   = 0;
static int         lset_query = 0;
static HEV         lset_start = NULLHANDLE;
static HEV         lset_done  = NULLHANDLE;
static HMTX        lset_mutex = NULLHANDLE;

KLBroker* broker;

/*--------------------------------------------------
 * Preferences control functions
 *--------------------------------------------------*/

void  EXPENTRY set_float_on_top   ( BOOL  enable ) { enable_float_on_top = enable;      }
void  EXPENTRY set_vdm_synchronize( BOOL  enable ) { enable_vdm_synchronize = enable;   }

BOOL  EXPENTRY is_float_on_top_enabled() { return enable_float_on_top;    }
BOOL  EXPENTRY is_beeps_enabled       () { return enable_beeps;           }
BOOL  EXPENTRY is_vdm_supported       () { return hvdd != NULLHANDLE;     }
BOOL  EXPENTRY is_vdm_synchronized    () { return enable_vdm_synchronize; }

int   EXPENTRY get_trigger_hotkeys() { return KLTrigger::selected(); }
ULONG EXPENTRY get_trigger_timeout() { return KLTrigger::timeout (); }
ULONG EXPENTRY get_default_layer  () { return layer_default_id;      }
ULONG EXPENTRY get_layouts_layer  () { return layer_layouts_id;      }

/*--------------------------------------------------
 * Enables sound signals at switching keyboard layers
 *--------------------------------------------------*/
void EXPENTRY set_beeps( BOOL enable )
{
  enable_beeps = enable;

  if( hvdd )
    DosRequestVDD( hvdd, 0, VLCMD_SETBEEP, sizeof(enable), &enable, 0, NULL );
}

/*--------------------------------------------------
 * Sets the VDM colors
 *--------------------------------------------------*/
void EXPENTRY set_vdm_color( LONG rgb )
{
  if( hvdd )
  {
    if( !is_vdm_synchronized())
      rgb = 0xFF0040;

    DosRequestVDD( hvdd, 0, VLCMD_SETBORDER, sizeof(rgb), &rgb, 0, NULL );
  }
}

/*--------------------------------------------------
 * Sets the keyboard layer for newly created windows
 *--------------------------------------------------*/
void EXPENTRY set_default_layer( ULONG layer )
{
  layer_default_id = layer;

  if( hvdd )
    DosRequestVDD( hvdd, 0, VLCMD_SETDEFLAYER,
                            sizeof(layer), &layer, 0, NULL );
}

/*--------------------------------------------------
 * Sets the keyboard layer for newly selected layouts
 *--------------------------------------------------*/
void EXPENTRY set_layouts_layer( ULONG layer )
{
  layer_layouts_id = layer;
}

/*--------------------------------------------------
 * Selects the trigger hotkeys
 *--------------------------------------------------*/
void EXPENTRY set_trigger_hotkeys( int type )
{
  KLTrigger::select( type );

  if( hvdd )
  {
    USHORT vdd_type = type;
    DosRequestVDD( hvdd, 0, VLCMD_SETTRIGGER, sizeof(vdd_type), &vdd_type, 0, NULL );
  }
}

/*--------------------------------------------------
 * Sets the trigger timeout value
 *--------------------------------------------------*/
void EXPENTRY set_trigger_timeout( ULONG time )
{
  KLTrigger::set_timeout( time );

  if( hvdd )
    DosRequestVDD( hvdd, 0, VLCMD_SETTIMEOUT, sizeof(time), &time, 0, NULL );
}

/*--------------------------------------------------
 * Selects the keyboard layout
 *--------------------------------------------------*/
BOOL EXPENTRY set_layout( const char* name )
{
  BOOL rc = FALSE;

  if( DosOpenMutexSem( NULL, &lset_mutex ) == NO_ERROR )
  {
    if( DosRequestMutexSem( lset_mutex, SEM_INDEFINITE_WAIT ) == NO_ERROR )
    {
      if( DosOpenEventSem( NULL, &lset_start ) == NO_ERROR )
      {
        lset_param = name;
        lset_query = LSET_LAYOUT;

        DosPostEventSem ( lset_start );
        DosCloseEventSem( lset_start );

        if( DosOpenEventSem( NULL, &lset_done ) == NO_ERROR )
        {
          ULONG pulPostCt;

          DosWaitEventSem ( lset_done, (ULONG)SEM_INDEFINITE_WAIT );
          DosResetEventSem( lset_done, &pulPostCt );
          DosCloseEventSem( lset_done );

          rc = lset_rc;
        }
      }

      DosReleaseMutexSem( lset_mutex );
    }

    DosCloseMutexSem( lset_mutex );
  }

  return rc;
}

/*--------------------------------------------------
 * Is current selected layout multilayered
 *--------------------------------------------------*/
BOOL EXPENTRY is_multilayered()
{
  BOOL rc = FALSE;

  if( DosOpenMutexSem( NULL, &lset_mutex ) == NO_ERROR )
  {
    if( DosRequestMutexSem( lset_mutex, SEM_INDEFINITE_WAIT ) == NO_ERROR )
    {
      if( DosOpenEventSem( NULL, &lset_start ) == NO_ERROR )
      {
        lset_query = LSET_IS_MULTILAYERED;

        DosPostEventSem ( lset_start );
        DosCloseEventSem( lset_start );

        if( DosOpenEventSem( NULL, &lset_done ) == NO_ERROR )
        {
          ULONG pulPostCt;

          DosWaitEventSem ( lset_done, (ULONG)SEM_INDEFINITE_WAIT );
          DosResetEventSem( lset_done, &pulPostCt );
          DosCloseEventSem( lset_done );

          rc = lset_rc;
        }
      }

      DosReleaseMutexSem( lset_mutex );
    }

    DosCloseMutexSem( lset_mutex );
  }

  return rc;
}

/*--------------------------------------------------
 * Sound signal at turn on of the keyboard layer
 *--------------------------------------------------*/
static void beep_trigger( ULONG id )
{
  if( is_beeps_enabled())
      DosBeep( id == KL_LATIN ? BEEP_LAT : BEEP_NAT, BEEP_LEN );
}

/*--------------------------------------------------
 * If specified window is VDM, returns TRUE
 *--------------------------------------------------*/
static BOOL is_vdm( HWND hwnd, PID *ppid )
{
  if( !hwnd ) return FALSE;

  SWCNTRL swcntrl;
  HSWITCH hswitch = WinQuerySwitchHandle( WinQueryWindow( hwnd, QW_PARENT ), 0 );

  if( hswitch )
  {
    if( WinQuerySwitchEntry( hswitch, &swcntrl ) == 0 )
    {
       if( ppid )
          *ppid = swcntrl.idProcess;
    }
    else
       hswitch = NULLHANDLE;
  }

  return hswitch && ( swcntrl.bProgType == PROG_VDM ||
                      swcntrl.bProgType == PROG_WINDOWEDVDM );
}

/*--------------------------------------------------
 * If specified window is VIO, returns TRUE
 *--------------------------------------------------*/
static BOOL is_vio( HWND hwnd )
{
  if( hwnd )
  {
    SWCNTRL swcntrl;
    HSWITCH hswitch = WinQuerySwitchHandle( WinQueryWindow( hwnd, QW_PARENT ), 0 );

    if( hswitch )
      if( WinQuerySwitchEntry( hswitch, &swcntrl ) == 0 )
        return hswitch && swcntrl.bProgType == PROG_WINDOWABLEVIO;
  }

  return FALSE;
}


/*--------------------------------------------------
 * Returns keyboard layer for specified VDM
 *--------------------------------------------------*/
static ULONG get_vdm_layer( PID pid )
{
  if( !hvdd )
    return KL_LATIN;

  ULONG  layer_id;
  DosRequestVDD( hvdd, pid, VLCMD_GETLAYER, 0, NULL, sizeof(layer_id), &layer_id );
  return layer_id;
}

/*--------------------------------------------------
 * Sets the keyboard layer for specified PM window
 *--------------------------------------------------*/
static void set_pm_layer( HWND hwnd, ULONG layer, ULONG flFlags )
{
  if( hwnd )
  {
    HENUM heEnum = WinBeginEnumWindows( hwnd );
    HWND  next;

    WinSetKbdLayer( hwnd, layer, flFlags );

    while(( next  = WinGetNextWindow( heEnum )) != NULLHANDLE )
      set_pm_layer( next, layer, flFlags );

    WinEndEnumWindows( heEnum );
  }
}

/*--------------------------------------------------
 * Clears hotkeys list
 *--------------------------------------------------*/
void EXPENTRY hkey_clear()
{
  hkey_size = 0;
  hkey_head = 0;

  if( hvdd )
    DosRequestVDD( hvdd, 0, VLCMD_HKEYCLEAR, 0, NULL, 0, NULL );
}

/*--------------------------------------------------
 * Appends another hotkey for a selection of a layout
 *--------------------------------------------------*/
BOOL EXPENTRY hkey_assign( const KLHotkey& hotkey, const char* name )
{
  if( hkey_size < sizeof(hkey_list)/sizeof(hkey_pair))
  {
    hkey_list[hkey_size].hotkey = hotkey;
    strncpy( hkey_list[hkey_size].layout, name, _MAX_LNAME );

    hkey_head = hkey_size++;

    if( hvdd )
    {
      VLHKEY vdd_hkey = { hotkey.scancode(),
                          hotkey.state   () };

      DosRequestVDD( hvdd, 0, VLCMD_HKEYASSIGN,
                              sizeof(vdd_hkey), &vdd_hkey, 0, NULL );
    }

    return TRUE;
  }
  else
    return FALSE;
}

/*--------------------------------------------------
 * Is the hotkey list is empty
 *--------------------------------------------------*/
BOOL EXPENTRY hkey_empty()
{
  return hkey_size == 0;
}

/*--------------------------------------------------
 * Disables hotkeys processing for specified window
 *--------------------------------------------------*/
HWND EXPENTRY hkey_exclude( HWND hwnd )
{
  HWND save = hkey_excl;
  hkey_excl = hwnd;

  return save;
}

/*--------------------------------------------------
 * Finds layout assigned to a specified hotkey
 *--------------------------------------------------*/
const char* hkey_find( const KLHotkey& hotkey )
{
  int head = hkey_head;

  if( hkey_size )
  {
    do
    {
      if( ++head >= hkey_size )
        head = 0;

      if( hkey_list[head].hotkey == hotkey )
      {
        hkey_head = head;
        return hkey_list[head].layout;
      }

    } while( head != hkey_head );
  }

  return NULL;
}

/*--------------------------------------------------
 * Dispatch keyboard events for PM windows
 *--------------------------------------------------*/
static VOID process_chars( HAB, PQMSG pqMsg )
{
  if( get_trigger_hotkeys() != XS_DONTTOUCH && pqMsg->msg == WM_CHAR )
  {
    ULONG detected = KL_NONE;

    // Check scan code if key is up or press (not of auto repeat!)
    if( CHARMSG(&(pqMsg->msg))->fs & KC_SCANCODE &&
      ( CHARMSG(&(pqMsg->msg))->fs & KC_KEYUP    || !(CHARMSG(&(pqMsg->msg))->fs & KC_PREVDOWN )))
    {

      detected = KLTrigger::dispatch( CHARMSG(&(pqMsg->msg))->scancode |
                                    ( CHARMSG(&(pqMsg->msg))->fs & KC_KEYUP ? 0x80 : 0 ),
                                      layer_id, pqMsg->time/100 );
    }

    // If this is a virtual key, reset to switch sequences
    if(!(CHARMSG(&(pqMsg->msg))->fs & KC_SCANCODE ))
      detected = KLTrigger::dispatch( 0, layer_id, 0 );

    if( detected != KL_NONE && is_multilayered() /* && !is_vdm(pqMsg->hwnd, NULL )*/ )
    {
      HWND active = WinQueryActiveWindow( HWND_DESKTOP );

      if( active )
      {
        layer_id = detected;

        set_pm_layer( active, detected, 0 );
        beep_trigger( layer_id );
        WinPostMsg  ( hviewer, KL_LAYERCHANGED, MPFROMLONG(layer_id), NULL );
      }
    }
  }
}

/*--------------------------------------------------
 * Dispatch keyboard events for PM windows
 *--------------------------------------------------*/
VOID EXPENTRY dispatch_chars( HAB hab, PQMSG pqMsg )
{
  PTIB ptib;

  DosGetInfoBlocks( &ptib, NULL );
  if((PBYTE)&ptib - (PBYTE)ptib->tib_pstack >= 1024 ) {
    process_chars( hab, pqMsg );
  }
}

/*--------------------------------------------------
 * Process other keyboard events
 *--------------------------------------------------*/
static BOOL process_posts( HAB hab, PQMSG pqMsg )
{
  if( hkey_size && pqMsg->msg == WM_CHAR )
  {
    if( CHARMSG(&(pqMsg->msg))->fs & KC_SCANCODE &&
      !(CHARMSG(&(pqMsg->msg))->fs & KC_KEYUP    ))
    {
      int state    = 0;
      int scancode = CHARMSG(&(pqMsg->msg))->scancode;

      if( CHARMSG(&(pqMsg->msg))->fs & KC_CTRL  ) state |= KLHotkey::control;
      if( CHARMSG(&(pqMsg->msg))->fs & KC_ALT   ) state |= KLHotkey::alt;
      if( CHARMSG(&(pqMsg->msg))->fs & KC_SHIFT ) state |= KLHotkey::shift;

      const char* name = hkey_find( KLHotkey( scancode, state ));

      if( name && !is_vdm( pqMsg->hwnd, NULL ) &&
        ( hkey_excl == NULLHANDLE || !WinIsChild( pqMsg->hwnd, hkey_excl )))
      {
        if(!(CHARMSG(&(pqMsg->msg))->fs & KC_PREVDOWN ))
        {
          set_layout( name );

          if( get_layouts_layer() != KL_NONE && is_multilayered())
          {
            HWND active = WinQueryActiveWindow( HWND_DESKTOP );

            if( active )
            {
              layer_id = get_layouts_layer();

              set_pm_layer( active, layer_id, 0 );
              beep_trigger( layer_id );
              WinPostMsg  ( hviewer, KL_LAYERCHANGED, MPFROMLONG(layer_id), NULL );
            }
          }
        }
        return TRUE;
      }
    }
  }

  if( get_trigger_hotkeys() != XS_DONTTOUCH && pqMsg->msg == WM_CHAR )
  {
    // Check scan code if key is up or press (not of auto repeat!)
    if( CHARMSG(&(pqMsg->msg))->fs & KC_SCANCODE &&
      ( CHARMSG(&(pqMsg->msg))->fs & KC_KEYUP    || !(CHARMSG(&(pqMsg->msg))->fs & KC_PREVDOWN )))
    {
      if( is_vio( pqMsg->hwnd ))
        process_chars( hab, pqMsg );
    }
  }

  if( pqMsg->msg == WM_KBDLAYERCHANGED )
  {
    HWND focus = WinQueryFocus( HWND_DESKTOP );
    if( focus == pqMsg->hwnd )
    {
      layer_id = WinQueryKbdLayer( focus );
      WinPostMsg( hviewer, KL_LAYERCHANGED, MPFROMLONG(layer_id), NULL );
    }
  }

  return FALSE;
}

/*--------------------------------------------------
 * Dispatch other keyboard events
 *--------------------------------------------------*/
BOOL EXPENTRY dispatch_posts( HAB hab, PQMSG pqMsg, ULONG )
{
  PTIB ptib;

  DosGetInfoBlocks( &ptib, NULL );
  if((PBYTE)&ptib - (PBYTE)ptib->tib_pstack < 1024 ) {
    return FALSE;
  } else {
    return process_posts( hab, pqMsg );
  }
}

/*--------------------------------------------------
 * Dispatch system lockup
 *--------------------------------------------------*/
BOOL EXPENTRY dispatch_lockup( HAB hab, HWND hwnd )
{
  hlockup = hwnd;
  return TRUE;
}

/*--------------------------------------------------
 * Process other windows messages
 *--------------------------------------------------*/
static void process_sends( HAB hab, PSMHSTRUCT psmh )
{
  // Prepare new window
  if( psmh->msg == WM_CREATE || psmh->msg == WM_INITDLG )
  {
    HWND  owner   = WinQueryWindow( psmh->hwnd, QW_OWNER   );
    HWND  parent  = WinQueryWindow( psmh->hwnd, QW_PARENT  );
    HWND  desktop = WinQueryDesktopWindow( hab, NULLHANDLE );

    if( parent == desktop && (owner == desktop || owner == 0))
    {
      if( get_default_layer() != KL_NONE )
        WinSetWindowULong( psmh->hwnd, QWL_KBDLAYER, get_default_layer());
    }
    else
      WinSetWindowULong( psmh->hwnd, QWL_KBDLAYER,
                         owner  ? WinQueryKbdLayer( owner  ) : 0 |
                         parent ? WinQueryKbdLayer( parent ) : 0 );
  }

  // Catch lockup window destroying
  if( psmh->msg == WM_DESTROY && psmh->hwnd == hlockup )
    hlockup = 0;

  // Make layer window floating to top
  if( psmh->msg  == WM_WINDOWPOSCHANGED &&
      psmh->hwnd != hviewer             &&
      is_float_on_top_enabled()         &&
      !hlockup
    )
  {
      SWP* swpWin = (PSWP)psmh->mp1;
      SWP  swpTst;

      WinQueryWindowPos( hviewer, &swpTst );

      if( swpTst.x < swpWin->x + swpWin->cx && swpWin->x < swpTst.x + swpTst.cx &&
          swpTst.y < swpWin->y + swpWin->cy && swpWin->y < swpTst.y + swpTst.cy  )
      {
        WinPostMsg( hviewer, KL_FLOATTOTOP, NULL, NULL );
      }

      return;
  }

  // Check layer state if window focus is changed
  if( psmh->msg == WM_FOCUSCHANGE && !SHORT1FROMMP(psmh->mp2))
  {
    SHORT detected = layer_id;
    PID   pid_vdm;
    HWND  focus = (HWND)psmh->mp1;

    if( focus )
    {
      if( is_vdm( focus, &pid_vdm ))
        detected = get_vdm_layer( pid_vdm );
      else
        detected = WinQueryKbdLayer(focus);
    }

    if( detected != layer_id )
    {
      layer_id = detected;
      WinPostMsg( hviewer, KL_LAYERCHANGED, MPFROMLONG(layer_id), NULL );
    }
  }
}

/*--------------------------------------------------
 * Dispatch other windows messages
 *--------------------------------------------------*/
void EXPENTRY dispatch_sends( HAB hab, PSMHSTRUCT psmh, BOOL )
{
  PTIB ptib;

  DosGetInfoBlocks( &ptib, NULL );
  if((PBYTE)&ptib - (PBYTE)ptib->tib_pstack >= 1024 ) {
    process_sends( hab, psmh );
  }
}

/*--------------------------------------------------
 * Dispatch VDM Events
 *--------------------------------------------------*/
static void _System dispatch_vdd()
{
  VLEVNT event;
  DosSetPriority( PRTYS_THREAD, PRTYC_TIMECRITICAL, PRTYD_MAXIMUM, 0 );

  for(;;)
  {
    if( DosRequestVDD( hvdd, 0, VLCMD_GETEVENT, 0, NULL, sizeof(event), &event ) != 0 )
      DosExit( EXIT_THREAD, 0 );

    switch( event.EventType )
    {
      case VLEVT_LAYERCHANGED:
      {
        layer_id = event.EventData;
        WinPostMsg( hviewer, KL_LAYERCHANGED, MPFROMLONG(layer_id), NULL );
        break;
      }

      case VLEVT_LAYOUTCHANGE:
      {
        if( event.EventData < hkey_size )
        {
          set_layout( hkey_list[event.EventData].layout );

          if( layer_layouts_id != KL_NONE && is_multilayered())
          {
            DosRequestVDD( hvdd, event.EventPID, VLCMD_SETLAYER,
                                 sizeof(layer_layouts_id), &layer_layouts_id, 0, NULL );
            beep_trigger ( layer_layouts_id );
          }
        }
      }
    }
  }
}

/*--------------------------------------------------
 * Dispatch requests to the keyboard layouts broker
 *--------------------------------------------------*/
static void _System dispatch_layouts()
{
  DosSetPriority( PRTYS_THREAD, PRTYC_TIMECRITICAL, PRTYD_MAXIMUM, 0 );

  for(;;)
  {
    ULONG pulPostCt;

    DosWaitEventSem ( lset_start, (ULONG)SEM_INDEFINITE_WAIT );
    DosResetEventSem( lset_start, &pulPostCt );

    switch( lset_query )
    {
      case LSET_LAYOUT:
        if( lset_param )
        {
          if( broker->select((const char*)lset_param ) == NO_ERROR )
             ini_put_chars( INI_LAYOUT, (const char*)lset_param );
          else
             DosBeep( 1000, 100 );

          WinPostMsg( hviewer, KL_SETLAYOUT, 0, 0 );
        }
        lset_rc = TRUE;
        break;

      case LSET_IS_MULTILAYERED:
      {
        lset_rc = broker->is_multilayered();
        break;
      }
    }

    DosPostEventSem( lset_done );
  }
}

/*--------------------------------------------------
 * Displays error message
 *--------------------------------------------------*/
void popup_error( const char* msg )
{
  WinMessageBox( HWND_DESKTOP, HWND_DESKTOP, msg,
                 PKG_NAME, 100, MB_OK | MB_ERROR | MB_MOVEABLE );
}

/*--------------------------------------------------
 * Startups the engine
 *--------------------------------------------------*/
BOOL EXPENTRY engine_start( HAB  hab,
                            HWND hwnd )
{
  if( hdll )
     return TRUE;

  hviewer = hwnd;
  broker  = new KLBroker();

  if( DosQueryModuleHandle( "LAYER3.DLL", &hdll ) != NO_ERROR )
  {
    popup_error( "Unable obtain the handle of a LAYER3.DLL.\n" );
    return FALSE;
  }

  if( !WinSetHook( hab, 0, HK_JOURNALRECORD, (PFN)dispatch_chars, hdll ))
  {
    popup_error( "Unable install the HK_JOURNALRECORD hook.\n" );
    return FALSE;
  }

  if( !WinSetHook( hab, 0, HK_INPUT,(PFN)dispatch_posts, hdll ))
  {
    popup_error( "Unable install the HK_INPUT hook.\n" );
    return FALSE;
  }

  if( !WinSetHook( hab, 0, HK_SENDMSG, (PFN)dispatch_sends, hdll ))
  {
    popup_error( "Unable install the HK_SENDMSG hook.\n" );
    return FALSE;
  }

  if( !WinSetHook( hab, 0, HK_LOCKUP, (PFN)dispatch_lockup, hdll ))
  {
    popup_error( "Unable install the HK_LOCKUP hook.\n" );
    return FALSE;
  }

  if( DosOpenVDD( "LAYER$", &hvdd ))
    hvdd = NULLHANDLE;
  else
  {
    VLVERS ver;

    if( DosRequestVDD( hvdd, 0, VLCMD_GETVERSION,
                       0, NULL, sizeof(ver), &ver ) == NO_ERROR

        && ver.major == VER_MAJOR
        && ver.minor == VER_MINOR
        && ver.level == VER_LEVEL )
    {
      DosCreateThread( &tvdd, (PFNTHREAD)&dispatch_vdd, 0,
                                          CREATE_READY, 12000L );
      BOOL enable = TRUE;
      DosRequestVDD( hvdd, 0, VLCMD_ENABLE, sizeof(enable), &enable, 0, NULL );
    }
    else
    {
      DosCloseVDD( hvdd );
      hvdd = NULLHANDLE;
      popup_error( "Incorrect VDD version. VDM support is disabled.\n" );
    }
  }

  if( DosCreateEventSem( NULL, &lset_start, DC_SEM_SHARED, FALSE ) != NO_ERROR )
  {
    popup_error( "Unable create the event semaphore.\n" );
    return FALSE;
  }

  if( DosCreateEventSem( NULL, &lset_done, DC_SEM_SHARED, FALSE ) != NO_ERROR )
  {
    popup_error( "Unable create the event semaphore.\n" );
    return FALSE;
  }

  if( DosCreateMutexSem( NULL, &lset_mutex, DC_SEM_SHARED, FALSE ) != NO_ERROR )
  {
    popup_error( "Unable create the mutex semaphore.\n" );
    return FALSE;
  }

  if( lset_start )
    DosCreateThread( &lset_tid, (PFNTHREAD)&dispatch_layouts, 0,
                                            CREATE_READY, 12000L );
  monitors_start();
  return TRUE;
}

/*--------------------------------------------------
 * Stops the engine
 *--------------------------------------------------*/
void EXPENTRY engine_stop( HAB hab )
{
  if( hdll && hab )
  {
    WinReleaseHook( hab, 0, HK_JOURNALRECORD, (PFN)dispatch_chars,  hdll );
    WinReleaseHook( hab, 0, HK_INPUT,         (PFN)dispatch_posts,  hdll );
    WinReleaseHook( hab, 0, HK_SENDMSG,       (PFN)dispatch_sends,  hdll );
    WinReleaseHook( hab, 0, HK_LOCKUP,        (PFN)dispatch_lockup, hdll );

    // This function releases the input hook and broadcasts a WM_NULL message
    // to all top level windows so that they will release the DLL.  If we don't
    // do this, the DLL will remain locked and we'll have to reboot in order to
    // recompile.

    WinBroadcastMsg( HWND_DESKTOP, WM_NULL, 0, 0, BMSG_FRAMEONLY | BMSG_POST );
    DosFreeModule( hdll );
    hdll = NULLHANDLE;
  }

  if( lset_tid )
  {
    DosKillThread( lset_tid );
    lset_tid = 0;
  }

  DosCloseEventSem( lset_start );
  lset_start = NULLHANDLE;
  DosCloseEventSem( lset_done  );
  lset_done  = NULLHANDLE;
  DosCloseMutexSem( lset_mutex );
  lset_mutex = NULLHANDLE;

  if( tvdd )
  {
    DosKillThread( tvdd );
    tvdd = 0;
  }

  if( hvdd )
  {
    BOOL enable = FALSE;
    DosRequestVDD( hvdd, 0, VLCMD_ENABLE, sizeof(enable), &enable, 0, NULL );
    DosCloseVDD  ( hvdd );
    hvdd = NULLHANDLE;
  }

  monitors_stop();
  delete broker;
}

