@rem project generation script
@rem author: Greg Wessels
@echo off

set in_dir=%~dp0
cd %in_dir%

mkdir build >nul 2>&1
set out_dir=%in_dir%build

@rem extract pre-built dependencies.
if exist build\lib goto winpkg_good
cscript "%in_dir%tools\zip.vbs" "%in_dir%deps\winpkg.zip" "%out_dir%\"
:winpkg_good

set opt_filters=0
:arg
if "%1"=="" goto continue	
   if "%1"=="--msvc-filters" set opt_filters=1
shift
goto arg
:continue

echo in_dir=%in_dir%
echo out_dir=%out_dir%

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

@rem download libuv.
if exist build\third_party\libuv goto libuv_good
echo git clone https://github.com/joyent/libuv.git build/third_party/libuv
git clone https://github.com/joyent/libuv.git build/third_party/libuv
if errorlevel 1 goto libuv_fail
goto libuv_build

:libuv_fail
echo Failed to download libuv.
exit /b 1

:libuv_build
set libuv_home=%in_dir%.\build\third_party\libuv
cd %libuv_home%
call vcbuild.bat release x86
call vcbuild.bat debug x86
mkdir "%out_dir%\include\uv"
xcopy /q /y include\*.h "%out_dir%\include\uv\"
xcopy /q /y Release\lib\libuv.lib "%out_dir%\lib\x86\release\"
xcopy /q /y Debug\lib\libuv.lib "%out_dir%\lib\x86\debug\"
:libuv_good

cd %in_dir%

echo generating msvc projects...

call "%gyp_home%\gyp.bat" reno.gyp ^
	--debug=all ^
	--depth=. ^
	-f msvs -G msvs_version=2013 ^
	--generator-output="%out_dir%" ^
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