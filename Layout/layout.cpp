/* Keyboard Layer/2
 *
 * Select a keyboard layout
 * Copyright (C) 1999 by Dmitry A.Steklenev
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

#include "getopt.h"
#include "kl_broker.h"
#include "kl_global.h"

#define  INCL_DOS
#define  INCL_ERRORS
#include <os2.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream.h>
#include <iomanip.h>

/*--------------------------------------------------
 * Errors
 *--------------------------------------------------*/
void error( ULONG error_code )
{
  char  msg[2048];
  ULONG len;

  DosGetMessage( 0, 0, msg, sizeof(msg), error_code, "OsO001.msg", &len );
  cerr << msg << endl;

  exit(1);
}

/*--------------------------------------------------
 * Usage
 *--------------------------------------------------*/
static void usage(void)
{
  fprintf( stderr, "Usage: layout <layout_name> [-c <codepage>] [-a]\n\n"

                   "  -c list available layouts for specified codepage\n"
                   "  -a list all available layouts\n"
         );

  exit(1);
}

/*--------------------------------------------------
 * Start here
 *--------------------------------------------------*/
void main( int argc, char **argv )
{
  cout << "Select a Keyboard Layout/2 Version "
       << VER_MAJOR << '.'
       << VER_MINOR << '.'
       << VER_LEVEL
       << endl
       << "Copyright (C) 1999-2004 by Dmitry A.Steklenev"
       << endl
       << endl;

  KLBroker broker;

  const KLBroker::cp_entry* cpList = broker.cp_list();
  int   c,
        cpage = cpList->selected;
  char  select_layout[_MAX_LNAME] = "";

  while(( c = getopt( argc, argv, "c:ah?")) != -1 )
  {
    switch( c ) {
      case 'c': {
        if( !(cpage = atoi(optarg)))
          usage();
        break;
      }
      case 'a': {
        cpage = 0;
        break;
      }
      default:
        usage();
    }
  }

  while( optind < argc &&
         strlen(argv[optind]) < sizeof(select_layout) - strlen(select_layout) - 1)
  {
    strcat( select_layout, argv[optind++] );
  }

  if( optind < argc )
    usage();

  if( broker.error_code() != NO_ERROR )
    error( broker.error_code());

  // Display keyboard type

  KLBroker::kb_type keytype = broker.keyboard();

  cout << setw(15)
       << setiosflags( ios::left )
       << "Keyboard type"
       << " : "
       << keytype;

  switch( keytype )
  {
    case 0:
      cout << " (Personal computer AT keyboard)";
      break;

    case 1:
      cout << " (Enchanced keyboard)";
      break;

    default:
      cout << " (Unknown keyboard)";
  }

  cout << endl;

  // Display current and prepared codepages

  if( broker.error_code() != NO_ERROR )
    error( broker.error_code());

  cout << setw(15)
       << "Codepages"
       << " : "
       << "current "
       << cpList->selected;

  if( cpList->size )
  {
    cout << ", prepared";

    for( int i = 0; i < cpList->size; i++ )
      cout << " " << cpList->prepared[i];
  }

  cout << endl;

  // Display keyboard table file

  cout << setw(15)
       << "Keyboard file"
       << " : "
       << broker.table_name()
       << endl;

  if( broker.error_code() != NO_ERROR )
    error( broker.error_code());

  // Display keyboard layout

  cout << setw(15)
       << "Keyboard layout"
       << " : "
       << broker.selected()
       << endl;

  if( broker.error_code() != NO_ERROR )
    error( broker.error_code());

  // Set keyboard layout

  if( strlen(select_layout))
  {
    cout << endl;
    broker.select( select_layout, TRUE );
    exit(0);
  }

  // Display prepared keyboard layouts

  cout << endl
       << "List of prepared keyboard layouts ";

  if( cpage )
  {
    cout << "("
         << cpage
         << ")";
  }

  cout << ":"
       << endl
       << endl;

  KLBroker::kl_entry entry;
  broker.table_open();

  if( broker.error_code() != NO_ERROR )
    error( broker.error_code());

  while( broker.next( &entry, cpage ))
  {
    cout << "* "
         << setw(8)
         << entry.name
         << "  "
         << setw(4)
         << entry.cp;

    if( entry.dflag & DFLAG_DEFAULTFORCTRY )
      cout << " [default]";
    else
      cout << "          ";

    if( entry.dflag & DFLAG_MULTILAYERED )
      cout << " [multilayered]";

    cout << endl;
  }

  if( broker.error_code() != NO_ERROR )
    error( broker.error_code());

  broker.table_close();
}

