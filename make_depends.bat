@rem project generation script
@rem author: Greg Wessels
@echo off

set in_dir=%~dp0
cd %in_dir%

@rem Setup build directory structure
mkdir build >nul 2>&1
set out_dir=%in_dir%build
mkdir %out_dir%\include >nul 2>&1
mkdir %out_dir%\lib >nul 2>&1
mkdir %out_dir%\lib\x86 >nul 2>&1
mkdir %out_dir%\lib\x86\debug >nul 2>&1
mkdir %out_dir%\lib\x86\release >nul 2>&1

set opt_filters=0
:arg
if "%1"=="" goto continue	
   if "%1"=="--msvc-filters" set opt_filters=1
shift
goto arg
:continue

echo in_dir=%in_dir%
echo out_dir=%out_dir%

@rem Download GYP.
if exist build\gyp goto gyp_good
echo git clone https://git.chromium.org/external/gyp.git build/gyp
git clone https://git.chromium.org/external/gyp.git build/gyp
if errorlevel 1 goto gyp_fail
goto gyp_good

:gyp_fail
echo Failed to download gyp. Make sure you have git installed, or
echo manually install gyp into %out_dir%\gyp.
exit /b 1

:gyp_good
set gyp_home=%out_dir%\gyp

@rem Select Visual Studio 2013
echo Searching for VS...
if not defined VS120COMNTOOLS goto vs_fail
if not exist "%VS120COMNTOOLS%\..\..\vc\vcvarsall.bat" goto vs_fail
call "%VS120COMNTOOLS%\..\..\vc\vcvarsall.bat" x86
goto libuv

:vs_fail
echo Visual Studio not found.  
echo Download Visual Studio 2013 Express for Windows Desktop.
exit /b 1

@rem download libuv.
:libuv
set GYP_MSVS_VERSION=2013
if exist %out_dir%\third_party\libuv goto libuv_good
echo git clone https://github.com/joyent/libuv.git build/third_party/libuv
git clone https://github.com/joyent/libuv.git build/third_party/libuv
if errorlevel 1 goto libuv_fail
goto libuv_build
:libuv_fail
echo Failed to download libuv.
exit /b 1
:libuv_build
set libuv_home=%out_dir%\third_party\libuv
cd %libuv_home%
call vcbuild.bat release x86
call vcbuild.bat debug x86
mkdir "%out_dir%\include\uv"
xcopy /q /y include\*.h "%out_dir%\include\uv\"
xcopy /q /y Release\lib\libuv.lib "%out_dir%\lib\x86\release\"
xcopy /q /y Debug\lib\libuv.lib "%out_dir%\lib\x86\debug\"
:libuv_good

cd %in_dir%

:freeimage
if exist build\third_party\FreeImage goto freeimage_good
cscript "%in_dir%tools\zip.vbs" "%in_dir%third_party\freeimage.zip" "%out_dir%\third_party\"
msbuild "%out_dir%\third_party\FreeImage\FreeImage.2008.sln" ^
		/m /t:Build /p:Configuration=Release ^
		/p:Platform="Win32" /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo
		
msbuild "%out_dir%\third_party\FreeImage\FreeImage.2008.sln" ^
		/m /t:Build /p:Configuration=Debug ^
		/p:Platform="Win32" /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo
		
copy "%out_dir%\third_party\FreeImage\Dist\FreeImage.h" "%out_dir%\include\freeimage.h" /y
move "%out_dir%\third_party\FreeImage\Dist\FreeImage.lib" "%out_dir%\lib\x86\release\FreeImage.lib"
move "%out_dir%\third_party\FreeImage\Dist\FreeImaged.lib" "%out_dir%\lib\x86\debug\FreeImage.lib"
:freeimage_good

:freetype
if exist build\third_party\freetype-2.4.11 goto freetype_good
cscript "%in_dir%tools\zip.vbs" "%in_dir%third_party\freetype-2.4.11.zip" "%out_dir%\third_party\"
msbuild "%out_dir%\third_party\freetype-2.4.11\builds\win32\vc2010\freetype.sln" ^
		/m /t:Build /p:Configuration="Release Multithreaded" ^
		/p:Platform="Win32" /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo
		
msbuild "%out_dir%\third_party\freetype-2.4.11\builds\win32\vc2010\freetype.sln" ^
		/m /t:Build /p:Configuration="Debug Multithreaded" ^
		/p:Platform="Win32" /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo
		
move "%out_dir%\third_party\freetype-2.4.11\objs\win32\vc2010\freetype2411MT.lib" ^
		"%out_dir%\lib\x86\release\freetype.lib"
		
move "%out_dir%\third_party\freetype-2.4.11\objs\win32\vc2010\freetype2411MT_D.lib" ^
		"%out_dir%\lib\x86\debug\freetype.lib"

xcopy /q /y /S "%out_dir%\third_party\freetype-2.4.11\include\*" "%out_dir%\include\"
:freetype_good

:v8
if exist %out_dir%\third_party\v8 goto v8_good
echo git clone https://github.com/v8/v8.git build/third_party/v8
git clone https://github.com/v8/v8.git build/third_party/v8
if errorlevel 1 goto v8_fail
goto v8_build

:v8_fail
echo Failed to download v8.
exit /b 1

:v8_build
mkdir "%out_dir%\third_party\v8\third_party\"
cscript "%in_dir%tools\zip.vbs" ^
		"%in_dir%third_party\cygwin.zip" ^
		"%out_dir%\third_party\v8\third_party\"
		
cscript "%in_dir%tools\zip.vbs" ^
		"%in_dir%third_party\icu.zip" ^
		"%out_dir%\third_party\v8\third_party\"
		
xcopy /q /y /S "%out_dir%\gyp" "%out_dir%\third_party\v8\build\gyp\"
xcopy /q /y /S "%out_dir%\third_party\v8\include\*.h" "%out_dir%\include\v8\"

cd "%out_dir%\third_party\v8"
python "build\gyp_v8"

msbuild "%out_dir%\third_party\v8\build\all.sln" ^
		/m /t:Build /p:Configuration="Release" ^
		/p:Platform="Win32" /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo

msbuild "%out_dir%\third_party\v8\build\all.sln" ^
		/m /t:Build /p:Configuration="Debug" ^
		/p:Platform="Win32" /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo

xcopy /q /y /S "%out_dir%\third_party\v8\build\Release\lib\*.lib" "%out_dir%\lib\x86\release\"
xcopy /q /y /S "%out_dir%\third_party\v8\build\Debug\lib\*.lib" "%out_dir%\lib\x86\debug\"
:v8_good


:vs_generate
echo Generating Visual Studio projects...
cd %in_dir%
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
      echo Removing msvc project *.filters
	  del "%out_dir%\*.filters"
    )
	
   echo Generation successful. 	
   exit /b

:failed
   echo Generation failed. (err=%ERRORLEVEL%)