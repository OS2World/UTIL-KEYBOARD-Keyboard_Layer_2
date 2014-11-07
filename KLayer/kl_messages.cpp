/* Keyboard Layer/2
 *
 * Messages catalog management class
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

#include "kl_messages.h"

#define  INCL_WIN
#define  INCL_DOS
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>

nl_catd KLMsg::msg_catd    = 0;
int     KLMsg::msg_country = 1;
KLMsg   KLMsg::msg_sentry;

/*--------------------------------------------------
 * Destructs messages catalog
 *--------------------------------------------------*/
KLMsg::~KLMsg()
{
  if( msg_catd )
    catclose( msg_catd );
}

/*--------------------------------------------------
 * Opens messages catalog
 *--------------------------------------------------*/
int KLMsg::open_catalog( const char* catalog_name )
{
  msg_catd    = catopen( catalog_name, 0 );
  msg_country = 1;

  if( msg_catd == CATD_ERR )
  {
    char msg_drive[_MAX_PATH] = "";
    char msg_path [_MAX_PATH] = "";
    char msg_name [_MAX_PATH] = "";

    PPIB ppib;
    DosGetInfoBlocks( NULL, &ppib );

    if( ppib->pib_pchcmd )
    {
      _splitpath( ppib->pib_pchcmd, msg_drive, msg_path, NULL, NULL );
      sprintf( msg_name, "%s%s%s", msg_drive, msg_path, catalog_name );
      msg_catd = catopen( msg_name, 0 );
    }
  }

  if( msg_catd == CATD_ERR )
  {
    char errmsg[2048];
    sprintf( errmsg, "Message catalog '%s' not found!", catalog_name );

    WinMessageBox( HWND_DESKTOP, HWND_DESKTOP, errmsg,
                   PKG_NAME, 100,
                   MB_OK | MB_ERROR | MB_APPLMODAL | MB_MOVEABLE );

    return FALSE;
  }
  else
    select_country();

  return TRUE;
}

/*--------------------------------------------------
 * Selects country
 *--------------------------------------------------*/
void KLMsg::select_country( int country )
{
  if( !msg_catd )
    return;

  if( !country  )
  {
    char country_code[16] = "";
    ULONG len = sizeof(country_code) - 1;

    PrfQueryProfileData( HINI_USERPROFILE,
                         "PM_National", "iCountry", &country_code, &len );

    country_code[len] = '\0';
    msg_country = atoi( country_code );
  }
  else
    msg_country = country;

  if( catgets( msg_catd, msg_country, 1, NULL ) == NULL )
    msg_country = 1;
}

/*--------------------------------------------------
 * Returns specified message
 *--------------------------------------------------*/
const char* KLMsg::get( int msg_no )
{
  return catgets( msg_catd, msg_country, msg_no, "Message not found" );
}

