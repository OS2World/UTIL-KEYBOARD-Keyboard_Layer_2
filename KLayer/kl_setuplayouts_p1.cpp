/* Keyboard Layer/2
 *
 * Setup page 01 of the keyboard layouts
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
 * $Id$
 */

#define  INCL_WIN
#define  INCL_ERRORS
#include <os2.h>
#include <string.h>

#include "kl_setuplayouts_p1.h"
#include "kl_ids.h"
#include "kl_messages.h"

#define KL_ASSIGNHOTKEY (KL_USER+2)

/*---------------------------------------------------
 * Constructs the setup page
 *--------------------------------------------------*/
KLSetupLayouts01::KLSetupLayouts01( const KLSetup& parent )

: KLSetupPage( parent, ID_WIN_LAYOUT01 ),
  pg_hcont   ( WinWindowFromID( handle(), ID_LOU_LIST ))
{
  strncpy( pg_title01, MSG_LAYOUT, sizeof(pg_title01));
  strncpy( pg_title02, MSG_HOTKEY, sizeof(pg_title02));

  FIELDINFO* first =
    (FIELDINFO*)WinSendMsg( pg_hcont, CM_ALLOCDETAILFIELDINFO, MPFROMLONG(3), 0 );

  FIELDINFO* field = first;

  field->flData    = CFA_BITMAPORICON | CFA_VCENTER | CFA_HORZSEPARATOR | CFA_SEPARATOR;
  field->offStruct = (USHORT)(ULONG)(&(((layout*)0)->rdcore.hptrIcon));

  field = field->pNextFieldInfo;

  field->flData     = CFA_STRING | CFA_VCENTER | CFA_HORZSEPARATOR | CFA_SEPARATOR;
  field->pTitleData = pg_title01;
  field->offStruct  = (USHORT)(ULONG)(&(((layout*)0)->rdcore.pszIcon));

  field = field->pNextFieldInfo;

  field->flData     = CFA_STRING | CFA_VCENTER | CFA_HORZSEPARATOR;
  field->pTitleData = pg_title02;
  field->offStruct  = (USHORT)(ULONG)(&(((layout*)0)->hkname));

  FIELDINFOINSERT insert;

  insert.cb                   = sizeof(insert);
  insert.pFieldInfoOrder      = (FIELDINFO*)CMA_FIRST;
  insert.fInvalidateFieldInfo = TRUE;
  insert.cFieldInfoInsert     = 3;

  if( !WinSendMsg( pg_hcont, CM_INSERTDETAILFIELDINFO,
                   MPFROMP(first), MPFROMP(&insert)))
    return;

  CNRINFO cnrinfo;

  cnrinfo.cb             = sizeof(cnrinfo);
  cnrinfo.pSortRecord    = compare;
  cnrinfo.pFieldInfoLast = first->pNextFieldInfo;
  cnrinfo.flWindowAttr   = CV_DETAIL | CV_MINI | CA_DRAWICON | CA_DETAILSVIEWTITLES;
  cnrinfo.xVertSplitbar  = 100;

  WinSendMsg( pg_hcont, CM_SETCNRINFO, MPFROMP(&cnrinfo),
                        MPFROMLONG( CMA_PFIELDINFOLAST | CMA_XVERTSPLITBAR |
                                    CMA_FLWINDOWATTR   | CMA_PSORTRECORD  ));
  KLBroker broker;
  ULONG    cp;

  if( broker.table_open() == NO_ERROR )
  {
     KLBroker::kl_entry entry;
     cp = broker.cp_list()->selected;

     for( ULONG i = 0; broker.next( &entry, cp ); i++ )
     {
        layout* record = (layout*)WinSendMsg( pg_hcont, CM_ALLOCRECORD,
                                              MPFROMLONG(sizeof(layout)-sizeof(RECORDCORE)),
                                              MPFROMLONG(1));
        if( !record )
          return;

        record->bundle = new KLBundle( broker, entry.name, KL_NATIONAL );
        record->hotkey = new KLHotkey( record->bundle->hotkey());

        record->rdcore.hptrIcon = record->bundle->icon();
        record->rdcore.pszIcon  = (char*)record->bundle->name();
        record->hkname          = (char*)record->hotkey->name();

        RECORDINSERT insert;

        insert.cb                = sizeof(insert);
        insert.pRecordOrder      = (RECORDCORE*)CMA_FIRST;
        insert.pRecordParent     = 0;
        insert.fInvalidateRecord = TRUE;
        insert.zOrder            = CMA_TOP;
        insert.cRecordsInsert    = 1;

        WinSendMsg( pg_hcont, CM_INSERTRECORD, MPFROMP(record), MPFROMP(&insert));
     }
     broker.table_close();
  }

  pg_hmenu = WinLoadMenu( HWND_DESKTOP, NULLHANDLE, ID_MNU_LAYOUTS );
}

