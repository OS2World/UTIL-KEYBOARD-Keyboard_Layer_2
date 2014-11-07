/* Keyboard Layer/2
 *
 * Setup notebook
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

#ifndef __KL_SETUP_H
#define __KL_SETUP_H

/**
 * Setup notebook.
 *
 * @author Dmitry A Steklenev
 * @pkgdoc Setup
 */

#include <kl_global.h>

/**
 * Setup notebook.
 *
 * The KLSetup is a control that is useful in organizing related
 * information on individual {@link KLSetupPage pages} so that a
 * user can quickly setup the Keyboard Layer/2.
 *
 * @author Dmitry A Steklenev
 */

class KLSetup
{
  public:

    /**
     * Constructs the setup notebook.
     *
     * @param howner  Windows handle of the layer viewer.
     */

    KLSetup( HWND howner );

    /** Destructs the setup notebook. */
   ~KLSetup();

    /**
     * Displays the setup notebook.
     *
     * While the setup notebook is being shown modally, its owner
     * are disabled. Other top level windows belonging to the
     * application are not disabled.
     */

    ULONG show_modally();

    /**
     * Applies the preferences values.
     *
     * This static method applies the preferences values maintained
     * by setup notebook from the program profile to the program
     * engine.
     *
     * @param hwnd Windows handle of the layer viewer.
     */

    static void apply( HWND hwnd );

    /** Returns the window handle of the layer viewer. */
    HWND owner () const;
    /** Returns the notebook handle. */
    HWND handle() const;

  protected:

    /**
     * Dispatch system native events.
     *
     * KLSetup calls this function when a system native window
     * event occurs.
     */

    virtual MRESULT dispatch( ULONG msg, MPARAM mp1, MPARAM mp2 );

  private:

    HWND nb_dialog;   //@- Handle of the dialog window.
    HWND nb_handle;   //@- Handle of the notebook.
    HWND nb_viewer;   //@- Handle of the layer viewer window.
    HWND nb_ok;       //@- Handle of the "Ok" button.
    HWND nb_default;  //@- Handle of the "Default" button.
    HWND nb_cancel;   //@- Handle of the "Cancel" button.

    class KLSetupLayer01   *nb_layer01;
    class KLSetupLayer02   *nb_layer02;
    class KLSetupLayouts01 *nb_layout01;
    class KLSetupLayouts02 *nb_layout02;
    class KLSetupLanguage  *nb_language;
    class KLSetupAbout     *nb_about;

    static MRESULT _System win_proc( HWND, ULONG, MPARAM, MPARAM );
};

/*---------------------------------------------------
 * Returns the window handle of the layer viewer
 *--------------------------------------------------*/
inline HWND KLSetup::owner() const
{
  return nb_viewer;
}

/*---------------------------------------------------
 * Returns the notebook handle
 *--------------------------------------------------*/
inline HWND KLSetup::handle() const
{
  return nb_handle;
}

#endif
