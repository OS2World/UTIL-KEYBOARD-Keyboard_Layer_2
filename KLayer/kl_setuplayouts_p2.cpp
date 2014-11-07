/* Keyboard Layer/2
 *
 * Setup page 02 of the keyboard layouts
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
 * $Id: kl_setuplayouts_p2.cpp,v 1.2 2005/10/01 08:42:53 glass Exp $
 */

#define  INCL_WIN
#define  INCL_ERRORS
#include <os2.h>
#include <string.h>

#include "kl_setuplayouts_p2.h"
#include "kl_ids.h"
#include "kl_messages.h"

/*---------------------------------------------------
 * Constructs the setup page
 *--------------------------------------------------*/
KLSetupLayouts02::KLSetupLayouts02( const KLSetup& parent )

: KLSetupPage( parent, ID_WIN_LAYOUT02 )
{
  SET_TEXT( handle(), ID_LOU_FRAME02,  MSG_SETAFTER    );
  SET_TEXT( handle(), ID_LOU_LATIN,    MSG_SETLATIN    );
  SET_TEXT( handle(), ID_LOU_NATIONAL, MSG_SETNATIONAL );
  SET_TEXT( handle(), ID_LOU_NOCHANGE, MSG_SETNOCHANGE );

  ULONG layouts_layer = ini_get_long( INI_LAY_LAYER, KL_NONE );

  if( layouts_layer == KL_NONE )
      layouts_layer = 2;

  SET_RADIOBOX( handle(), ID_LOU_LATIN+layouts_layer, TRUE );
}

/*---------------------------------------------------
 * Save page preferences
 *--------------------------------------------------*/
void KLSetupLayouts02::save()
{
  ULONG layouts_layer = GET_RADIOBOX( handle(), ID_LOU_NOCHANGE );

  if( layouts_layer != KL_LATIN && layouts_layer != KL_NATIONAL )
      layouts_layer  = KL_NONE;

  ini_put_long( INI_LAY_LAYER, layouts_layer );
}

/*---------------------------------------------------
 * Applies page preferences
 *--------------------------------------------------*/
void KLSetupLayouts02::apply( HWND hwnd )
{
  set_layouts_layer( ini_get_long( INI_LAY_LAYER, KL_NONE ));
}

/*---------------------------------------------------
 * Dispatch system native events
 *--------------------------------------------------*/
MRESULT KLSetupLayouts02::dispatch( ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  switch( msg )
  {
    case KL_SETDEFAULT:
      SET_CHECKBOX( handle(), ID_LOU_NOCHANGE, TRUE );
      return MRFROMLONG(TRUE);
  }

  return KLSetupPage::dispatch( msg, mp1, mp2 );
}

