/* Keyboard Layer/2
 *
 * Keyboard layouts broker class
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

#include "kl_broker.h"

#define  INCL_WIN
#define  INCL_DOSDEVIOCTL
#define  INCL_DOS
#define  INCL_ERRORS
#include <os2.h>

#include <iomanip.h>
#include <ctype.h>

const KLBroker::info KLBroker::lb_list[] =

  {{ "BE120" , KBDL_BE       , 0                }, /* Belgium        - 120              */
   { "CF058" , KBDL_CF       , 0                }, /* Canadian       - 058 French       */
   { "DK159" , KBDL_DK       , 0                }, /* Denmark        - 159              */
   { "FR189" , KBDL_FR       , 0                }, /* France         - 189              */
   { "GR129" , KBDL_GR       , 0                }, /* Germany        - 129              */
   { "IT141" , KBDL_IT       , 0                }, /* Italy          - 141              */
   { "LA171" , KBDL_LA       , 0                }, /* Latin America  - 171              */
   { "NL143" , KBDL_NL       , 0                }, /* Netherlands    - 143              */
   { "NO155" , KBDL_NO       , 0                }, /* Norway         - 155              */
   { "PO163" , KBDL_PO       , 0                }, /* Portugal       - 163              */
   { "SF150F", KBDL_SF       , 0                }, /* Swiss-French   - 150f             */
   { "SF150G", KBDL_SG       , 0                }, /* Swiss-German   - 150d             */
   { "SP172" , KBDL_ES       , 0                }, /* Spain          - 172              */
   { "SU153" , KBDL_FI       , 0                }, /* Finland        - 153              */
   { "SV153" , KBDL_SV       , 0                }, /* Sweden         - 153              */
   { "UK166" , KBDL_UK       , 0                }, /* United Kingdom - 166              */
   { "US103" , KBDL_US       , 0                }, /* United States  - 103p             */
   { "FR120" , KBDL_FR120    , 0                }, /* French         - 120              */
   { "IT142" , KBDL_IT142    , 0                }, /* Italian        - 142              */
   { "UK168" , KBDL_UK168W   , 0                }, /* United Kingdom - 168              */
   { "TR179" , KBDL_TR       , 0                }, /* Turkey         - 179              */
   { "CZ243" , KBDL_CZ       , 0                }, /* Czech          - 243              */
   { "SK245" , KBDL_SK       , 0                }, /* Slovakia       - 245              */
   { "HU208" , KBDL_HU       , 0                }, /* Hungarian      - 208              */
   { "HR234" , KBDL_HR       , 0                }, /* Croatia        - 234              */
   { "PL214" , KBDL_PL       , 0                }, /* Poland         - 163              */
   { "IS197" , KBDL_IS       , 0                }, /* Iceland        - 197              */
   { "BR275" , KBDL_BR       , 0                }, /* Brazil         - 275              */
   { "HE212" , KBDL_HE_LATIN , KBDL_HE_NATIONAL }, /* Hebrew         - 212              */
   { "AR238" , KBDL_AR_LATIN , KBDL_AR_NATIONAL }, /* Arabic         - 238              */
   { "BR274" , KBDL_BR274    , 0                }, /* Brazil         - 274              */
   { "GK319" , KBDL_GK_LAT319, KBDL_GK_NAT319   }, /* Greek          - 319              */
   { "GK220" , KBDL_GK_LAT220, KBDL_GK_NAT220   }, /* Greek          - 220              */
   { "AR470" , KBDL_AR_LAT470, KBDL_AR_NAT470   }, /* Arabic         - 470              */
   { "TR440" , KBDL_TR440    , 0                }, /* Turkey         - 440              */
   { "SL234" , KBDL_SL       , 0                }, /* Slovenia       - 234              */
   { "RO446" , KBDL_RO       , 0                }, /* Romania        - 446              */
   { "BG442" , KBDL_BG_LATIN , KBDL_BG_NATIONAL }, /* Bulgaria       - 442              */
   { "MK449" , KBDL_MK_LATIN , KBDL_MK_NATIONAL }, /* Macedonia      - 449              */
   { "SR450" , KBDL_SR_LATIN , KBDL_SR_NATIONAL }, /* Serbia         - 450              */
   { "RU441" , KBDL_RU_LATIN , KBDL_RU_NATIONAL }, /* Russia         - 441              */
   { "PL457" , KBDL_PL274    , 0                }, /* Poland         - 274 Programmer   */
   { "RU443" , KBDL_RU_LAT443, KBDL_RU_NAT443   }, /* Russia         - 443              */
   { "BA234" , KBDL_BA       , 0                }, /* Bosnia         - 234              */
   { "SQ452" , KBDL_SQ       , 0                }, /* Albania        - 452              */
   { "UX103" , KBDL_US_INTER , 0                }, /* International  - 103              */
   { "CA445" , KBDL_CA       , 0                }, /* Canadian       - 445              */
   { "DE453" , KBDL_DE453    , 0                }, /* German         - 453              */
   { "IS458" , KBDL_IS458    , 0                }, /* Iceland        - 458              */
   { "EE454" , KBDL_EE       , 0                }, /* Estonia        - 454              */
   { "USDV " , KBDL_US_DV    , 0                }, /* US Dvorak      -                  */
   { "USDVL" , KBDL_US_LEFT  , 0                }, /* US Left        -                  */
   { "USDVR" , KBDL_US_RIGHT , 0                }, /* US Right       -                  */
   { "LT456" , KBDL_LTL      , KBDL_LTP         }, /* Lithuania      - 456              */
   { "LV455" , KBDL_LVL      , KBDL_LVP         }, /* Latvia 455     - 455              */
   { "BY463" , KBDL_BYL      , KBDL_BYC         }, /* Belarus 463    - 463              */
   { "UA465" , KBDL_UAL      , KBDL_UAU         }, /* Ukraine 465    - 465 - Ukraine    */
  };

