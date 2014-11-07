/* Keyboard Layer/2
 *
 * Trigger hotkeys management class
 * Copyright (C) 2001 Dmitry A.Steklenev
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

#ifndef  __KL_TRIGGER_H
#define  __KL_TRIGGER_H

#include <kl_global.h>

#if !defined(__ccdoc__)

#define  XS_LCONTROL  0x0000    // LEFT  CONTROL
#define  XS_LCTRANDS  0x0001    // LEFT  CONTROL AND SHIFT
#define  XS_LSHIFT    0x0002    // LEFT  SHIFT
#define  XS_SHIFTS    0x0003    // LEFT  SHIFT   AND RIGHT SHIFT
#define  XS_LWIN      0x0004    // LEFT  WINDOWS
#define  XS_RCONTROL  0x0005    // RIGHT CONTROL
#define  XS_RCTRLSH   0x0006    // RIGHT CONTROL OR  RIGHT SHIFT
#define  XS_RCTRANDS  0x0007    // RIGHT CONTROL AND SHIFT
#define  XS_RSHIFT    0x0008    // RIGHT SHIFT
#define  XS_RWIN      0x0009    // RIGHT WINDOWS
#define  XS_ALTSH     0x000A    // ALT AND SHIFTs
#define  XS_WINMENU   0x000B    // WINDOWS MENU
#define  XS_DONTTOUCH 0x000C    // Don't touch my keyboard

#define  CS_LCONTROL      29    // Left Control   0x1D
#define  CS_RCONTROL      91    // Right Control  0x5B
#define  CS_LSHIFT        42    // Left Shift     0x2A
#define  CS_RSHIFT        54    // Right Shift    0x36
#define  CS_LALT          56    // Left Alt       0x38
#define  CS_RALT          94    // Right Alt      0x5E
#define  CS_WINMENU      124    // Windows Menu   0x7C
#define  CS_LWIN         126    // Left Windows   0x7E
#define  CS_RWIN         127    // Right Windows  0x7F
#define  CS_UP           128    // Key released   0x80

#endif
#if defined(__cplusplus) || defined(__ccdoc__)

/**
 * Trigger hotkeys management class.
 *
 * On keyboards used for bidirectional languages, two characters
 * (a Latin one and a "National Language" one) are engraved on each
 * key and the user can select which of these characters is used,
 * by selecting the active keyboard language (or "Keyboard Layer").
 *
 * Changing the keyboard layer can be done by the user pressing
 * one of the "trigger" hotkeys.
 *
 * Note that there is no need to create an instance of this class
 * because all methods are static.
 *
 * @author Dmitry A Steklenev
 */

class KLTrigger
{
  public:

    /**
     * Selects the trigger hotkeys.
     *
     * Use these definitions to specify trigger hotkeys:
     *
     * <dl>
     * <dt><i>XS_LCONTROL</i><dd>LEFT  CONTROL
     * <dt><i>XS_LCTRANDS</i><dd>LEFT  CONTROL AND SHIFT
     * <dt><i>XS_LSHIFT  </i><dd>LEFT  SHIFT
     * <dt><i>XS_SHIFTS  </i><dd>LEFT  SHIFT   AND RIGHT SHIFT
     * <dt><i>XS_LWIN    </i><dd>LEFT  WINDOWS
     * <dt><i>XS_RCONTROL</i><dd>RIGHT CONTROL
     * <dt><i>XS_RCTRLSH </i><dd>RIGHT CONTROL OR  RIGHT SHIFT
     * <dt><i>XS_RCTRANDS</i><dd>RIGHT CONTROL AND SHIFT
     * <dt><i>XS_RSHIFT  </i><dd>RIGHT SHIFT
     * <dt><i>XS_RWIN    </i><dd>RIGHT WINDOWS
     * <dt><i>XS_ALTSH   </i><dd>ALT AND SHIFTs
     * <dt><i>XS_WINMENU </i><dd>WINDOWS MENU
     * <dt><i>XS_DONTME  </i><dd>Don't touch my keyboard
     * </dl>
     */

    static void select( int type );

    /**
     * Returns the trigger hotkeys.
     *
     * @return See {@link KLTrigger::select select} for possible
     *         trigger hotkeys.
     */

    static int selected();

    /**
     * Dispatches the next pressed or released key.
     *
     * @param  scancode       Hardware scan code of the pressed key.
     * @param  current_layer  Current keyboard layer, KL_LATIN or KL_NATIONAL.
     * @param  time           Current time in tenths of a seconds.
     *
     * @return If the hotkey is pressed
     *         returns KL_LATIN or KL_NATIONAL. Otherwise returns KL_NONE.
     */

    static ULONG dispatch( BYTE scancode, ULONG current_layer, ULONG time );

    /**
     * Sets the current timeout value.
     *
     * Sets timeout for switching combination in tenths of a seconds.
     * If switching sequence won't be completed during defined time
     * interval, layer switching won't happen.
     */

    static void set_timeout( ULONG time );

    /** Returns the current timeout value. */
    static ULONG timeout();

  private:
    struct sequence {
      BYTE key_1st;
      BYTE key_2st;
    };

    static sequence sw_latin;
    static sequence sw_natio;
    static sequence sw_current;
    static ULONG    sw_time;
    static ULONG    sw_timeout;
    static int      sw_hotkey;
};

#else
    typedef struct {
      BYTE key_1st;
      BYTE key_2st;

    } htrg_sequence;

    #pragma alloc_text( CSWAP_TEXT, htrg_select      )
    #pragma alloc_text( CSWAP_TEXT, htrg_selected    )
    #pragma alloc_text( CSWAP_TEXT, htrg_dispatch    )
    #pragma alloc_text( CSWAP_TEXT, htrg_set_timeout )
    #pragma alloc_text( CSWAP_TEXT, htrg_timeout     )

    extern void  htrg_select     ( int type );
    extern int   htrg_selected   ( void );
    extern ULONG htrg_dispatch   ( BYTE scancode, ULONG current_layer, ULONG time );
    extern void  htrg_set_timeout( ULONG time );
    extern ULONG htrg_timeout    ( void );
#endif
#endif
