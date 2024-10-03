@rem set VSCMD_DEBUG=2

call :DetectVsPath     VisualStudioCmd
call :DetectProgramDir ProgramDir

echo ProgramDir=%ProgramDir%

@rem copy compiler\mr_helper.mrp compiler\mr_helpere.mrp

set CurDir=%~dp0
set ProjDir=%CurDir:~0,-1%

set old_sys_include="%include%"
set old_sys_lib="%lib%"
set old_sys_path="%path%"

set PERL5LIB=%PERL5LIB%
set PerlPath=%ProgramDir%\Perl\bin
set NASMPath=%ProgramDir%\nasm\bin
set CMakePath=%ProgramDir%\cmake\bin
set PythonHome=%ProgramDir%\python
set PATH=%NASMPath%;%PerlPath%;%CMakePath%;%PythonHome%;%PATH%

set CurDir=%~dp0

set ProjDir=%CurDir:~0,-1%
echo ProjDir %ProjDir%
set software_dir="%ProjDir%\thirdparty"
set HomeDir=%ProjDir%\out\windows

set SystemBinDir=.\

set BuildType=Release
set ProjName=

call %VisualStudioCmd% x64

set all_srcs=src\main.c
set objs_dir=out
set lib_root_dir=out\windows\pcre-8.42
set lib_dir=%lib_root_dir%\lib
set inc_dir=%lib_root_dir%\include
set lib_name=%lib_dir%\pcred.lib
set obj_name=main.obj
set func_name=pcre_config
call :CompileAllSrcs      "%all_srcs%"     "%objs_dir%"
call :search_func_in_lib  "%lib_name%"     "%func_name%"
call :search_func_in_lib  "%obj_name%"     "%func_name%"

pause
goto :eof

:DetectProgramDir
    setlocal EnableDelayedExpansion
    @rem SkySdk\VS2005\VC
    set SkySdkDiskSet=C;D;E;F;G;
    set CurProgramDir=
    set idx=0
    call :color_text 2f "+++++++++++++++++++DetectProgramDir+++++++++++++++++++++++"
    for %%i in (%SkySdkDiskSet%) do (
        set /a idx+=1
        for /f "tokens=1-2 delims=|" %%B in ("programs|program") do (
            set CurProgramDir=%%i:\%%B
            echo [!idx!] !CurProgramDir!
            if exist !CurProgramDir!\SkySdk (
                goto :DetectProgramDirBreak
            )
            set CurProgramDir=%%i:\%%C
            echo [!idx!] !CurProgramDir!
            if exist !CurProgramDir!\SkySdk (
                goto :DetectProgramDirBreak
            )
        )
    )
    :DetectProgramDirBreak
    set ProgramDir=!CurProgramDir!
    call :color_text 2f "--------------------DetectProgramDir-----------------------"
    endlocal & set %~1=%ProgramDir%
goto :eof

:CheckLibInDir
    setlocal EnableDelayedExpansion
    set Libs=%~1
    set LibDir="%~2"
    set ProjDir=%~3
    set MyPlatformSDK=%ProjDir%\lib
    if not exist "%MyPlatformSDK%" (
        mkdir %MyPlatformSDK%
    )
    call :color_text 2f "+++++++++++++++++++CheckLibInDir+++++++++++++++++++++++"
    echo LibDir %LibDir%
    if not exist %LibDir% (
        call :color_text 4f "--------------------CheckLibInDir-----------------------"
        goto :eof
    )

    pushd %LibDir%
    set idx=0
    for %%i in (%Libs%) do (
        set /a idx+=1
        set CurLib=%%i
        echo [!idx!] !LibDir!\!CurLib!
        if not exist !LibDir!\!CurLib! (
            echo !LibDir!\!CurLib!
        ) else (
            copy !LibDir!\!CurLib! %MyPlatformSDK%
        )
    )
    popd
    call :color_text 2f "--------------------CheckLibInDir-----------------------"
    endlocal
goto :eof

:DetectVsPath
    setlocal EnableDelayedExpansion
    set VsBatFileVar=%~1
    call :color_text 2f "++++++++++++++++++DetectVsPath++++++++++++++++++++++++"
    set VSDiskSet=C;D;E;F;G;
    set AllProgramsPathSet=program
    set AllProgramsPathSet=%AllProgramsPathSet%;programs
    set AllProgramsPathSet=%AllProgramsPathSet%;"Program Files"
    set AllProgramsPathSet=%AllProgramsPathSet%;"Program Files (x86)"
    set VCPathSet="Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build"
    set VCPathSet=%VCPathSet%;SkySdk\VS2005\VC
    set VCPathSet=%VCPathSet%;"Microsoft Visual Studio 8\VC"
    set VCPathSet=%VCPathSet%;"Microsoft Visual Studio 12.0\VC\bin"
    set VCPathSet=%VCPathSet%;"Microsoft Visual Studio 14.0\VC\bin"
    set VCPathSet=%VCPathSet%;"Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build"
    set VCPathSet=%VCPathSet%;"Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build"
    set idx_a=0
    for %%C in (%VCPathSet%) do (
        set /a idx_a+=1
        set idx_b=0
        for %%B in (!AllProgramsPathSet!) do (
            set /a idx_b+=1
            set idx_c=0
            for %%A in (!VSDiskSet!) do (
                set /a idx_c+=1
                set CurBatFile=%%A:\%%B\%%C\vcvarsall.bat
                echo [!idx_a!][!idx_b!][!idx_c!] !CurBatFile!
                if exist !CurBatFile! (
                    goto DetectVsPathBreak
                )
            )
        )
    )
    :DetectVsPathBreak
    echo Use:%CurBatFile%
    call :color_text 2f "--------------------DetectVsPath-----------------------"
    endlocal & set "%~1=%CurBatFile%"
