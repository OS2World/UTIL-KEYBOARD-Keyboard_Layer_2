/* Keyboard Layer/2
 *
 * Application class
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
 * $Id: kl_application.cpp,v 1.4 2005/10/01 08:42:52 glass Exp $
 */

#define  INCL_WIN
#define  INCL_GPI
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kl_application.h"

/*--------------------------------------------------
 * Initializes the GUI facilities
 *--------------------------------------------------*/
KLApp::KLApp()

: app_hab( NULLHANDLE ),
  app_hmq( NULLHANDLE )
{
  if( !( app_hab = WinInitialize(0)) ||
      !( app_hmq = WinCreateMsgQueue( app_hab, 0 )))
  {
    exit( LAYER_EXIT_INITERROR );
  }
}

/*--------------------------------------------------
 * Releases all of associated resources
 *--------------------------------------------------*/
KLApp::~KLApp()
{
  if( app_hmq ) WinDestroyMsgQueue( app_hmq );
  if( app_hab ) WinTerminate( app_hab );
}

/*--------------------------------------------------
 * Dispatch messages
 *--------------------------------------------------*/
void KLApp::run()
{
  QMSG qms;

  while( WinGetMsg( app_hab, &qms, 0, 0, 0 ))
    WinDispatchMsg( app_hab, &qms );
}

/*--------------------------------------------------
 * Sets a presentation font for specified window
 *--------------------------------------------------*/
void KLApp::do_warpsans( HWND handle )
{
  static int have_warpsans = -1;

  if( have_warpsans == -1 )
  {
    LONG   count  = 0;
    HPS    hps    = WinGetPS( HWND_DESKTOP );
    APIRET rc     = GpiQueryFonts( hps, QF_PUBLIC, "WarpSans", &count, 0, NULL );
    have_warpsans = ( rc != 0 && rc != GPI_ALTERROR );
  }

  char *fontname = have_warpsans ? "9.WarpSans" : "8.Helv";
  WinSetPresParam( handle, PP_FONTNAMESIZE, strlen(fontname) + 1, fontname );
}

/*--------------------------------------------------
 * Sets a presentation font for specified control
 *--------------------------------------------------*/
void KLApp::do_warpsans( HWND handle, SHORT id )
{
  do_warpsans( WinWindowFromID( handle, id ));
}

