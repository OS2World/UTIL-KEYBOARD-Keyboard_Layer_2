/* Keyboard Layer/2
 *
 * Messages catalog management class
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

#ifndef __KL_MESSAGES_H
#define __KL_MESSAGES_H

#include <nl_types.h>
#include <kl_global.h>

/**
 * Messages catalog management class.
 *
 * Note that there is no need to create an instance of this class
 * because all methods are static.
 *
 * @author Dmitry A Steklenev
 */

class KLMsg {

  private:
     /** Destructs messages catalog. */
    ~KLMsg();

  public:
     /** Opens the messages catalog. */
     static int open_catalog( const char* catalog_file );
     /** Returns the message. */
     static const char* get( int msg_no );
     /** Selects a country. */
     static void select_country( int country = 0 );

  private:
     static nl_catd msg_catd;
     static int     msg_country;
     static KLMsg   msg_sentry;
};

#if !defined(__ccdoc__)

#define MSG_SETUP        KLMsg::get(  1)
#define MSG_ABOUT        KLMsg::get(  2)
#define MSG_HIDE         KLMsg::get(  3)
#define MSG_TASKLIST     KLMsg::get(  4)
#define MSG_TRANSPFC     KLMsg::get(  5)
#define MSG_FLOAT        KLMsg::get(  6)
#define MSG_CLOSE        KLMsg::get(  7)
#define MSG_SWFRAME      KLMsg::get(  8)
#define MSG_ENABLEPIPE   KLMsg::get(  9)
#define MSG_BEEPAFTER    KLMsg::get( 10)
#define MSG_VDMCOLOR     KLMsg::get( 11)
#define MSG_OK           KLMsg::get( 12)
#define MSG_DEFAULT      KLMsg::get( 13)
#define MSG_CANCEL       KLMsg::get( 14)
#define MSG_RUNONE       KLMsg::get( 15)
#define MSG_TIMEOUT      KLMsg::get( 16)
#define MSG_LANGUAGE     KLMsg::get( 17)
#define MSG_LNGSELECT    KLMsg::get( 18)
#define MSG_LNGRUSSIAN   KLMsg::get( 19)
#define MSG_LNGENGLISH   KLMsg::get( 20)
#define MSG_LNGAUTO      KLMsg::get( 21)
#define MSG_PAGEOF       KLMsg::get( 22)
#define MSG_LAYDEFAULT   KLMsg::get( 23)
#define MSG_LAYLATIN     KLMsg::get( 24)
#define MSG_LAYNATIONAL  KLMsg::get( 25)
#define MSG_LAYSYSTEM    KLMsg::get( 26)
#define MSG_SELLAYOUT    KLMsg::get( 27)
#define MSG_KEYPROMPT    KLMsg::get( 31)
#define MSG_CHNGLAYOUT   KLMsg::get( 32)
#define MSG_SYSTRAY      KLMsg::get( 33)
#define MSG_LAYOUTS      KLMsg::get( 34)
#define MSG_LAYOUT       KLMsg::get( 35)
#define MSG_HOTKEY       KLMsg::get( 36)
#define MSG_HK_ASSIGN    KLMsg::get( 37)
#define MSG_HK_DELETE    KLMsg::get( 38)
#define MSG_SETAFTER     KLMsg::get( 39)
#define MSG_SETLATIN     KLMsg::get( 40)
#define MSG_SETNATIONAL  KLMsg::get( 41)
#define MSG_SETNOCHANGE  KLMsg::get( 42)
#define MSG_SHOWICON     KLMsg::get( 43)

#define MSG_SWLCONTROL   KLMsg::get(500)
#define MSG_SWLCTRANDS   KLMsg::get(501)
#define MSG_SWLSHIFT     KLMsg::get(502)
#define MSG_SWSHIFTS     KLMsg::get(503)
#define MSG_SWLWIN       KLMsg::get(504)
#define MSG_SWRCONTROL   KLMsg::get(505)
#define MSG_SWRCTRLSH    KLMsg::get(506)
#define MSG_SWRCTRANDS   KLMsg::get(507)
#define MSG_SWRSHIFT     KLMsg::get(508)
#define MSG_SWRWIN       KLMsg::get(509)
#define MSG_SWALTSH      KLMsg::get(510)
#define MSG_SWWINMENU    KLMsg::get(511)
#define MSG_SWDONTTOUCH  KLMsg::get(512)
#endif

#if !defined(__ccdoc__)

#define SET_TEXT( hWnd, id, pMsg ) \
        WinSetDlgItemText( hWnd, id, pMsg )
#define SET_ITEM( hWnd, id, pMsg ) \
        WinSendMsg( hWnd, MM_SETITEMTEXT, MPFROMLONG(id), MPFROMP(pMsg))
#endif
#endif
