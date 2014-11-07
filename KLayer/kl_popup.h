/* Keyboard Layer/2
 *
 * Context pop-up menu
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

#ifndef __KL_POPUP_H
#define __KL_POPUP_H

#include <kl_broker.h>

/**
 * Context pop-up menu.
 *
 * The KLPopUp class creates a pop-up menu. When a user makes a
 * choice from a menu, the menu posts a message WM_COMMAND
 * with unique identifier for the menu item to the layer viewer.
 *
 * @author Dmitry A Steklenev
 */

class KLPopUp
{
  public:

    /**
     * Constructs the pop-up menu object.
     *
     * @param id    The identifier of the menu you are constructing.
     * @param hwnd  Windows handle of the layer viewer.
     */

    KLPopUp( HWND hwnd, ULONG id );

    /** Destructs the pop-up menu object. */
   ~KLPopUp();

    /** Displays the pop-up menu at the specified location.*/
    void show( LONG x, LONG y );
    /** Returns a name of the selected keyboard layout. */
    const char* selected( ULONG id );
    /** Returns the menu handle. */
    HWND handle() const;

    /**
     * Places a select state indicator to the menu item.
     *
     * Places a select state indicator to the left of the menu item.
     * The default visual of this indicator is a check mark.
     */

    KLPopUp& check( ULONG id, BOOL state = TRUE );

    /** Removes the select state indicator from the menu item. */
    KLPopUp& uncheck( ULONG id );

    /**
     * Is the menu item checked.
     *
     * @return If the menu item with the specified identifier
     *         is checked, TRUE is returned. Otherwise, FALSE is
     *         returned.
     */

    BOOL is_checked( ULONG id ) const;

    /** Makes a menu item selectable. */
    KLPopUp& enable ( ULONG id, BOOL state = TRUE );
    /** Prevents the menu item from being selected. */
    KLPopUp& disable( ULONG id );

    /**
     * Is the menu item enabled.
     *
     * @return If the menu item is selectable, TRUE is returned.
     *         Otherwise, FALSE is returned.
     */

    BOOL is_enabled( ULONG id ) const;

  private:
    HWND mn_viewer;
    HWND mn_handle;
    char mn_layout[_MAX_LNAME];
};

/*--------------------------------------------------
 * Returns the menu handle
 *--------------------------------------------------*/
inline HWND KLPopUp::handle() const
{
  return mn_handle;
}

/*--------------------------------------------------
 * Removes the select state indicator from the item
 *--------------------------------------------------*/
inline KLPopUp& KLPopUp::uncheck( ULONG id )
{
  return check( id, FALSE );
}

/*--------------------------------------------------
 * Prevents the menu item from being selected
 *--------------------------------------------------*/
inline KLPopUp& KLPopUp::disable( ULONG id )
{
  return enable( id, FALSE );
}

#endif
