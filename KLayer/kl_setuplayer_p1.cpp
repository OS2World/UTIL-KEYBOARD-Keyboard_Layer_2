/* Keyboard Layer/2
 *
 * Setup page 01 of the keyboard layer
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
 * $Id: kl_setuplayer_p1.cpp,v 1.12 2005/10/01 08:42:52 glass Exp $
 */

#define  INCL_WIN
#include <os2.h>

#include "kl_setuplayer_p1.h"
#include "kl_ids.h"
#include "kl_messages.h"
#include "kl_engine.h"
#include "kl_pipe.h"
#include "kl_systray.h"
#include "kl_application.h"

/*---------------------------------------------------
 * Constructs the setup page
 *--------------------------------------------------*/
KLSetupLayer01::KLSetupLayer01( const KLSetup& parent )

: KLSetupPage( parent, ID_WIN_LAYER01 )
{
  SET_TEXT( handle(), ID_LAY_FRAME01,  MSG_SWFRAME     );
  SET_TEXT( handle(), ID_LAY_BEEP,     MSG_BEEPAFTER   );
  SET_TEXT( handle(), ID_LAY_PIPES,    MSG_ENABLEPIPE  );
  SET_TEXT( handle(), ID_LAY_VDMCOLOR, MSG_VDMCOLOR    );
  SET_TEXT( handle(), ID_LAY_TIMETXT,  MSG_TIMEOUT     );
  SET_TEXT( handle(), ID_LAY_SYSTRAY,  MSG_SYSTRAY     );
  ADD_ITEM( handle(), ID_LAY_SWITCH,   MSG_SWLCONTROL  );
  ADD_ITEM( handle(), ID_LAY_SWITCH,   MSG_SWLCTRANDS  );
  ADD_ITEM( handle(), ID_LAY_SWITCH,   MSG_SWLSHIFT    );
  ADD_ITEM( handle(), ID_LAY_SWITCH,   MSG_SWSHIFTS    );
  ADD_ITEM( handle(), ID_LAY_SWITCH,   MSG_SWLWIN      );
  ADD_ITEM( handle(), ID_LAY_SWITCH,   MSG_SWRCONTROL  );
  ADD_ITEM( handle(), ID_LAY_SWITCH,   MSG_SWRCTRLSH   );
  ADD_ITEM( handle(), ID_LAY_SWITCH,   MSG_SWRCTRANDS  );
  ADD_ITEM( handle(), ID_LAY_SWITCH,   MSG_SWRSHIFT    );
  ADD_ITEM( handle(), ID_LAY_SWITCH,   MSG_SWRWIN      );
  ADD_ITEM( handle(), ID_LAY_SWITCH,   MSG_SWALTSH     );
  ADD_ITEM( handle(), ID_LAY_SWITCH,   MSG_SWWINMENU   );
  ADD_ITEM( handle(), ID_LAY_SWITCH,   MSG_SWDONTTOUCH );
  SEL_ITEM( handle(), ID_LAY_SWITCH,   ini_get_short( INI_SWITCH, XS_RCONTROL ));

  SET_CHECKBOX( handle(), ID_LAY_BEEP,     ini_get_long( INI_BEEP,     FALSE ));
  SET_CHECKBOX( handle(), ID_LAY_SYSTRAY,  ini_get_long( INI_SYSTRAY,  FALSE ));
  SET_CHECKBOX( handle(), ID_LAY_PIPES,    ini_get_long( INI_PIPES,    FALSE ));
  SET_CHECKBOX( handle(), ID_LAY_VDMCOLOR, ini_get_long( INI_VDMCOLOR, FALSE ));
  SET_SPIN    ( handle(), ID_LAY_TIMEOUT,  ini_get_long( INI_TIMEOUT,  0     ));

  ENABLEID( handle(), ID_LAY_VDMCOLOR, is_vdm_supported());
  KLApp::do_warpsans( handle(), ID_LAY_TIMETXT );
}

/*---------------------------------------------------
 * Save page preferences
 *--------------------------------------------------*/
void KLSetupLayer01::save()
{
  LONG timeout;
  GET_SPIN( handle(), ID_LAY_TIMEOUT, &timeout );

  ini_put_short( INI_SWITCH,   GET_ITEM    ( handle(), ID_LAY_SWITCH   ));
  ini_put_long ( INI_BEEP,     GET_CHECKBOX( handle(), ID_LAY_BEEP     ));
  ini_put_long ( INI_PIPES,    GET_CHECKBOX( handle(), ID_LAY_PIPES    ));
  ini_put_long ( INI_VDMCOLOR, GET_CHECKBOX( handle(), ID_LAY_VDMCOLOR ));
  ini_put_long ( INI_SYSTRAY,  GET_CHECKBOX( handle(), ID_LAY_SYSTRAY  ));
  ini_put_long ( INI_TIMEOUT,  timeout );
}

/*---------------------------------------------------
 * Applies page preferences
 *--------------------------------------------------*/
void KLSetupLayer01::apply( HWND hwnd )
{
  set_trigger_hotkeys( ini_get_short( INI_SWITCH,   XS_RCONTROL  ));
  set_trigger_timeout( ini_get_long ( INI_TIMEOUT,  0            ));
  set_beeps          ( ini_get_long ( INI_BEEP,     FALSE        ));
  set_vdm_synchronize( ini_get_long ( INI_VDMCOLOR, FALSE        ));

  KLPipe::enable( ini_get_long ( INI_PIPES, FALSE ));
  KLSysTray::enable( hwnd, ini_get_long( INI_SYSTRAY, FALSE ));

  // FIX ME!!! set_vdm_color( bundle.vdm_border());
}

/*---------------------------------------------------
 * Dispatch system native events
 *--------------------------------------------------*/
MRESULT KLSetupLayer01::dispatch( ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  switch ( msg )
  {
    case KL_SETDEFAULT:

      SEL_ITEM    ( handle(), ID_LAY_SWITCH,   XS_RCONTROL );
      SET_CHECKBOX( handle(), ID_LAY_BEEP,     FALSE       );
      SET_CHECKBOX( handle(), ID_LAY_PIPES,    FALSE       );
      SET_CHECKBOX( handle(), ID_LAY_VDMCOLOR, FALSE       );
      SET_CHECKBOX( handle(), ID_LAY_SYSTRAY , FALSE       );
      SET_SPIN    ( handle(), ID_LAY_TIMEOUT,  0           );

      return MRFROMLONG( TRUE );

    case WM_CONTROL:
      if( SHORT2FROMMP(mp1) == CBN_EFCHANGE && SHORT1FROMMP(mp1) == ID_LAY_SWITCH )
      {
        BOOL enable_switch = GET_ITEM( handle(), ID_LAY_SWITCH ) != XS_DONTTOUCH;

        ENABLEID( handle(), ID_LAY_BEEP   , enable_switch );
        ENABLEID( handle(), ID_LAY_TIMEOUT, enable_switch );
        ENABLEID( handle(), ID_LAY_TIMETXT, enable_switch );
      }
      break;
  }

  return KLSetupPage::dispatch( msg, mp1, mp2 );
}

