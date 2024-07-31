@rem set VSCMD_DEBUG=2

call :DetectVsPath VisualStudioCmd

set old_sys_include="%include%"
set old_sys_lib="%lib%"
set old_sys_path="%path%"

set ProgramDir=F:\program
set PerlPath=%ProgramDir%\Perl\bin
set NASMPath=%ProgramDir%\nasm
set CMakePath=%ProgramDir%\cmake\bin
set PythonHome=%ProgramDir%\python
set PATH=%NASMPath%;%PerlPath%;%CMakePath%;%PythonHome%;%PATH%

set CurDir=%~dp0

set ProjDir=%CurDir:~0,-1%
echo ProjDir %ProjDir%
set software_dir="%ProjDir%\thirdparty"
set HomeDir=%ProjDir%\out\windows
@rem set HomeDir=%ProgramDir%

call :SetProjEnv %CurDir% include lib path CMAKE_INCLUDE_PATH CMAKE_LIBRARY_PATH CMAKE_MODULE_PATH
call :ShowProjEnv

set SystemBinDir=.\

set BuildType=Release
set ProjName=
set build_mrp_ext=%CurDir%\mythroad\build_mrp_ext.bat

call :get_suf_sub_str %ProjDir% \ ProjName
echo ProjName %ProjName%
CALL %VisualStudioCmd%

set BuildDir=BuildSrc
@rem copy srcCMakeLists.txt CMakeLists.txt /y
call :CompileProject %BuildDir% %BuildType% %ProjName%

@rem set BuildDir=BuildLib
@rem copy libCMakeLists.txt CMakeLists.txt /y
@rem call :CompileProject %BuildDir% %BuildType% %ProjName%

pause
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

    set VisualStudioCmdSet="F:\Program Files\Microsoft Visual Studio 8\VC\vcvarsall.bat"
    set VisualStudioCmdSet=%VisualStudioCmdSet%;"C:\Program Files (x86)\Microsoft Visual Studio 8\VC\vcvarsall.bat"

    set VisualStudioCmdSet=%VisualStudioCmdSet%;"C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\vcvars32.bat"
    set VisualStudioCmdSet=%VisualStudioCmdSet%;"C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\amd64\vcvars64.bat"

    set VisualStudioCmdSet=%VisualStudioCmdSet%;"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\vcvars32.bat"
    set VisualStudioCmdSet=%VisualStudioCmdSet%;"C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\amd64\vcvars64.bat"

    set VisualStudioCmdSet=%VisualStudioCmdSet%;"E:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars32.bat"
    set VisualStudioCmdSet=%VisualStudioCmdSet%;"E:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    
    set VisualStudioCmdSet=%VisualStudioCmdSet%;"E:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars32.bat"
    set VisualStudioCmdSet=%VisualStudioCmdSet%;"E:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"
    
    set VisualStudioCmdSet=%VisualStudioCmdSet%;"E:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"
    
    set VisualStudioCmdSet=%VisualStudioCmdSet%;"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"
    set VisualStudioCmdSet=%VisualStudioCmdSet%;"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    call :color_text 2f "+++++++++++++++++++DetectVsPath+++++++++++++++++++++++"
    set CurBat=
    set idx=0
    for %%i in (%VisualStudioCmdSet%) do (
        set /a idx+=1
        set CurBat=%%i
        echo [!idx!] !CurBat!
        if exist !CurBat! (
            goto DetectVsPathBreak
        )
    )
    :DetectVsPathBreak
    echo Use:%CurBat%
    call :color_text 2f "--------------------DetectVsPath-----------------------"
    endlocal & set "%~1=%CurBat%"
goto :eof

