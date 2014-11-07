/* Keyboard Layer/2
 *
 * Setup notebook
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
 * $Id$
 */

#define  INCL_WIN
#include <os2.h>

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "kl_setup.h"
#include "kl_setuplayer_p1.h"
#include "kl_setuplayer_p2.h"
#include "kl_setuplayouts_p1.h"
#include "kl_setuplayouts_p2.h"
#include "kl_setuplanguage.h"
#include "kl_setupabout.h"
#include "kl_messages.h"
#include "kl_engine.h"
#include "kl_ids.h"
#include "kl_application.h"

/*---------------------------------------------------
 * Constructs the setup notebook
 *--------------------------------------------------*/
KLSetup::KLSetup( HWND howner )

: nb_viewer ( howner     ),
  nb_dialog ( NULLHANDLE ),
  nb_handle ( NULLHANDLE ),
  nb_ok     ( NULLHANDLE ),
  nb_default( NULLHANDLE ),
  nb_cancel ( NULLHANDLE )
{
  MRESULT id;
  char    status[256];

  nb_dialog = WinLoadDlg( HWND_DESKTOP, howner, win_proc,
                          NULLHANDLE, ID_WIN_NOTEBOOK, 0 );
  if( nb_dialog )
  {
    WinSetWindowULong( nb_dialog, QWL_USER, (ULONG)this );
    KLApp::do_warpsans( nb_dialog );

    SET_TEXT( nb_dialog, DID_OK        , MSG_OK      );
    SET_TEXT( nb_dialog, ID_NBK_DEFAULT, MSG_DEFAULT );
    SET_TEXT( nb_dialog, DID_CANCEL    , MSG_CANCEL  );

    nb_handle  = WinWindowFromID( nb_dialog, ID_NBK_SETUP   );
    nb_ok      = WinWindowFromID( nb_dialog, DID_OK         );
    nb_default = WinWindowFromID( nb_dialog, ID_NBK_DEFAULT );
    nb_cancel  = WinWindowFromID( nb_dialog, DID_CANCEL     );

    WinSendMsg( nb_handle, BKM_SETDIMENSIONS, MPFROM2SHORT(100,25), MPFROMSHORT(BKA_MAJORTAB));
    WinSendMsg( nb_handle, BKM_SETDIMENSIONS, MPFROMLONG(0), MPFROMSHORT(BKA_MINORTAB));
    WinSendMsg( nb_handle, BKM_SETNOTEBOOKCOLORS, MPFROMLONG(SYSCLR_FIELDBACKGROUND),
                MPFROMSHORT(BKA_BACKGROUNDPAGECOLORINDEX));

    nb_layer01 = new KLSetupLayer01( *this );

    sprintf( status, MSG_PAGEOF, 1, 2 );
    id = WinSendMsg( nb_handle, BKM_INSERTPAGE, 0,
                     MPFROM2SHORT( BKA_AUTOPAGESIZE | BKA_MAJOR | BKA_STATUSTEXTON, BKA_LAST ));

    WinSendMsg ( nb_handle, BKM_SETTABTEXT, MPFROMLONG(id), MPFROMP(MSG_SETUP));
    WinSendMsg ( nb_handle, BKM_SETSTATUSLINETEXT, MPFROMLONG(id), MPFROMP(status));
    WinSendMsg ( nb_handle, BKM_SETPAGEWINDOWHWND, MPFROMLONG(id), MPFROMHWND(nb_layer01->handle()));
    WinSetOwner( nb_layer01->handle(), nb_handle );

    nb_layer02 = new KLSetupLayer02( *this );

    sprintf( status, MSG_PAGEOF, 2, 2 );
    id = WinSendMsg( nb_handle, BKM_INSERTPAGE, 0,
                     MPFROM2SHORT( BKA_AUTOPAGESIZE | BKA_MINOR | BKA_STATUSTEXTON, BKA_LAST ));

    WinSendMsg ( nb_handle, BKM_SETTABTEXT, MPFROMLONG(id), MPFROMP(status));
    WinSendMsg ( nb_handle, BKM_SETSTATUSLINETEXT, MPFROMLONG(id), MPFROMP(status));
    WinSendMsg ( nb_handle, BKM_SETPAGEWINDOWHWND, MPFROMLONG(id), MPFROMHWND(nb_layer02->handle()));
    WinSetOwner( nb_layer02->handle(), nb_handle );

    nb_layout01 = new KLSetupLayouts01( *this );

    sprintf( status, MSG_PAGEOF, 1, 2 );
    id = WinSendMsg( nb_handle, BKM_INSERTPAGE, 0,
                     MPFROM2SHORT( BKA_AUTOPAGESIZE | BKA_MAJOR | BKA_STATUSTEXTON, BKA_LAST ));

    WinSendMsg ( nb_handle, BKM_SETTABTEXT, MPFROMLONG(id), MPFROMP(MSG_LAYOUTS));
    WinSendMsg ( nb_handle, BKM_SETSTATUSLINETEXT, MPFROMLONG(id), MPFROMP(status));
    WinSendMsg ( nb_handle, BKM_SETPAGEWINDOWHWND, MPFROMLONG(id), MPFROMHWND(nb_layout01->handle()));
    WinSetOwner( nb_layout01->handle(), nb_handle );

    nb_layout02 = new KLSetupLayouts02( *this );

    sprintf( status, MSG_PAGEOF, 2, 2 );
    id = WinSendMsg( nb_handle, BKM_INSERTPAGE, 0,
                     MPFROM2SHORT( BKA_AUTOPAGESIZE | BKA_MINOR | BKA_STATUSTEXTON, BKA_LAST ));

    WinSendMsg ( nb_handle, BKM_SETTABTEXT, MPFROMLONG(id), MPFROMP(status));
    WinSendMsg ( nb_handle, BKM_SETSTATUSLINETEXT, MPFROMLONG(id), MPFROMP(status));
    WinSendMsg ( nb_handle, BKM_SETPAGEWINDOWHWND, MPFROMLONG(id), MPFROMHWND(nb_layout02->handle()));
    WinSetOwner( nb_layout02->handle(), nb_handle );

    nb_language = new KLSetupLanguage( *this );

    id = WinSendMsg( nb_handle, BKM_INSERTPAGE, 0,
                     MPFROM2SHORT( BKA_AUTOPAGESIZE | BKA_MAJOR, BKA_LAST ));

    WinSendMsg ( nb_handle, BKM_SETTABTEXT, MPFROMLONG(id), MPFROMP(MSG_LANGUAGE));
    WinSendMsg ( nb_handle, BKM_SETPAGEWINDOWHWND, MPFROMLONG(id), MPFROMHWND(nb_language->handle()));
    WinSetOwner( nb_language->handle(), nb_handle );

    nb_about = new KLSetupAbout( *this );

    id = WinSendMsg( nb_handle, BKM_INSERTPAGE, 0,
                     MPFROM2SHORT( BKA_AUTOPAGESIZE | BKA_MAJOR, BKA_LAST ));

    WinSendMsg ( nb_handle, BKM_SETTABTEXT, MPFROMLONG(id), MPFROMP(MSG_ABOUT));
    WinSendMsg ( nb_handle, BKM_SETPAGEWINDOWHWND, MPFROMLONG(id), MPFROMHWND(nb_about->handle()));
    WinSetOwner( nb_about->handle(), nb_handle );

    ini_get_window( INI_SETUPPOS, nb_dialog );
    WinSetFocus( HWND_DESKTOP, nb_handle );
  }
}

