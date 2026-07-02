
@echo off

set VERSION=1.7
set SO=windows
set ARCH=x64

set PROJ_NAME=foxmake

set APP_FILE_NAME=%PROJ_NAME%

set BUILD_FOLDER=build
set APP_FOLDER=%PROJ_NAME%-%VERSION%-%SO%-%ARCH%

set APP_DIR=%BUILD_FOLDER%\%APP_FOLDER%

set XUTEST_LIB_FILE=lib\windows\libxutest.a

rem LIMPANDO...

if exist %APP_DIR% (
    rmdir /s /q %APP_DIR%
    echo Removido: %APP_DIR%
)

if exist %APP_DIR%.zip (
    del %APP_DIR%.zip
    echo Removido: %APP_DIR%.zip
)

mkdir %APP_DIR%

rem COMPILANDO...

echo.
echo Compilando...
echo.

.\foxmake.exe buildall --script=FoxMakefile -var target=release

rem COPIANDO HEADERS PARA PASTA INCLUDE

echo.
echo Copiando headers para pasta include
mkdir %APP_DIR%\include
xcopy include\* %APP_DIR%\include /E /I /Y

rem COPIANDO STATIC LIBRARY DO xutest PARA PASTA LIB

echo.
echo Copiando libxutest.a para pasta lib
mkdir %APP_DIR%\lib
copy %XUTEST_LIB_FILE% %APP_DIR%\lib

rem EMPACOTANDO EM .ZIP

set CURR_DIR=%cd%

cd %BUILD_FOLDER%

echo.
echo Empacotando... %APP_DIR%.zip

tar acvf %APP_FOLDER%.zip %APP_FOLDER%
echo Finalizado.

cd %CURR_DIR%

rem BUILD CONCLUIDO

echo.
echo Build concluido!