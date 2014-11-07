/* Keyboard Layer/2
 *
 * Supports of the SysBar/2 Pipe Monitor
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
 * $Id: kl_pipe.h,v 1.4 2005/10/01 08:42:52 glass Exp $
 */

#ifndef __KL_PIPE_H
#define __KL_PIPE_H

/**
 * Supports of the SysBar/2 Pipe Monitor.
 *
 * Note that there is no need to create an instance of this class
 * because all methods are static.
 *
 * @author Dmitry A Steklenev
 */

class KLPipe
{
  public:
    /** Outputs specified layer state to pipe. */
    static void write( const char* state );
    /** Clears pipe. */
    static void clear();

    /** Enables support of the SysBar/2 Pipe Monitor. */
    static void enable ( BOOL enable = TRUE );
    /** Disables support of the SysBar/2 Pipe Monitor. */
    static void disable();

    /**
     * Is the support enabled.
     *
     * @return If the support of the SysBar/2 Pipe Monitor is enabled,
     *         TRUE is returned. Otherwise, FALSE is returned.
     */

    static BOOL is_enabled();

  private:
    static BOOL np_enabled;
};

/*----------------------------------------------------
 * Is the support of the SysBar/2 Pipe Monitor enabled
 *----------------------------------------------------*/
inline BOOL KLPipe::is_enabled()
{
  return np_enabled;
}

#endif
