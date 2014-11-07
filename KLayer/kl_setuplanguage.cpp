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
 * $Id: kl_setuplanguage.cpp,v 1.6 2005/10/01 08:42:52 glass Exp $
 */

#define  INCL_WIN
#include <os2.h>

#include "kl_setuplanguage.h"
#include "kl_ids.h"
#include "kl_messages.h"
#include "kl_engine.h"

/*---------------------------------------------------
 * Constructs the setup page
 *--------------------------------------------------*/
KLSetupLanguage::KLSetupLanguage( const KLSetup& parent )

: KLSetupPage( parent, ID_WIN_LANGUAGE )
{
  SET_TEXT( handle(), ID_LNG_FRAME,  MSG_LNGSELECT  );
  ADD_ITEM( handle(), ID_LNG_SELECT, MSG_LNGAUTO    );
  ADD_ITEM( handle(), ID_LNG_SELECT, MSG_LNGRUSSIAN );
  ADD_ITEM( handle(), ID_LNG_SELECT, MSG_LNGENGLISH );
  SEL_ITEM( handle(), ID_LNG_SELECT, ini_get_short( INI_LANGUAGE, 0 ));
}

/*---------------------------------------------------
 * Save page preferences
 *--------------------------------------------------*/
void KLSetupLanguage::save()
{
  ini_put_short( INI_LANGUAGE, GET_ITEM( handle(), ID_LNG_SELECT ));
}

/*---------------------------------------------------
 * Applies page preferences
 *--------------------------------------------------*/
void KLSetupLanguage::apply( HWND hwnd )
{
  SHORT country;
  SHORT language = ini_get_short( INI_LANGUAGE, 0 );

  switch( language )
  {
    case  1: country = 7; break;
    case  2: country = 1; break;
    default: country = 0;
  }

  KLMsg::select_country( country );
}

/*---------------------------------------------------
 * Dispatch system native events
 *--------------------------------------------------*/
MRESULT KLSetupLanguage::dispatch( ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  switch ( msg )
  {
    case KL_SETDEFAULT:
      SEL_ITEM( handle(), ID_LNG_SELECT, 0 );
      return MRFROMLONG( TRUE );
  }

  return KLSetupPage::dispatch( msg, mp1, mp2 );
}

