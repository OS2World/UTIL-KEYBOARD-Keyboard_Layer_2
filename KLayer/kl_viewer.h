/* Keyboard Layer/2
 *
 * Viewer
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
 * $Id: kl_viewer.h,v 1.9 2005/12/06 09:38:57 glass Exp $
 */

#ifndef __KL_VIEWER_H
#define __KL_VIEWER_H

#include <kl_global.h>
#include <kl_popup.h>
#include <kl_bundle.h>

/**
 * Keyboard layer viewer.
 * @author Dmitry A Steklenev
 */

class KLViewer
{
  public:
    /** Constructs the keyboard layer viewer. */
    KLViewer( HAB hab );
    /** Destructs the keyboard layer viewer. */
    virtual ~KLViewer();

    /** Returns the viewer window handle. */
    HWND handle() const;
    /** Returns the handle of the viewer client window. */
    HWND client() const;

    /**
     * Has the input focus.
     *
     * @return If the viewer window has the input focus,
     *         TRUE is returned. Otherwise, FALSE is returned.
     */

    BOOL has_focus() const;

    /**
     * Capture mouse pointer.
     *
     * If capture is true, pointer events are sent only to viewer window
     * even if the pointer is outside of this window. If <i>capture</i>
     * is FALSE, the window releases the pointer capture.
     */

    KLViewer& capture_pointer( BOOL capture = TRUE );

    /**
     * Release mouse pointer.
     *
     * Causes the window to release the pointer capture.
     */

    KLViewer& release_pointer();

    /**
     * Has pointer captured.
     *
     * @return If this window is currently capturing pointer events,
     *         TRUE is returned. Otherwise, FALSE is returned.
     */

    BOOL has_pointer_captured() const;

    /** Makes the viewer window visible. */
    KLViewer& show( BOOL show = TRUE );

    /**
     * Is the viewer window visible.
     *
     * If the window's style is set to visible, true is returned.
     * Otherwise, false is returned.
     * <p>
     * <b>Note:</b> A window can have the style visible and yet not be showing
     * if it is covered by another window.
     */

    BOOL is_visible() const;

    /**
     * Saves the current position and size of the viewer window.
     *
     * This method will also save the presentation parameters
     * of the viewer window.
     */

    void save();

    /**
     * Restores the current position and size of the viewer window.
     *
     * This method will also restore the presentation parameters
     * of the viewer window.
     */

    void load();

    /** Removes the layer viewer entry from the window list. */
    KLViewer& remove_entry ( BOOL remove  = TRUE );
    /** Forces the viewer window to float on top of all windows. */
    KLViewer& float_on_top ( BOOL ontop   = TRUE );
    /** Forces the viewer window to transit input focus. */
    KLViewer& transit_focus( BOOL transit = TRUE );
    /** Forces the viewer window to show the layer icon. */
    KLViewer& show_icon    ( BOOL show    = TRUE );
    /** Find the first instance of the viewer window. */
    static HWND find();

    /**
     * Adds the whole viewer window to a window's update region.
     *
     * The update region is a subregion of a window that is
     * deemed "invalid" or incorrect in visual terms and is
     * in need of redrawing.
     */

    KLViewer& invalidate();

  protected:

    /**
     * Dispatch system native events.
     *
     * KLViewer calls this function when a system native window
     * event occurs.
     */

    virtual MRESULT dispatch( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 );

  private:
    void paint( HWND hwnd );
    void adjust_size();
    void synchronize();

    HWND     v_hviewer;
    HWND     v_hclient;
    KLPopUp* v_popup_full;
    KLPopUp* v_popup_mini;
    KLBroker v_broker;
    KLBundle v_bundle;
    BOOL     v_has_focus;
    ULONG    v_layer_id;
    POINTL   v_mouse_pos;
    BOOL     v_remove;
    BOOL     v_float_top;
    BOOL     v_transit;
    BOOL     v_showicon;

    static ULONG v_initialized;
    static MRESULT _System win_proc( HWND, ULONG, MPARAM, MPARAM );
};

/*---------------------------------------------------
 * Returns the viewer window handle
 *--------------------------------------------------*/
inline HWND KLViewer::handle() const
{
  return v_hviewer;
}

/*---------------------------------------------------
 * Returns the handle of the viewer client window
 *--------------------------------------------------*/
inline HWND KLViewer::client() const
{
  return v_hclient;
}

/*---------------------------------------------------
 * Has the input focus
 *--------------------------------------------------*/
inline BOOL KLViewer::has_focus() const
{
  return v_has_focus;
}

#endif
