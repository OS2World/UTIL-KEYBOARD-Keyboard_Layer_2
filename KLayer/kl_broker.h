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
 * $Id: kl_broker.h,v 1.12 2005/10/01 08:42:52 glass Exp $
 */

#ifndef __KL_BROKER_H
#define __KL_BROKER_H

#include <fstream.h>
#include <stdlib.h>
#include <os2def.h>
#include <kl_global.h>

#if !defined(__ccdoc__)

#define DFLAG_DEFAULTCPAGE    0x01 // Default codepage for country
#define DFLAG_MULTILAYERED    0x02 // Multilayered layout
#define DFLAG_SECONDARY       0x04 // Secondary multilayered layout
#define DFLAG_DEFAULTFORCTRY  0x08 // Default for country
#define MFLAG_MULTIPLE        0x01 // Multiple layout for country

/** Defines maximum size of the keyboard layout name. */
#define _MAX_LNAME 10
#endif

/**
 * Keyboard layouts broker class.
 * @author Dmitry A Steklenev
 */

class KLBroker
{
  public:
    /** Constructs objects of the broker class. */
    KLBroker();
    /** Destructs objects of the broker class. */
   ~KLBroker();

    #pragma pack(1)

    /** Index entry of the keyboard translation file. */
    struct kl_entry {

      char   name[8];       //@- Includes subcountry id.
      SHORT  cp;            //@- Code page number.
      USHORT kbtype;        //@- Keyboard type.
      ULONG  trans;         //@- Offset of translate table in the file.
      UCHAR  dflag;         //@- Default translate table for this cp.
      UCHAR  mflag;         //@- Machine dependent keyboard.
    };

    /** Codepages entry. */
    struct cp_entry {

      ULONG  size;          //@- Count of the prepared codepages.
      ULONG  selected;      //@- Current selected codepage.
      ULONG  prepared[10];  //@- Prepared codepages.
    };

    #pragma pack()

    /**
     * Use these enumerators to specify keyboard layout type.
     *
     * <dl>
     * <dt><i>primary  </i><dd>Primary part of the keyboard layout.
     * <dt><i>secondary</i><dd>Secondary part of the multilayered keyboard layout.
     * <dt><i>any      </i><dd>Any of them.
     * </dl>
     */

    enum kl_type { any       = 0x0001,
                   primary   = 0x0002,
                   secondary = 0x0003 };
    /**
     * Use these enumerators to obtain attached keyboard type.
     *
     * <dl>
     * <dt><i>standard </i><dd>Personal computer AT keyboard.
     * <dt><i>enchanced</i><dd>Enchanced keyboard.
     * <dt><i>unknown  </i><dd>Error.
     * </dl>
     */

    enum kb_type { standard  = 0x0000,
                   enchanced = 0x0002,
                   unknown   = 0xFFFF };

  public:

    /**
     * Returns the error code set by the failure of a broker method.
     *
     * @return One of the standard OS/2 error code if error occured
     *         during call of the broker method. Returns NO_ERROR in
     *         other cases.
     */

    ULONG error_code() const;

    /**
     * Returns a type of attached keyboard.
     *
     * @return  A <i>unknown</i> return value indicates an error.
     */

    kb_type keyboard();

    /**
     * Returns a list of the prepared codepages.
     *
     * @return A zero value of the <i>cp_entry::size</i>
     *         indicates an error.
     */

    const cp_entry* cp_list();

    /**
     * Returns a name of the selected keyboard layout.
     *
     * @return  A empty string return value indicates an
     *          error.
     */

    const char* selected();

    /** Returns a name of the keyboard translation file. */
    const char* table_name();

    /**
     * Opens a keyboard translation file.
     *
     * @return This function returns one of the following values:
     *         <dl>
     *         <dt>NO_ERROR
     *         <dt>ERROR_READ_FAULT
     *         <dt>ERROR_OPEN_FAILED
     *         </dl>
     */

    ULONG table_open ();

    /**
     * Closes a keyboard translation file.
     *
     * @return This function always returns the
     *         NO_ERROR value.
     */

    ULONG table_close();

    /**
     * Reads the next keyboard layout entry from the keyboard
     * translation file.
     *
     * @return TRUE if <i>entry</i> is filled by valid data.
     */

    BOOL next( kl_entry* entry, ULONG cp = 0, kl_type tp = primary );

    /**
     * Reads the specified keyboard layout entry from the keyboard
     * translation file.
     *
     * @return TRUE if <i>entry</i> is filled by valid data.
     */

    BOOL find( kl_entry* entry, const char* name, ULONG cp = 0, kl_type tp = primary );

    /** Selects specifed keyboard layout. */
    BOOL select( const char* name, BOOL verbose = FALSE );

    /** Is specified layout multilayered. */
    BOOL is_multilayered( const char* name, ULONG cp );
    /** Is current selected layout multilayered. */
    BOOL is_multilayered();

  private:
    /** Removes leading and trailing spaces. */
    char* strip( char *s );
    /** Opens a keyboard device driver.  */
    ULONG kb_open ();
    /** Close a keyboard device driver. */
    ULONG kb_close();
    /** Sets translation table for VIO, FS and VDM. */
    ULONG select_fs( const kl_entry& entry, ULONG index, BOOL verbose );
    /** Sets translation table for PM. */
    BOOL  select_pm( const kl_entry& entry, BOOL verbose );

  private:
    struct info
    {
      char* name;
      ULONG pri_id;
      ULONG sec_id;
    };

    static const info lb_list[];

    ULONG    lb_error;
    cp_entry lb_cp;
    HFILE    lb_handle;
    char     lb_file[_MAX_PATH ];
    char     lb_name[_MAX_LNAME];

    unsigned tb_open;     //@- > 0, if file is open.
    ifstream tb_file;
    USHORT   tb_size;     //@- Count of a keyboard layouts in layouts file.
    ULONG    tb_entry;    //@- Layouts file entry point.

    ULONG    search_pos;  //@- Current keyboard layout number in read() operation.
    ULONG    search_ofs;  //@- Current file offset for read() operation.
};

#if !defined(__ccdoc__)
/*--------------------------------------------------
 * Returns the error code set by the failure
 *--------------------------------------------------*/
inline ULONG KLBroker::error_code() const
{
  return lb_error;
}
#endif
#endif
