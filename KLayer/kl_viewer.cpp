/* Keyboard Layer/2
 *
 * Viewer
 * Copyright (C) 2004 by Dmitry A.Steklenev
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
 * $Id: kl_viewer.cpp,v 1.16 2005/12/12 06:29:14 glass Exp $
 */

#define  INCL_WIN
#define  INCL_GPI
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>

#include "kl_viewer.h"
#include "kl_engine.h"
#include "kl_systray.h"
#include "kl_pipe.h"
#include "kl_setup.h"
#include "kl_ids.h"

ULONG KLViewer::v_initialized = 0;

#define KL_TRANSITFOCUS (KL_USER+1)

/*--------------------------------------------------
 * Constructs the keyboard layer viewer
 *--------------------------------------------------*/
KLViewer::KLViewer( HAB hab )

: v_hviewer   ( NULLHANDLE ),
  v_hclient   ( NULLHANDLE ),
  v_popup_full( NULL       ),
  v_popup_mini( NULL       ),
  v_bundle    ( v_broker   ),
  v_has_focus ( FALSE      ),
  v_layer_id  ( KL_LATIN   ),
  v_remove    ( FALSE      ),
  v_float_top ( TRUE       ),
  v_transit   ( TRUE       ),
  v_showicon  ( TRUE       )
{
  if( !v_initialized++ )
    if( !WinRegisterClass( hab, PKG_NAME, KLViewer::win_proc, 0, sizeof(ULONG)))
    {
      WinMessageBox( HWND_DESKTOP, HWND_DESKTOP,
                     "Unable register the keyboard layer window class.",
                     PKG_NAME, 100,
                     MB_OK | MB_ERROR | MB_APPLMODAL | MB_MOVEABLE );

      exit( LAYER_EXIT_INITERROR );
    }

  ULONG flCreateFlags = FCF_ICON        |
                        FCF_NOBYTEALIGN |
                        FCF_TASKLIST    |
                        FCF_BORDER;

  v_hviewer = WinCreateStdWindow( HWND_DESKTOP,
                                  WS_VISIBLE | WS_SAVEBITS,
                                  &flCreateFlags,
                                  PKG_NAME,
                                  PKG_NAME,
                                  0, 0,
                                  ID_WIN_INDICATOR,
                                  &v_hclient );
  if( !v_hviewer || !v_hclient )
    exit( LAYER_EXIT_INITERROR );

  WinSetWindowULong( v_hclient, QWL_USER, (ULONG)this );

  v_popup_full = new KLPopUp( handle(), ID_MNU_FULL );
  v_popup_mini = new KLPopUp( handle(), ID_MNU_MINI );
}

/*--------------------------------------------------
 * Destructs the keyboard layer viewer
 *--------------------------------------------------*/
KLViewer::~KLViewer()
{
  KLSysTray::disable( handle());
  KLPipe::disable();

  delete v_popup_full;
  delete v_popup_mini;
}

/*--------------------------------------------------
 * Capture mouse pointer
 *-------------------------------------------------*/
KLViewer& KLViewer::capture_pointer( BOOL capture )
{
  WinSetCapture( HWND_DESKTOP, capture ? client() : NULLHANDLE );
  return *this;
}

/*--------------------------------------------------
 * Release mouse pointer
 *-------------------------------------------------*/
KLViewer& KLViewer::release_pointer()
{
  return capture_pointer( FALSE );
}

/*--------------------------------------------------
 * Has pointer captured
 *-------------------------------------------------*/
BOOL KLViewer::has_pointer_captured() const
{
  return (WinQueryCapture( HWND_DESKTOP ) == client());
}

/*--------------------------------------------------
 * Makes the window visible
 *--------------------------------------------------*/
KLViewer& KLViewer::show( BOOL show )
{
  WinShowWindow( handle(), show );
  return *this;
}

/*--------------------------------------------------
 * Is the window visible
 *-------------------------------------------------*/
BOOL KLViewer::is_visible() const
{
  return WinIsWindowVisible( handle());
}

/*--------------------------------------------------
 * Dispatch paint event
 *--------------------------------------------------*/
