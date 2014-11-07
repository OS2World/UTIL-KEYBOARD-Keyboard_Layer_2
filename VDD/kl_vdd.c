/* Keyboard Layer/2
 *
 * Virtual Device Driver
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
 * $Id$
 */

#define  INCL_VDDHELPERS
#define  INCL_MI
#include <mvdm.h>

#include "kl_vdd.h"
#include "kl_trigger.h"
#include "kl_hotkey.h"

typedef struct _VLCTX
{
  HHOOK hhook;
  BOOL  armed;

} VLCTX;

#pragma data_seg( "DATA32" )

BOOL    enable_beeps     = TRUE;
BOOL    enable_vdd       = FALSE;
ULONG   layer_default_id = KL_NONE;
BYTE    nat_color        = 0x0C;
BOOL    is_extended_key  = FALSE;

struct  VLHKEY  hkey_list[_MAX_HKEYS];

int     hkey_size     = 0;
int     hkey_head     = 0;
int     hkey_capacity = sizeof(hkey_list )/sizeof(struct VLHKEY);

struct  VLEVNT  evnt_queue[50];

HVDHSEM evnt_ready    = 0;
HVDHSEM evnt_mutex    = 0;
int     evnt_head     = 0;
int     evnt_tail     = 0;
int     evnt_capacity = sizeof(evnt_queue)/sizeof(struct VLEVNT);

void PRIVENTRY VLSetBorder( PCRF pcrf );

#pragma data_seg( "MVDMINSTDATA", "MIDATA" )
/*--------------------------------------------------
 * VDM instance data
 *--------------------------------------------------*/

BOOL  border_is_painted;
ULONG border_is_requested;
BYTE  lat_color;
BYTE  latest_scancode;
ULONG layer_id;
ULONG layer_requested;
VLCTX hook_set_border;
VLCTX hook_set_layer;
HHOOK hook_ret_border;

#pragma alloc_text( CSWAP_TEXT, VLBeepTrigger )
/*--------------------------------------------------
 * Sound signal at turn on of the keyboard layer
 *--------------------------------------------------*/
void PRIVENTRY VLBeepTrigger( SHORT id )
{
  if( enable_beeps )
  {
    if( id == KL_LATIN )
      VDHDevBeep( BEEP_LAT, BEEP_LEN );
    else
      VDHDevBeep( BEEP_NAT, BEEP_LEN );
  }
}

#pragma alloc_text( CSWAP_TEXT, VLGetLayer )
/*--------------------------------------------------
 * Returns layer for current VDM
 *--------------------------------------------------*/
ULONG PRIVENTRY VLGetLayer()
{
  return ROMBios.rb_fbKFlag2 & BIOSKFLAG2_CYR ? KL_NATIONAL : KL_LATIN;
}

#pragma alloc_text( CSWAP_TEXT, VLSetLayer )
/*--------------------------------------------------
 * Selects the keyboard layer for the current VDM
 *--------------------------------------------------*/
void PRIVENTRY VLSetLayer( ULONG id, BOOL beep )
{
  if( id == KL_LATIN    )
      ROMBios.rb_fbKFlag2 &= ~BIOSKFLAG2_CYR;
  if( id == KL_NATIONAL )
      ROMBios.rb_fbKFlag2 |=  BIOSKFLAG2_CYR;

  if( beep )
      VLBeepTrigger( id );

  layer_id = id;
}

#pragma alloc_text( CSWAP_TEXT, VLPostEvent )
/*--------------------------------------------------
 * Sends message about status changes
 *--------------------------------------------------*/
