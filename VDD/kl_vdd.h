/* Keyboard Layer/2
 *
 * Virtual Device Driver
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
 * $Id: kl_vdd.h,v 1.12 2005/10/01 08:43:49 glass Exp $
 */

#ifndef __LAYER_VDD_H
#define __LAYER_VDD_H

  /**
   * Virtual Device Driver interface.
   *
   * @author Dmitry A Steklenev
   * @pkgdoc VDD
   */

  #if !defined(__ccdoc__)

      #pragma pack(1)
      typedef struct _ROMBIOS
      {
        BYTE NotInterested01[1047];
        BYTE rb_fbKFlag;      // 0x417
        BYTE NotInterested02[  84];
        LONG rb_ulTickCount;  // 0x46C
        BYTE NotInterested03[  39];
        BYTE rb_fbKFlag2;     // 0x497

      } ROMBIOS;
      #pragma pack()

      typedef ROMBIOS    *PROMBIOS;
      #define ROMBios  (*(PROMBIOS)0)
  #endif

  /** VDD event structure. */

  #pragma pack(1)
  struct  VLEVNT
  {
    BYTE  EventType;  //@- Event type.
    SHORT EventData;  //@- Event data.
    LONG  EventPID;   //@- Process ID.
    LONG  EventNULL;  //@- Reserved.

  };
  #pragma pack()

  /** VDD version structure. */

  #pragma pack(1)
  struct VLVERS
  {
    SHORT major;      //@- A major number.
    SHORT minor;      //@- A minor number.
    SHORT level;      //@- A patch level.

  };
  #pragma pack()

  /** VDD hotkey definition. */

  #pragma pack(1)
  struct VLHKEY
  {
    LONG scancode;    //@- The keyboard scancode.
    LONG state;       //@- The keyboard shift state.

  };
  #pragma pack()

  /**
   * Enables the VDM support.
   *
   * <dl>
   * <dt><i>Input </i><dd>BOOL enable
   * <dt><i>Output</i><dd>none
   * </dl>
   */

  #define VLCMD_ENABLE      0

  /**
   * Returns keyboard layer for specified VDM.
   *
   * <dl>
   * <dt><i>Input </i><dd>none
   * <dt><i>Output</i><dd>ULONG The keyboard layer ID
   * </dl>
   */

  #define VLCMD_GETLAYER    1

  /**
   * Clears hotkeys list.
   *
   * <dl>
   * <dt><i>Input </i><dd>none
   * <dt><i>Output</i><dd>none
   * </dl>
   */

  #define VLCMD_HKEYCLEAR   2

  /**
   * Appends another hotkey for a selection of a keyboard layout.
   *
   * <dl>
   * <dt><i>Input </i><dd>{@link VLHKEY VLHKEY} hotkey
   * <dt><i>Output</i><dd>none
   * </dl>
   */

  #define VLCMD_HKEYASSIGN  3

  /**
   * Enables sound signals at switching keyboard layers.
   *
   * <dl>
   * <dt><i>Input </i><dd>BOOL enable
   * <dt><i>Output</i><dd>none
   * </dl>
   */

  #define VLCMD_SETBEEP     4

  /**
   * Sets the VDM screen border color.
   *
   * <dl>
   * <dt><i>Input </i><dd>LONG rgb
   * <dt><i>Output</i><dd>none
   * </dl>
   */

  #define VLCMD_SETBORDER   5

  /**
   * Sets the keyboard layer for newly created VDMs.
   *
   * <dl>
   * <dt><i>Input </i><dd>ULONG The keyboard layer ID
   * <dt><i>Output</i><dd>none
   * </dl>
   */

  #define VLCMD_SETDEFLAYER 6

  /**
   * Selects keyboard layer for specified VDM.
   *
   * <dl>
   * <dt><i>Input </i><dd>ULONG The keyboard layer ID
   * <dt><i>Output</i><dd>none
   * </dl>
   */

  #define VLCMD_SETLAYER    8

  /**
   * Sets the trigger timeout value.
   *
   * Sets timeout for switching combination in tenths of a seconds.
   * If switching sequence won't be completed during defined time
   * interval, layer switching won't happen.
   *
   * <dl>
   * <dt><i>Input </i><dd>ULONG timeout
   * <dt><i>Output</i><dd>none
   * </dl>
   */

  #define VLCMD_SETTIMEOUT  9

  /**
   * Selects the trigger hotkeys.
   *
   * <dl>
   * <dt><i>Input </i><dd>USHORT See {@link KLTrigger::select KLTrigger::select}
   *                             for possible trigger hotkeys.
   * <dt><i>Output</i><dd>none
   * </dl>
   */

  #define VLCMD_SETTRIGGER 10

  /**
   * Returns the next VDD event.
   *
   * <dl>
   * <dt><i>Input </i><dd>none
   * <dt><i>Output</i><dd>{@link VLEVNT VLEVNT} event
   * </dl>
   */

  #define VLCMD_GETEVENT   11

  /**
   * Returns the VDD version.
   *
   * <dl>
   * <dt><i>Input </i><dd>none
   * <dt><i>Output</i><dd>{@link VLVERS VLVERS} version
   * </dl>
   */

  #define VLCMD_GETVERSION 12

  /**
   * Keyboard layer changed event.
   *
   * @param EventType VLEVT_LAYERCHANGED
   * @param EventData Keyboard layer ID.
   * @param EventPID  PID of VDM.
   */

  #define VLEVT_LAYERCHANGED 0

  /**
   * Keyboard layout change request event.
   *
   * @param EventType VLEVT_LAYOUTCHANGE
   * @param EventData Hotkey ID.
   * @param EventPID  PID of VDM.
   */

  #define VLEVT_LAYOUTCHANGE 1

#endif
