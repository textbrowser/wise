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
mkdir -p ./opt/wise/Lib
cp -p ~/Qt/6.8.3/gcc_64/lib/libQt6Core.so.6 ./opt/wise/Lib/.
cp -p ~/Qt/6.8.3/gcc_64/lib/libQt6DBus.so.6 ./opt/wise/Lib/.
cp -p ~/Qt/6.8.3/gcc_64/lib/libQt6Gui.so.6 ./opt/wise/Lib/.
cp -p ~/Qt/6.8.3/gcc_64/lib/libQt6Network.so.6 ./opt/wise/Lib/.
cp -p ~/Qt/6.8.3/gcc_64/lib/libQt6Pdf.so.6 ./opt/wise/Lib/.
cp -p ~/Qt/6.8.3/gcc_64/lib/libQt6PdfWidgets.so.6 ./opt/wise/Lib/.
cp -p ~/Qt/6.8.3/gcc_64/lib/libQt6PrintSupport.so.6 ./opt/wise/Lib/.
cp -p ~/Qt/6.8.3/gcc_64/lib/libQt6Sql.so.6 ./opt/wise/Lib/.
cp -p ~/Qt/6.8.3/gcc_64/lib/libQt6Svg.so.6 ./opt/wise/Lib/.
cp -p ~/Qt/6.8.3/gcc_64/lib/libQt6Widgets.so.6 ./opt/wise/Lib/.
cp -p ~/Qt/6.8.3/gcc_64/lib/libQt6XcbQpa.so.6 ./opt/wise/Lib/.
cp -p ~/Qt/6.8.3/gcc_64/lib/libicudata.so.73 ./opt/wise/Lib/.
cp -p ~/Qt/6.8.3/gcc_64/lib/libicui18n.so.73 ./opt/wise/Lib/.
cp -p ~/Qt/6.8.3/gcc_64/lib/libicuuc.so.73 ./opt/wise/Lib/.
cp -pr ~/Qt/6.8.3/gcc_64/plugins ./opt/wise/.
chmod -x,g+w ./opt/wise/Lib/*
~/Qt/6.8.3/gcc_64/bin/qmake -o Makefile wise.pro && make -j $(nproc)
cp -p ./Icons/wise.png ./opt/wise/.
cp -p ./Wise ./opt/wise/.
cp -p ./wise.sh ./opt/wise/.
find ./opt/wise/plugins -name '*.so' -exec chmod -x {} \;
mkdir -p wise-debian.d/opt
mkdir -p wise-debian.d/usr/share/applications
cp -p Distributions/wise.desktop wise-debian.d/usr/share/applications/.
cp -pr Distributions/DEBIAN wise-debian.d/DEBIAN
cp -r ./opt/wise wise-debian.d/opt/.
fakeroot dpkg-deb --build wise-debian.d Wise-2026.01.31_amd64.deb
make distclean
rm -fr ./opt
rm -fr wise-debian.d
