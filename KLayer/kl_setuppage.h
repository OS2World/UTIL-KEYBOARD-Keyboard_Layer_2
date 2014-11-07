/* Keyboard Layer/2
 *
 * Setup page base class
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
 * $Id: kl_setuppage.h,v 1.10 2005/10/01 08:42:53 glass Exp $
 */

#ifndef __KL_SETUPPAGE_H
#define __KL_SETUPPAGE_H

#include <kl_setup.h>
#include <kl_engine.h>

#if !defined(__ccdoc__)

  #define ADD_ITEM( handle, id, item    ) \
          WinSendDlgItemMsg( handle, id, LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(item))
  #define SEL_ITEM( handle, id, item_no ) \
          WinSendDlgItemMsg( handle, id, LM_SELECTITEM, MPFROMSHORT(item_no), MPFROMSHORT(TRUE))
  #define GET_ITEM( handle, id          ) \
          WinQueryLboxSelectedItem( WinWindowFromID( handle, id ))

  #define SET_CHECKBOX( handle, id, value ) \
          WinSendDlgItemMsg( handle, id, BM_SETCHECK, MPFROMLONG(value), 0 )
  #define GET_CHECKBOX( handle, id        ) \
          SHORT1FROMMR( WinSendDlgItemMsg( handle, id, BM_QUERYCHECK, 0, 0 ))

  #define SET_RADIOBOX( handle, id, value ) \
          WinSendDlgItemMsg( handle, id, BM_SETCHECK, MPFROMLONG(value), 0 )
  #define GET_RADIOBOX( handle, id        ) \
          SHORT1FROMMR( WinSendDlgItemMsg( handle, id, BM_QUERYCHECKINDEX, 0, 0 ))

  #define SET_SPIN( handle, id, value  ) \
          WinSendDlgItemMsg( handle, id, SPBM_SETCURRENTVALUE, MPFROMLONG(value), 0 )
  #define GET_SPIN( handle, id, value  ) \
          WinSendDlgItemMsg( handle, id, SPBM_QUERYVALUE, MPFROMP(value), 0 )

  #define ENABLEID( handle, id, enable ) \
          WinEnableWindow( WinWindowFromID( handle, id ), enable )

  #define KL_SETDEFAULT  (KL_USER+1)
#endif

/**
 * Setup page base class.
 *
 * The KLSetupPage provides the ability to display a individual pages in
 * {@link KLSetup setup notebook control}.
 *
 * @author Dmitry A Steklenev
 */

class KLSetupPage
{
  public:

    /**
     * Constructs the setup page.
     *
     * @param id     The identifier of the dialog template.
     * @param parent Parent notebook.
     */

    KLSetupPage( const KLSetup& parent, ULONG id );

    /** Destructs the setup page. */
    virtual ~KLSetupPage();

    /** Returns the page handle. */
    virtual HWND handle() const;
    /** Returns a reference to the parent notebook. */
    virtual const KLSetup& parent() const;

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
     * KLSetupPage calls this function when a system native window
     * event occurs.
     */

    virtual MRESULT dispatch( ULONG msg, MPARAM mp1, MPARAM mp2 );

  private:
    HWND           pg_handle;
    const KLSetup& pg_parent;

    static MRESULT _System win_proc( HWND, ULONG, MPARAM,MPARAM );
};

#endif