/*--------------------------------------------------
 * Constructs objects of the broker class
 *--------------------------------------------------*/
KLBroker::KLBroker()

: lb_error ( NO_ERROR ),
  lb_handle(        0 ),
  tb_open  (        0 )
{
  lb_file[0] = 0;
}

/*--------------------------------------------------
 * Destructs objects of the broker class
 *--------------------------------------------------*/
KLBroker::~KLBroker()
{}

/*--------------------------------------------------
 * Removes leading and trailing spaces
 *--------------------------------------------------*/
char *KLBroker::strip( char *s )
{
  char *p = s;

  if( p && *p )
  {
    // removes trailing spaces
    p += strlen(s) - 1;
    while( p != s && *p == ' ' )
      *p-- = '\0';

    // removes leading spaces
    p = s;
    while( *p && *p == ' ' )
      p++;
  }
  return p;
}

/*--------------------------------------------------
 * Opens a keyboard device driver
 *--------------------------------------------------*/
ULONG KLBroker::kb_open()
{
  ULONG  action;
  return lb_error = DosOpen( "KBD$", &lb_handle, &action, 0, 0, OPEN_ACTION_OPEN_IF_EXISTS,
                              OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE, 0 );
}

/*--------------------------------------------------
 * Closes a keyboard device driver
 *--------------------------------------------------*/
ULONG KLBroker::kb_close()
{
  return lb_error = DosClose( lb_handle );
}

/*--------------------------------------------------
 * Returns a type of attached keyboard
 *--------------------------------------------------*/
KLBroker::kb_type KLBroker::keyboard()
{
  struct DATAPACKET
  {
    USHORT key_type;
    ULONG  reserved;
  };

  DATAPACKET d_packet = { 0xFFFF, 0 };
  ULONG      d_len;

  if( kb_open() == NO_ERROR )
  {
    lb_error = DosDevIOCtl( lb_handle, IOCTL_KEYBOARD, KBD_GETKEYBDTYPE,
                            0, 0, 0, (PVOID)&d_packet, sizeof(d_packet), &d_len );
    kb_close();
  }

  return KLBroker::kb_type( d_packet.key_type );
}

/*--------------------------------------------------
 * Returns a list of the prepared codepages
 *--------------------------------------------------*/
