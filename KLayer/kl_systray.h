/* Keyboard Layer/2
 *
 * Supports of the SysTray/WPS
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

#ifndef __KL_SYSTRAY_H
#define __KL_SYSTRAY_H

/**
 * Supports of the SysTray/WPS.
 *
 * Note that there is no need to create an instance of this class
 * because all methods are static.
 *
 * @author Dmitry A Steklenev
 */

class KLSysTray
{
  public:

    /**
     * Enables support of the SysTray/WPS.
     *
     * @param hwnd  Windows handle of the layer viewer.
     */

    static void enable ( HWND hwnd, BOOL enable = TRUE );

    /**
     * Disables support of the SysTray/WPS.
     *
     * @param hwnd  Windows handle of the layer viewer.
     */

    static void disable( HWND hwnd );

    /**
     * Sets the icon of the layer viewer.
     *
     * If SysTray/WPS support is enabled, sets the
     * layer viewer icon.
     *
     * @param hwnd  Windows handle of the layer viewer.
     * @param hicon Handle of the new icon.
     */

    static void set_icon( HWND hwnd, HPOINTER hicon );

    /**
     * Is the support of the SysTray/WPS enabled.
     *
     * @return If the support of the SysTray/WPS is enabled, TRUE is returned.
     *         Otherwise, FALSE is returned.
     */

    static BOOL is_enabled();

    /**
     * Dispatch system native events.
     *
     * @return TRUE, if message processed.
     */

    static MRESULT dispatch( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

  private:
    static HPOINTER sys_deficon;
    static HWND     sys_hviewer;
    static BOOL     sys_enabled;
};

/*--------------------------------------------------
 * Is the support of the SysTray/WPS enabled
 *--------------------------------------------------*/
inline BOOL KLSysTray::is_enabled()
{
  return sys_enabled;
}

#endif
