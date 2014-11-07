/* Keyboard Layer/2
 *
 * Trigger hotkeys management class
 * Copyright (C) 2001 Dmitry A.Steklenev
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
 * $Id: kl_trigger.cpp,v 1.8 2005/10/01 08:42:53 glass Exp $
 */

#define  INCL_WIN
#include <os2.h>

#include "kl_trigger.h"

#if defined(__cplusplus)
  KLTrigger::sequence KLTrigger::sw_latin;
  KLTrigger::sequence KLTrigger::sw_natio;
  KLTrigger::sequence KLTrigger::sw_current;
  ULONG               KLTrigger::sw_time;
  ULONG               KLTrigger::sw_timeout;
  int                 KLTrigger::sw_hotkey;

  #define DECLARE(x) KLTrigger::##x
#else
  #pragma data_seg( "DATA32" )

  static htrg_sequence sw_latin;
  static htrg_sequence sw_natio;
  static htrg_sequence sw_current;
  static ULONG         sw_time;
  static ULONG         sw_timeout;
  static int           sw_hotkey;

  #define DECLARE(x) htrg_##x
#endif

/*--------------------------------------------------
 * Sets the current timeout value
 *--------------------------------------------------*/
void DECLARE( set_timeout( ULONG time ))
{
  sw_timeout = time;
  sw_time    = 0;
}

/*--------------------------------------------------
 * Returns the current timeout value
 *--------------------------------------------------*/
ULONG DECLARE( timeout())
{
  return sw_timeout;
}

/*--------------------------------------------------
 * Returns the trigger hotkeys
 *--------------------------------------------------*/
int DECLARE( selected())
{
  return sw_hotkey;
}

/*--------------------------------------------------
 * Selects the trigger hotkeys
 *--------------------------------------------------*/
void DECLARE( select( int type ))
{
  sw_current.key_1st = 0;
  sw_current.key_2st = 0;
  sw_time   = 0;
  sw_hotkey = type;

  switch( type )
  {
    case XS_LCONTROL:
      sw_latin.key_1st = CS_LCONTROL;
      sw_latin.key_2st = CS_LCONTROL | CS_UP;
      sw_natio.key_1st = CS_LCONTROL;
      sw_natio.key_2st = CS_LCONTROL | CS_UP;
      break;

    case XS_LCTRANDS:
      sw_latin.key_1st = CS_LCONTROL;
      sw_latin.key_2st = CS_LSHIFT;
      sw_natio.key_1st = CS_LCONTROL;
      sw_natio.key_2st = CS_LSHIFT;
      break;

    case XS_LSHIFT:
      sw_latin.key_1st = CS_LSHIFT;
      sw_latin.key_2st = CS_LSHIFT | CS_UP;
      sw_natio.key_1st = CS_LSHIFT;
      sw_natio.key_2st = CS_LSHIFT | CS_UP;
      break;

    case XS_SHIFTS:
      sw_latin.key_1st = CS_LSHIFT;
      sw_latin.key_2st = CS_RSHIFT;
      sw_natio.key_1st = CS_LSHIFT;
      sw_natio.key_2st = CS_RSHIFT;
      break;

    case XS_LWIN:
      sw_latin.key_1st = CS_LWIN;
      sw_latin.key_2st = CS_LWIN | CS_UP;
      sw_natio.key_1st = CS_LWIN;
      sw_natio.key_2st = CS_LWIN | CS_UP;
      break;

    case XS_RCONTROL:
      sw_latin.key_1st = CS_RCONTROL;
      sw_latin.key_2st = CS_RCONTROL | CS_UP;
      sw_natio.key_1st = CS_RCONTROL;
      sw_natio.key_2st = CS_RCONTROL | CS_UP;
      break;

    case XS_RCTRLSH:
      sw_latin.key_1st = CS_RCONTROL;
      sw_latin.key_2st = CS_RCONTROL | CS_UP;
      sw_natio.key_1st = CS_RSHIFT;
      sw_natio.key_2st = CS_RSHIFT | CS_UP;
      break;

    case XS_RCTRANDS:
      sw_latin.key_1st = CS_RCONTROL;
      sw_latin.key_2st = CS_RSHIFT;
      sw_natio.key_1st = CS_RCONTROL;
      sw_natio.key_2st = CS_RSHIFT;
      break;

    case XS_RSHIFT:
      sw_latin.key_1st = CS_RSHIFT;
      sw_latin.key_2st = CS_RSHIFT | CS_UP;
      sw_natio.key_1st = CS_RSHIFT;
      sw_natio.key_2st = CS_RSHIFT | CS_UP;
      break;

    case XS_RWIN:
      sw_latin.key_1st = CS_RWIN;
      sw_latin.key_2st = CS_RWIN | CS_UP;
      sw_natio.key_1st = CS_RWIN;
      sw_natio.key_2st = CS_RWIN | CS_UP;
      break;

    case XS_ALTSH:
      sw_latin.key_1st = CS_LALT;
      sw_latin.key_2st = CS_LSHIFT;
      sw_natio.key_1st = CS_RALT;
      sw_natio.key_2st = CS_RSHIFT;
      break;

    case XS_WINMENU:
      sw_latin.key_1st = CS_WINMENU;
      sw_latin.key_2st = CS_WINMENU | CS_UP;
      sw_natio.key_1st = CS_WINMENU;
      sw_natio.key_2st = CS_WINMENU | CS_UP;
      break;

    default:
      sw_latin.key_1st = 0;
      sw_latin.key_2st = 0;
      sw_natio.key_1st = 0;
      sw_natio.key_2st = 0;
      sw_hotkey        = XS_DONTTOUCH;
  }
}

/*--------------------------------------------------
 * Dispatches the next pressed or released key
 *--------------------------------------------------*/
ULONG DECLARE( dispatch( BYTE scancode, ULONG current_layer, ULONG time ))
{
  BOOL is_natio;
  BOOL is_latin;
  BOOL is_timeout = sw_timeout > 0 && sw_time > 0 && (time - sw_time) >= sw_timeout;

  sw_current.key_1st = sw_current.key_2st;
  sw_current.key_2st = scancode;
  sw_time            = time;

  is_natio = (sw_current.key_1st == sw_natio.key_1st &&
              sw_current.key_2st == sw_natio.key_2st );
  is_latin = (sw_current.key_1st == sw_latin.key_1st &&
              sw_current.key_2st == sw_latin.key_2st );

  if( !is_timeout )
  {
    if( is_latin && is_natio )
      return current_layer == KL_NATIONAL ? KL_LATIN : KL_NATIONAL;
    if( is_latin )
      return KL_LATIN;
    if( is_natio )
      return KL_NATIONAL;
  }

  return KL_NONE;
}
