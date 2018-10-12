 #!/bin/bash
echo "THIS IS an AppImage creator"
cd ..
BASELONGNAME=$(pwd)
echo 'THIS IS **base long name'
echo "$BASELONGNAME"
DIR="$BASELONGNAME/kafkacow.AppDir/usr"
if [ -d $BASELONGNAME/kafkacow.AppDir ];
then
    printf '%s\n' "Overwriting directory($BASELONGNAME/kafkacow.AppDir)"
    rm -rf $BASELONGNAME/kafkacow.AppDir
fi
mkdir -p "$DIR";
cp -r "$BASELONGNAME/build/." "$DIR"
cd kafkacow.AppDir
curl -L https://github.com/probonopd/AppImageKit/releases/download/5/AppRun --output AppRun
chmod a+x AppRun
cd ..
curl -L https://github.com/probonopd/AppImageKit/releases/download/5/AppImageAssistant --output AppImageAssistant
chmod a+x AppImageAssistant

mv $BASELONGNAME/build/kafkacow.desktop $BASELONGNAME/kafkacow.AppDir
mv $BASELONGNAME/build/icon.png $BASELONGNAME/kafkacow.AppDir

if [ -f "$BASELONGNAME/kafkacow-image.AppImage" ];
then
    printf '%s\n' "Overwriting image..."
    rm -rf "$BASELONGNAME/kafkacow-image.AppImage"
fi


echo '.................current path:'
echo "$PWD"
ls
cd kafkacow.AppDir
echo '.................current path:'
echo "$PWD"
ls
cd usr
echo '.................current path:'
echo "$PWD"
ls
cd ..
cd ..

./AppImageAssistant kafkacow.AppDir kafkacow.AppImage
