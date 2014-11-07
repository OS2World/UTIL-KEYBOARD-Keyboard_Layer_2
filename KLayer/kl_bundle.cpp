/* Keyboard Layer/2
 *
 * Keyboard layout presentation map
 * Copyright (C) 2002 by Dmitry A.Steklenev
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
 * $Id: kl_bundle.cpp,v 1.15 2005/12/06 09:38:57 glass Exp $
 */

#include "kl_bundle.h"
#include "kl_engine.h"
#include "kl_ids.h"

#define  INCL_WIN
#include <os2.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>

#define  ID_FLG(s) (( s[0] << 8 ) + s[1] )

/*--------------------------------------------------
 * Constructs the keyboard layout bundle
 *--------------------------------------------------*/
KLBundle::KLBundle( KLBroker& broker, const char* name, ULONG id )
{
  lay_name[0]      = 0;
  lay_cp           = 0;
  lay_id           = KL_LATIN;
  lay_multilayered = FALSE;
  lay_persistent   = TRUE;
  pri_bg_color     = 0;
  pri_fg_color     = 0;
  sec_bg_color     = 0;
  sec_fg_color     = 0;
  pri_icon         = 0;
  sec_icon         = 0;

  select( broker, name, id );
}

/*--------------------------------------------------
 * Constructs the keyboard layout bundle
 *--------------------------------------------------*/
KLBundle::KLBundle( KLBroker& broker, ULONG id )
{
  lay_name[0]      = 0;
  lay_cp           = 0;
  lay_id           = KL_LATIN;
  lay_multilayered = FALSE;
  lay_persistent   = TRUE;
  pri_bg_color     = 0;
  pri_fg_color     = 0;
  sec_bg_color     = 0;
  sec_fg_color     = 0;
  pri_icon         = 0;
  sec_icon         = 0;

  select( broker, id );
}

/*--------------------------------------------------
 * Destructs the keyboard layout bundle
 *--------------------------------------------------*/
KLBundle::~KLBundle()
{
  if( pri_icon ) WinDestroyPointer( pri_icon );
  if( sec_icon ) WinDestroyPointer( sec_icon );

  save();
}

/*--------------------------------------------------
 * Selects the new keyboard layout
 *--------------------------------------------------*/
KLBundle& KLBundle::select( KLBroker& broker, const char* name, ULONG id )
{
  if( stricmp( lay_name, name ) != 0 || lay_cp != broker.cp_list()->selected )
  {
    strncpy( lay_name, name, sizeof(lay_name));

    lay_cp = broker.cp_list()->selected;
    lay_multilayered = broker.is_multilayered( lay_name, lay_cp );

    load();
  }

  lay_id = id;
  return *this;
}

/*--------------------------------------------------
 * Selects the current active keyboard layout
 *--------------------------------------------------*/
KLBundle& KLBundle::select( KLBroker& broker, ULONG id )
{
  return select( broker, broker.selected(), id );
}

/*--------------------------------------------------
 * Saves keyboard layout presentation parameters
 *--------------------------------------------------*/
KLBundle& KLBundle::save()
{
  if( !lay_persistent )
  {
    properties parms;
    char key[64];

    strncpy( parms.name, lay_name, sizeof(parms.name));

    parms.pri_bg_color    = pri_bg_color;
    parms.pri_fg_color    = pri_fg_color;
    parms.sec_bg_color    = sec_bg_color;
    parms.sec_fg_color    = sec_fg_color;
    parms.lay_hk_scancode = lay_hotkey.scancode();
    parms.lay_hk_state    = lay_hotkey.state();

    sprintf( key, "%06u@%s", lay_cp, lay_name );
    ini_put_data( key, &parms, sizeof(parms));

    lay_persistent = TRUE;
  }

  return *this;
}

/*--------------------------------------------------
 * Loads keyboard layout presentation parameters
 *--------------------------------------------------*/