/*---------------------------------------------------
 * Destructs the setup page
 *--------------------------------------------------*/
KLSetupLayouts01::~KLSetupLayouts01()
{
  layout* record;
  while(( record = (layout*)WinSendMsg( pg_hcont, CM_QUERYRECORD, NULL,
                                        MPFROM2SHORT( CMA_FIRST, CMA_ITEMORDER ))) != NULL )
  {
    KLBundle* bundle = record->bundle;
    KLHotkey* hotkey = record->hotkey;

    WinSendMsg( pg_hcont, CM_REMOVERECORD,
                          MPFROMP(&record), MPFROM2SHORT( 1, CMA_FREE ));
    delete bundle;
    delete hotkey;
  }

  WinDestroyWindow( pg_hmenu );
}

/*---------------------------------------------------
 * Save page preferences
 *--------------------------------------------------*/
void KLSetupLayouts01::save()
{
  layout* record = (layout*)WinSendMsg( pg_hcont, CM_QUERYRECORD, NULL,
                                        MPFROM2SHORT( CMA_FIRST, CMA_ITEMORDER ));
  while( record )
  {
    record->bundle->assign_hotkey( *record->hotkey );
    record = (layout*)WinSendMsg( pg_hcont, CM_QUERYRECORD, record,
                                  MPFROM2SHORT( CMA_NEXT, CMA_ITEMORDER ));
  }
}

/*---------------------------------------------------
 * Applies page preferences
 *--------------------------------------------------*/
void KLSetupLayouts01::apply( HWND hwnd )
{
  KLBroker broker;
  ULONG    cp;

  if( broker.table_open() == NO_ERROR )
  {
     KLBroker::kl_entry entry;
     cp = broker.cp_list()->selected;

     hkey_clear();

     for( ULONG i = 0; broker.next( &entry, cp ); i++ )
     {
        KLBundle bundle( broker, entry.name );

        if( bundle.hotkey().scancode())
          hkey_assign( bundle.hotkey(), entry.name );
     }
     broker.table_close();
  }
}

/*---------------------------------------------------
 * Shows container context menu
 *--------------------------------------------------*/
void KLSetupLayouts01::menu_show( layout* record )
{
  POINTL pos;
  SWP    swp;

  WinSetWindowULong ( pg_hmenu, QWL_USER, (ULONG)record );
  WinQueryPointerPos( HWND_DESKTOP, &pos );
  WinMapWindowPoints( HWND_DESKTOP, handle(), &pos, 1 );

  if( WinWindowFromPoint( parent().handle(), &pos, TRUE ) == NULLHANDLE )
  {
    // The context menu is probably activated from the keyboard.
    WinQueryWindowPos( handle(), &swp );
    pos.x = swp.cx/2;
    pos.y = swp.cy/2;
  }

  SET_ITEM( pg_hmenu, ID_MNU_HK_ASSIGN, MSG_HK_ASSIGN );
  SET_ITEM( pg_hmenu, ID_MNU_HK_DELETE, MSG_HK_DELETE );

  WinPopupMenu( handle(), handle(), pg_hmenu, pos.x, pos.y, ID_MNU_HK_ASSIGN,
                          PU_POSITIONONITEM | PU_HCONSTRAIN   | PU_VCONSTRAIN |
                          PU_MOUSEBUTTON1   | PU_MOUSEBUTTON2 | PU_KEYBOARD   );
  if( record )
    WinSendMsg( pg_hcont, CM_SETRECORDEMPHASIS, record,
                          MPFROM2SHORT( TRUE, CRA_SOURCE ));
}

/*---------------------------------------------------
 * Dones container context menu
 *--------------------------------------------------*/
void KLSetupLayouts01::menu_done()
{
  layout* record = (layout*)WinQueryWindowULong( pg_hmenu, QWL_USER );

  if( record )
    WinSendMsg( pg_hcont, CM_SETRECORDEMPHASIS, record,
                          MPFROM2SHORT( FALSE, CRA_SOURCE ));
}