:CompileProject
    setlocal EnableDelayedExpansion
    set BuildDir=%~1
    set BuildType=%~2
    set ProjName=%~3

    if not exist %BuildDir% (
        mkdir %BuildDir%
    ) 
    if not exist %BuildDir%\%BuildType% (
        mkdir %BuildDir%\%BuildType%\
    )
    @rem for /f %%i in ('dir /s /b "out\windows\*.dll"') do (   copy %%i %BuildDir%\%BuildType%\ )
    pushd %BuildDir%
        @rem del * /q /s
        @rem cmake .. -G"Visual Studio 16 2019" -A Win64
        @rem cmake -G "Visual Studio 8 2005"  ..
        @rem cmake --build . --target clean
        cmake .. -DDYZ_DBG=ON -DCMAKE_BUILD_TYPE=%BuildType% -DCMAKE_INSTALL_PREFIX=%ProgramDir%\%ProjName%
        @rem call :ResetSystemEnv
        @rem cmake --build .  --config %BuildType%  --target %ProjName%
        cmake --build . -j16  --config %BuildType% --target  %ProjName%
    popd
    endlocal
goto :eof

:GenMythroad
    setlocal EnableDelayedExpansion

    set mythroad_dir=mythroad
    pushd %mythroad_dir%
        if not exist %build_mrp_ext% (
            call :color_text 4f "++++++++++++++GenMythroad build_mrp_ext.bat does not exist++++++++++++++"
        ) else (
            call %build_mrp_ext%
        )
    popd
    endlocal
goto :eof

:CopyMythroad
    setlocal EnableDelayedExpansion

    set work_dir=%1
    if not exist %work_dir% (
        mkdir %work_dir%
    ) 
    if not exist %work_dir%\mythroad (
        mkdir %work_dir%\mythroad
    )
    xcopy /S /E /Y wasm\dist\fs\mythroad %work_dir%\
    copy wasm\dist\fs\cfunction.ext      %work_dir%\
    xcopy /S /E /Y wasm\dist\fs %work_dir%\
    endlocal
goto :eof

:UpdateMythroad
    setlocal EnableDelayedExpansion

    set work_dir=%1
    set mythroad_dir=%work_dir%\mythroad
    set cfunction_ext=%mythroad_dir%\cfunction.ext
    if not exist %cfunction_ext% (
        call :color_text 2f "++++++++++++++UpdateMythroad file does not exist++++++++++++++"
        call :GenMythroad
    ) else (
        call :color_text 9f "++++++++++++++UpdateMythroad cfunction.ext does exist++++++++++++++"
    )

    copy %cfunction_ext%  %work_dir%\wasm\dist\fs\cfunction.ext

    endlocal
goto :eof

:RunWinSvr
    setlocal ENABLEDELAYEDEXPANSION
    set ProjName=%~1
    set BuildDir="%~2"
    set BuildType=%3
    set BinPath="%~4"
    @rem 
    if not exist %BinPath% (
        call :color_text 4f "++++++++++++++RunWinSvr file does not exist++++++++++++++"
        echo BinPath %BinPath% .
    ) else (
        sc create %ProjName% binPath= %BinPath%
        sc config %ProjName% start=auto
        @rem sc start %ProjName%
        net start %ProjName%
        if !errorlevel! equ 0 (
            sc stop %ProjName%
        ) else (
            call :color_text 4f "--------------RunWinSvr net start error --------------"
            %BinPath%
        )
        @rem sc delete %ProjName%
    )
    endlocal
goto :eof

:SetProjEnv
    setlocal ENABLEDELAYEDEXPANSION
    set loc_dir=%~1
    set loc_inc=%2
    set loc_lib=%3
    set loc_bin=%4

    call :gen_all_env %software_dir% %HomeDir% loc_inc loc_lib loc_bin cmake_inc cmake_lib cmake_path
    set all_inc=%loc_inc%;%include%;%loc_dir%\include;
    set all_lib=%loc_lib%;%lib%;%loc_dir%\lib;%loc_dir%\bin;
    set all_bin=%loc_bin%;%path%;%loc_dir%\bin;
    endlocal & set %~2=%all_inc% & set %~3=%all_lib% & set %~4=%all_bin% & set %~5=%cmake_inc% & set %~6=%cmake_lib% & set %~7=%cmake_path%
goto :eof