goto :eof

:CompileAllSrcs
    setlocal EnableDelayedExpansion
    set AllSrcs=%~1
    set ObjsDir="%~2"
    set PcreDir=%HomeDir%\pcre-8.42
    call :color_text 2f "++++++++++++++++++CompileAllSrcs++++++++++++++++++++++++"
    set IncDir=/I .\inc    /I .\src    /I  .\    /I  %PcreDir%\include
    set PcreDefs=/D PCRE_STATIC
    set WinDefs=/D _WINDOWS  /D WIN32  /D _NDEBUG  /D NDEBUG  
    set AllDefs=%PcreDefs%  /TC  /MD
    set OutDir=out
    if not exist %OutDir% (
        mkdir %OutDir%
    )
    set idx=0
    set AllObjs=
    for %%i in (%AllSrcs%) do (
        set /a idx+=1
        set srcFile=%%i
        set objFile=%OutDir%\%%~ni.obj
        set ext=%%~xi
        echo [!idx!] !srcFile!  !objFile!
        set AllObjs=!AllObjs! !objFile!
        echo cl  %IncDir%  %AllDefs%  /Fo"!objFile!"  /c  !srcFile!
        cl  %IncDir%  %AllDefs%  /Fo"!objFile!"  /c  !srcFile!
    )

    set LibDir=/LIBPATH:.\    /LIBPATH:.\lib    /LIBPATH:%PcreDir%\lib
    set PcreOpts=pcred.lib 
    set WinOpts=/subsystem:windows    /entry:WinMainCRTStartup
    set ArmOpts=/SAFESEH:NO    /NODEFAULTLIB:LIBCD.lib    /NODEFAULTLIB:LIBC.lib    /NODEFAULTLIB:LIBCMT.lib    /NODEFAULTLIB:LIBCMTD.lib    /INCLUDE:_sim_dummy    MSCOREE.lib
    set AllOpts=%PcreOpts%
    echo     link    /link    %LibDir%    %AllOpts%    %AllObjs% /OUT:%OutDir%\app.exe
    link    /link    %LibDir%    %AllOpts%    %AllObjs% /OUT:%OutDir%\app.exe
    call :color_text 2f "-----------------CompileAllSrcs-----------------"
    endlocal
goto :eof

:search_func_in_lib
    setlocal ENABLEDELAYEDEXPANSION
    set lib_name=%1
    set func_name=%~2
    call:color_text 2f "++++++++++++++++++dumpbin SYMBOLS lib_name++++++++++++++++++"
    @rem dumpbin /?
    @rem dumpbin /ARCHIVEMEMBERS  %lib_name%
    @rem dumpbin /HEADERS %lib_name%  /SECTION:.drectve
    dumpbin /HEADERS %lib_name%  | grep "machine"
    dumpbin /SYMBOLS %lib_name%  | grep "%func_name%"
    call:color_text 2f "------------------dumpbin SYMBOLS lib_name------------------"
    endlocal
goto :eof

@rem YellowBackground    6f  ef
@rem BlueBackground      9f  bf   3f
@rem GreenBackground     af  2f
@rem RedBackground       4f  cf
@rem GreyBackground      7f  8f
@rem PurpleBackground    5f

:color_text
    setlocal EnableDelayedExpansion
    for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
        set "DEL=%%a"
    )
    echo off
    <nul set /p ".=%DEL%" > "%~2"
    findstr /v /a:%1 /R "^$" "%~2" nul
    del "%~2" > nul 2>&1
    endlocal
    echo .
goto :eof

:GetCurSysTime
    setlocal EnableDelayedExpansion
    set dateStr=
    set timeStr=
    set year=%date:~6,4%
    set month=%date:~4,2%
    set day=%date:~0,2%
    set dateStr=%date: =_%
    set dateStr=%dateStr:/=x%
    set timeStr=%time::=x%
    set timeStr=%time: =%
    endlocal & set %~1=%dateStr%__%timeStr%
goto :eof