BOOL PRIVENTRY VLPostEvent( BYTE type, SHORT data )
{
  if( !enable_vdd || !VDHRequestMutexSem( evnt_mutex, SEM_INDEFINITE_WAIT ))
    return FALSE;

  if(( evnt_head == 0 && evnt_tail == evnt_capacity - 1 ) ||
     ( evnt_tail == evnt_head - 1 ))
  {
    // The event queue is overflow.
    return FALSE;
  }

  evnt_queue[evnt_tail].EventType = type;
  evnt_queue[evnt_tail].EventData = data;
  evnt_queue[evnt_tail].EventPID  = VDHQuerySysValue( 0, VDHLSV_PID );
  evnt_queue[evnt_tail].EventNULL = 0;

  if( ++evnt_tail == evnt_capacity )
    evnt_tail = 0;

  VDHPostEventSem   ( evnt_ready );
  VDHReleaseMutexSem( evnt_mutex );
}

#pragma alloc_text( CSWAP_TEXT, VLReadEvent )
/*--------------------------------------------------
 * Reads message about status changes
 *--------------------------------------------------*/
BOOL PRIVENTRY VLReadEvent( struct VLEVNT* event )
{
  BOOL rc = FALSE;

  if( !VDHRequestMutexSem( evnt_mutex, SEM_INDEFINITE_WAIT ))
    return FALSE;

  if( evnt_head != evnt_tail )
  {
    event->EventType = evnt_queue[evnt_head].EventType;
    event->EventData = evnt_queue[evnt_head].EventData;
    event->EventPID  = evnt_queue[evnt_head].EventPID;
    event->EventNULL = evnt_queue[evnt_head].EventNULL;

    if( ++evnt_head == evnt_capacity )
      evnt_head = 0;

    rc = TRUE;
  }

  if( evnt_head == evnt_tail )
    VDHResetEventSem( evnt_ready );

  VDHReleaseMutexSem( evnt_mutex );
  return rc;
}

#pragma alloc_text( CSWAP_TEXT, VLSetBorderReturn )
/*--------------------------------------------------
 * Return from int 10
 *--------------------------------------------------*/
VOID HOOKENTRY VLSetBorderReturn( PVOID p, PCRF pcrf )
{
  VDHPopRegs( VDHREG_AX | VDHREG_BX );
  border_is_painted = FALSE;

  if( border_is_requested )
  {
    --border_is_requested;
    VLSetBorder( pcrf );
  }
}

#pragma alloc_text( CSWAP_TEXT, VLSetBorder )
/*--------------------------------------------------
 * Sets the screen border color for current VDM
 *--------------------------------------------------*/
void PRIVENTRY VLSetBorder( PCRF pcrf )
{
  BYTE color;

  if( border_is_painted )
  {
    ++border_is_requested;
    return;
  }

  border_is_painted = TRUE;
  color = lat_color;

  if( VLGetLayer() == KL_NATIONAL )
    color = nat_color;

  // ÄÄÄÄÄÄÄÄÄ´INT 10H 0bH: Select Graphics Palette or Text Border ColorÃÄÄÄÄÄÄÄ
  //
  // Expects: AH    0bH
  //          BL    text modes: select overscan (border) color
  //                     00H-0fH low-intensity colors
  //                     10H-1fH high-intensity colors
  //                graphics:   select palette combination
  //                     0 = green/red/brown
  //                     1 = cyan/magenta/white
  //
  // ÄÄÄÄÄÄÄÄÄÄÄÄÄ´INT 10H 1001H: Set Overscan/Border Color RegisterÃÄÄÄÄÄÄÄÄÄÄÄ
  //
  // Expects: AX    1001H
  //          BH    6-bit RGB color to display for that attribute
  //
  //  Info: This sets the EGA/VGA screen border to a specific rgbRGB color.
  //
  //  BH is a 6-bit rgbRGB color setting, as follows:
  //
  //        Ö7Â6Â5Â4Â3Â2Â1Â0·
  //        º   ³r³g³b³R³G³Bº             Note: R,G,B are 2/3-intensity
  //        ÓÄÁÄÁÒÁÒÁÒÁÒÁÒÁÒ½ bit               r,g,b are 1/3-intensity
  //             º º º º º ÈÍ 0: 01H Blue
  //             º º º º ÈÍÍÍ 1: 02H Green
  //             º º º ÈÍÍÍÍÍ 2: 04H Red
  //             º º ÈÍÍÍÍÍÍÍ 3: 08H blue'
  //             º ÈÍÍÍÍÍÍÍÍÍ 4: 10H green' (also CGA-emulation intensity)
  //             ÈÍÍÍÍÍÍÍÍÍÍÍ 5: 20H red'

  VDHPushRegs( VDHREG_AX | VDHREG_BX );

  if( color & 0x80 )
  {
    AH(pcrf) = 0x0B;
    BH(pcrf) = 0x00;
    BL(pcrf) = color & 0x7F;
  }
  else
  {
    AX(pcrf) = 0x1001;
    BH(pcrf) = color;
  }

  VDHArmReturnHook( hook_ret_border, VDHARH_RECURSIVE_CSEIP_HOOK );
  VDHPushInt( 0x10 );
}

