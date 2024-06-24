@echo OFF 
echo Starting Build...
call "%VCINSTALLDIR%\..\Common7\Tools\VSDevCmd.bat"
echo.
RD /S /Q build
mkdir build
mkdir build\x64
MSBuild "shroudtopia\shroudtopia.sln" /t:Build /p:Configuration=Release /p:OutDir="..\build\x64" /p:Platform=x64
MSBuild "winmm\winmm.sln" /t:Build /p:Configuration=Release /p:OutDir="..\build\x64" /p:Platform=x64
echo. 
echo All builds completed.
echo.
erase build\x64\*.pdb
erase build\x64\*.exp
erase build\x64\*.lib
echo. 
echo Cleaned up.