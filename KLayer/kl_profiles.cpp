/* Keyboard Layer/2
 *
 * User profile management
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

#include "kl_profiles.h"

#define  INCL_WIN
#include <os2.h>
#include <string.h>

/*--------------------------------------------------
 * Puts a binary data
 *--------------------------------------------------*/
void EXPENTRY ini_put_data( const char* key,
                            const void* exp, ULONG len )
{
  PrfWriteProfileData( HINI_USERPROFILE, PKG_NAME,
                       key, (PVOID)exp, len );
}

/*--------------------------------------------------
 * Gets a binary data
 *--------------------------------------------------*/
ULONG EXPENTRY ini_get_data( const char* key, void* exp, ULONG len )
{
  ULONG get_len = len;

  if( PrfQueryProfileData( HINI_USERPROFILE, PKG_NAME, key, exp, &get_len ))
    return get_len;
  else
    return 0;
}

/*--------------------------------------------------
 * Puts a long integer value
 *--------------------------------------------------*/
void EXPENTRY ini_put_long( const char *key, LONG exp )
{
  PrfWriteProfileData( HINI_USERPROFILE, PKG_NAME,
                       key, &exp, sizeof(exp));
}

/*--------------------------------------------------
 * Gets a long integer value
 *--------------------------------------------------*/
LONG EXPENTRY ini_get_long( const char *key, LONG def )
{
  LONG  exp = def;
  ULONG len = sizeof(exp);

  return PrfQueryProfileData( HINI_USERPROFILE, PKG_NAME,
                              key, &exp, &len ) ? exp : def;
}

/*--------------------------------------------------
 * Puts a short integer value
 *--------------------------------------------------*/
void EXPENTRY ini_put_short( const char *key, SHORT exp )
{
  PrfWriteProfileData( HINI_USERPROFILE, PKG_NAME,
                       key, &exp, sizeof(exp));
}

/*--------------------------------------------------
 * Gets a short integer value
 *--------------------------------------------------*/
SHORT EXPENTRY ini_get_short( const char *key, SHORT def )
{
  SHORT exp = def;
  ULONG len = sizeof(exp);

  return PrfQueryProfileData( HINI_USERPROFILE, PKG_NAME,
                              key, &exp, &len ) ? exp : def;
}

/*--------------------------------------------------
 * Puts a string data
 *--------------------------------------------------*/
void EXPENTRY ini_put_chars( const char *key, const char* exp )
{
  PrfWriteProfileData( HINI_USERPROFILE, PKG_NAME,
                       key, (PVOID)exp, strlen(exp)+1 );
}

/*--------------------------------------------------
 * Gets a string data
 *--------------------------------------------------*/
char* EXPENTRY ini_get_chars( const char *key,
                              char* exp, ULONG len, char* def )
{
  ULONG get_len = len;

  if( !PrfQueryProfileData( HINI_USERPROFILE, PKG_NAME,
                            key, exp, &get_len ))
  {
    strcpy( exp, def );
  }

  return exp;
}

/*--------------------------------------------------
 * Saves the current position and size of the window
 *--------------------------------------------------*/
void EXPENTRY ini_put_window( const char* key, HWND hwnd )
{
  POINTL pos[2];
  SWP    swp;

  if( WinQueryWindowUShort( hwnd, QWS_FLAGS ) & FF_DIALOGBOX )
  {
    if( WinQueryWindowPos( hwnd, &swp ))
    {
      pos[0].x = swp.x;
      pos[0].y = swp.y;
      pos[1].x = swp.x + swp.cx;
      pos[1].y = swp.y + swp.cy;

      WinMapDlgPoints( hwnd, pos, 2, FALSE );
      ini_put_data( key, &pos, sizeof(pos));
    }
  }
  else
    WinStoreWindowPos( PKG_NAME, key, hwnd );
}

/*--------------------------------------------------
 * Restores the current position of the window
 *--------------------------------------------------*/
BOOL EXPENTRY ini_get_window( const char* key, HWND hwnd )
{
  POINTL pos[2];
  SWP    swp;
  SWP    desktop;
  BOOL   success = FALSE;

  if( WinQueryWindowUShort( hwnd, QWS_FLAGS ) & FF_DIALOGBOX )
  {
    if( ini_get_data( key, pos, sizeof(pos)) == sizeof(pos))
    {
      WinMapDlgPoints( hwnd, pos, 2, TRUE );
      WinSetWindowPos( hwnd, 0, pos[0].x, pos[0].y,
                       pos[1].x-pos[0].x, pos[1].y-pos[0].y, SWP_MOVE | SWP_SIZE );
      success = TRUE;
    }
  }
  else
    success = WinRestoreWindowPos( PKG_NAME, key, hwnd );

  if( success && WinQueryWindowPos( hwnd, &swp )
              && WinQueryWindowPos( HWND_DESKTOP, &desktop ))
  {
    if( swp.y + swp.cy > desktop.cy )
    {
      swp.y = desktop.cy - swp.cy;
      WinSetWindowPos( hwnd, 0, swp.x, swp.y, 0, 0, SWP_MOVE );
    }
  }

  return success;
}