:ShowProjEnv
    setlocal ENABLEDELAYEDEXPANSION
    call :color_text 9f "++++++++++++++ShowProjEnv++++++++++++++"
    echo include:%include%
    echo lib:%lib%
    echo path:%path%
    echo CMAKE_INCLUDE_PATH:%CMAKE_INCLUDE_PATH%
    echo CMAKE_LIBRARY_PATH:%CMAKE_LIBRARY_PATH%
    echo CMAKE_MODULE_PATH:%CMAKE_MODULE_PATH%
    call :color_text 9f "--------------ShowProjEnv--------------"
    endlocal
goto :eof

:ResetSystemEnv
    setlocal ENABLEDELAYEDEXPANSION
    call :color_text 9f "++++++++++++++ResetSystemEnv++++++++++++++"
    @rem set include=%old_sys_include%
    @rem set lib=%old_sys_lib%
    @rem set path=%old_sys_path%
    call :color_text 9f "--------------ResetSystemEnv--------------"
    endlocal
goto :eof

:ShowVS2022InfoOnWin10
    setlocal EnableDelayedExpansion
    call :color_text 2f "+++++++++++++++++++ShowVS2022InfoOnWin10+++++++++++++++++++++++"
    @rem HKCU\SOFTWARE  or  HKCU\SOFTWARE\Wow6432Node
    @rem see winsdk.bat -> GetWin10SdkDir -> GetWin10SdkDirHelper -> reg query "%1\Microsoft\Microsoft SDKs\Windows\v10.0" /v "InstallationFolder"
    @rem see winsdk.bat -> GetUniversalCRTSdkDir -> GetUniversalCRTSdkDirHelper -> reg query "%1\Microsoft\Windows Kits\Installed Roots" /v "KitsRoot10"

    reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Microsoft SDKs\Windows\v10.0" /v "InstallationFolder"
    @rem reg delete "HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Microsoft SDKs\Windows\v10.0" /v InstallationFolder
    @rem reg add    "HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Microsoft SDKs\Windows\v10.0" /v InstallationFolder /f /t REG_SZ /d "D:\Program Files (x86)\Windows Kits\10\"

    reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Windows Kits\Installed Roots" /v "KitsRoot10"
    @rem reg delete "HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Windows Kits\Installed Roots" /v KitsRoot10 
    @rem reg add    "HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\Windows Kits\Installed Roots" /v KitsRoot10         /f /t REG_SZ /d "D:\Program Files (x86)\Windows Kits\10\"
    call :color_text 2f "--------------------ShowVS2022InfoOnWin10-----------------------"
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


:get_dir_by_tar
    setlocal ENABLEDELAYEDEXPANSION
    set tar_file="%~1"
    call :color_text 2f "++++++++++++++get_dir_by_tar++++++++++++++"
    @rem for /f "tokens=8 delims= " %%i in ('tar -tf %tar_file%') do ( echo %%~i )
    set FileDir=
    set file_name=
    echo "    tar -tf %tar_file% | grep "/$" | gawk -F"/" "{ print $1 }" | sed -n "1p"    "
    FOR /F "usebackq" %%i IN (` tar -tf %tar_file% ^| grep "/$" ^| gawk -F"/" "{print $1}" ^| sed -n "1p" `) DO (set FileDir=%%i)
    @rem echo tar_file:%tar_file% FileDir:!FileDir!
    call :is_contain %tar_file% %FileDir% file_name
    if "%file_name%" == "false" (
        call :color_text 4f "-------------get_dir_by_tar--------------"
        echo tar_file:%tar_file% FileDir:%FileDir%
    )
    endlocal & set %~2=%FileDir%
goto :eof

