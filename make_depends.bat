@rem project generation script
@rem author: Greg Wessels
echo off

set IN_DIR=%~dp0
cd %IN_DIR%

if "%PROCESSOR_ARCHITECTURE%"=="x86" (set ARCH=x86) else (set ARCH=x64)
if "%ARCH%"=="x86" (set VS_PLATFORM=Win32&set VS_TOOLSET=x86) else (set VS_PLATFORM=x64&set VS_TOOLSET=x86_amd64)

@rem Setup build directory structure
mkdir build >nul 2>&1
set BUILD_DIR=%IN_DIR%build
mkdir %BUILD_DIR%\include >nul 2>&1
mkdir %BUILD_DIR%\lib >nul 2>&1
mkdir %BUILD_DIR%\lib\%ARCH% >nul 2>&1
mkdir %BUILD_DIR%\lib\%ARCH%\debug >nul 2>&1
mkdir %BUILD_DIR%\lib\%ARCH%\release >nul 2>&1

@rem Select Visual Studio 2013
echo Searching for Visual Studio ...
if not defined VS120COMNTOOLS goto vs_fail
if not exist "%VS120COMNTOOLS%\..\..\vc\vcvarsall.bat" goto vs_fail
call "%VS120COMNTOOLS%\..\..\vc\vcvarsall.bat" %VS_TOOLSET%
set GYP_MSVS_VERSION=2013
goto vs_good

:vs_fail
echo Visual Studio not found.  
echo Download Visual Studio 2013 Express for Windows Desktop.
exit /b 1

:vs_good
echo IN_DIR=%IN_DIR%
echo BUILD_DIR=%BUILD_DIR%
echo ARCH=%ARCH%
echo VS_PLATFORM=%VS_PLATFORM%
echo VS_TOOLSET=%VS_TOOLSET%
echo GYP_MSVS_VERSION=%GYP_MSVS_VERSION%
echo.

@rem Download GYP.
if exist build\gyp goto gyp_good
echo git clone https://git.chromium.org/external/gyp.git build/gyp
git clone https://git.chromium.org/external/gyp.git build/gyp
if errorlevel 1 goto gyp_fail
goto gyp_good

:gyp_fail
echo Failed to download gyp. Make sure you have git installed, or
echo manually install gyp into %BUILD_DIR%\gyp.
exit /b 1

:gyp_good
set gyp_home=%BUILD_DIR%\gyp

@rem download libuv.
:libuv
echo Checking for libuv ...
set GYP_MSVS_VERSION=2013
if exist %BUILD_DIR%\third_party\libuv goto libuv_done
echo git clone https://github.com/joyent/libuv.git build/third_party/libuv
git clone https://github.com/joyent/libuv.git build/third_party/libuv

set UV_HOME=%BUILD_DIR%\third_party\libuv
cd %UV_HOME%
git clone https://git.chromium.org/external/gyp.git build/gyp

python gyp_uv.py -Dtarget_arch=%ARCH% -Duv_library=static_library

msbuild uv.sln /t:Build /p:Configuration=Release ^
	/p:Platform="%VS_PLATFORM%" ^
	/clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo

msbuild uv.sln /t:Build /p:Configuration=Debug ^
	/p:Platform="%VS_PLATFORM%" ^
	/clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo

mkdir "%BUILD_DIR%\include\uv" >nul 2>&1
xcopy /q /y include\*.h "%BUILD_DIR%\include\uv\"
xcopy /q /y Release\lib\libuv.lib "%BUILD_DIR%\lib\%ARCH%\release\"
xcopy /q /y Debug\lib\libuv.lib "%BUILD_DIR%\lib\%ARCH%\debug\"

if errorlevel 1 goto libuv_fail
goto libuv_done
:libuv_fail
echo Failed to download libuv.
exit /b 1
:libuv_done

cd %IN_DIR%

echo Checking for freeimage ...
if exist build\third_party\FreeImage goto freeimage_done
cscript "%IN_DIR%tools\zip.vbs" "%IN_DIR%third_party\freeimage.zip" "%BUILD_DIR%\third_party\"

msbuild "%BUILD_DIR%\third_party\FreeImage\FreeImage.2008.sln" ^
		/m /t:Build /p:Configuration=Release ^
		/p:Platform="%VS_PLATFORM%" /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo
		
msbuild "%BUILD_DIR%\third_party\FreeImage\FreeImage.2008.sln" ^
		/m /t:Build /p:Configuration=Debug ^
		/p:Platform="%VS_PLATFORM%" /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo
		
copy "%BUILD_DIR%\third_party\FreeImage\Dist\FreeImage.h" "%BUILD_DIR%\include\freeimage.h" /y
move "%BUILD_DIR%\third_party\FreeImage\Dist\FreeImage.lib" "%BUILD_DIR%\lib\%ARCH%\release\freeimage.lib"
move "%BUILD_DIR%\third_party\FreeImage\Dist\FreeImaged.lib" "%BUILD_DIR%\lib\%ARCH%\debug\freeimage.lib"
:freeimage_done


