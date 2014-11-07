/****************************************************************************
 *                             S Y S T R A Y / 2                            *
 *               Systemp TRAY Application Interface Library                 *
 *                                                                          *
 * (C) 2000, OS2.Ru DevTeam                  http://devteam.os2.ru/systray  *
 * Written by Dmitry Zaharov                                 madint@os2.ru  *
 ****************************************************************************/

#ifndef __OS2_H__
#define  INCL_WIN
#include <os2.h>
#endif

#ifndef __TRAYAPI_H
#define __TRAYAPI_H

#define WM_TRAYADDME (WM_USER+1)
#define WM_TRAYDELME (WM_USER+2)
#define WM_TRAYICON  (WM_USER+3)
#define WM_TRAYEXIT  (0xCD20)

#define SZAPP   	"SystrayServer"
#define SZTOPIC     "TRAY"

#ifndef __IMPLEMENTATION__

extern HWND hwndTrayServer;

BOOL InitializeTrayApi(HWND hwnd);
BOOL AnswerTrayApiDdeAck(MPARAM mp1);
BOOL AddTrayIcon(HWND hwnd, HPOINTER hptr);
BOOL ChangeTrayIcon(HWND hwnd, HPOINTER hptr);
BOOL DeleteTrayIcon(HWND hwnd);

#else

HWND hwndTrayServer = 0;

BOOL InitializeTrayApi(HWND hwnd)
{
	WinDdeInitiate(hwnd,SZAPP,SZTOPIC,NULL);
	return TRUE;
}

BOOL AnswerTrayApiDdeAck(MPARAM mp1)
{
	hwndTrayServer = (HWND)mp1;

	return TRUE;
}

BOOL AddTrayIcon(HWND hwnd,HPOINTER hptr) // hptr unused now and must be zero
{
    if(!hwndTrayServer)
    	return FALSE; // api not initialized

	WinPostMsg(hwndTrayServer,WM_TRAYADDME,(MPARAM)hwnd,(MPARAM)hptr);

	return TRUE;
}

BOOL ChangeTrayIcon(HWND hwnd,HPOINTER hptr) // hptr must be zero
{
    if(!hwndTrayServer)
    	return FALSE; // api not initialized

	WinPostMsg(hwndTrayServer,WM_TRAYICON,(MPARAM)hwnd,(MPARAM)hptr);

	return TRUE;
}

BOOL DeleteTrayIcon(HWND hwnd)
{
    if(!hwndTrayServer)
    	return FALSE; // api not initialized

	WinPostMsg(hwndTrayServer,WM_TRAYDELME,(MPARAM)hwnd,(MPARAM)0L);

	return TRUE;
}

#endif /* __IMPLEMENTATION__ */

#endif /*__TRAYAPI_H*/

