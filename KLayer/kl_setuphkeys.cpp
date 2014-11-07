/* Keyboard Layer/2
 *
 * Keyboard hotkey management class
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
 * $Id: kl_setuphkeys.cpp,v 1.3 2005/10/01 08:42:52 glass Exp $
 */

#define  INCL_WIN
#include <os2.h>

#include "kl_setuphkeys.h"
#include "kl_ids.h"
#include "kl_messages.h"
#include "kl_engine.h"
#include "kl_application.h"

/*--------------------------------------------------
 * Constructs the setup hotkey object
 *--------------------------------------------------*/
KLSetupHKeys::KLSetupHKeys()

: hk_scancode( 0          ),
  hk_state   ( 0          ),
  hk_exclude ( NULLHANDLE )
{}

/*---------------------------------------------------
 * Assigns the new hotkey
 *--------------------------------------------------*/
BOOL KLSetupHKeys::assign( HWND howner, KLHotkey* hkey )
{
  hk_assigned = hkey;

  return WinDlgBox( HWND_DESKTOP, howner, KLSetupHKeys::dlg_proc,
                                  NULLHANDLE, ID_WIN_KEYPROMPT, this );
}

/*---------------------------------------------------
 * Process dialog messages
 *--------------------------------------------------*/
MRESULT _System KLSetupHKeys::dlg_proc( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  switch ( msg )
  {
    case WM_INITDLG:
    {
      KLSetupHKeys* instance = (KLSetupHKeys*)mp2;

      SET_TEXT( hwnd, ID_KEY_TEXT, MSG_KEYPROMPT );
      KLApp::do_warpsans( hwnd, ID_KEY_TEXT );
      WinSetWindowULong( hwnd, QWL_USER, (ULONG)instance );
      instance->hk_exclude = hkey_exclude( hwnd );
      break;
    }

    case WM_DESTROY:
    {
      KLSetupHKeys* instance = (KLSetupHKeys*)WinQueryWindowULong( hwnd, QWL_USER );

      hkey_exclude( instance->hk_exclude );
      instance->hk_exclude = NULLHANDLE;
    }

    case WM_COMMAND:
      if( SHORT1FROMMP(mp1) == DID_CANCEL )
        WinDismissDlg( hwnd, FALSE );

      return MRFROMLONG(TRUE);

    case WM_CLOSE:
      WinDismissDlg( hwnd, FALSE );
      return MRFROMLONG(TRUE);

    case WM_CHAR:
      if( !( CHARMSG(&msg)->fs & KC_KEYUP )
          && CHARMSG(&msg)->vkey != VK_NUMLOCK
          && CHARMSG(&msg)->vkey != VK_ESC
          && CHARMSG(&msg)->vkey != VK_PRINTSCRN
          && CHARMSG(&msg)->vkey != VK_PAUSE
          && CHARMSG(&msg)->vkey != VK_SHIFT
          && CHARMSG(&msg)->vkey != VK_CTRL
          && CHARMSG(&msg)->vkey != VK_ALT
          && CHARMSG(&msg)->vkey != VK_CAPSLOCK
          && CHARMSG(&msg)->vkey != VK_SCRLLOCK
          && CHARMSG(&msg)->scancode
        )
      {
        KLSetupHKeys* instance = (KLSetupHKeys*)WinQueryWindowULong( hwnd, QWL_USER );

        if( instance )
        {
          instance->hk_state    = 0;
          instance->hk_scancode = CHARMSG(&msg)->scancode;

          if( CHARMSG(&msg)->fs & KC_CTRL  ) instance->hk_state |= KLHotkey::control;
          if( CHARMSG(&msg)->fs & KC_ALT   ) instance->hk_state |= KLHotkey::alt;
          if( CHARMSG(&msg)->fs & KC_SHIFT ) instance->hk_state |= KLHotkey::shift;

          instance->hk_assigned->assign( instance->hk_scancode, instance->hk_state );
        }

        WinDismissDlg( hwnd, TRUE );
        return MRFROMLONG(TRUE);
      }
      break;
  }

  return WinDefDlgProc( hwnd, msg, mp1, mp2 );
}

