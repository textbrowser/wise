#!/usr/bin/env sh

# Alexis Megas.

export AA_ENABLEHIGHDPISCALING=1
export AA_USEHIGHDPIPIXMAPS=1
export QT_AUTO_SCREEN_SCALE_FACTOR=1
export QT_QPA_PLATFORMTHEME=qt6ct
export QT_X11_NO_MITSHM=1

if [ -f ./Wise ] && [ -r ./Wise ] && [ -x ./Wise ]
then
    echo "Launching a local Wise."
    ./Wise "$@"
    exit $?
fi

if [ -f /opt/wise/Wise ] && [ -r /opt/wise/Wise ] && [ -x /opt/wise/Wise ]
then
    echo "Launching an official Wise (/opt/wise)."

    export LD_LIBRARY_PATH=/opt/wise/Lib
    export QT_PLUGIN_PATH=/opt/wise/plugins

    /opt/wise/Wise "$@"
    exit $?
fi

if [ -f /usr/local/wise/Wise ] && \
   [ -r /usr/local/wise/Wise ] && \
   [ -x /usr/local/wise/Wise ]
then
    echo "Launching an official Wise (/usr/local/wise)."

    export LD_LIBRARY_PATH=/usr/local/wise/Lib
    export QT_PLUGIN_PATH=/usr/local/wise/plugins

    /usr/local/wise/Wise "$@"
    exit $?
fi

echo "Cannot locate Wise. Why?"
exit 1