KLBundle& KLBundle::load()
{
  if( pri_icon ) if( !WinDestroyPointer( pri_icon )) DosBeep( 1000, 100 );
  if( sec_icon ) if( !WinDestroyPointer( sec_icon )) DosBeep( 1000, 100 );

  save();

  pri_icon       = 0;
  sec_icon       = 0;
  lay_persistent = TRUE;

  properties parms;
  char key[64];

  if( lay_multilayered )
  {
    parms.pri_bg_color  = 0x0000FFL;
    parms.pri_fg_color  = 0xFFFFFFL;
    parms.sec_bg_color  = 0xFF0000L;
    parms.sec_fg_color  = 0xFFFFFFL;
  }
  else
  {
    parms.pri_bg_color  = 0x000000L;
    parms.pri_fg_color  = 0xFFFFFFL;
    parms.sec_bg_color  = 0x000000L;
    parms.sec_fg_color  = 0xFFFFFFL;
  }

  parms.lay_hk_scancode = 0;
  parms.lay_hk_state    = 0;

  sprintf( key, "%06u@%s", lay_cp, lay_name );
  ini_get_data( key, &parms, sizeof(parms));

  pri_bg_color = parms.pri_bg_color;
  pri_fg_color = parms.pri_fg_color;
  sec_bg_color = parms.sec_bg_color;
  sec_fg_color = parms.sec_fg_color;
  lay_hotkey   = KLHotkey( parms.lay_hk_scancode, parms.lay_hk_state );

  if( lay_multilayered )
  {
    pri_icon = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, ID_FLG_LATIN      );
    sec_icon = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, ID_FLG( lay_name ));

    if( !sec_icon )
      sec_icon = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, ID_FLG_NATIONAL );
  }
  else
  {
    pri_icon = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, ID_FLG( lay_name ));
    sec_icon = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, ID_FLG( lay_name ));

    if( !pri_icon )
      pri_icon = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, ID_FLG_OTHER    );
    if( !sec_icon )
      sec_icon = WinLoadPointer( HWND_DESKTOP, NULLHANDLE, ID_FLG_OTHER    );
  }

  return *this;
}

/*--------------------------------------------------
 * Returns the layer viewer background color
 *--------------------------------------------------*/
LONG KLBundle::background() const
{
  return lay_id == KL_LATIN ? pri_bg_color : sec_bg_color;
}

/*--------------------------------------------------
 * Returns the layer viewer foreground color
 *--------------------------------------------------*/
LONG KLBundle::foreground() const
{
  return lay_id == KL_LATIN ? pri_fg_color : sec_fg_color;
}

/*--------------------------------------------------
 * Returns the VDM screen border color
 *--------------------------------------------------*/
LONG KLBundle::vdm_border() const
{
  return sec_bg_color;
}

/*--------------------------------------------------
 * Returns the keyboard layer name
 *--------------------------------------------------*/
const char* KLBundle::layer_name() const
{
  return lay_id == KL_LATIN && lay_multilayered ? "LATIN" : lay_name;
}

/*--------------------------------------------------
 * Returns the keyboard layout name
 *--------------------------------------------------*/
const char* KLBundle::name() const
{
  return lay_name;
}

/*--------------------------------------------------
 * Returns the layer viewer icon handle
 *--------------------------------------------------*/
HPOINTER KLBundle::icon() const
{
  return lay_id == KL_LATIN ? pri_icon : sec_icon;
}

/*--------------------------------------------------
 * Sets the layer viewer background color
 *--------------------------------------------------*/
void KLBundle::set_background( LONG color )
{
  if( lay_id == KL_LATIN )
    pri_bg_color = color;
  else
    sec_bg_color = color;

  lay_persistent = FALSE;
  save();
}

/*--------------------------------------------------
 * Sets the layer viewer foreground color
 *--------------------------------------------------*/
void KLBundle::set_foreground( LONG color )
{
  if( lay_id == KL_LATIN )
    pri_fg_color = color;
  else
    sec_fg_color = color;

  lay_persistent = FALSE;
  save();
}

/*--------------------------------------------------
 * Assigns a hotkey for a selection of the layout
 *--------------------------------------------------*/
void KLBundle::assign_hotkey( const KLHotkey& hotkey )
{
  lay_hotkey = hotkey;
  lay_persistent = FALSE;
  save();
}

/*--------------------------------------------------
 * Removes a hotkey for a selection of the layout
 *--------------------------------------------------*/
void KLBundle::delete_hotkey()
{
  if( lay_hotkey.scancode() || lay_hotkey.state())
  {
    lay_hotkey = KLHotkey();
    lay_persistent = FALSE;
    save();
  }
}