void KLViewer::paint( HWND hwnd )
{
  RECTL rec;
  SWP   swp;
  HPS   hps = WinBeginPaint( hwnd, NULL, NULL );

  LONG  bg_color = v_bundle.background();
  LONG  fg_color = v_bundle.foreground();
  LONG  cx_icon  = WinQuerySysValue( HWND_DESKTOP, SV_CXICON ) / 2;
  LONG  cy_icon  = WinQuerySysValue( HWND_DESKTOP, SV_CYICON ) / 2;

  GpiCreateLogColorTable( hps, 0, LCOLF_RGB, 0, 0, 0 );
  WinQueryWindowPos( hwnd, &swp );

  rec.xLeft   = 0;
  rec.yBottom = 0;
  rec.xRight  = swp.cx;
  rec.yTop    = swp.cy;

  WinFillRect  ( hps, &rec, bg_color );

  if( v_showicon )
  {
    rec.xLeft = cx_icon + 5;
    WinDrawText( hps, -1L, (PCH)v_bundle.name(), &rec, fg_color, bg_color, DT_LEFT | DT_VCENTER );
    WinDrawPointer( hps, 3, ( swp.cy  - cy_icon ) / 2 + 1, v_bundle.icon(), DP_MINI );
  }
  else
  {
    WinDrawText( hps, -1L, (PCH)v_bundle.layer_name(), &rec, fg_color, bg_color, DT_CENTER | DT_VCENTER );
  }

  if( has_focus())
  {
    POINTL pos;

    GpiSetLineWidth( hps, LINEWIDTH_NORMAL   );
    GpiSetLineType ( hps, LINETYPE_ALTERNATE );
    GpiSetColor    ( hps, fg_color );

    pos.y = 1;
    pos.x = 1;
    GpiMove( hps, &pos );
    pos.x = swp.cx - 2;
    pos.y = swp.cy - 2;
    GpiBox( hps, DRO_OUTLINE, &pos, 0, 0 );
  }

  WinEndPaint( hps );
}

/*--------------------------------------------------
 * Adds the whole viewer window to a update region
 *--------------------------------------------------*/
KLViewer& KLViewer::invalidate()
{
  WinInvalidateRect( handle(), NULL, TRUE );
  return *this;
}

/*--------------------------------------------------
 * Adjusts the viewer window size
 *--------------------------------------------------*/
void KLViewer::adjust_size()
{
  HPS   hps     = WinGetPS( client());
  RECTL rect    = { 0, 0, 32000, 32000 };
  LONG  cx_icon = WinQuerySysValue( HWND_DESKTOP, SV_CXICON ) / 2;
  LONG  cy_icon = WinQuerySysValue( HWND_DESKTOP, SV_CYICON ) / 2 * 0.6;
  LONG  cx_view = 0;
  LONG  cy_view = 0;

  WinDrawText ( hps, -1L, "XXXXX ", &rect, 0L, 0L, DT_QUERYEXTENT | DT_LEFT );
  WinReleasePS( hps );

  cx_view = rect.xRight - rect.xLeft   + 1;
  cy_view = rect.yTop   - rect.yBottom + 1;

  if( v_showicon ) {
    cy_view = max( cy_view, cy_icon );
    cx_view = cx_icon + cx_view + 3;
  }

  cx_view += 2;
  cy_view += 4;

  WinSetWindowPos( handle(), 0, 0, 0, cx_view, cy_view, SWP_SIZE );
}

/*---------------------------------------------------
 * Synchronize viewer state with the current layout
 *--------------------------------------------------*/
void KLViewer::synchronize()
{
  v_bundle.select( v_broker, v_layer_id );
  KLSysTray::set_icon( handle(), v_bundle.icon());
  KLPipe::write( v_bundle.layer_name());
  invalidate();
}

/*--------------------------------------------------
 * Removes the viewer entry from the window list
 *--------------------------------------------------*/
KLViewer& KLViewer::remove_entry( BOOL remove )
{
  SWCNTRL swcntrl;
  HSWITCH hswitch = WinQuerySwitchHandle( handle(), 0 );

  WinQuerySwitchEntry ( hswitch, &swcntrl );
  swcntrl.uchVisibility = remove ? SWL_INVISIBLE : SWL_VISIBLE;
  WinChangeSwitchEntry( hswitch, &swcntrl );

  v_popup_full->check ( ID_MNU_TASKLIST,  remove );
  v_popup_full->enable( ID_MNU_HIDE,     !remove );

  v_remove = remove;
  return *this;
}

/*--------------------------------------------------
 * Forces the viewer window to float on top
 *--------------------------------------------------*/
KLViewer& KLViewer::float_on_top( BOOL ontop )
{
  set_float_on_top( ontop );

  v_popup_full->check ( ID_MNU_FLOAT,    ontop );
  v_popup_full->enable( ID_MNU_TRANSPFC, ontop );

  v_float_top = ontop;
  return *this;
}

/*--------------------------------------------------
 * Forces the viewer window to transit input focus
 *--------------------------------------------------*/
