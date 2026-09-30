#!/usr/bin/env bash
# Alexis Megas.

if [ ! -e wise.macos.pro ]
then
    echo "Please issue $0 from the primary directory."
    exit 1
fi

if [ ! -z "${SSH_TTY}" ]
then
    echo "SSH session detected. " \
	 "MacOS codesign password prompt may be invisible."
fi

make distclean 1>/dev/null 2>/dev/null

declare -a qmakes=("$HOME/Qt/6.11.1/macos/bin/qmake"
		   "$HOME/Qt/6.8.3/macos/bin/qmake")
qmake=""

for i in "${qmakes[@]}"
do
    qmake="$(echo $i)"

    if [ -x "$qmake" ]
    then
	break
    fi
done

if [ -x "$qmake" ]
then
    echo "Found $qmake."
    $qmake -o Makefile wise.macos.pro 1>/dev/null 2>/dev/null
else
    echo "Cannot locate qmake. Please install the official Qt."
    exit 1
fi

echo "Making Wise."
make -j $(sysctl -n hw.ncpu) 1>/dev/null 2>/dev/null
make install 1>/dev/null 2>/dev/null
echo "Signing ./Wise.d/Wise.app."
codesign --deep --force -s "textbrowser" ./Wise.d/Wise.app \
	 1>/dev/null 2>/dev/null

if [ ! $? -eq 0 ]
then
    echo "Signing error. Bye!"
    exit 1
fi

echo "Building Wise.d.dmg."
make dmg 1>dev/null 2>/dev/null

if [ ! -r Wise.d.dmg ]
then
    echo "Wise.d.dmg is not a readable file."
    exit 1
fi

mv Wise.d.dmg Wise-2026.10.03_Universal.dmg
make distclean 1>/dev/null 2>/dev/null
rm -fr ./Wise.d
