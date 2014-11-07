/* REXX installation script for Keyboard Layer/2 
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
say color.bold  || "Keyboard Layer/2" || color.usual || " Installation"
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
sys.dll_path = sys.boot_disk"\OS2\DLL"
sys.vdd_path = sys.boot_disk"\OS2\BOOT"
sys.vdd_inst = 0
sys.reb_need = 0
sys.sav_need = 0

/* Customizing of installation process */

sys.ins_path = strip( SysIni( "BOTH", "Keyboard Layer/2", "Home" ),, '0'x )

if sys.ins_path == "ERROR:" then
   sys.ins_path = ""
else if MsgYesNo( ">>> "msg.install_in" "sys.ins_path ) == 0 then
   sys.ins_path = ""

do while sys.ins_path == ""
   call charout, ">>> "msg.install_dir": "
   parse pull sys.ins_path
end

sys.vdd_inst = MsgYesNo( ">>> "msg.install_vdd )

if MsgYesNo( ">>> "msg.install ) == 0 then
   exit 0

say
call LogDo inf, msg.wait_stopped
"layer3.exe /stop"

/* Create of installation directorys */

if \CreateDirectory( sys.ins_path        , 0 ) then call terminate
if \CreateDirectory( sys.ins_path"\NLS"  , 0 ) then call terminate
if \CreateDirectory( sys.ins_path"\Icons", 0 ) then call terminate

/* Copying files */

if \CopyFile( "Icons\foldera.ico", sys.ins_path"\Icons\foldera.ico" ) then call terminate
if \CopyFile( "Icons\folderb.ico", sys.ins_path"\Icons\folderb.ico" ) then call terminate
if \CopyFile( "Icons\layout.ico" , sys.ins_path"\Icons\layout.ico"  ) then call terminate
if \CopyFile( "Icons\uninstl.ico", sys.ins_path"\Icons\uninstl.ico" ) then call terminate
if \CopyFile( "Icons\read.ico"   , sys.ins_path"\Icons\read.ico"    ) then call terminate
if \CopyFile( "Icons\url.ico"    , sys.ins_path"\Icons\url.ico"     ) then call terminate
if \CopyFile( "NLS\readme.001"   , sys.ins_path"\NLS\readme.001"    ) then call terminate
if \CopyFile( "NLS\readme.007"   , sys.ins_path"\NLS\readme.007"    ) then call terminate
if \CopyFile( "NLS\insmsg.001"   , sys.ins_path"\NLS\insmsg.001"    ) then call terminate
if \CopyFile( "NLS\insmsg.007"   , sys.ins_path"\NLS\insmsg.007"    ) then call terminate
if \CopyFile( "license"          , sys.ins_path"\license"           ) then call terminate
if \CopyFile( "layer3.cat"       , sys.ins_path"\layer3.cat"        ) then call terminate
if \CopyFile( "layer3.exe"       , sys.ins_path"\layer3.exe"        ) then call terminate
if \CopyFile( "layer3.dll"       , sys.ins_path"\layer3.dll"        ) then call terminate
if \CopyFile( "layout.exe"       , sys.ins_path"\layout.exe"        ) then call terminate
if \CopyFile( "uninstl.cmd"      , sys.ins_path"\uninstl.cmd"       ) then call terminate

if sys.vdd_inst then do
   if \CopyFile( "layer3.sys"    , sys.vdd_path"\layer3.sys"        ) then call terminate
end

if SysOs2Ver() <= "2.30" then do
   if stream( sys.dll_path"cyrime.dll", "C", "QUERY EXIST"    ) == "" then do
      call CopyFile "OS3\cyrime.dll", sys.dll_path"\cyrime.dll"
      sys.reb_need = 1
   end
   if stream( sys.dll_path"\pmbidi.dll", "C", "QUERY EXIST"   ) == "" then do
      call CopyFile "OS3\pmbidi.dll", sys.dll_path"\pmbidi.dll"
      sys.reb_need = 1
   end
   if stream( sys.dll_path"\bdcall32.dll", "C", "QUERY EXIST" ) == "" then do
      call CopyFile "OS3\bdcall32.dll", sys.dll_path"\bdcall32.dll"
      sys.reb_need = 1
   end
end

/* Create Folders and Objects */