KLViewer& KLViewer::transit_focus( BOOL transit )
{
  v_popup_full->check( ID_MNU_TRANSPFC, transit );
  v_transit = transit;
  return *this;
}

/*--------------------------------------------------
 * Forces the viewer window to show the layer icon
 *--------------------------------------------------*/
KLViewer& KLViewer::show_icon( BOOL show )
{
  v_popup_full->check( ID_MNU_SHOWICON, show );
  v_showicon = show;
  return *this;
}

/*--------------------------------------------------
 * Saves the current state of the viewer window
 *--------------------------------------------------*/
void KLViewer::save()
{
  ini_put_window( INI_POS,        v_hviewer   );
  ini_put_long  ( INI_REMOVE,     v_remove    );
  ini_put_long  ( INI_FLOATONTOP, v_float_top );
  ini_put_long  ( INI_TRANSPFC,   v_transit   );
  ini_put_long  ( INI_SHOWICON,   v_showicon  );
}

/*--------------------------------------------------
 * Restores the current state of the viewer window
 *--------------------------------------------------*/
void KLViewer::load()
{
  remove_entry ( ini_get_long( INI_REMOVE,     FALSE ));
  float_on_top ( ini_get_long( INI_FLOATONTOP, TRUE  ));
  transit_focus( ini_get_long( INI_TRANSPFC,   TRUE  ));
  show_icon    ( ini_get_long( INI_SHOWICON,   TRUE  ));

  if( !ini_get_window( INI_POS, handle()))
  {
    WinSetPresParam( client(), PP_FONTNAMESIZE, 16, "9.WarpSans Bold" );

    SWP swp;
    WinQueryWindowPos( handle(), &swp );
    WinSetWindowPos  ( handle(), HWND_TOP,
                       WinQuerySysValue( HWND_DESKTOP, SV_CXSCREEN ) - swp.cx,
                       WinQuerySysValue( HWND_DESKTOP, SV_CYSCREEN ) - swp.cy,
                       0, 0, SWP_MOVE );
  }
  else
  {
    if( ini_get_long( INI_REMOVE, FALSE ) == TRUE )
      WinSetWindowPos( handle(), HWND_TOP, 0,0,0,0, SWP_SHOW );
  }

  adjust_size();

  if( v_float_top )
    WinSetWindowPos( handle(), HWND_TOP, 0, 0, 0, 0, SWP_ZORDER );
}

/*--------------------------------------------------
 * Dispatch system native events
 *--------------------------------------------------*/