echo Checking for freetype ...
if exist build\third_party\freetype-2.4.11 goto freetype_done
cscript "%IN_DIR%tools\zip.vbs" "%IN_DIR%third_party\freetype-2.4.11.zip" "%BUILD_DIR%\third_party\"
msbuild "%BUILD_DIR%\third_party\freetype-2.4.11\builds\win32\vc2010\freetype.sln" ^
		/m /t:Build /p:Configuration="Release Multithreaded" ^
		/p:Platform="%VS_PLATFORM%" /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo
		
msbuild "%BUILD_DIR%\third_party\freetype-2.4.11\builds\win32\vc2010\freetype.sln" ^
		/m /t:Build /p:Configuration="Debug Multithreaded" ^
		/p:Platform="%VS_PLATFORM%" /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo
		
move "%BUILD_DIR%\third_party\freetype-2.4.11\objs\win32\vc2010\freetype2411MT.lib" ^
		"%BUILD_DIR%\lib\%ARCH%\release\freetype.lib"
		
move "%BUILD_DIR%\third_party\freetype-2.4.11\objs\win32\vc2010\freetype2411MT_D.lib" ^
		"%BUILD_DIR%\lib\%ARCH%\debug\freetype.lib"

xcopy /q /y /S "%BUILD_DIR%\third_party\freetype-2.4.11\include\*" "%BUILD_DIR%\include\"
:freetype_done


echo Checking for v8 ...
if exist %BUILD_DIR%\third_party\v8 goto v8_good
echo git clone https://github.com/v8/v8.git build/third_party/v8
git clone -b 3.24 https://github.com/v8/v8.git build/third_party/v8
if errorlevel 1 goto v8_fail
goto v8_build

:v8_fail
echo Failed to download v8.
exit /b 1

:v8_build
mkdir "%BUILD_DIR%\third_party\v8\third_party\"
cscript "%IN_DIR%tools\zip.vbs" ^
		"%IN_DIR%third_party\cygwin.zip" ^
		"%BUILD_DIR%\third_party\v8\third_party\"
		
REM cscript "%IN_DIR%tools\zip.vbs" ^
REM 		"%IN_DIR%third_party\icu.zip" ^
REM 		"%BUILD_DIR%\third_party\v8\third_party\"
		
xcopy /q /y /S "%BUILD_DIR%\gyp" "%BUILD_DIR%\third_party\v8\build\gyp\"
xcopy /q /y /S "%BUILD_DIR%\third_party\v8\include\*.h" "%BUILD_DIR%\include\v8\"

cd "%BUILD_DIR%\third_party\v8"

if "%ARCH%"=="x86" (set V8_ARCH=ia32) else (set V8_ARCH=%ARCH%)
python build\gyp_v8 -Dv8_target_arch=%V8_ARCH% ^
		-Dv8_use_snapshot=false ^
		-Dv8_enable_i18n_support=0

msbuild "%BUILD_DIR%\third_party\v8\build\all.sln" ^
  		/m /t:Build /p:Configuration="Release" ^
 		/p:Platform="%VS_PLATFORM%" /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo

msbuild "%BUILD_DIR%\third_party\v8\build\all.sln" ^
  		/m /t:Build /p:Configuration="Debug" ^
 		/p:Platform="%VS_PLATFORM%" /clp:NoSummary;NoItemAndPropertyList;Verbosity=minimal /nologo
		
move "%BUILD_DIR%\third_party\v8\build\Release\lib\v8_base.%V8_ARCH%.lib" "%BUILD_DIR%\lib\%ARCH%\release\v8_base.lib"
move "%BUILD_DIR%\third_party\v8\build\Release\lib\v8_nosnapshot.%V8_ARCH%.lib" "%BUILD_DIR%\lib\%ARCH%\release\v8_nosnapshot.lib"
move "%BUILD_DIR%\third_party\v8\build\Debug\lib\v8_base.%V8_ARCH%.lib" "%BUILD_DIR%\lib\%ARCH%\debug\v8_base.lib"
move "%BUILD_DIR%\third_party\v8\build\Debug\lib\v8_nosnapshot.%V8_ARCH%.lib" "%BUILD_DIR%\lib\%ARCH%\debug\v8_nosnapshot.lib"
:v8_good


:vs_generate
echo Generating Visual Studio projects...
cd %IN_DIR%
call "%gyp_home%\gyp.bat" keno.gyp ^
	--debug=all ^
	--depth=. ^
	-f msvs -G msvs_version=%GYP_MSVS_VERSION% ^
	--generator-output="%BUILD_DIR%" ^
	-Darch=%ARCH%

echo.
 
if ERRORLEVEL 0 goto success
goto failed
	
:success
echo Removing msvc project *.filters
del "%BUILD_DIR%\*.filters"
echo Generation successful. 	
exit /b

:failed
echo Generation failed. (err=%ERRORLEVEL%)