/*---------------------------------------------------
 * Destructs the setup notebook
 *--------------------------------------------------*/
KLSetup::~KLSetup()
{
  if( nb_dialog )
  {
    delete nb_layer01;
    delete nb_layer02;
    delete nb_layout01;
    delete nb_layout02;
    delete nb_language;
    delete nb_about;

    WinDestroyWindow( nb_dialog );
  }
}

/*---------------------------------------------------
 * Displays the setup notebook
 *--------------------------------------------------*/
ULONG KLSetup::show_modally()
{
  QMSG qms;

  if( nb_dialog ) {
    while( WinGetDlgMsg( nb_dialog, &qms )) {
      if( qms.msg == WM_CONTEXTMENU && qms.hwnd == nb_dialog ) {
        // It is necessary to redirect the WM_CONTEXTMENU to active window,
        // otherwise the context menu in the containers will not work.
        HWND focus = WinQueryFocus( HWND_DESKTOP );
        if( focus ) {
          WinPostMsg( focus, qms.msg, qms.mp1, qms.mp2 );
          continue;
        }
      }
      dispatch( qms.msg, qms.mp1, qms.mp2 );
    }
  }
  return 0;
}

/*---------------------------------------------------
 * Apply preferences
 *--------------------------------------------------*/
void KLSetup::apply( HWND hwnd )
{
  KLSetupLayer01  ::apply( hwnd );
  KLSetupLayer02  ::apply( hwnd );
  KLSetupLayouts01::apply( hwnd );
  KLSetupLayouts02::apply( hwnd );
  KLSetupLanguage ::apply( hwnd );
  KLSetupAbout    ::apply( hwnd );
}

