/* Keyboard Layer/2
 *
 * Definitions
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
 * $Id: kl_global.h,v 1.15 2006/02/22 17:14:03 glass Exp $
 */

#ifndef __KL_GLOBAL_H
#define __KL_GLOBAL_H

#define PKG_NAME    "Keyboard Layer/2"
#define VER_MAJOR   3
#define VER_MINOR   0
#define VER_LEVEL   2

#ifndef KL_LATIN
#define KL_LATIN    0x00000000
#endif
#ifndef KL_NATIONAL
#define KL_NATIONAL 0x00000001
#endif
#define KL_NONE     0xFFFFFFFF

#define BEEP_LAT  800
#define BEEP_NAT 1000
#define BEEP_LEN   20

#define LAYER_EXIT_NORMAL     0
#define LAYER_EXIT_INITERROR  1
#define LAYER_EXIT_DUPLICATE  4

#endif