call CreateObject "R", "WPFolder", "Keyboard Layer/2",,
                  "<WP_DESKTOP>",,
                  "OBJECTID=<KL2_FLD>;"                           ||,
                  "ICONFILE="sys.ins_path"\Icons\foldera.ico;"    ||,
                  "ICONNFILE=1,"sys.ins_path"\Icons\folderb.ico;"

call CreateObject "R", "WPProgram", "Keyboard Layer/2",,
                  "<KL2_FLD>",,
                  "OBJECTID=<KL2_EXE>;"                           ||,
                  "EXENAME="sys.ins_path"\layer3.exe;"            ||,
                  "STARTUPDIR="sys.ins_path

call CreateObject "R", "WPShadow", "Keyboard Layer/2",,
                  "<WP_START>",,
                  "SHADOWID=<KL2_EXE>;"

call CreateObject "R", "WPProgram", "Select a Keyboard Layout/2",,
                  "<KL2_FLD>",,
                  "OBJECTID=<KL2_SEL>;"                           ||,
                  "EXENAME=*;STARTUPDIR="sys.ins_path";"          ||,
                  "PROGTYPE=WINDOWABLEVIO;"                       ||,
                  "ICONFILE="sys.ins_path"\Icons\layout.ico;"     ||,
                  "PARAMETERS=/c (LAYOUT.EXE [Parameters for LAYOUT (-h for usage)] > layout.out 2>&1 & e.exe layout.out )"

call CreateObject "R", "WPProgram", "Readme (Russian)",,
                  "<KL2_FLD>",,
                  "OBJECTID=<KL2_RRU>;"                           ||,
                  "ICONFILE="sys.ins_path"\Icons\read.ico;"       ||,
                  "EXENAME=E.EXE;PROGTYPE=PM;"                    ||,
                  'PARAMETERS="'sys.ins_path'\NLS\readme.007"'

call CreateObject "R", "WPProgram", "Readme (English)",,
                  "<KL2_FLD>",,
                  "OBJECTID=<KL2_REN>;"                           ||,
                  "ICONFILE="sys.ins_path"\Icons\read.ico;"       ||,
                  "EXENAME=E.EXE;PROGTYPE=PM;"                    ||,
                  'PARAMETERS="'sys.ins_path'\NLS\readme.001"'

call CreateObject "R", "WPProgram", "License (English)",,
                  "<KL2_FLD>",,
                  "OBJECTID=<KL2_LEN>;"                           ||,
                  "ICONFILE="sys.ins_path"\Icons\read.ico;"       ||,
                  "EXENAME=E.EXE;PROGTYPE=PM;"                    ||,
                  'PARAMETERS="'sys.ins_path'\license"'

if SysOs2Ver() > "2.30" then do

call CreateObject "R", "WPUrl", "GlassMan Website",,
                  "<KL2_FLD>",,
                  "OBJECTID=<KL2_URL>;"                           ||,
                  "ICONFILE="sys.ins_path"\Icons\url.ico;"        ||,
                  "URL=http://glass.ptv.ru/klayer.html;"
end

call CreateObject "R", "WPProgram", "Uninstall Keyboard Layer/2",,
                  "<KL2_FLD>",,
                  "OBJECTID=<KL2_UNI>;"                           ||,
                  'ICONFILE='sys.ins_path'\Icons\uninstl.ico;'    ||,
                  'EXENAME=*;PROGTYPE=WINDOWABLEVIO;'             ||,
                  'STARTUPDIR='sys.ins_path';'                    ||,
                  'PARAMETERS=/C start "Uninstall Keyboard Layer/2" /C/F "UNINSTL.CMD";'

/* Modifying CONFIG.SYS */

do i = 1 while lines(sys.cfg_file)

  cfg.i = linein(sys.cfg_file)
  parse value translate(cfg.i) with command "=" devpath
   
  if command == "DEVICE" & ( lastpos( "LAYER.SYS" , devpath ) \= 0 |,
                             lastpos( "LAYER3.SYS", devpath ) \= 0 ) then do
     i = i - 1
     sys.sav_need = 1
  end
end

if sys.vdd_inst then do
   cfg.i = "DEVICE="sys.vdd_path"\layer3.sys"
   sys.sav_need = 1
   i = i + 1
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

/* Delete old files */