:get_dir_by_zip
    setlocal ENABLEDELAYEDEXPANSION
    set zip_file="%~1"
    call :color_text 2f "++++++++++++++get_dir_by_zip++++++++++++++"
    @rem for /f "tokens=8 delims= " %%i in ('unzip -v %zip_file%') do ( echo %%~i )
    set FileDir=
    set file_name=
    @rem unzip -v %zip_file% | gawk -F" "  "{ print $8 } " | gawk  -F"/" "{ print $1 }" | sed -n "5p"
    echo zip_file:%zip_file%
    FOR /F "usebackq" %%i IN (` unzip -v %zip_file% ^| gawk -F" "  "{ print $8 } " ^| gawk  -F"/" "{ print $1 }" ^| sed -n "5p" `) DO (set FileDir=%%i)
    FOR /F "usebackq" %%i IN (` unzip -v %zip_file% ^| gawk -F" "  "{print $8}" ^| grep -v "/[^$]" ^| gawk  -F"/" "{ print $1 }" ^| sed -n "5p" `) DO (set FileDir=%%i)
    @rem echo zip_file:%zip_file% FileDir:!FileDir!
    call :is_contain %zip_file% %FileDir% file_name
    if "%file_name%" == "false" (
        call :color_text 4f "-------------get_dir_by_zip--------------"
        echo zip_file:%zip_file% FileDir:%FileDir%
        pause
    )
    endlocal & set %~2=%FileDir%
goto :eof

:gen_env_by_file
    setlocal ENABLEDELAYEDEXPANSION
    set zip_file="%~1"
    set HomeDir=%~2
    set FileDir=

    call :get_pre_sub_str !zip_file! . file_name
    call :get_last_char_pos !zip_file! . ext_name_pos
    echo file_name:!file_name! ext_name_pos:!ext_name_pos!
    call :get_suf_sub_str !zip_file! . ext_name
    echo ext_name:!ext_name!
    if "%ext_name%" == "zip" (
        call :get_dir_by_zip %zip_file% FileDir
    ) else if "%ext_name%" == "gz" (
        call :get_dir_by_tar %zip_file% FileDir
    ) else if "%ext_name%" == "xz" (
        call :get_dir_by_tar %zip_file% FileDir
    ) else (
        echo "%ext_name%"
    )
    call :color_text 9f "++++++++++++++gen_env_by_file++++++++++++++"
    set DstDirWithHome=%HomeDir%\%FileDir%
    echo %0 %zip_file% %DstDirWithHome%
    endlocal & set %~3=%DstDirWithHome%
goto :eof

:gen_all_env
    setlocal ENABLEDELAYEDEXPANSION
    set tools_dir="%~1"
    set home_dir="%~2"
    set DstDirWithHome=
    call :color_text 2f "++++++++++++++gen_all_env++++++++++++++"
    if not exist %tools_dir% (
        echo Dir '%tools_dir%' doesn't exist!
        goto :eof
    )
    pushd %tools_dir%
        for /f %%i in ( 'dir /b *.tar.* *.zip' ) do (
            set tar_file=%%i
            call :gen_env_by_file !tar_file! !home_dir! DstDirWithHome
            set inc=!DstDirWithHome!/include;!inc!
            set lib=!DstDirWithHome!/lib;!lib!
            set bin=!DstDirWithHome!/bin;!bin!
            set CMAKE_INCLUDE_PATH=!DstDirWithHome!/include;!CMAKE_INCLUDE_PATH!
            set CMAKE_LIBRARY_PATH=!DstDirWithHome!/lib;!CMAKE_LIBRARY_PATH!
            set CMAKE_MODULE_PATH=!DstDirWithHome!/cmake;!CMAKE_MODULE_PATH!
        )
    popd
    echo inc:%inc%
    echo lib:%lib%
    echo bin:%bin%
    call :color_text 9f "----------------gen_all_env--------------------"
    endlocal & set %~3=%inc% & set %~4=%lib% & set %~5=%bin% & set %~6=%CMAKE_INCLUDE_PATH% & set %~7=%CMAKE_LIBRARY_PATH% & set %~8=%CMAKE_MODULE_PATH%
goto :eof