const KLBroker::cp_entry* KLBroker::cp_list()
{
  memset( &lb_cp, 0, sizeof(lb_cp));

  lb_error = DosQueryCp( sizeof(lb_cp)-sizeof(lb_cp.size), &lb_cp.selected, &lb_cp.size );

  if( !lb_error && lb_cp.size )
    lb_cp.size = lb_cp.size / sizeof(lb_cp.selected) - 1;

  return &lb_cp;
}

/*--------------------------------------------------
 * Returns a name of the selected keyboard layout
 *--------------------------------------------------*/
const char* KLBroker::selected()
{
  struct DATAPACKET
  {
    USHORT Length;
    USHORT KbdCP;
    CHAR   Ctry[20];

  } d_packet = { sizeof(d_packet), 0, "" };

  ULONG d_len;
  lb_name[0] = 0;

  if( kb_open() == NO_ERROR )
  {
    lb_error = DosDevIOCtl( lb_handle,
                            IOCTL_KEYBOARD,
                            KBD_QUERYKBDCODEPAGESUPPORT,
                            0, 0, 0,
                            &d_packet, sizeof(d_packet), &d_len );

    if( !lb_error && d_len >= 7 )
    {
      strcpy( lb_name, d_packet.Ctry );

      if( d_len >= 12 )
          strcat( lb_name, strchr( d_packet.Ctry, 0 ) + 1 );
    }

    kb_close();
    strip( lb_name );
  }

  return lb_name;
}

/*--------------------------------------------------
 * Returns a name of the keyboard layouts file
 *--------------------------------------------------*/
const char* KLBroker::table_name()
{
  if( lb_file[0] ) return lb_file;

  char     config[_MAX_PATH];
  ifstream config_stream;
  char     string[_MAX_PATH];
  char*    token;

  // Finds config.sys file
  DosQuerySysInfo( QSV_BOOT_DRIVE, QSV_BOOT_DRIVE, &config, sizeof(LONG));
  config[0] += 0x60;
  strcpy( config + 1, ":\\config.sys" );

  // Finds DEVINFO=KBD,*,LAYOUT_FILE statement in config.sys
  config_stream.open( config );
  while( config_stream )
  {
    config_stream.getline( string, sizeof(string));
    token = strtok( string, "=" );

    if( token && strcmp( strip(token), "DEVINFO" ) == 0 )
    {
      token = strtok( NULL, "," );
      if( token &&
          stricmp( strip(token), "KBD" ) == 0 &&
          strtok( NULL, ","  ))
      {
          token = strtok( NULL, "\0" );
          if( token )
          {
            strcpy( lb_file, strip( token ));
            break;
          }
      }
    }
  }

  config_stream.close();

  // If DEVINFO statement not found
  if( !lb_file[0] )
  {
    strcpy( config+2, "\\os2\\keyboard.dcp" );
    strcpy( lb_file, config );
  }

  return lb_file;
}

/*--------------------------------------------------
 * Opens a keyboard layouts file
 *--------------------------------------------------*/
ULONG KLBroker::table_open()
{
  lb_error = 0;

  if( !tb_open )
  {
    tb_file.open( table_name(), ios::in | ios::binary );

    if( tb_file )
    {
      if( tb_file.read ( (char*)&tb_entry, 4 ) &&
          tb_file.seekg( tb_entry )            &&
          tb_file.read ( (char*)&tb_size , 2 )  )
      {
        tb_entry  += 4L;
        search_pos = 0;
        search_ofs = tb_entry;
        lb_error   = 0;

        tb_open++;
      }
      else
      {
        lb_error = ERROR_READ_FAULT;
        tb_file.close();
      }
    }
    else
      lb_error = ERROR_OPEN_FAILED;
  }
  else
    ++tb_open;

  return lb_error;
}

/*--------------------------------------------------
 * Closes a keyboard layouts file
 *--------------------------------------------------*/
ULONG KLBroker::table_close()
{
  lb_error = 0;

  if( tb_open && !--tb_open )
      tb_file.close();

  return lb_error;
}

/*--------------------------------------------------
 * Returns the next keyboard layout
 *--------------------------------------------------*/