old.1  = sys.ins_path"\layer.bmp"
old.2  = sys.ins_path"\cyrime.dll"
old.3  = sys.ins_path"\key32.dll"
old.4  = sys.ins_path"\foldera.ico"
old.5  = sys.ins_path"\folderb.ico"
old.6  = sys.ins_path"\uninstl.ico"
old.7  = sys.ins_path"\instlen.msg"
old.8  = sys.ins_path"\instlru.msg"
old.9  = sys.ins_path"\readme.eng"
old.10 = sys.ins_path"\readme.rus"
old.11 = sys.ins_path"\install.001"
old.12 = sys.ins_path"\install.007"
old.13 = sys.ins_path"\Icons\layer.bmp"
old.14 = sys.ins_path"\OS30DLL\bdcall32.dll"
old.15 = sys.ins_path"\OS30DLL\cyrime.dll"
old.16 = sys.ins_path"\OS30DLL\pmbidi.dll"
old.17 = sys.ins_path"\layer.cat"
old.18 = sys.ins_path"\layer.exe"
old.19 = sys.ins_path"\layer.sys"
old.20 = sys.ins_path"\layer.dll"
old.21 = sys.ins_path"\layer3.sys"
old.22 = sys.ins_path"\NLS\donate.001"
old.23 = sys.ins_path"\Icons\donate.ico"
old.0  = 23

do i = 1 to old.0
   if stream( old.i, "c", "query exists" ) \= "" then do
      '@del "'old.i'" 1> nul 2> nul'
   end
end

call SysIni "USER", "Keyboard Layer/2", "Home", sys.ins_path
call LogDo inf, "Done!"

if sys.reb_need then do
   say '07'x 
   say color.usual || msg.reboot
  "pause"
end

exit 0

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
 * Write Log Record
 *------------------------------------------------------------------*/
LogDo: procedure expose (globals)

  parse arg type, message
  type = translate(type)

  if type == "INF" then type = "INFO"
  if type == "ERR" then type = "ERROR"

  rc = stream( sys.ins_path"\install.log", "c", "open write" )
  call lineout sys.ins_path"\install.log", message
  rc = stream( sys.ins_path"\install.log", "c", "close" )

  say color.type"þþþ" message || color.usual
return ""

/*------------------------------------------------------------------
 * Create directory
 *------------------------------------------------------------------*/
CreateDirectory: procedure expose (globals)

 parse arg path, silence
 rc = 0

 call setlocal
 exist = ( directory( path ) \= "" )
 call endlocal

 if \exist then do
    rc = SysMkDir( path )
    if rc == 3 & pos( "\", path ) \= 0 then do

       parent = left( path, length(path) - pos( "\", reverse(path)))

       if CreateDirectory( parent, 1 ) then
          rc = SysMkDir( path )
    end

    if \silence then do
      if rc == 0 then
         call LogDo inf, msg.dir_done  || ": "path
      else
         call LogDo err, msg.dir_error || ": "path", rc="rc
    end
 end
return rc == 0

/*------------------------------------------------------------------
 * Copy file
 *------------------------------------------------------------------*/
CopyFile: procedure expose (globals)

 parse arg from, to
 'lcopy3 "'from'" "'to

 select
   when rc == 0 then do
      call LogDo inf, msg.copy_done || ": "from 
   end  
   when rc == 1 then do
      call LogDo inf, msg.copy_done || ": "from 
      sys.reb_need = 1
   end
   otherwise
      call LogDo err, msg.copy_error || ": "from", rc="rc
 end

return rc == 0 | rc == 1

/*------------------------------------------------------------------
 * Create WPS object
 *------------------------------------------------------------------*/
CreateObject: procedure expose (globals)

   parse arg action, class, objname, folder, prmstr

   rc = SysCreateObject( class, objname, folder, prmstr, action )

   i = pos( '0D0A'x, objname )
   do while i > 0
      objname = substr( objname, 1, i-1 ) || " " || substr( objname, i+2 )
      i = pos( '0D0A'x, objname )
   end

   if rc then
      call LogDo inf, msg.object_done  || " "substr(class,1,12) || ": "objname
   else
      call LogDo err, msg.object_error || " "substr(class,1,12) || ": "objname
return rc

/*------------------------------------------------------------------
 * Terminate installation
 *------------------------------------------------------------------*/
Terminate: procedure expose (globals)

  call beep 1000, 100
  call LogDo err, msg.install_aborted
  exit 1
return
