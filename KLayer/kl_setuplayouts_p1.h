/* Keyboard Layer/2
 *
 * Setup page 01 of the keyboard layouts
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

#ifndef __KL_SETUPLAYOUTS_P1_H
#define __KL_SETUPLAYOUTS_P1_H

#include <kl_setuppage.h>
#include <kl_setuphkeys.h>
#include <kl_broker.h>
#include <kl_bundle.h>

/**
 * Setup page 01 of the keyboard layouts.
 * @author Dmitry A Steklenev
 */

class KLSetupLayouts01 : public KLSetupPage
{
  public:

    /**
     * Constructs the setup page.
     * @param parent Parent notebook.
     */

    KLSetupLayouts01( const KLSetup& parent );

    /** Destructs the setup page. */
    virtual ~KLSetupLayouts01();

    /**
     * Saves the preferences values.
     *
     * This method saves the current preferences values maintained
     * by setup page to the program profile.
     */

    virtual void save();

    /**
     * Applies the preferences values.
     *
     * This static method applies the preferences values maintained
     * by setup page from the program profile to the program engine.
     *
     * @param hwnd Windows handle of the layer viewer.
     */

    static void apply( HWND hwnd );

  protected:

    /**
     * Dispatch system native events.
     *
     * KLSetupLayer01 calls this function when a system native window
     * event occurs.
     */

    virtual MRESULT dispatch( ULONG msg, MPARAM mp1, MPARAM mp2 );

  private:
    struct layout {
      RECORDCORE rdcore;
      char*      hkname;

      KLBundle*  bundle;
      KLHotkey*  hotkey;
    };

    /* Provides mechanisms for records comparison. */
    static SHORT EXPENTRY compare( PRECORDCORE p1,
                                   PRECORDCORE p2, PVOID pStorage );
    HWND pg_hcont;
    HWND pg_hmenu;
    char pg_title01[256];
    char pg_title02[256];

    KLSetupHKeys hk_manager;

    /* Supports of the context menu. */
    void menu_show( layout* record );
    void menu_done();

    /* Supports of the hotkeys. */
    void assign_hotkey( layout* record );
    void delete_hotkey( layout* record );
    void setall_hotkey();
};

#endif
