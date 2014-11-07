/* Keyboard Layer/2
 *
 * Setup page 02 of the keyboard layer
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
 * $Id: kl_setuplayer_p2.cpp,v 1.9 2005/10/01 08:42:52 glass Exp $
 */

#define  INCL_WIN
#include <os2.h>

#include "kl_setuplayer_p2.h"
#include "kl_ids.h"
#include "kl_messages.h"
#include "kl_engine.h"
#include "kl_systray.h"

/*---------------------------------------------------
 * Constructs the setup page
 *--------------------------------------------------*/
KLSetupLayer02::KLSetupLayer02( const KLSetup& parent )

: KLSetupPage( parent, ID_WIN_LAYER02 )
{
  SET_TEXT( handle(), ID_LAY_FRAME02,  MSG_LAYDEFAULT  );
  SET_TEXT( handle(), ID_LAY_LATIN,    MSG_LAYLATIN    );
  SET_TEXT( handle(), ID_LAY_NATIONAL, MSG_LAYNATIONAL );
  SET_TEXT( handle(), ID_LAY_SYSTEM,   MSG_LAYSYSTEM   );

  ULONG default_layer = ini_get_long( INI_DEF_LAYER, KL_NONE );

  if( default_layer == KL_NONE )
      default_layer = 2;

  SET_RADIOBOX( handle(), ID_LAY_LATIN+default_layer, TRUE );
}

/*---------------------------------------------------
 * Save page preferences
 *--------------------------------------------------*/
void KLSetupLayer02::save()
{
  ULONG default_layer = GET_RADIOBOX( handle(), ID_LAY_SYSTEM );

  if( default_layer != KL_LATIN && default_layer != KL_NATIONAL )
      default_layer  = KL_NONE;

  ini_put_long( INI_DEF_LAYER, default_layer );
}

/*---------------------------------------------------
 * Applies page preferences
 *--------------------------------------------------*/
void KLSetupLayer02::apply( HWND hwnd )
{
  set_default_layer( ini_get_long( INI_DEF_LAYER, KL_NONE ));
}

/*---------------------------------------------------
 * Dispatch system native events
 *--------------------------------------------------*/
MRESULT KLSetupLayer02::dispatch( ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  switch ( msg )
  {
    case KL_SETDEFAULT:
      SET_CHECKBOX( handle(), ID_LAY_SYSTEM, TRUE );
      return MRFROMLONG( TRUE );
  }

  return KLSetupPage::dispatch( msg, mp1, mp2 );
}

