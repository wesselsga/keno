@rem project generation script
@rem author: Greg Wessels
@echo off

cd %~dp0

set in_dir=%~dp0
set out_dir=%in_dir%build
set opt_filters=0

:arg
if "%1"=="" goto continue	
   if "%1"=="--msvc-filters" set opt_filters=1
shift
goto arg
:continue


@rem Generate the VS project.
if exist build\gyp goto gyp_good
echo git clone https://git.chromium.org/external/gyp.git build/gyp
git clone https://git.chromium.org/external/gyp.git build/gyp
if errorlevel 1 goto gyp_fail
goto gyp_good

:gyp_fail
echo Failed to download gyp. Make sure you have git installed, or
echo manually install gyp into %~dp0build\gyp.
exit /b 1

:gyp_good
set gyp_home=%in_dir%.\build\gyp

echo generating msvc projects...
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