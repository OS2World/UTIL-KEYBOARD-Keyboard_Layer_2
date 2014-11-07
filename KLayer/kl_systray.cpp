/* Keyboard Layer/2
 *
 * Supports of the SysTray/WPS
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
 * $Id$
 */

#define  INCL_WIN
#include <os2.h>
#include <string.h>

#include "kl_systray.h"

#define __IMPLEMENTATION__
#include <trayapi.h>

HPOINTER KLSysTray::sys_deficon = NULLHANDLE;
HWND     KLSysTray::sys_hviewer = NULLHANDLE;
BOOL     KLSysTray::sys_enabled = FALSE;

/*--------------------------------------------------
 * Enables support of the SysTray/WPS
 *--------------------------------------------------*/
void KLSysTray::enable( HWND hwnd, BOOL enable )
{
  if( enable )
  {
    if( !sys_enabled )
    {
      if( sys_deficon == NULLHANDLE )
          sys_deficon = (HPOINTER)WinSendMsg( hwnd, WM_QUERYICON, 0, 0 );

      InitializeTrayApi( hwnd );
      sys_hviewer = hwnd;
    }

    sys_enabled = TRUE;
  }
  else
    disable( hwnd );

}

/*--------------------------------------------------
 * Disables support of the SysTray/WPS
 *--------------------------------------------------*/
void KLSysTray::disable( HWND hwnd )
{
  if( sys_enabled )
  {
    DeleteTrayIcon( WinWindowFromID( hwnd, FID_CLIENT ));

    if( sys_deficon != NULLHANDLE && sys_hviewer == hwnd )
    {
      WinSendMsg( hwnd, WM_SETICON, (MPARAM)sys_deficon, 0 );
      sys_deficon = NULLHANDLE;
    }
  }

  sys_enabled = FALSE;
}

/*--------------------------------------------------
 * Sets the icon of the layer viewer
 *--------------------------------------------------*/
void KLSysTray::set_icon( HWND hwnd, HPOINTER hicon )
{
  if( sys_enabled )
  {
    WinSendMsg( hwnd, WM_SETICON, (MPARAM)hicon, 0 );
    ChangeTrayIcon( WinWindowFromID( hwnd, FID_CLIENT ), 0 );
  }
}

/*--------------------------------------------------
 * Dispatch the system native events
 *--------------------------------------------------*/
MRESULT KLSysTray::dispatch( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  switch( msg )
  {
    case WM_DDE_INITIATEACK:
    {
      DDEINIT* ddeinit = (DDEINIT*)mp2;

      if( strcmp( ddeinit->pszAppName, SZAPP   ) == 0 &&
          strcmp( ddeinit->pszTopic  , SZTOPIC ) == 0 )
      {
        AnswerTrayApiDdeAck( mp1 );
        AddTrayIcon( hwnd, 0 );
        return MRFROMLONG(TRUE);
      }
    }
  }

  return MRFROMLONG(FALSE);
}

