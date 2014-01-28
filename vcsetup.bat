@rem project generation script
@rem author: Greg Wessels
@echo off

set in_dir=%~dp0
set out_dir=%in_dir%build
set gyp_home=%in_dir%.\tools\gyp
set opt_filters=0

:arg
if "%1"=="" goto continue	
   if "%1"=="--msvc-filters" set opt_filters=1
shift
goto arg
:continue

echo.
echo generating projects...
echo.

call "%gyp_home%\gyp.bat" reno.gyp^
	--debug=all^
	--depth=.^
	-f msvs -G msvs_version=2013^
	--generator-output="%out_dir%"^
	-Darch=x86

echo.
 
if ERRORLEVEL 0 goto success
goto failed
	
:success
   if "%opt_filters%"=="0" (
      echo removing msvc project *.filters
	  del "%out_dir%\*.filters"
    )
	
   echo generation successful. 	
   exit /b

:failed
   echo generation failed. (err=%ERRORLEVEL%)
 