:show_all_env
    setlocal ENABLEDELAYEDEXPANSION
    call :color_text 2f "++++++++++++++show_all_env++++++++++++++"
    echo all_inc:%all_inc%
    echo all_lib:%all_lib%
    echo all_bin:%all_bin%
    echo CMAKE_INCLUDE_PATH:%CMAKE_INCLUDE_PATH%
    echo CMAKE_LIBRARY_PATH:%CMAKE_LIBRARY_PATH%
    echo CMAKE_MODULE_PATH:%CMAKE_MODULE_PATH%
    endlocal
goto :eof

:get_str_len
    setlocal ENABLEDELAYEDEXPANSION
    set mystr=%~1
    set mystrlen="%~2"
    set count=0
    call :color_text 2f "++++++++++++++get_str_len++++++++++++++"
    :intercept_str_len
    set /a count+=1
    for /f %%i in ("%count%") do (
        if not "!mystr:~%%i,1!"=="" (
            goto :intercept_str_len
        )
    )
    echo %0 %mystr% %count%
    endlocal & set %~2=%count%
goto :eof

:get_first_char_pos
    setlocal ENABLEDELAYEDEXPANSION
    set mystr=%~1
    set char_sym=%~2
    set char_pos="%~3"
    call :get_str_len %mystr% mystrlen
    set count=0
    call :color_text 2f "++++++++++++++get_first_char_pos++++++++++++++"
    :intercept_first_char_pos
    for /f %%i in ("%count%") do (
        set /a count+=1
        if not "!mystr:~%%i,1!"=="!char_sym!" (
            goto :intercept_first_char_pos
        )
    )
    echo %0 %mystr% %char_sym% %count%
    endlocal & set %~3=%count%
goto :eof

:get_last_char_pos
    setlocal ENABLEDELAYEDEXPANSION
    set mystr=%~1
    set char_sym=%~2
    set char_pos="%~3"
    call :get_str_len %mystr% mystrlen
    set count=%mystrlen%
    call :color_text 2f "++++++++++++++get_last_char_pos++++++++++++++"
    @rem set /a count-=1
    :intercept_last_char_pos
    for /f %%i in ("%count%") do (
        if not "!mystr:~%%i,1!"=="!char_sym!" (
            set /a count-=1
            goto :intercept_last_char_pos
        )
    )
    echo %0 %mystr% %char_sym% %count%
    endlocal & set %~3=%count%
goto :eof

:get_pre_sub_str
    setlocal ENABLEDELAYEDEXPANSION
    set mystr=%~1
    set char_sym=%~2
    set mysubstr="%~3"
    call :get_str_len %mystr% mystrlen
    set count=0
    call :color_text 2f "++++++++++++++get_pre_sub_str++++++++++++++"
    set substr=
    :intercept_pre_sub_str
    for /f %%i in ("%count%") do (
        set /a count+=1
        if not "!mystr:~%%i,1!"=="!char_sym!" (
            set /a mysubstr_len=%%i
            set substr=!mystr:~0,%%i!
            if "%count%" == "%mystrlen%" (
                goto :pre_sub_str_break
            )
            goto :intercept_pre_sub_str
        ) else (
            set /a mysubstr_len=%%i
            set substr=!mystr:~0,%%i!
            goto :pre_sub_str_break
        )
    )
    :pre_sub_str_break
    echo %0 %mystr% %char_sym% %count% %mysubstr_len%
    endlocal & set %~3=%substr%
goto :eof

:get_suf_sub_str
    setlocal ENABLEDELAYEDEXPANSION
    set mystr=%~1
    set char_sym=%~2
    set mysubstr="%~3"
    call :get_str_len %mystr% mystrlen
    set count=%mystrlen%
    call :color_text 2f "++++++++++++++get_suf_sub_str++++++++++++++"
    set substr=
    :intercept_suf_sub_str
    for /f %%i in ("%count%") do (
        if not "!mystr:~%%i,1!"=="!char_sym!" (
            set /a mysubstr_len=!mystrlen! - %%i
            set substr=!mystr:~%%i!
            set /a count-=1
            goto :intercept_suf_sub_str
        )
    )
    echo %0 %mystr% %char_sym% %count% %mysubstr_len%
    endlocal & set %~3=%substr%
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