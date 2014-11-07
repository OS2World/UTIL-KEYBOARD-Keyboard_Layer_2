/* REXX readme for Keyboard Layer/2 
 * Copyright (C) 1998-2006 Dmitry A.Steklenev
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
 * $Id: readme.cmd,v 1.2 2006/02/22 17:15:58 glass Exp $
 */

if RxFuncQuery( 'SysLoadFuncs' ) then do
   call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
   call SysLoadFuncs
end

country  = NlsCountryID()

if stream( "NLS\readme."country, "c", "query exists" ) == "" then
   country = "001"

"@start /F e.exe NLS\readme."country
exit 0

/*------------------------------------------------------------------
 * Returns country identifier
 *------------------------------------------------------------------*/
NlsCountryID: procedure

  parse upper value VALUE( "LANG",, "OS2ENVIRONMENT" ) with lang "_" .

  select
    when lang == "RU" then country = "007" /* Use russian */
    when lang == "EN" then country = "001" /* Use english */
    otherwise
       country = strip( SysIni( "BOTH", "PM_National", "iCountry" ),, '0'x )
  end

  if country == "ERROR:" then
     country =  "001"
  else
     country =  right( country, 3, "0" )

return country
