/* Keyboard Layer/2
 *
 * Fullscreen monitors support
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
 * $Id: kl_monitors.h,v 1.7 2005/10/01 08:42:52 glass Exp $
 */

#ifndef __KL_MONITORS_H
#define __KL_MONITORS_H

#include <kl_global.h>
#include <os2def.h>

extern void monitors_start();
extern void monitors_stop ();

#define MONITOR_DEFAULT 0x0000
#define MONITOR_BEGIN   0x0001
#define MONITOR_END     0x0002

typedef SHANDLE   HMONITOR;
typedef HMONITOR *PHMONITOR;

struct  KEYPACKET
{
  UCHAR  MonFlagWord;
  UCHAR  OriginScan;
  UCHAR  XlatedChar;
  UCHAR  XlatedScan;
  UCHAR  DBCSStatus;
  UCHAR  DBCSShift;

  union
  {
    USHORT ShiftState;
    USHORT LayerIndex;
  };

  ULONG  Milliseconds;
  USHORT KbdDDFlagWord;
};

struct MLNPACKET
{
  USHORT MonFlagWord;
  USHORT IOCTL;
  USHORT CPId;
  USHORT CPIndex;
  ULONG  Reserved;
  USHORT KbdDDFlagWord;
};

// DBCSStatus

#define SF_SHIFTS          1  // If set to 1, shift status returned without a character
#define SF_NOTCHAR         2  // 0 - Scan code is a character
                              // 1 - Scan code is not a character;
                              //     instead it is an extended key code from the keyboard.
#define SF_IMMEDIATE      32  // If set to 1, immediate conversion requested
#define SF_TYPEMASK      192  // Has the following values:
                              // 00 - Undefined
                              // 01 - Final character; interim character flag is turned off
                              // 10 - Interim character
                              // 11 - Final character; interim character flag is turned on.
// MonFlagWord

#define MF_OPEN            1  // open
#define MF_CLOSE           2  // close
#define MF_FLUSH           4  // is flush packet

// KbdDDFlagWord

#define KF_NOTSQPACKET  1024  // Don't put this packet in SQ buffer
#define KF_ACCENTEDKEY   512  // Key was translated using previous accent.
#define KF_MULTIMAKE     256  // Key was repeated make of a toggle key.
#define KF_SECONDARYKEY  128  // Previous scan code was the E0 prefix code.
#define KF_KEYBREAK       64  // This is the break of the key.
#define KF_KEYTYPEMASK    63  // Isolates the Key Type field of DDFlags.
#define KF_UNDEFKEY       63  // Key packet is undefined
#define KF_SYSREQKEY      23  // This key packet is the SysReq key (4990)
#define KF_PRINTFLUSHKEY  22  // This packet is Ct-Alt-PrtScr
#define KF_PSPRINTECHOKEY 21  // This packet is Ctl-P
#define KF_PRINTECHOKEY   20  // This packet is Ctl-PrtScr
#define KF_PRTSCRKEY      19  // This packet is PrtScr
#define KF_PSBREAKKEY     18  // This packet is Ctl-C
#define KF_BREAKKEY       17  // This packet is Ctl-Break
#define KF_ACCENTKEY      16  // This packet is an accent key
#define KF_XRORPNOT       13  // This packet is a Read or Peek Notification Pct.
#define KF_MLNOTIFICATION 14  // packet is a Multi-Layer NLS packet
#define KF_HOTKEYPACKET   12  // This packet is the hot key.
#define KF_BADKEYCOMBO    11  // Accent/char combo undefined, beep only.
#define KF_WAKEUPKEY      10  // This packet is one following PAUSEKEY
#define KF_PSPAUSEKEY      9  // This packet is Ctl-S
#define KF_PAUSEKEY        8  // This packet is Ctl-Numlock or PAUSE
#define KF_SHIFTMASK       7  // Key is a shift Key
#define KF_DUMPKEY         6  // This packet is Ctl-Numlock-NumLock
#define KF_REBOOTKEY       5  // This packet is Ctl-Alt-Del
#define KF_RESENDCODE      4  // This packet is resend code from controller
#define KF_OVERRUNCODE     3  // This packet is overrun code from controller
#define KF_SECPREFIXCODE   2  // This packet is E0/E1 scan code
#define KF_ACKCODE         1  // This packet is ack code from keyboard

struct MONBUF
{
  USHORT    BufferLen;
  KEYPACKET Buffer;
  BYTE      Reserved[20];
};

#pragma seg16(MONBUF)

#define RS_SYSREG      32768  // Bit 15  SysReq key down
#define RS_CAPSLOCK    16384  // Bit 14  Caps Lock key down
#define RS_NUMLOCK      8192  // Bit 13  NumLock key down
#define RS_SCROLLLOCK   4096  // Bit 12  Scroll Lock key down
#define RS_RALT         2048  // Bit 11  Right Alt key down
#define RS_RCONTROL     1024  // Bit 10  Right Ctrl key down
#define RS_LALT          512  // Bit  9  Left Alt key down
#define RS_LCONTROL      256  // Bit  8  Left Ctrl key down
#define RS_INSERT        128  // Bit  7  Insert on
#define RS_CAPS           64  // Bit  6  Caps Lock on
#define RS_NUM            32  // Bit  5  NumLock on
#define RS_SCROLL         16  // Bit  4  Scroll Lock on
#define RS_ALT             8  // Bit  3  Either Alt key down
#define RS_CONTROL         4  // Bit  2  Either Ctrl key down
#define RS_LSHIFT          2  // Bit  1  Left Shift key down
#define RS_RSHIFT          1  // Bit  0  Right Shift key down

#define DosMonOpen    DOS16MONOPEN
#define DosMonClose   DOS16MONCLOSE
#define DosMonReg     DOS16MONREG
#define DosMonRead    DOS16MONREAD
#define DosMonWrite   DOS16MONWRITE
#define DosGetInfoSeg DOS16GETINFOSEG

APIRET16 APIENTRY16 DosMonOpen ( PSZ pszDevName, PHMONITOR phmon );
APIRET16 APIENTRY16 DosMonClose( HMONITOR hmon );
APIRET16 APIENTRY16 DosMonReg  ( HMONITOR hmon, MONBUF *pbInBuf, MONBUF *pbOutBuf, USHORT fPosition, USHORT usIndex );
APIRET16 APIENTRY16 DosMonRead ( PBYTE pbInBuf , USHORT fWait, PBYTE pbDataBuf, PUSHORT pcbData );
APIRET16 APIENTRY16 DosMonWrite( PBYTE pbOutBuf, PBYTE pbDataBuf, USHORT cbData );

#endif
