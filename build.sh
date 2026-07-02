#!/bin/bash

VERSION=1.7

PROJ_NAME=foxmake

DEB_CONTROL_PACKAGE_NAME=$PROJ_NAME
DEB_CONTROL_INSTALLED_SIZE=2796
DEB_CONTROL_MAINTAINER="Italo Herbert Siqueira Gabriel (italoherbert@outlook.com)"
DEB_CONTROL_DESCRIPTION="Trata-se de um software automatizador de tarefas de build em projetos C/C++"

APP_FILE_NAME=$PROJ_NAME

BUILD_FOLDER=build
APP_FOLDER=$PROJ_NAME-$VERSION-linux-x64
WINDOWS_APP_FOLDER=$PROJ_NAME-$VERSION-windows-x64

APP_DIR=$BUILD_FOLDER/$APP_FOLDER
WINDOWS_APP_DIR=$BUILD_FOLDER/$WINDOWS_APP_FOLDER

APP_FILE=$APP_DIR/$APP_FILE_NAME

DEB_APP_DIR=$APP_DIR-deb

DEB_CONTROL_FILE=$DEB_APP_DIR/DEBIAN/control

# LIMPANDO...

if [ -d "$APP_DIR" ]; then
    rm -rf $APP_DIR
    echo "Removido: $APP_DIR"
fi

if [ -d "$DEB_APP_DIR" ]; then
    rm -rf $DEB_APP_DIR
    echo "Removido: $DEB_APP_DIR"
fi

if [ -f "$APP_DIR.tar.gz" ]; then
    rm "$APP_DIR.tar.gz"
    echo "Removido: $APP_DIR.tar.gz"
fi

if [ -f "$WINDOWS_APP_DIR.zip" ]; then
    rm "$WINDOWS_APP_DIR.zip"
    echo "Removido: $WINDOWS_APP_DIR.zip"
fi

if [ -f "$APP_DIR.deb" ]; then
    rm "$APP_DIR.deb"
    echo "Removido: $APP_DIR.deb"
fi

mkdir -p $APP_DIR
mkdir -p $WINDOWS_APP_DIR
mkdir -p $DEB_APP_DIR

# COMPILANDO...

echo
echo Compilando...
./foxmake buildall --script=FoxMakefile -var target=release

echo
echo Compilando para windows...
./foxmake buildall --script=FoxMakefile-windows

# COPIANDO LIB E INCLUDE PASTAS PARA A PASTA DO APLICATIVO EM BUILD

cp -r lib/linux $APP_DIR/lib
cp -r lib/windows $WINDOWS_APP_DIR/lib

cp -r include $APP_DIR
cp -r include $WINDOWS_APP_DIR

# EMPACOTANDO VERSÃO x86_64 EM .TAR.GZ

cd $BUILD_FOLDER

echo
echo "Empacotando... $APP_DIR.tar.gz"

tar -czf "$APP_FOLDER.tar.gz" $APP_FOLDER
echo "Finalizado." 

cd - &> /dev/null

echo

# EMPACOTANDO VERSÃO WINDOWS EM .ZIP

cd $BUILD_FOLDER

echo
echo "Empacotando... $WINDOWS_APP_DIR.zip"

zip -r -q "$WINDOWS_APP_FOLDER.zip" $WINDOWS_APP_FOLDER
echo "Finalizado." 

cd - &> /dev/null

echo


# GERANDO ARQUIVO .DEB

# Copiando executável para pasta /usr/bin do .DEB

mkdir -p $DEB_APP_DIR/usr/bin

cp $APP_FILE $DEB_APP_DIR/usr/bin/$APP_FILE_NAME

# Copiando a static library do xutest para pasta /usr/lib do .DEB

mkdir -p $DEB_APP_DIR/usr/lib/x86_64-linux-gnu
cp $APP_DIR/lib/libxutest.a $DEB_APP_DIR/usr/lib/x86_64-linux-gnu/libxutest.a

# Copiando os headers para pasta /usr/include do .DEB

mkdir -p $DEB_APP_DIR/usr/include
cp -r $APP_DIR/include/* $DEB_APP_DIR/usr/include

# Gerando arquivo control do .DEB

mkdir -p $DEB_APP_DIR/DEBIAN

echo "Package: $DEB_CONTROL_PACKAGE_NAME" >> $DEB_CONTROL_FILE
echo "Version: $VERSION" >> $DEB_CONTROL_FILE
echo "Section: contrib" >> $DEB_CONTROL_FILE
echo "Architecture: amd64" >> $DEB_CONTROL_FILE
echo "Installed-Size: $DEB_CONTROL_INSTALLED_SIZE" >> $DEB_CONTROL_FILE
echo "Maintainer: $DEB_CONTROL_MAINTAINER" >> $DEB_CONTROL_FILE
echo "Description: $DEB_CONTROL_DESCRIPTION" >> $DEB_CONTROL_FILE

# Empacotando em .DEB

echo
echo "Empacotando... $APP_DIR.deb"
dpkg-deb --root-owner-group -b "$DEB_APP_DIR/" "$APP_DIR.deb"
echo "Finalizado."

# BUILD CONCLUIDO

echo
echo "Build concluido!"