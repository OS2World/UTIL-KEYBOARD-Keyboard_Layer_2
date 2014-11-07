/* Keyboard Layer/2
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
 * $Id: kl_application.h,v 1.3 2005/10/01 08:42:52 glass Exp $
 */

#ifndef __KL_APPLICATION_H
#define __KL_APPLICATION_H

#include <os2def.h>
#include <kl_global.h>

/**
 * Application class.
 *
 * The KLApp class represents the program that is
 * currently running.
 *
 * @author Dmitry A Steklenev
 */

class KLApp
{
  public:
    /** Initializes the GUI facilities for use by an application. */
    KLApp();
    /** Releases all of associated resources */
   ~KLApp();

    /** Sets a presentation font for specified window. */
    static void do_warpsans( HWND handle );
    /** Sets a presentation font for specified control. */
    static void do_warpsans( HWND handle, SHORT id );

    /** Returns program anchor block handle. */
    HAB  hab() const;
    /** Returns message queue handle. */
    HMQ  hmq() const;
    /** Dispatch messages. */
    void run();

  private:
    HAB app_hab;
    HMQ app_hmq;
};

/*--------------------------------------------------
 * Returns program anchor block handle
 *--------------------------------------------------*/
inline HAB KLApp::hab() const
{
  return app_hab;
}

/*--------------------------------------------------
 * Returns message queue handle
 *--------------------------------------------------*/
inline HMQ KLApp::hmq() const
{
  return app_hmq;
}

#endif