/*---------------------------------------------------
 * Dispatch system native events
 *--------------------------------------------------*/
MRESULT KLSetup::dispatch( ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  switch ( msg )
  {
    case WM_DESTROY:
      ini_put_window( INI_SETUPPOS, nb_dialog );
      break;

    case WM_WINDOWPOSCHANGED:
    {
      PSWP   pswp = (PSWP)mp1;
      RECTL  rect;
      POINTL pos[2];

      if( pswp[0].fl & SWP_SIZE )
      {
        WinEnableWindowUpdate( nb_dialog, FALSE );

        // Resizes notebook window.
        if( WinQueryWindowRect( nb_dialog, &rect ))
        {
          WinCalcFrameRect( nb_dialog, &rect, TRUE );
          pos[0].x = rect.xLeft;
          pos[0].y = rect.yBottom;
          pos[1].x = rect.xRight;
          pos[1].y = rect.yTop;
          WinMapDlgPoints( nb_dialog, pos, 2, FALSE );
          pos[0].y += 26;
          WinMapDlgPoints( nb_dialog, pos, 2, TRUE  );
          WinSetWindowPos( nb_handle, 0, pos[0].x, pos[0].y,
                           pos[1].x-pos[0].x, pos[1].y-pos[0].y, SWP_MOVE | SWP_SIZE );
        }

        SWP  swps[3];
        HWND hbtn[3] = { nb_ok, nb_default, nb_cancel };
        LONG ibtn    = sizeof(hbtn)/sizeof(HWND)-1;

        // Centers notebook buttons.
        if( WinQueryWindowPos( hbtn[   0], &swps[   0]) &&
            WinQueryWindowPos( hbtn[ibtn], &swps[ibtn]))
        {
          LONG move = (pswp[0].cx - (swps[ibtn].x + swps[ibtn].cx - swps[0].x))/2 - swps[0].x;

          for( LONG i = 0; i <= ibtn; i++ )
            if( WinQueryWindowPos( hbtn[i], &swps[i] ))
              WinSetWindowPos( hbtn[i], 0, swps[i].x + move, swps[i].y, 0, 0, SWP_MOVE );
        }

        WinEnableWindowUpdate( nb_dialog, TRUE );
      }
      break;
    }

    case WM_COMMAND:
      switch( SHORT1FROMMP(mp1))
      {
        case DID_OK:
          nb_layer01 ->save();
          nb_layer02 ->save();
          nb_layout01->save();
          nb_layout02->save();
          nb_language->save();
          nb_about   ->save();

          apply( owner());
          break;

        case ID_NBK_DEFAULT:
        {
          LONG id;
          HWND hwnd;

          id = (LONG)WinSendMsg( nb_handle, BKM_QUERYPAGEID, 0,
                                 MPFROM2SHORT(BKA_TOP,BKA_MAJOR));

          if( id && id != BOOKERR_INVALID_PARAMETERS )
              hwnd = (HWND)WinSendMsg( nb_handle,
                                       BKM_QUERYPAGEWINDOWHWND, MPFROMLONG(id), 0 );

          if( hwnd && hwnd != BOOKERR_INVALID_PARAMETERS )
            WinSendMsg( hwnd, KL_SETDEFAULT, 0, 0 );

          return MRFROMLONG(1L);
        }
      }
      break;
  }
  return WinDefDlgProc( nb_dialog, msg, mp1, mp2 );
}

/*---------------------------------------------------
 * Process window messages
 *--------------------------------------------------*/
MRESULT _System KLSetup::win_proc( HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  KLSetup* instance = (KLSetup*)WinQueryWindowULong( hWnd, QWL_USER );

  if( instance )
    return instance->dispatch(  msg, mp1, mp2 );
  else
    return WinDefDlgProc( hWnd, msg, mp1, mp2 );
}