#pragma alloc_text( CSWAP_TEXT, VLSetContextLayer )
/*--------------------------------------------------
 * Selects keyboard layer for current VDM
 *--------------------------------------------------*/
VOID HOOKENTRY VLSetContextLayer( PVOID p, PCRF pcrf )
{
  if( layer_requested != KL_NONE )
  {
    VLSetLayer ( layer_requested, FALSE );
    VLPostEvent( VLEVT_LAYERCHANGED, layer_requested );
    VLSetBorder( pcrf );
  }
  hook_set_layer.armed  = FALSE;
}

#pragma alloc_text( CSWAP_TEXT, VLSetContextBorder )
/*--------------------------------------------------
 * Sets the screen border color for current VDM
 *--------------------------------------------------*/
VOID HOOKENTRY VLSetContextBorder( PVOID p, PCRF pcrf )
{
  VLSetBorder( pcrf );
  hook_set_border.armed = FALSE;
}

#pragma alloc_text( CSWAP_TEXT, VLArmContextBorder )
/*--------------------------------------------------
 * Sets the screen border color for current VDM
 *--------------------------------------------------*/
BOOL HOOKENTRY VLArmContextBorder( HVDM hvdm, ULONG data )
{
  if( !REFHVDM( hvdm, VLCTX, hook_set_border ).armed )
  {
    REFHVDM( hvdm, VLCTX, hook_set_border ).armed = TRUE;
    VDHArmContextHook( REFHVDM( hvdm, VLCTX, hook_set_border ).hhook, hvdm );
  }
  return TRUE;
}

#pragma alloc_text( CSWAP_TEXT, VLInt10Proc )
/*--------------------------------------------------
 * INT 10 Handler
 *--------------------------------------------------*/
BOOL VDHENTRY VLInt10Proc( PCRF pcrf )
{
  // ÄÄÄÄÄÄÄÄÄ´INT 10H 0bH: Select Graphics Palette or Text Border ColorÃÄÄÄÄÄÄÄ
  //
  // Expects: AH    0bH
  //          BL    text modes: select overscan (border) color
  //                     00H-0fH low-intensity colors
  //                     10H-1fH high-intensity colors
  //                graphics:   select palette combination
  //                     0 = green/red/brown
  //                     1 = cyan/magenta/white
  //
  // ÄÄÄÄÄÄÄÄÄÄÄÄÄ´INT 10H 1001H: Set Overscan/Border Color RegisterÃÄÄÄÄÄÄÄÄÄÄÄ
  //
  // Expects: AX    1001H
  //          BH    6-bit RGB color to display for that attribute
  //
  //  Info: This sets the EGA/VGA screen border to a specific rgbRGB color.
  //
  //  BH is a 6-bit rgbRGB color setting, as follows:
  //
  //        Ö7Â6Â5Â4Â3Â2Â1Â0·
  //        º   ³r³g³b³R³G³Bº             Note: R,G,B are 2/3-intensity
  //        ÓÄÁÄÁÒÁÒÁÒÁÒÁÒÁÒ½ bit               r,g,b are 1/3-intensity
  //             º º º º º ÈÍ 0: 01H Blue
  //             º º º º ÈÍÍÍ 1: 02H Green
  //             º º º ÈÍÍÍÍÍ 2: 04H Red
  //             º º ÈÍÍÍÍÍÍÍ 3: 08H blue'
  //             º ÈÍÍÍÍÍÍÍÍÍ 4: 10H green' (also CGA-emulation intensity)
  //             ÈÍÍÍÍÍÍÍÍÍÍÍ 5: 20H red'

  if( !border_is_painted && enable_vdd )
  {
     if( AX(pcrf) == 0x1001 )
       lat_color = BH(pcrf) & 0x3F;

     if( AH(pcrf) == 0x0B && BH(pcrf) == 0 )
       lat_color = BL(pcrf) | 0x80;
  }
  return FALSE;
}