BOOL KLBroker::next( kl_entry* entry, ULONG cp, kl_type tp )
{
  lb_error = 0;

  USHORT kb    = keyboard();
  BOOL   found = FALSE;

  while( !lb_error && search_pos < tb_size && tb_open && !found )
  {
    if( tb_file.seekg( search_ofs ) && tb_file.read ((char*)entry, 18 ))
    {
      search_ofs = tb_file.tellg();
      search_pos++;

      found = ( entry->kbtype == kb
                && (entry->cp == cp || !cp )
                && (tp == any
                    || (tp == primary   && !(entry->dflag & DFLAG_SECONDARY))
                    || (tp == secondary &&  (entry->dflag & DFLAG_SECONDARY))
                   )
              );
    }
    else
      lb_error = ERROR_READ_FAULT;
  }

  if( found )
  {
    char c = entry->name[0];
    entry->name[0] = entry->name[1];
    entry->name[1] = c;
    strip( entry->name );
  }
  return found;
}

/*--------------------------------------------------
 * Returns the specified keyboard layout
 *--------------------------------------------------*/
BOOL KLBroker::find( kl_entry* entry, const char* name, ULONG cp, kl_type tp )
{
  BOOL  found    = FALSE;
  ULONG save_pos = search_pos;
  ULONG save_ofs = search_ofs;

  if( table_open() == NO_ERROR )
  {
    search_pos = 0;
    search_ofs = tb_entry;

    while( next( entry, cp, tp ))
    {
      if( stricmp( entry->name, name ) == 0
          && ( cp || ( entry->dflag & DFLAG_DEFAULTCPAGE )))
      {
        found = TRUE;
        break;
      }
    }

    search_pos = save_pos;
    search_ofs = save_ofs;

    table_close();
  }
  return found;
}

/*--------------------------------------------------
 * Sets a translation table for VIO, VDM and FS
 *--------------------------------------------------*/
ULONG KLBroker::select_fs( const kl_entry& entry, ULONG index, BOOL verbose )
{
  struct XTable
  {
    USHORT TableID;          /* Code Page ID           */
    USHORT TableFlags1;      /* Flags Word 1           */
    USHORT TableFlags2;      /* Flags Word 2           */
    USHORT TableKbdType;     /* Keyboard AT or PS/2    */
    USHORT TableKbdSubType;  /* Keyboard sub type      */
    USHORT TableLen;         /* Translate Table Length */

  } XHeader;

  char* XT = 0;
  ULONG parm_len;

  if( table_open() == NO_ERROR )
  {
    if( tb_file.seekg( entry.trans ) &&
        tb_file.read ((char*)&XHeader, sizeof(XTable)))
    {
      XT = new char[XHeader.TableLen];
      memcpy( XT, &XHeader, sizeof(XHeader));

      if( !tb_file.read( XT+sizeof(XHeader), XHeader.TableLen-sizeof(XHeader)))
        lb_error = ERROR_READ_FAULT;
    }
    else
      lb_error = ERROR_READ_FAULT;

    if( !lb_error && kb_open() == NO_ERROR )
    {
      if( index == 0 )
      {
        lb_error = DosDevIOCtl( lb_handle, IOCTL_KEYBOARD, KBD_SETTRANSTABLE,
                                XT, XHeader.TableLen, &parm_len, 0, 0, 0 );
        if( verbose )
        {
          cout << "Set VIO keyboard to "
               << entry.name
               << endl;
        }
      }
      else
      {
        struct PARMPACKET
        {
          void*  _Seg16 table_ptr;
          USHORT cp;
          USHORT index;

        } parm_packet = { XT, XHeader.TableID,
                          entry.dflag & DFLAG_SECONDARY ? index + 2 : index };

        lb_error = DosDevIOCtl( lb_handle, IOCTL_KEYBOARD, KBD_SETNLS,
                                &parm_packet, sizeof(parm_packet), &parm_len, 0, 0, 0 );

        // If load not multilayered keyboard layout,  it is necessary
        // to switch the keyboard driver to a latin layer.

        if( index == 1 && !(entry.dflag & DFLAG_MULTILAYERED))
        {
           struct PARMPACKET
           {
             USHORT cp;
             USHORT layer;

           } parm_packet = { XHeader.TableID, KL_LATIN };

           DosDevIOCtl( lb_handle, IOCTL_KEYBOARD, 0x58,
                        &parm_packet, sizeof(parm_packet), &parm_len, 0, 0, 0 );
        }


        if( verbose )
        {
          cout << "Set VIO keyboard layer "
               << parm_packet.index
               << " to "
               << entry.name
               << " codepage "
               << entry.cp
               << endl;
        }
      }
      kb_close();
    }
    table_close();
  }

  delete XT;
  return lb_error;
}

