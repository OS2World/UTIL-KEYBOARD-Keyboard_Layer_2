/* Keyboard Layer/2
 *
 * Engine
 * Copyright (C) 1998 by Dmitry A.Steklenev
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

#ifndef  __KL_ENGINE_H
#define  __KL_ENGINE_H

  /**
   * Engine API.
   *
   * @author Dmitry A Steklenev
   * @pkgdoc Engine
   */

#include <kl_global.h>
#include <kl_profiles.h>
#include <kl_trigger.h>
#include <kl_hotkey.h>
#include <os2def.h>

  /**
   * This message occurs when a viewer window is overlapped by
   * another window.
   *
   * <dl>
   * <dt><i>Param1</i><dd>NULL
   * <dt><i>Param2</i><dd>NULL
   * </dl>
   */

  #define KL_FLOATTOTOP (WM_USER+1)

  /**
   * This message occurs when a keyboard layer is changed.
   *
   * <dl>
   * <dt><i>Param1</i><dd>ULONG keyboard layer ID
   * <dt><i>Param2</i><dd>NULL
   * </dl>
   */

  #define KL_LAYERCHANGED (WM_USER+2)

  /**
   * This message occurs when a new keyboard layout is selected.
   *
   * <dl>
   * <dt><i>Param1</i><dd>NULL
   * <dt><i>Param2</i><dd>NULL
   * </dl>
   */

  #define KL_SETLAYOUT (WM_USER+3)

  /**
   * All messages below this value are reserved for engine use.
   *
   * Private messages must have an identifier with a value of
   * KL_USER or higher.
   */

  #define KL_USER (WM_USER+100)

#if defined(__cplusplus)
extern "C" {
#endif

  /**
   * Startups the Keyboard Layer/2 engine.
   *
   * @param hab   Program anchor block handle.
   * @param hwnd  Windows handle of the layer viewer.
   */

  BOOL EXPENTRY engine_start( HAB hab, HWND hwnd );

  /**
   * Stops the Keyboard Layer/2 engine.
   *
   * @param hab   Program anchor block handle.
   */

  void EXPENTRY engine_stop( HAB hab );

  /**
   * Enables the float on top mode for window of the layer viewer.
   *
   * If enabled, causes a post {@link KL_FLOATTOTOP KL_FLOATTOTOP}
   * message to a viewer window when it is overlapped by another
   * window.
   */

  void EXPENTRY set_float_on_top( BOOL enable );

  /** Enables sound signals at switching keyboard layers. */
  void EXPENTRY set_beeps( BOOL enable );

  /**
   * Enables synchronization of the VDM colors with the layer viewer.
   *
   * Does not affect the engine. Use {@link set_vdm_color set_vdm_color} to set
   * a VDM border color.
   */

  void EXPENTRY set_vdm_synchronize( BOOL enable );

  /** Sets the VDM colors. */
  void EXPENTRY set_vdm_color( LONG rgb );
  /** Sets the keyboard layer for newly created windows. */
  void EXPENTRY set_default_layer( ULONG layer_id );
  /** Sets the keyboard layer for newly selected layouts. */
  void EXPENTRY set_layouts_layer( ULONG layer_id );

  /**
   * Selects the trigger hotkeys.
   *
   * See {@link KLTrigger::select KLTrigger::select} for possible
   * trigger hotkeys.
   */

  void EXPENTRY set_trigger_hotkeys( int type );

  /**
   * Sets the trigger timeout value.
   *
   * Sets timeout for switching combination in tenths of a seconds.
   * If switching sequence won't be completed during defined time
   * interval, layer switching won't happen.
   */

  void EXPENTRY set_trigger_timeout( ULONG time );

  /**
   * Selects the keyboard layout.
   *
   * Causes a post {@link KL_SETLAYOUT KL_SETLAYOUT}
   * message to a viewer window after keyboard layout selection.
   */

  BOOL EXPENTRY set_layout( const char* name );

  /**
   * If the float on top mode is enabled, TRUE is returned.
   *
   * If enabled, causes a post {@link KL_FLOATTOTOP KL_FLOATTOTOP}
   * message to a viewer window when it is overlapped by another
   * window.
   */

  BOOL EXPENTRY is_float_on_top_enabled();

  /** If the sound signals at switching is enabled, TRUE is returned. */
  BOOL EXPENTRY is_beeps_enabled();
  /** If the VDM is supported, TRUE is returned. */
  BOOL EXPENTRY is_vdm_supported();
  /** If the current selected keyboard layout multilayered, TRUE is returned. */
  BOOL EXPENTRY is_multilayered();

  /**
   * If the VDM colors is synchronized with the layer viewer, TRUE is returned.
   *
   * Does not affect the engine. Use {@link set_vdm_color set_vdm_color} to set
   * a VDM border color.
   */

  BOOL EXPENTRY is_vdm_synchronized();

  /** Returns the keyboard layer for newly created windows. */
  ULONG EXPENTRY get_default_layer();
  /** Returns the keyboard layer for newly selected layouts. */
  ULONG EXPENTRY get_layouts_layer();

  /**
   * Returns the trigger hotkeys.
   *
   * @return See {@link KLTrigger::select KLTrigger::select} for possible
   *         trigger hotkeys.
   */

  int EXPENTRY get_trigger_hotkeys();

  /**
   * Returns the trigger timeout value.
   *
   * Returns timeout for switching combination in tenths of a seconds.
   * If switching sequence won't be completed during defined time
   * interval, layer switching won't happen.
   */

  ULONG EXPENTRY get_trigger_timeout();

  /** Clears hotkeys list. */
  void EXPENTRY hkey_clear();
  /** If the hotkey list is empty, TRUE is returned. */
  BOOL EXPENTRY hkey_empty();
  /** Disables hotkeys processing for specified window. */
  HWND EXPENTRY hkey_exclude( HWND hwnd );

  /**
   * Finds layout assigned to a specified hotkey.
   *
   * @return A pointer to layout name or NULL if
   *         specified hotkey not have any layout assigned.
   */

  const char* EXPENTRY hkey_find( const KLHotkey& hotkey );

  /**
   * Appends another hotkey for a selection of a keyboard layout.
   *
   * @return  If the call is successful, TRUE is returned.
   *          Otherwise, FALSE is returned.
   */

  BOOL EXPENTRY hkey_assign( const KLHotkey& hotkey, const char* layout_name );

#if defined(__cplusplus)
}
#endif
#endif