#pragma alloc_text( CSWAP_TEXT, VLInt15Proc )
/*--------------------------------------------------
 * INT 15 Handler
 *--------------------------------------------------*/
BOOL VDHENTRY VLInt15Proc( PCRF pcrf )
{
  BYTE scancode;
  LONG detected;

  // ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´INT 15H 4FH: Keyboard InterceptÃÄÄÄÄÄÄÄÄÄÄÄÄ
  //
  // Expects: AH    4FH
  //          AL    keyboard scan code received from hardware
  //          CX    process ID
  //          ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
  // Returns: AL    keyboard scan code
  //          CF = CY: replace input scan code with the one on AL
  //          CF = NC: no change; use hardware scan code
  // ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

  if( !enable_vdd || AH(pcrf) != 0x4F )
    return FALSE;

  if( hkey_size )
  {
    int state = 0;
    int head  = hkey_head;

    if( ROMBios.rb_fbKFlag & BIOSKFLAG_CTRL      ) state |= hkey_control;
    if( ROMBios.rb_fbKFlag & BIOSKFLAG_ALT       ) state |= hkey_alt;
    if( ROMBios.rb_fbKFlag & BIOSKFLAG_SHIFTMASK ) state |= hkey_shift;

    do
    {
      if( ++head >= hkey_size )
        head = 0;

      if( hkey_list[head].scancode == AL(pcrf) &&
          hkey_list[head].state    == state     )
      {
        // If it is auto repeat key, did not interested :)
        if( latest_scancode != AL(pcrf))
        {
          hkey_head = head;
          VLPostEvent( VLEVT_LAYOUTCHANGE, head );
        }

        VDHPopInt();
        FL(pcrf) &= ~F_CARRY;
        latest_scancode = AL(pcrf);
        return TRUE;
      }

    } while( head != hkey_head );
  }

  scancode = AL(pcrf);

  if( htrg_selected() != XS_DONTTOUCH )
  {
    if( scancode == 0xE0 )  // Extended scan code
    {
      is_extended_key = TRUE;
      return FALSE;
    }

    if( is_extended_key )   // Convert extended key to PM scan code
    {
      switch( scancode & 0x7F )
      {
        case CS_LALT     : scancode = CS_RALT;     break;
        case CS_LCONTROL : scancode = CS_RCONTROL; break;
        case 0x5B        : scancode = CS_LWIN;     break;
        case 0x5C        : scancode = CS_RWIN;     break;
        case 0x5D        : scancode = CS_WINMENU;  break;
      }

      scancode |= ( AL(pcrf) & 0x80 );
      is_extended_key = FALSE;
    }

    // If it is auto repeat key, did not interested :)
    if( scancode == latest_scancode )
      return FALSE;

    latest_scancode = scancode;
    detected = htrg_dispatch( scancode, VLGetLayer(), ROMBios.rb_ulTickCount );

    if( detected != KL_NONE )
    {
      VLSetLayer ( detected, TRUE );
      VLPostEvent( VLEVT_LAYERCHANGED, VLGetLayer());
      VLSetBorder( pcrf );
    }
  }

  // To check up, if the status of the layer is changed by anyone,
  // will notify PM and to establish the appropriate colour of a
  // screen border:

  if( layer_id != VLGetLayer())
  {
    VLPostEvent( VLEVT_LAYERCHANGED, layer_id = VLGetLayer());
    VLSetBorder( pcrf );
  }

  return FALSE;
}

