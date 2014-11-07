/* Keyboard Layer/2
 *
 * Context pop-up menu
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
#include <stdio.h>

#include "kl_popup.h"
#include "kl_ids.h"
#include "kl_messages.h"

/*--------------------------------------------------
 * Constructs the pop-up menu object
 *--------------------------------------------------*/
KLPopUp::KLPopUp( HWND hwnd, ULONG id )

: mn_viewer( hwnd ),
  mn_handle( WinLoadMenu( HWND_OBJECT, NULLHANDLE, id ))
{
  MENUITEM item;
  KLBroker broker;
  ULONG    cp;

  if( mn_handle )
  {
    // OS/2 always assigns identifier FID_MENU to any loaded menu.
    // Here we correct this bug.
    WinSetWindowUShort( mn_handle, QWS_ID, id );

    if( WinSendMsg( mn_handle, MM_QUERYITEM,
                    MPFROM2SHORT( ID_MNU_LAYOUT, TRUE ), MPFROMP(&item)))
    {
      HWND layouts = item.hwndSubMenu;

      if( broker.table_open() == NO_ERROR )
      {
         item.iPosition   = MIT_END;
         item.afStyle     = MIS_TEXT;
         item.afAttribute = 0;
         item.hwndSubMenu = NULLHANDLE;
         item.hItem       = NULLHANDLE;

         KLBroker::kl_entry entry;
         cp = broker.cp_list()->selected;

         for( ULONG i = 0; broker.next( &entry, cp ); i++ )
         {
           item.id = ID_LAY_FIRST + i;

           if( i % 25 == 0 )
             item.afStyle |=  MIS_BREAKSEPARATOR;
           else
             item.afStyle &= ~MIS_BREAKSEPARATOR;

           WinSendMsg( layouts, MM_INSERTITEM, MPFROMP(&item), MPFROMP(entry.name));
         }
         broker.table_close();
      }
    }
  }
}

/*--------------------------------------------------
 * Destructs the pop-up menu object
 *--------------------------------------------------*/
KLPopUp::~KLPopUp()
{
  if( mn_handle )
    WinDestroyWindow( mn_handle );
}

/*--------------------------------------------------
 * Returns a name of the selected keyboard layout
 *--------------------------------------------------*/
const char* KLPopUp::selected( ULONG id )
{
  MENUITEM item;

  WinSendMsg( mn_handle, MM_QUERYITEM,
              MPFROM2SHORT( ID_MNU_LAYOUT, TRUE ), MPFROMP(&item));

  WinSendMsg( item.hwndSubMenu, MM_QUERYITEMTEXT,
              MPFROM2SHORT(id, sizeof(mn_layout)),
              MPFROMP(&mn_layout));

  return mn_layout;
}

/*--------------------------------------------------
 * Displays the pop-up menu
 *--------------------------------------------------*/
void KLPopUp::show( LONG x, LONG y )
{
  MENUITEM item;
  KLBroker broker;

  SET_ITEM( mn_handle, ID_MNU_HIDE,     MSG_HIDE      );
  SET_ITEM( mn_handle, ID_MNU_TASKLIST, MSG_TASKLIST  );
  SET_ITEM( mn_handle, ID_MNU_TRANSPFC, MSG_TRANSPFC  );
  SET_ITEM( mn_handle, ID_MNU_FLOAT,    MSG_FLOAT     );
  SET_ITEM( mn_handle, ID_MNU_SHOWICON, MSG_SHOWICON  );
  SET_ITEM( mn_handle, ID_MNU_SETUP,    MSG_SETUP     );
  SET_ITEM( mn_handle, ID_MNU_CLOSE,    MSG_CLOSE     );
  SET_ITEM( mn_handle, ID_MNU_LAYOUT,   MSG_SELLAYOUT );

  if( WinSendMsg( mn_handle, MM_QUERYITEM,
                  MPFROM2SHORT( ID_MNU_LAYOUT, TRUE ), MPFROMP(&item)))
  {
    HWND  layouts = item.hwndSubMenu;
    LONG  icount  = (LONG)WinSendMsg( layouts, MM_QUERYITEMCOUNT, 0, 0 );
    char  itext[_MAX_LNAME];

    const char* current_layout = broker.selected();

    for( ULONG i = 0; i < icount; i++ )
    {
      WinSendMsg( layouts, MM_QUERYITEMTEXT,
                  MPFROM2SHORT( ID_LAY_FIRST+i, sizeof(itext)),
                  MPFROMP(&itext));

      WinCheckMenuItem( layouts, ID_LAY_FIRST+i,
                        stricmp( itext, current_layout ) == 0 );
    }
  }

  WinPopupMenu( mn_viewer, mn_viewer, mn_handle, x, y, ID_MNU_SETUP,
                PU_POSITIONONITEM | PU_HCONSTRAIN   | PU_VCONSTRAIN |
                PU_MOUSEBUTTON1   | PU_MOUSEBUTTON2 | PU_KEYBOARD   );
}

/*--------------------------------------------------
 * Places a select state indicator to the menu item
 *--------------------------------------------------*/
KLPopUp& KLPopUp::check( ULONG id, BOOL state )
{
  WinCheckMenuItem( mn_handle, id, state );
  return *this;
}

/*--------------------------------------------------
 * Is the menu item checked
 *--------------------------------------------------*/
BOOL KLPopUp::is_checked( ULONG id ) const
{
  return WinIsMenuItemChecked( mn_handle, id );
}

/*--------------------------------------------------
 * Makes a menu item selectable
 *--------------------------------------------------*/
KLPopUp& KLPopUp::enable( ULONG id, BOOL state )
{
  WinEnableMenuItem( mn_handle, id, state );
  return *this;
}

/*--------------------------------------------------
 * Is the menu item enabled
 *--------------------------------------------------*/
BOOL KLPopUp::is_enabled( ULONG id ) const
{
  return WinIsMenuItemEnabled( mn_handle, id );
}

