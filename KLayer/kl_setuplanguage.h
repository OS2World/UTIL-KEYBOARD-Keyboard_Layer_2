/* Keyboard Layer/2
 *
 * Setup page of the national language
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
 * $Id: kl_setuplanguage.h,v 1.4 2005/10/01 08:42:52 glass Exp $
 */

#ifndef __KL_SETUPLANGUAGE_H
#define __KL_SETUPLANGUAGE_H

#include <kl_setuppage.h>

/**
 * Setup page of the national language.
 * @author Dmitry A Steklenev
 */

class KLSetupLanguage : public KLSetupPage
{
  public:

    /**
     * Constructs the setup page.
     * @param parent Parent notebook.
     */

    KLSetupLanguage( const KLSetup& parent );

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
     * KLSetupLanguage calls this function when a system native window
     * event occurs.
     */

    virtual MRESULT dispatch( ULONG msg, MPARAM mp1, MPARAM mp2 );
};

#endif
