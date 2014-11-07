/* Keyboard Layer/2
 *
 * About setup page
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
 * $Id: kl_setupabout.cpp,v 1.7 2005/12/05 07:46:59 glass Exp $
 */

#define  INCL_WIN
#include <os2.h>
#include <stdio.h>

#include "kl_setupabout.h"
#include "kl_ids.h"
#include "kl_messages.h"
#include "kl_application.h"

/*---------------------------------------------------
 * Constructs the setup page
 *--------------------------------------------------*/
KLSetupAbout::KLSetupAbout( const KLSetup& parent )

: KLSetupPage( parent, ID_WIN_ABOUT )
{
  char version[64];
  sprintf ( version, "Version %d.%d.%d (built %s)", VER_MAJOR, VER_MINOR, VER_LEVEL, __DATE__ );
  SET_TEXT( handle(), ID_ABO_VERSION, version  );
  KLApp::do_warpsans( handle(), ID_ABO_RIGHTS  );
  KLApp::do_warpsans( handle(), ID_ABO_VERSION );
}

