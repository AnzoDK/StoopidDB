#!/bin/bash
PKGMAN=pacman

pacman -h >> /dev/null
if [ $? -ne 0 ]
then
	dpkg -h >> /dev/null
	if [ $? -ne 0 ]
	then
		echo "Could not determine packagemanager - Aborting..."
		exit 1
	fi
PKGMAN=dpkg
fi
if [ "$PKGMAN" == "pacman" ]
then
rm -rf ./stoopiddb-header-*
makepkg -f
sudo pacman -U ./stoopiddb-header-*
exit 0
else
#TODO make install process for .deb
echo "Not implemented"
exit 0
fi 
