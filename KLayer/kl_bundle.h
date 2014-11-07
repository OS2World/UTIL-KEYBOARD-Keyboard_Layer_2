/* Keyboard Layer/2
 *
 * Keyboard layout bundle
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
 * $Id$
 */

#ifndef __KL_BUNDLE_H
#define __KL_BUNDLE_H

#include <kl_broker.h>
#include <kl_hotkey.h>

/**
 * Keyboard layout bundle.
 *
 * KLBundle is a collection of attributes that contain various
 * information for the selected keyboard layout.
 *
 * @author Dmitry A Steklenev
 */

class KLBundle
{
  public:

    /**
     * Constructs the keyboard layout bundle.
     *
     * @param broker    Keyboard layouts broker.
     * @param name      Name of the keyboard layout.
     * @param layer_id  Selects a keyboard layer for specified keyboard layout.
     */

    KLBundle( KLBroker& broker, const char* name, ULONG layer_id = KL_LATIN );

    /**
     * Constructs the keyboard layout bundle.
     *
     * @param broker    Keyboard layouts broker.
     * @param layer_id  Selects a keyboard layer for selected keyboard layout.
     */

    KLBundle( KLBroker& broker, ULONG layer_id = KL_LATIN );

    /** Destructs the keyboard layout bundle. */
   ~KLBundle();

    /**
     * Selects the new keyboard layout.
     *
     * @param broker    Keyboard layouts broker.
     * @param name      Name of the keyboard layout.
     * @param layer_id  Selects a keyboard layer for specified keyboard layout.
     */

    KLBundle& select( KLBroker& broker, const char* name, ULONG layer_id = KL_LATIN );

    /**
     * Selects the current active keyboard layout.
     *
     * @param broker    Keyboard layouts broker.
     * @param layer_id  Selects a keyboard layer for selected keyboard layout.
     */

    KLBundle& select( KLBroker& broker, ULONG layer_id );

    /** Returns the layer viewer background color. */
    LONG background() const;
    /** Returns the layer viewer foreground color. */
    LONG foreground() const;
    /** Returns the VDM screen border color. */
    LONG vdm_border() const;

    /** Returns the layout selection hotkey. */
    const KLHotkey& hotkey() const;
    /** Assigns a hotkey for a selection of the keyboard layout. */
    void assign_hotkey( const KLHotkey& hotkey );
    /** Removes a hotkey for a selection of the keyboard layout. */
    void delete_hotkey();

    /** Returns the layer viewer icon handle. */
    HPOINTER icon() const;

    /** Sets the layer viewer background color. */
    void set_background( LONG color  );
    /** Sets the layer viewer foreground color. */
    void set_foreground( LONG color  );

    /** Returns the keyboard layer  name. */
    const char* layer_name() const;
    /** Returns the keyboard layout name. */
    const char* name() const;

  private:
    KLBundle& save();
    KLBundle& load();

    char lay_name[_MAX_LNAME];

    ULONG     lay_cp;
    ULONG     lay_id;
    BOOL      lay_multilayered;
    BOOL      lay_persistent;
    LONG      pri_bg_color;
    LONG      pri_fg_color;
    LONG      sec_bg_color;
    LONG      sec_fg_color;
    HPOINTER  pri_icon;
    HPOINTER  sec_icon;
    KLHotkey  lay_hotkey;

    #pragma pack(1)
    struct  properties
    {
       char name[_MAX_LNAME];
       LONG pri_bg_color;
       LONG pri_fg_color;
       LONG sec_bg_color;
       LONG sec_fg_color;
       char null[_MAX_LNAME];
       LONG lay_hk_scancode;
       LONG lay_hk_state;
    };
    #pragma pack()
};

/*--------------------------------------------------
 * Returns the layout selection hotkey
 *--------------------------------------------------*/
inline const KLHotkey& KLBundle::hotkey() const
{
  return lay_hotkey;
}

#endif