/*---------------------------------------------------
 * Assigns a hotkey for a selection of the layout
 *--------------------------------------------------*/
void KLSetupLayouts01::assign_hotkey( layout* record )
{
  if( record )
  {
    hk_manager.assign( handle(), record->hotkey );
    WinSendMsg( pg_hcont, CM_INVALIDATERECORD, &record,
                          MPFROM2SHORT( 1, CMA_TEXTCHANGED ));
  }
}

/*---------------------------------------------------
 * Assigns a default hotkey for all layouts
 *--------------------------------------------------*/
void KLSetupLayouts01::setall_hotkey()
{
  layout* record = (layout*)WinSendMsg( pg_hcont, CM_QUERYRECORD, NULL,
                                        MPFROM2SHORT( CMA_FIRST, CMA_ITEMORDER ));
  while( record )
  {
    delete_hotkey( record );
    record = (layout*)WinSendMsg( pg_hcont, CM_QUERYRECORD, record,
                                  MPFROM2SHORT( CMA_NEXT, CMA_ITEMORDER ));
  }
}

/*---------------------------------------------------
 * Deletes a hotkey for a selection of the layout
 *--------------------------------------------------*/
void KLSetupLayouts01::delete_hotkey( layout* record )
{
  if( record )
  {
    *record->hotkey = KLHotkey();
    WinSendMsg( pg_hcont, CM_INVALIDATERECORD, &record,
                          MPFROM2SHORT( 1, CMA_TEXTCHANGED ));
  }
}

/*---------------------------------------------------
 * Dispatch system native events
 *--------------------------------------------------*/
MRESULT KLSetupLayouts01::dispatch( ULONG msg, MPARAM mp1, MPARAM mp2 )
{
  switch( msg )
  {
    case WM_CHAR:
      if(  CHARMSG(&msg)->vkey == VK_DELETE    &&
           CHARMSG(&msg)->fs & (KC_VIRTUALKEY) &&
         !(CHARMSG(&msg)->fs & (KC_ALT|KC_CTRL|KC_SHIFT|KC_KEYUP|KC_PREVDOWN)))
      {
        delete_hotkey((layout*)WinSendMsg( pg_hcont, CM_QUERYRECORDEMPHASIS,
                                           MPFROMLONG(CMA_FIRST), MPFROMSHORT(CRA_SELECTED)));
        return MPFROMLONG(TRUE);
      }
      break;

    case WM_CONTROL:
      if( SHORT1FROMMP(mp1) == ID_LOU_LIST && SHORT2FROMMP(mp1) == CN_CONTEXTMENU && mp2 )
      {
        menu_show((layout*)mp2);
        return MPFROMLONG(TRUE);
      }

      if( SHORT1FROMMP(mp1) == ID_LOU_LIST && SHORT2FROMMP(mp1) == CN_ENTER && mp2 )
      {
        // Don't use direct call of the assign_hotkey() method because of strange
        // behaviour by pressing Alt-Enter and Shift-Enter.
        WinPostMsg( handle(), KL_ASSIGNHOTKEY, ((PNOTIFYRECORDENTER)mp2)->pRecord, 0 );
        return MPFROMLONG(TRUE);
      }
      break;

    case KL_ASSIGNHOTKEY:
      assign_hotkey((layout*)mp1 );
      return MPFROMLONG(TRUE);

    case WM_MENUEND:
      menu_done();
      break;

    case WM_COMMAND:
      switch( SHORT1FROMMP(mp1))
      {
        case ID_MNU_HK_ASSIGN:
          assign_hotkey((layout*)WinQueryWindowULong( pg_hmenu, QWL_USER ));
          return MPFROMLONG(TRUE);

        case ID_MNU_HK_DELETE:
          delete_hotkey((layout*)WinQueryWindowULong( pg_hmenu, QWL_USER ));
          return MPFROMLONG(TRUE);
      }
      break;

    case KL_SETDEFAULT:
      setall_hotkey();
      return MRFROMLONG(TRUE);
  }

  return KLSetupPage::dispatch( msg, mp1, mp2 );
}

/*---------------------------------------------------
 * Provides mechanisms for records comparison
 *--------------------------------------------------*/
SHORT EXPENTRY KLSetupLayouts01::compare( PRECORDCORE p1,
                                          PRECORDCORE p2, PVOID pStorage )
{
  return strcmp( p1->pszIcon, p2->pszIcon );
}

