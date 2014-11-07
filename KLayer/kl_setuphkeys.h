/* Keyboard Layer/2
 *
 * Keyboard hotkey management class
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
 * $Id: kl_setuphkeys.h,v 1.2 2005/10/01 08:42:52 glass Exp $
 */

#ifndef __KL_SETUPHKEYS_H
#define __KL_SETUPHKEYS_H

#include <kl_hotkey.h>

/**
 * Keyboard hotkey manager.
 * @author Dmitry A Steklenev
 */

class KLSetupHKeys
{
  public:

    /** Constructs the manager object. */
    KLSetupHKeys();

    /**
     * Assigns the new hotkey.
     *
     * @param  howner  Handle of the owner window.
     * @param  phkey   Pointer to the hotkey object.
     *
     * @return If the new hotkey is assigned,
     *         TRUE is returned. Otherwise, FALSE is returned.
     */

    BOOL assign( HWND howner, KLHotkey* phkey );

  private:
    int       hk_scancode;
    int       hk_state;
    HWND      hk_exclude;
    KLHotkey* hk_assigned;

    static MRESULT _System dlg_proc( HWND, ULONG, MPARAM, MPARAM );
};

#endif
