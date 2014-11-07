/* Keyboard Layer/2
 *
 * Keyboard hotkey
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
#include <stdlib.h>
#include <stdio.h>

#include "kl_hotkey.h"

/*--------------------------------------------------
 * Constructs the hotkey object
 *--------------------------------------------------*/
KLHotkey::KLHotkey()

: hk_scancode( 0          ),
  hk_state   ( 0          ),
  hk_exclude ( NULLHANDLE )
{
  mangle_name();
}

/*--------------------------------------------------
 * Constructs the hotkey object
 *--------------------------------------------------*/
KLHotkey::KLHotkey( int scancode, int state )

: hk_scancode( scancode   ),
  hk_state   ( state      ),
  hk_exclude ( NULLHANDLE )
{
  mangle_name();
}

/*--------------------------------------------------
 * Mangles the hotkey presentation name
 *--------------------------------------------------*/
void KLHotkey::mangle_name()
{
  char* key;

  hk_name[0] = 0;

  if( hk_scancode == 0 )
    return;

  if( hk_state & control ) strcat( hk_name, "Control + " );
  if( hk_state & alt     ) strcat( hk_name, "Alt + "  );
  if( hk_state & shift   ) strcat( hk_name, "Shift + " );

  switch( hk_scancode )
  {
    case 0x01: key = "Esc"         ; break;
    case 0x02: key = "1"           ; break;
    case 0x03: key = "2"           ; break;
    case 0x04: key = "3"           ; break;
    case 0x05: key = "4"           ; break;
    case 0x06: key = "5"           ; break;
    case 0x07: key = "6"           ; break;
    case 0x08: key = "7"           ; break;
    case 0x09: key = "8"           ; break;
    case 0x0A: key = "9"           ; break;
    case 0x0B: key = "0"           ; break;
    case 0x0C: key = "-"           ; break;
    case 0x0D: key = "="           ; break;
    case 0x0E: key = "Backspace"   ; break;
    case 0x0F: key = "Tab"         ; break;
    case 0x10: key = "Q"           ; break;
    case 0x11: key = "W"           ; break;
    case 0x12: key = "E"           ; break;
    case 0x13: key = "R"           ; break;
    case 0x14: key = "T"           ; break;
    case 0x15: key = "Y"           ; break;
    case 0x16: key = "U"           ; break;
    case 0x17: key = "I"           ; break;
    case 0x18: key = "O"           ; break;
    case 0x19: key = "P"           ; break;
    case 0x1A: key = "["           ; break;
    case 0x1B: key = "]"           ; break;
    case 0x1C: key = "Enter"       ; break;
    case 0x1D: key = "Control"     ; break;
    case 0x1E: key = "A"           ; break;
    case 0x1F: key = "S"           ; break;
    case 0x20: key = "D"           ; break;
    case 0x21: key = "F"           ; break;
    case 0x22: key = "G"           ; break;
    case 0x23: key = "H"           ; break;
    case 0x24: key = "J"           ; break;
    case 0x25: key = "K"           ; break;
    case 0x26: key = "L"           ; break;
    case 0x27: key = ";"           ; break;
    case 0x28: key = "\""          ; break;
    case 0x29: key = "`"           ; break;
    case 0x2A: key = "Left Shift"  ; break;
    case 0x2B: key = "\\"          ; break;
    case 0x2C: key = "Z"           ; break;
    case 0x2D: key = "X"           ; break;
    case 0x2E: key = "C"           ; break;
    case 0x2F: key = "V"           ; break;
    case 0x30: key = "B"           ; break;
    case 0x31: key = "N"           ; break;
    case 0x32: key = "M"           ; break;
    case 0x33: key = ","           ; break;
    case 0x34: key = "."           ; break;
    case 0x35: key = "/"           ; break;
    case 0x36: key = "Right Shift" ; break;
    case 0x37: key = "PrtScr"      ; break;
    case 0x38: key = "Alt"         ; break;
    case 0x39: key = "Space"       ; break;
    case 0x3A: key = "Caps Lock"   ; break;
    case 0x3B: key = "F1"          ; break;
    case 0x3C: key = "F2"          ; break;
    case 0x3D: key = "F3"          ; break;
    case 0x3E: key = "F4"          ; break;
    case 0x3F: key = "F5"          ; break;
    case 0x40: key = "F6"          ; break;
    case 0x41: key = "F7"          ; break;
    case 0x42: key = "F8"          ; break;
    case 0x43: key = "F9"          ; break;
    case 0x44: key = "F10"         ; break;
    case 0x45: key = "Num"         ; break;
    case 0x46: key = "Scroll Lock" ; break;
    case 0x47: key = "Pad Home"    ; break;
    case 0x48: key = "Pad Up"      ; break;
    case 0x49: key = "Pad PgUp"    ; break;
    case 0x4A: key = "Pad -"       ; break;
    case 0x4B: key = "Pad Left"    ; break;
    case 0x4C: key = "Pad 5"       ; break;
    case 0x4D: key = "Pad Right"   ; break;
    case 0x4E: key = "Pad +"       ; break;
    case 0x4F: key = "Pad End"     ; break;
    case 0x50: key = "Pad Down"    ; break;
    case 0x51: key = "Pad PgDn"    ; break;
    case 0x52: key = "Pad Ins"     ; break;
    case 0x53: key = "Pad Del"     ; break;
    case 0x57: key = "F11"         ; break;
    case 0x58: key = "F12"         ; break;
    case 0x5A: key = "Pad Enter"   ; break;
    case 0x5C: key = "Pad /"       ; break;

    case 0x60: key = "Home"        ; break;
    case 0x61: key = "Up"          ; break;
    case 0x62: key = "PgUp"        ; break;
    case 0x63: key = "Left"        ; break;
    case 0x64: key = "Right"       ; break;
    case 0x65: key = "End"         ; break;
    case 0x66: key = "Down"        ; break;
    case 0x67: key = "PgDn"        ; break;
    case 0x68: key = "Ins"         ; break;
    case 0x69: key = "Del"         ; break;

    case 0x7C: key = "Menu"        ; break;
    case 0x7E: key = "Left Win"    ; break;
    case 0x7F: key = "Right Win"   ; break;

    default:
      key = 0;
  }

  if( !key )
  {
    strcat( hk_name, "0x" );
    _ultoa( hk_scancode, hk_name + strlen(hk_name), 16 );
  }
  else
    strcat( hk_name, key  );
}

/*---------------------------------------------------
 * Assigns the new hotkey
 *--------------------------------------------------*/
void KLHotkey::assign( int scancode, int state )
{
  hk_scancode = scancode;
  hk_state    = state;

  mangle_name();
}

/*---------------------------------------------------
 * Compares this object to another object for equality
 *--------------------------------------------------*/
BOOL KLHotkey::operator==( const KLHotkey& hotkey ) const
{
  return hk_scancode == hotkey.hk_scancode &&
         hk_state    == hotkey.hk_state;
}

/*---------------------------------------------------
 * Compares this object to another object for inequality
 *--------------------------------------------------*/
BOOL KLHotkey::operator!=( const KLHotkey& hotkey ) const
{
  return hk_scancode != hotkey.hk_scancode ||
         hk_state    != hotkey.hk_state;
}