/*--------------------------------------------------
 * Sets a translation table for PM
 *--------------------------------------------------*/
BOOL KLBroker::select_pm( const kl_entry& entry, BOOL verbose )
{
  char  name[_MAX_LNAME];
  BOOL  found = FALSE;

  strcpy( name, entry.name );
  strip ( name );

  for( int i = 0; i < sizeof(lb_list)/sizeof(info); i++ )
  {
    if( stricmp( lb_list[i].name, name ) == 0 )
    {
      if( lb_list[i].sec_id && ( entry.dflag & DFLAG_MULTILAYERED ))
      {
        if( verbose )
        {
          cout << "Loading secondary PM layout "
                << name
                << " ("
                << lb_list[i].sec_id
                << ")"
                << endl;
        }

        found = WinSetKbdLayout( HWND_DESKTOP, lb_list[i].sec_id );
      }
      else if( lb_list[i].pri_id )
      {
        if( verbose )
        {
          cout << "Loading primary PM layout "
                << name
                << " ("
                << lb_list[i].pri_id
                << ")"
                << endl;
        }

        found = WinSetKbdLayout( HWND_DESKTOP, lb_list[i].pri_id );
      }

      break;
    }
  }

  return found;
}

/*--------------------------------------------------
 * Selects specifed keyboard layout.
 *--------------------------------------------------*/
ULONG KLBroker::select( const char* name, BOOL verbose )
{
  const cp_entry* cp = cp_list();
  kl_entry entry;

  if( cp->size )
  {
    for( int i = 0; i < cp->size && !lb_error; i++ )
      if( find( &entry, name, cp->prepared[i], primary ))
      {
        if( select_fs( entry, i+1, verbose ) == NO_ERROR && ( entry.dflag & DFLAG_MULTILAYERED ))
        {
          if( find( &entry, name, cp->prepared[i], secondary ))
            select_fs( entry, i+1, verbose );
        }
      }
      else
        if( verbose )
        {
          cout << "Not found VIO keyboard "
               << name
               << " for codepage "
               << cp->prepared[i]
               << endl;
        }
  }
  else
  {
    if( find( &entry, name, 0, primary ))
      select_fs( entry, 0, verbose );
  }

  if( !lb_error )
    if( !find( &entry, name, cp->selected, any ) || !select_pm( entry, verbose ))
    {
      cout << "Not found PM keyboard "
           << name
           << endl;

      lb_error = ERROR_INVALID_FUNCTION;
    }

  return lb_error;
}

/*--------------------------------------------------
 * Is specified layout multilayered.
 *--------------------------------------------------*/
BOOL KLBroker::is_multilayered( const char* name, ULONG cp )
{
  static char  cached_name[10];
  static ULONG cached_cp = 0;
  static BOOL  cached_rc = FALSE;

  kl_entry entry;

  if( strnicmp( cached_name, name, sizeof(cached_name)) != 0 || cached_cp != cp )
  {
    strncpy( cached_name, name, sizeof(cached_name));

    cached_cp = cp;
    cached_rc = FALSE;

    if( table_open() == NO_ERROR )
    {
      if( find( &entry, name, cp, primary ))
        cached_rc = entry.dflag & DFLAG_MULTILAYERED;

      table_close();
    }
  }

  return cached_rc;
}

/*--------------------------------------------------
 * Is current selected layout multilayered
 *--------------------------------------------------*/
BOOL KLBroker::is_multilayered()
{
  return is_multilayered( selected(), cp_list()->selected );
}

