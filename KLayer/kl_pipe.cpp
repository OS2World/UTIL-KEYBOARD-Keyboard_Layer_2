/* Keyboard Layer/2
 *
 * SysBar/2 Pipe Monitor support module
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

#define  INCL_DOS
#define  INCL_ERRORS
#include <os2.h>
#include <stdio.h>
#include <string.h>

#include "kl_engine.h"
#include "kl_pipe.h"

BOOL KLPipe::np_enabled = FALSE;

/*--------------------------------------------------
 * Enables support of the SysBar/2 Pipe Monitor
 *--------------------------------------------------*/
void KLPipe::enable( BOOL enable )
{
  if( enable )
    np_enabled = TRUE;
  else
    disable();
}

/*--------------------------------------------------
 * Disables support of the SysBar/2 Pipe Monitor
 *--------------------------------------------------*/
void KLPipe::disable()
{
  clear();
  np_enabled = FALSE;
}

/*--------------------------------------------------
 * Outputs specified layer state to pipe
 *--------------------------------------------------*/
void KLPipe::write( const char* state )
{
  if( is_enabled())
  {
    HFILE  pipe;
    APIRET rc;
    ULONG  attempts = 0;
    ULONG  action   = 0;
    char   data[64];
    ULONG  written;

    while(( rc = DosOpen( "\\PIPE\\KBDLAY2", &pipe, &action, 0, 0,
                          OPEN_ACTION_OPEN_IF_EXISTS,
                          OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYNONE,
                          NULL )) == ERROR_PIPE_BUSY && attempts++ < 10 )
    {
      DosSleep( 10 );
    }

    if( rc == NO_ERROR ) {
      sprintf ( data, "%s\n", state );
      DosWrite( pipe, data, strlen(data), &written );
      DosClose( pipe );
    }
  }
}

/*--------------------------------------------------
 * Clears pipe
 *--------------------------------------------------*/
void KLPipe::clear()
{
  write( "" );
}
