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
 * $Id: kl_profiles.h,v 1.13 2005/12/06 09:38:57 glass Exp $
 */

#ifndef __KL_PROFILES_H
#define __KL_PROFILES_H

#include <os2def.h>
#include <kl_global.h>

#if defined(__cplusplus)
extern "C" {
#endif

  /**
   * Puts a binary data.
   *
   * Places the specified binary data to profile.
   *
   * @param key    The profile key name.
   * @param data   This is the value of the data that is written to
   *               the profile. It is not zero-terminated, and its
   *               length is given by the <i>len</i> parameter.
   * @param len    The size of the data to be written.
   */

  void EXPENTRY ini_put_data( const char* key, const void* data, ULONG len );

  /**
   * Puts a long integer value.
   *
   * Places the specified long integer value to profile.
   *
   * @param key    The profile key name.
   * @param value  A integer value.
   */

  void EXPENTRY ini_put_long( const char* key, LONG value );

  /**
   * Puts a short integer value.
   *
   * Places the specified short integer value to profile.
   *
   * @param key    The profile key name.
   * @param value  A integer value.
   */

  void EXPENTRY ini_put_short( const char* key, SHORT value );

  /**
   * Puts a string data.
   *
   * Places the specified string to profile.
   *
   * @param key    The profile key name.
   * @param value  A string pointer.
   */

  void EXPENTRY ini_put_chars( const char* key, const char* value );

  /**
   * Saves the current position and size of the window.
   *
   * This function will also save the presentation parameters
   * of the window and all his children.
   */

  void EXPENTRY ini_put_window( const char* key, HWND hwnd );

  /**
   * Gets a binary data.
   *
   * Reads the profile binary data.
   *
   * @param key    The key name.
   * @param data   A buffer in which the value corresponding to the
   *               key name is returned. The returned data is not null
   *               terminated, unless the value data is explicitly null
   *               terminated within the file.
   * @param len    This is the size of the buffer specified by the
   *               <i>data</i> parameter.
   * @return       If the call is successful, the number of bytes copied
   *               into the buffer.
   */


  ULONG EXPENTRY ini_get_data( const char* key, void* data, ULONG len );

  /**
   * Gets a long integer value.
   *
   * Reads the profile data and returns it as an long integer.
   *
   * @param key         The profile key name.
   * @param def_value   Default value.
   */

  LONG EXPENTRY ini_get_long( const char* key, LONG def_value );

  /**
   * Gets a long integer value.
   *
   * Reads the profile data and returns it as an short integer.
   *
   * @param key         The profile key name.
   * @param def_value   Default value.
   */

  SHORT EXPENTRY ini_get_short( const char* key, SHORT def_value );

  /**
   * Gets a string data.
   *
   * Reads the profile data and returns it as an string.
   *
   * @param key         The profile key name.
   * @param data        A buffer in which the string corresponding to the
   *                    key name is returned.
   * @param len         This is the size of the buffer specified by the
   *                    <i>data</i> parameter.
   * @param def_value   Default string.
   */

  char* EXPENTRY ini_get_chars( const char* key, char* data, ULONG len,
                                                 char* def_value );

  /**
   * Restores the current position and size of the window.
   *
   * This function will also restore the presentation parameters
   * of the window and all his children.
   */

  BOOL EXPENTRY ini_get_window( const char* key, HWND hwnd );

#if defined(__cplusplus)
}
#endif

#if !defined(__ccdoc__)

#define INI_SWITCH      "Switch key"
#define INI_BEEP        "Beep"
#define INI_PIPES       "Output to pipe"
#define INI_POS         "Pos"
#define INI_SETUPPOS    "Setup Pos"
#define INI_REMOVE      "Remove from task list"
#define INI_TRANSPFC    "Transparent focus"
#define INI_VDMCOLOR    "VDM color synchronize"
#define INI_FLOATONTOP  "Float on top"
#define INI_TIMEOUT     "TimeOut"
#define INI_LANGUAGE    "Language"
#define INI_DEF_LAYER   "Default layer"
#define INI_LAY_LAYER   "Layouts layer"
#define INI_LAYOUT      "Layout"
#define INI_SYSTRAY     "SysTray/WPS"
#define INI_SHOWICON    "Show layer icon"
#endif
#endif
