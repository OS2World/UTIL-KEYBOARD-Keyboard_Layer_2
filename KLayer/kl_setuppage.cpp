/* Keyboard Layer/2
 *
 * Setup page base class
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
 * $Id: kl_setuppage.cpp,v 1.5 2005/10/01 08:42:53 glass Exp $
 */

#define  INCL_WIN
#include <os2.h>

#include "kl_setuppage.h"
#include "kl_application.h"

/*---------------------------------------------------
 * Constructs the setup page
 *--------------------------------------------------*/
KLSetupPage::KLSetupPage( const KLSetup& parent, ULONG id )

: pg_parent( parent     ),
  pg_handle( NULLHANDLE )
{
  pg_handle = WinLoadDlg( parent.handle(),
                          parent.handle(), win_proc, NULLHANDLE, id, 0 );
  if( pg_handle ) {
    WinSetWindowULong ( pg_handle, QWL_USER, (ULONG)this );
    KLApp::do_warpsans( pg_handle );
  }
}

/*---------------------------------------------------
 * Destructs the setup page
 *--------------------------------------------------*/
KLSetupPage::~KLSetupPage()
{
  if( pg_handle )
    WinDestroyWindow( pg_handle );
}

/*---------------------------------------------------
 * Returns the page handle
 *--------------------------------------------------*/
HWND KLSetupPage::handle() const
{
  return pg_handle;
}

/*---------------------------------------------------
 * Returns the parent notebook
 *--------------------------------------------------*/
const KLSetup& KLSetupPage::parent() const
{
  return pg_parent;
}

/*---------------------------------------------------
 * Save page preferences
 *--------------------------------------------------*/
void KLSetupPage::save()
{}

/*---------------------------------------------------
 * Applies page preferences
 *--------------------------------------------------*/
void KLSetupPage::apply( HWND )
{}

/*---------------------------------------------------
 * Dispatch system native events
 *--------------------------------------------------*/
MRESULT KLSetupPage::dispatch( ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  switch ( msg )
  {
    case WM_COMMAND:
      if( SHORT1FROMMP(mp1) == DID_CANCEL )
        return MRFROMLONG(TRUE);

      break;
  }
  return WinDefDlgProc( handle(), msg, mp1, mp2 );
}

/*---------------------------------------------------
 * Process window messages
 *--------------------------------------------------*/
MRESULT _System KLSetupPage::win_proc( HWND hWnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  KLSetupPage* instance =
              (KLSetupPage*)WinQueryWindowULong( hWnd, QWL_USER );

  if( instance )
    return instance->dispatch(  msg, mp1, mp2 );
  else
    return WinDefDlgProc( hWnd, msg, mp1, mp2 );
}