MRESULT KLViewer::dispatch( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  static in_setup = FALSE;

  if( KLSysTray::dispatch( hwnd, msg, mp1, mp2 ))
    return MRFROMLONG(TRUE);

  switch( msg )
  {
    case WM_PAINT:
      paint( hwnd );
      return MPFROMLONG(TRUE);

    case WM_PRESPARAMCHANGED:
    {
      if( LONGFROMMP(mp1) == PP_FOREGROUNDCOLOR )
      {
        LONG fg_color = v_bundle.foreground();

        WinQueryPresParam( hwnd, PP_FOREGROUNDCOLOR, 0, 0, sizeof(fg_color), &fg_color, QPF_NOINHERIT );
        v_bundle.set_foreground( fg_color );
      }

      if( LONGFROMMP(mp1) == PP_BACKGROUNDCOLOR )
      {
        LONG bg_color = v_bundle.background();

        WinQueryPresParam( hwnd, PP_BACKGROUNDCOLOR, 0, 0, sizeof(bg_color), &bg_color, QPF_NOINHERIT );
        v_bundle.set_background( bg_color );
        set_vdm_color( v_bundle.vdm_border());
      }

      if( LONGFROMMP(mp1) == PP_FONTNAMESIZE )
        adjust_size();

      invalidate();
      save();
      break;
    }

    case KL_FLOATTOTOP:
      if( v_float_top )
        WinSetWindowPos( handle(), HWND_TOP, 0, 0, 0, 0, SWP_ZORDER );

      return MPFROMLONG( TRUE );

    case WM_SETFOCUS:
      if((v_has_focus = LONGFROMMP(mp2)) == TRUE && v_float_top && v_transit )
        WinPostMsg( hwnd, KL_TRANSITFOCUS, mp1, mp2 );
      else
        invalidate();

      break;

    case KL_TRANSITFOCUS:
    {
      BOOL hlose = LONGFROMMP(mp1);

      if( hlose && hlose != v_popup_full->handle() && !WinIsWindowShowing( hlose ))
      {
        HWND hnext = WinQueryWindow( handle(), QW_NEXTTOP );

        if( WinIsWindowShowing( hnext ))
            WinSetWindowPos( hnext, 0, 0, 0, 0, 0, SWP_ACTIVATE );
      }

      invalidate();
      return MPFROMLONG( TRUE );
    }

    case KL_SETLAYOUT:
      synchronize();
      set_vdm_color( v_bundle.vdm_border());
      return MPFROMLONG(TRUE);

    case KL_LAYERCHANGED:
      v_layer_id = LONGFROMMP(mp1);
      synchronize();
      return MPFROMLONG(TRUE);

    case WM_BUTTON2CLICK | 0x2000:
    {
      POINTL pos;
      WinQueryPointerPos( HWND_DESKTOP, &pos );
      WinMapWindowPoints( HWND_DESKTOP, hwnd , &pos, 1 );
      v_popup_mini->show( pos.x, pos.y );
      return MPFROMLONG(TRUE);
    }

    case WM_CONTEXTMENU:
      v_popup_full->show( SHORT1FROMMP(mp1), SHORT2FROMMP(mp1));
      return MPFROMLONG(TRUE);

    case WM_BUTTON1DOWN:
      WinQueryPointerPos( HWND_DESKTOP, &v_mouse_pos );
      WinSetWindowPos( handle(), HWND_TOP, 0,0,0,0, SWP_ZORDER );
      capture_pointer();
      break;

    case WM_BUTTON1UP:
      release_pointer();
      save();
      break;

    case WM_MOUSEMOVE:
      if( has_pointer_captured())
      {
        SWP    swp;
        POINTL pos;
        WinQueryPointerPos( HWND_DESKTOP, &pos );

        if( v_mouse_pos.x != pos.x || v_mouse_pos.y != pos.y )
        {
          WinQueryWindowPos( handle(), &swp );
          WinSetWindowPos  ( handle(), HWND_TOP, swp.x + pos.x - v_mouse_pos.x,
                                                 swp.y + pos.y - v_mouse_pos.y, 0, 0, SWP_MOVE );
          v_mouse_pos = pos;
        }
      }
      break;

    case WM_COMMAND:
      switch( LONGFROMMP(mp1))
      {
        case ID_MNU_CLOSE:
          WinPostMsg( handle(), WM_QUIT, 0, 0 );
          break;

        case ID_MNU_HIDE:
          show( FALSE );
          save();
          break;

        case ID_MNU_TASKLIST:
          remove_entry( !v_remove );
          save();
          break;

        case ID_MNU_FLOAT:
          float_on_top( !v_float_top );
          save();
          break;

        case ID_MNU_TRANSPFC:
          transit_focus( !v_transit );
          save();
          break;

        case ID_MNU_SHOWICON:
          show_icon( !v_showicon );
          adjust_size();
          invalidate();
          save();
          break;

        case ID_MNU_SETUP:
          if( !in_setup )
          {
            BOOL save_transit = v_transit;
            in_setup = TRUE;

            if( v_has_focus )
              transit_focus( FALSE );

            KLSetup( handle()).show_modally();
            transit_focus( save_transit );
            in_setup = FALSE;
          }
          break;
      }

      // FIX ME!!! v_popup_full or v_popup_mini?
      if( LONGFROMMP(mp1) >= ID_LAY_FIRST )
         set_layout( v_popup_full->selected(LONGFROMMP(mp1)));

      return MPFROMLONG(TRUE);
  }

  return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

/*---------------------------------------------------
 * Process window messages
 *--------------------------------------------------*/
MRESULT _System KLViewer::win_proc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  KLViewer* instance =
            (KLViewer*)WinQueryWindowULong( hwnd, QWL_USER );

  if( instance )
    return instance->dispatch( hwnd, msg, mp1, mp2 );
  else
    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}

/*---------------------------------------------------
 * Find the first instance of the viewer window
 *--------------------------------------------------*/
HWND KLViewer::find()
{
  HENUM hEnum;
  HWND  hTop;
  HWND  hClient;
  CHAR  cClass[256];

  hEnum = WinBeginEnumWindows( HWND_DESKTOP );
  hTop  = WinGetNextWindow( hEnum );

  while( hTop != NULLHANDLE )
  {
    if(( hClient = WinWindowFromID( hTop, FID_CLIENT )) != NULLHANDLE )
    {
      if( WinQueryClassName( hClient, sizeof(cClass), cClass ))
        if( strcmp( cClass, PKG_NAME ) == 0 )
        {
          WinEndEnumWindows( hEnum );
          return hClient;
        }
    }
    hTop = WinGetNextWindow( hEnum );
  }

  WinEndEnumWindows( hEnum );
  return NULLHANDLE;
}

