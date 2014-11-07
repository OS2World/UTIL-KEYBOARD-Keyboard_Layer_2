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
 * $Id: kl_hotkey.h,v 1.9 2005/10/01 08:42:52 glass Exp $
 */

#ifndef __KL_HOTKEY_H
#define __KL_HOTKEY_H

#include <kl_global.h>

#if !defined(__ccdoc__)
/** Defines maximum number of the keyboard layouts. */
#define _MAX_HKEYS 512
#endif

#if !defined(__cplusplus) && !defined(__ccdoc__)

enum hkey_state { hkey_shift   =  0x0001,
                  hkey_alt     =  0x0002,
                  hkey_control =  0x0004 };
#else

/**
 * Keyboard hotkey.
 *
 * @see    KLSetupHKeys
 * @author Dmitry A Steklenev
 */

class KLHotkey
{
  public:

    /**
     * Use these enumerators to obtain keyboard shift state.
     *
     * <dl>
     * <dt><i>shift  </i><dd>Meaning, that the key Shift was pressed.
     * <dt><i>alt    </i><dd>Meaning, that the key Alt was pressed.
     * <dt><i>control</i><dd>Meaning, that the key Control was pressed.
     * </dl>
     */

    enum state_type { shift   =  0x0001,
                      alt     =  0x0002,
                      control =  0x0004 };

    /** Constructs the hotkey object. */
    KLHotkey();
    /** Constructs the hotkey object. */
    KLHotkey( int scancode, int state );

    /**
     * Returns the scancode.
     * @return Scancode or 0, if key not have it.
     */

    int scancode() const;

    /** Returns the keyboard shift state. */
    int state() const;
    /** Returns the hotkey name. */
    const char* name() const;
    /** Assigns the new hotkey. */
    void assign( int scancode, int state );

    /** Compares this object to another hotkey object for equality.   */
    BOOL operator==( const KLHotkey& hotkey ) const;
    /** Compares this object to another hotkey object for inequality. */
    BOOL operator!=( const KLHotkey& hotkey ) const;

  private:
    void mangle_name();

    int  hk_scancode;
    int  hk_state;
    char hk_name[128];
    HWND hk_exclude;

    static MRESULT _System dlg_proc( HWND, ULONG, MPARAM, MPARAM );
};

/*--------------------------------------------------
 * Returns the scancode
 *-------------------------------------------------*/
inline int KLHotkey::scancode() const
{
  return hk_scancode;
}

/*--------------------------------------------------
 * Returns the keyboard shift state
 *-------------------------------------------------*/
inline int KLHotkey::state() const
{
  return hk_state;
}

/*--------------------------------------------------
 * Returns the hotkey name
 *-------------------------------------------------*/
inline const char* KLHotkey::name() const
{
  return hk_name;
}

#endif
#endif