#pragma alloc_text( CSWAP_TEXT, VLCreateVDM )
/*--------------------------------------------------
 * Create VDM
 *--------------------------------------------------*/
BOOL HOOKENTRY VLCreateVDM( HVDM hvdm )
{
  border_is_painted   = FALSE;
  border_is_requested = 0;
  lat_color           = 0;
  latest_scancode     = 0;
  layer_id            = KL_NONE;
  layer_requested     = KL_NONE;

  VDHInstallIntHook( hvdm, 0x15, VLInt15Proc, !VDH_ASM_HOOK );
  VDHInstallIntHook( hvdm, 0x10, VLInt10Proc, !VDH_ASM_HOOK );

  hook_ret_border = VDHAllocHook( VDH_RETURN_HOOK, (PFNARM)VLSetBorderReturn, 0 );

  if( !hook_ret_border )
    return FALSE;

  hook_set_border.armed = FALSE;
  hook_set_border.hhook = VDHAllocHook( VDH_CONTEXT_HOOK, (PFNARM)VLSetContextBorder, 0 );

  if( !hook_set_border.hhook )
    return FALSE;

  hook_set_layer.armed  = FALSE;
  hook_set_layer.hhook  = VDHAllocHook( VDH_CONTEXT_HOOK, (PFNARM)VLSetContextLayer , 0 );

  if( !hook_set_layer.hhook  )
    return FALSE;

  if( enable_vdd && layer_default_id != KL_NONE )
  {
    VLSetLayer( layer_default_id, FALSE );
    hook_set_border.armed = TRUE;
    VDHArmContextHook( hook_set_border.hhook, hvdm );
  }

  return TRUE;
}

#pragma alloc_text( CSWAP_TEXT, VLCodePageChange )
/*--------------------------------------------------
 * CodePage Change
 *--------------------------------------------------*/
BOOL HOOKENTRY VLCodePageChange( ULONG codepage )
{
  if( enable_vdd && layer_default_id != KL_NONE )
    VLSetLayer( layer_default_id, FALSE );

  VLPostEvent( VLEVT_LAYERCHANGED, VLGetLayer());
  return TRUE;
}

#pragma alloc_text( CSWAP_TEXT, VLSysReqProc )
/*--------------------------------------------------
 * VDD Interface
 *--------------------------------------------------*/
