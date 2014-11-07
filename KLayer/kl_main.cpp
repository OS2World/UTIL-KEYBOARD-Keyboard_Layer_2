/* Keyboard Layer/2
 *
 * Main module
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
#define  INCL_DOS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>

#include "kl_application.h"
#include "kl_viewer.h"
#include "kl_engine.h"
#include "kl_messages.h"
#include "kl_setup.h"

/*--------------------------------------------------
 * Start here
 *--------------------------------------------------*/
int main( int argc, char **argv )
{
  KLApp application;

  if( !KLMsg::open_catalog( "layer3.cat" ))
  {
    return LAYER_EXIT_INITERROR;
  }

  if( argc == 2 && stricmp( argv[1], "/stop" ) == 0 )
  {
    HWND hfound = KLViewer::find();
    int  time   = 10;

    if( hfound )
    {
      WinPostMsg( hfound, WM_QUIT, 0, 0 );

      while( time-- && KLViewer::find())
        DosSleep( 1000 );
    }

    return LAYER_EXIT_NORMAL;
  }

  if( KLViewer::find() != NULLHANDLE )
  {
    WinMessageBox( HWND_DESKTOP, HWND_DESKTOP, MSG_RUNONE,
                   PKG_NAME, 100, MB_OK | MB_ERROR | MB_MOVEABLE );

    return LAYER_EXIT_DUPLICATE;
  }

  KLViewer viewer( application.hab());

  if( engine_start( application.hab(), viewer.handle()))
  {
    viewer.load();
    KLSetup::apply( viewer.handle());

    char name[_MAX_LNAME];
    ini_get_chars( INI_LAYOUT, name, sizeof(name), "" );

    if( strlen(name))
      set_layout(name);

    application.run();

    engine_stop( application.hab());
    viewer.save();

    return LAYER_EXIT_NORMAL;
  }
  else
  {
    engine_stop( application.hab());
    return LAYER_EXIT_INITERROR;
  }
}

