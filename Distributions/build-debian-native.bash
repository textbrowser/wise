#!/usr/bin/env bash
# Alexis Megas.

if [ ! -x /usr/bin/dpkg-deb ]
then
    echo "Please install dpkg-deb."
    exit 1
fi

if [ ! -x /usr/bin/fakeroot ]
then
    echo "Please install fakeroot."
    exit 1
fi

if [ ! -e wise.pro ]
then
    echo "Please issue $0 from the primary directory."
    exit 1
fi

make distclean 2>/dev/null
mkdir -p ./opt/wise

if [ -x /usr/bin/qmake6 ]
then
    qmake6 && make -j $(nproc)
else
    qmake && make -j $(nproc)
fi

cp -p ./Icons/wise.png ./opt/wise/.
cp -p ./Wise ./opt/wise/.
cp -p ./wise.sh ./opt/wise/.
mkdir -p wise-debian.d/opt
mkdir -p wise-debian.d/usr/share/applications
cp -p Distributions/wise.desktop wise-debian.d/usr/share/applications/.

architecture="$(dpkg --print-architecture)"

if [ $architecture = "arm64" ]
then
    cp -pr Distributions/PiOS64 wise-debian.d/DEBIAN
elif [ $architecture = "armhf" ]
then
    cp -pr Distributions/PiOS32 wise-debian.d/DEBIAN
else
    cp -pr Distributions/KUBUNTU-NATIVE wise-debian.d/DEBIAN
fi

cp -r ./opt/wise wise-debian.d/opt/.

if [ $architecture = "arm64" ]
then
    fakeroot dpkg-deb --build wise-debian.d Wise-2026.06.13_PiOS_arm64.deb
elif [ $architecture = "armhf" ]
then
    fakeroot dpkg-deb --build wise-debian.d Wise-2026.06.13_PiOS_arm32.deb
else
    fakeroot dpkg-deb --build wise-debian.d Wise-2026.06.13_Kubuntu_amd64.deb
fi

make distclean
rm -fr ./opt
rm -fr wise-debian.d