LONG EXPENTRY VLSysReqProc( SGID  sgid,
                            ULONG ulFunc,
                            ULONG nbIn,
                            PVOID pIn,
                            ULONG nbOut,
                            PVOID pOut    )
{
  switch( ulFunc )
  {
    // Enables the VDM Support.
    case VLCMD_ENABLE:
      if( nbIn == sizeof(BOOL))
        enable_vdd = *(BOOL*)pIn;

      break;

    // Returns keyboard layer for specified VDM.
    case VLCMD_GETLAYER:
      if( nbOut == sizeof(ULONG))
      {
        HVDM hvdm = VDHHandleFromPID(sgid);

        if( hvdm )
          (*(ULONG*)pOut) = ((PROMBIOS)(hvdm))->rb_fbKFlag2 & BIOSKFLAG2_CYR ? KL_NATIONAL : KL_LATIN;
        else
          (*(ULONG*)pOut) = 0;
      }
      break;

    // Clears hotkeys list.
    case VLCMD_HKEYCLEAR:
    {
      hkey_size = 0;
      hkey_head = 0;
      break;
    }

    // Appends another hotkey for a selection of a keyboard layout.
    case VLCMD_HKEYASSIGN:
    {
      if( nbIn != sizeof(struct VLHKEY) || hkey_size >= hkey_capacity )
        return 1;

      hkey_list[hkey_size].scancode = ((struct VLHKEY*)pIn)->scancode;
      hkey_list[hkey_size].state    = ((struct VLHKEY*)pIn)->state;

      hkey_head = hkey_size++;
      break;
    }

    // Enables sound signals at switching keyboard layers.
    case VLCMD_SETBEEP:
      if( nbIn == sizeof(BOOL))
        enable_beeps = *(BOOL*)pIn;

      break;

    // Sets the VDM screen border color
    case VLCMD_SETBORDER:
      if( nbIn == sizeof(LONG))
      {
        RGB  color  = *(RGB*)pIn;
        BYTE border = 0;

        switch(( color.bBlue  + 50 ) / 85 )
        {
          case  0 : break;
          case  1 : border |= 0x08; break;
          default : border |= 0x01; break;
        }

        switch(( color.bGreen + 50 ) / 85 )
        {
          case  0 : break;
          case  1 : border |= 0x10; break;
          default : border |= 0x02;
        }

        switch(( color.bRed   + 50 ) / 85 )
        {
          case  0 : break;
          case  1 : border |= 0x20; break;
          default : border |= 0x04;
        }

        nat_color = border;
        VDHEnumerateVDMs( VLArmContextBorder, 0 );
      }
      break;

    // Sets the keyboard layer for newly created VDMs.
    case VLCMD_SETDEFLAYER:
      if( nbIn == sizeof(ULONG))
        layer_default_id = *(ULONG*)pIn;

      break;

    // Selects keyboard layer for specified VDM.
    case VLCMD_SETLAYER:
      if( nbIn == sizeof(ULONG))
      {
        HVDM hvdm = VDHHandleFromPID(sgid);

        if( hvdm )
        {
          REFHVDM( hvdm, ULONG, layer_requested ) = *(ULONG*)pIn;

          if( !REFHVDM( hvdm, VLCTX, hook_set_layer ).armed )
          {
            REFHVDM( hvdm, VLCTX, hook_set_layer ).armed = TRUE;
            VDHArmContextHook( REFHVDM( hvdm, VLCTX, hook_set_layer ).hhook, hvdm );
          }
        }
      }
      break;

    // Sets the trigger timeout value.
    case VLCMD_SETTIMEOUT:
      if( nbIn >= sizeof(ULONG))
          // Convert time from tens to ticks.
          htrg_set_timeout( (*(ULONG*)pIn * 100 )/55 );

      break;

    // Selects the trigger hotkeys.
    case VLCMD_SETTRIGGER:
      if( nbIn == sizeof(USHORT))
        htrg_select( *(USHORT*)pIn );

      break;

    // Returns the next VDD event.
    case VLCMD_GETEVENT:
      if( nbOut == sizeof(struct VLEVNT))
      {
        VDHWaitEventSem( evnt_ready, SEM_INDEFINITE_WAIT );
        VLReadEvent((struct VLEVNT*)pOut );
      }
      break;

    case VLCMD_GETVERSION:
      if( nbOut == sizeof(struct VLVERS))
      {
        ((struct VLVERS*)pOut)->major = VER_MAJOR;
        ((struct VLVERS*)pOut)->minor = VER_MINOR;
        ((struct VLVERS*)pOut)->level = VER_LEVEL;
      }
      break;

    default:
      return 1;
  }

  return 0;
}

#pragma entry(VLInit)
#pragma alloc_text( CINIT_TEXT, VLInit )
/*--------------------------------------------------
 * VDD Initialization
 *--------------------------------------------------*/
BOOL EXPENTRY VLInit( PSZ psz )
{
  VDHInstallUserHook( VDM_CREATE, VLCreateVDM  );
  VDHInstallUserHook( VDM_CODEPAGE_CHANGE, VLCodePageChange );

  if( !VDHCreateSem( &evnt_ready, VDH_EVENTSEM ))
    return FALSE;
  if( !VDHCreateSem( &evnt_mutex, VDH_MUTEXSEM ))
    return FALSE;

  VDHRegisterVDD( "LAYER$", VLSysReqProc, NULL );
  return TRUE;
}

