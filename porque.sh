#!/usr/bin/env sh

# Alexis Megas.

export AA_ENABLEHIGHDPISCALING=1
export AA_USEHIGHDPIPIXMAPS=1
export QT_AUTO_SCREEN_SCALE_FACTOR=1
export QT_QPA_PLATFORMTHEME=qt6ct
export QT_X11_NO_MITSHM=1

if [ -r ./Porque ] && [ -x ./Porque ]
then
    echo "Launching a local Porque."
    ./Porque "$@"
    exit $?
elif [ -r /opt/porque/Porque ] && [ -x /opt/porque/Porque ]
then
    echo "Launching an official Porque."
    /opt/porque/Porque "$@"
    exit $?
elif [ -r /usr/local/porque/Porque ] && [ -x /usr/local/porque/Porque ]
then
    echo "Launching an official Porque."
    /usr/local/porque/Porque "$@"
    exit $?
else
    echo "Cannot locate Porque. Why?"
    exit 1
fi
