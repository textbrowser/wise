#!/usr/bin/env bash

# Alexis Megas.

echo "The command sed may fail on MacOS."

VERSION=$1

if [ -z "$VERSION" ]
then
    echo "Please specify the version: $0 <VERSION>."
    exit 1
fi

if [ -r Distributions/*/control ]
then
    for file in Distributions/*/control
    do
	sed -i "s/Version: .*/Version: $VERSION/" $file
    done
fi

if [ -r Distributions/build* ]
then
    for file in Distributions/build*
    do
	sed -i \
	"s/Wise-[[:digit:]]\+\.[[:digit:]]\+\.[[:digit:]]\+/Wise-$VERSION/" \
	$file
    done
fi

FILE="Source/wise.cc"

sed -i \
    's/\(WISE_VERSION_STRING "\)[0-9]\+\(\.[0-9]\+\)*"/\1'"$VERSION"'"/' \
    $FILE

FILE="Android/AndroidManifest.xml"

sed -i \
    's/\(android:versionCode="\)[0-9]\+\([0-9]\+\)*"/\1'"${VERSION//./}"'"/' \
    $FILE
sed -i \
    's/\(android:versionName="\)[0-9]\+\(\.[0-9]\+\)*"/\1'"$VERSION"'"/' \
    $FILE
echo "Please modify ReleaseNotes.html."
