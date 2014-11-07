/* REXX deinstallation script for Keyboard Layer/2 
 * Copyright (C) 1998-2013 Dmitry A.Steklenev
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

globals = "color. sys. msg."

/* enable ANSI extended screen and keyboard control */

"@ansi on > nul"
"@echo off"

color.usual = "1B"x"[0m"
color.bold  = "1B"x"[1;37m"
color.error = "1B"x"[1;31m"
color.info  = "1B"x"[1;32m"

cls
say color.bold  || "Keyboard Layer/2" || color.usual || " Deinstallation"
say color.usual || "Copyright (C) 1998-2013 Dmitry A.Steklenev"
say color.usual || ""

/* Initializing */

if RxFuncQuery('SysLoadFuncs') then do
   call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
   call SysLoadFuncs
end

call MsgRead "insmsg"

parse upper value VALUE( "PATH",, "OS2ENVIRONMENT" ) with "\OS2\SYSTEM" -2,
      sys.boot_disk +2

sys.kbd_file = sys.boot_disk"\OS2\KEYBOARD.DCP"
sys.cfg_file = sys.boot_disk"\CONFIG.SYS"
sys.vdd_path = sys.boot_disk"\OS2\BOOT"
sys.ins_path = directory()
sys.reb_need = 0
sys.sav_need = 0

if MsgYesNo( ">>> "msg.uninstall ) == 0 then
   exit 0

say
call LogDo inf, msg.wait_stopped
"layer3.exe /stop"

call DeleteObject "<KL2_FLD>"

/* Modifying CONFIG.SYS */

do i = 1 while lines(sys.cfg_file)

  cfg.i = linein(sys.cfg_file)
  parse value translate(cfg.i) with command "=" devpath
   
  if command == "DEVICE" & lastpos( "LAYER3.SYS", devpath ) \= 0 then do
     i = i - 1
     sys.sav_need = 1
  end
end

cfg.0 = i - 1
rc = stream( sys.cfg_file, "c", "close" )

if sys.sav_need then do

   sys.cfg_save = SysTempFileName( sys.boot_disk"\config.???" )
   call CopyFile sys.cfg_file, sys.cfg_save
  "@del "sys.cfg_file

   do i = 1 to cfg.0
      rc = lineout( sys.cfg_file, cfg.i )
   end

   rc = stream( sys.cfg_file, "c", "close" )
   sys.rebo_need = 1
end

call DeleteFile "Icons\foldera.ico"
call DeleteFile "Icons\folderb.ico"
call DeleteFile "Icons\layout.ico"
call DeleteFile "Icons\uninstl.ico"
call DeleteFile "Icons\read.ico"
call DeleteFile "Icons\url.ico"
call DeleteFile "NLS\readme.001"
call DeleteFile "NLS\readme.007"
call DeleteFile "NLS\insmsg.001"
call DeleteFile "NLS\insmsg.007"
call DeleteFile "license"
call DeleteFile "layer3.cat"
call DeleteFile "layer3.exe"
call DeleteFile "layer3.dll"
call DeleteFile "layout.exe"
call DeleteFile "uninstl.cmd"

if stream( sys.vdd_path"\layer3.sys", "c", "query exist" ) \= "" then
   call DeleteFile sys.vdd_path"\layer3.sys"
if stream( "install.log", "c", "query exist" ) \= "" then
   call DeleteFile "install.log"
if stream( "layout.out" , "c", "query exist" ) \= "" then
   call DeleteFile "layout.out"

call DeleteDirectory "NLS"
call DeleteDirectory "Icons"
call directory ".."
call DeleteDirectory sys.ins_path

rc = SysIni(  "BOTH", "Keyboard Layer/2",, "DELETE:" )

call LogDo inf, "Done!"
say; pause
exit 0

exit

/*------------------------------------------------------------------
 * Read messages
 *------------------------------------------------------------------*/
MsgRead: procedure expose (globals)

  parse arg msgfile
  parse source OS2 what msgpath

  msgfile = filespec( "disk", msgpath ) ||,
            filespec( "path", msgpath ) || "NLS\" || msgfile

  country = strip( SysIni( "BOTH", "PM_National", "iCountry" ),, '0'x )

  if country == "ERROR:" then
     country =  "001"
  else
     country =  right( country, 3, "0" )

  if stream( msgfile"."country, "c", "query exists" ) == "" then
     country = "001"

  msgfile = msgfile"."country
  rc = stream( msgfile, "C", "OPEN READ" )

  if rc \= "READY:" then do
     say  color.error || "þþþ Error on open NLS file: "msgfile
     pause
     exit 1
  end

  do while lines(msgfile) > 0
     line = strip(linein(msgfile))

     do while right(line,1) == "\"
        line = left( line, length(line)-1 )
        line = line || strip(linein(msgfile))
     end

     if line \= "" & left(line,1) \= "#" then do
        parse value line with id "=" msg

        id  = strip(id )
        msg = strip(msg)

        i = pos( "\n", msg )
        do while i > 0
           msg = substr( msg, 1, i-1 ) || '0D0A'x || substr( msg, i+2 )
           i = pos( "\n", msg )
        end

        msg.id = msg
     end
  end

  rc = stream( msgfile, "C", "CLOSE" )
return

/*------------------------------------------------------------------
 * Get Yes or No
 *------------------------------------------------------------------*/
MsgYesNo: procedure expose (globals)

  parse arg prompt
  ok = 0

  do until ok
     call charout, prompt"? "
     pull reply
     reply = left(reply,1)

     ok = (reply == "Y") |,
          (reply == "N") |,
          (pos( reply, msg.yes ) > 0 ) |,
          (pos( reply, msg.no  ) > 0 )

     if \ok then do
        say msg.bad_yesno
     end
  end

return (reply = "Y") | ( pos( reply, msg.yes ) > 0 )

/*------------------------------------------------------------------
 * Display Log Record
 *------------------------------------------------------------------*/
LogDo: procedure expose (globals)

  parse arg type, message
  type = translate(type)

  if type == "INF" then type = "INFO"
  if type == "ERR" then type = "ERROR"

  say color.type"þþþ" message || color.usual
return ""

/*------------------------------------------------------------------
 * Copy file
 *------------------------------------------------------------------*/
CopyFile: procedure expose (globals)

 parse arg from, to
 'copy "'from'" "'to'" 1>nul 2>nul'

 if rc == 0 then
    call LogDo inf, msg.copy_done || ": "from 
 else
    call LogDo err, msg.copy_error || ": "from", rc="rc

return rc == 0

/*------------------------------------------------------------------
 * Delete WPS object
 *------------------------------------------------------------------*/
DeleteObject: procedure expose (globals)

 parse arg id
 rc = SysDestroyObject( id )

 if rc then
    call LogDo inf, msg.erase_done  || ": "id
 else
    call LogDo err, msg.erase_error || ": "id
return rc

/*------------------------------------------------------------------
 * Delete file
 *------------------------------------------------------------------*/
DeleteFile: procedure expose (globals)

 parse arg file
 'del "'file'" /F 1>nul 2>nul'

 if rc == 0 then
    call LogDo inf, msg.erase_done  || ": "file
 else
    call LogDo err, msg.erase_error || ": "file", rc="rc
return rc == 0

/*------------------------------------------------------------------
 * Remove directory
 *------------------------------------------------------------------*/
DeleteDirectory: procedure expose (globals)

 parse arg path
 rc = SysRmDir(path)

 if rc == 0 then
    call LogDo inf, msg.erase_done  || ": "path
 else
    call LogDo err, msg.erase_error || ": "path", rc="rc
return rc == 0
