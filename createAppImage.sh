 #!/bin/bash
echo "THIS IS an AppImage creator"
cd ..
DIR="$1/kafkacow.AppDir/usr"
if [ -d $DIR ]; 
then
    printf '%s\n' "Overwriting directory($DIR)"
    rm -rf $DIR
fi
mkdir -p "$DIR";
cp -r "$1/build" "$DIR"
cd kafkacow.AppDir
curl -L https://github.com/probonopd/AppImageKit/releases/download/5/AppRun --output AppRun
chmod a+x AppRun
cd ..
curl -L https://github.com/probonopd/AppImageKit/releases/download/5/AppImageAssistant --output AppImageAssistant
chmod a+x AppImageAssistant

mv $DIR/kafkacow.desktop $1/kafkacow.AppDir
mv $DIR/icon.png $1/kafkacow.AppDir

if [ -f "$1/kafkacow-image.AppImage" ]; 
then
    printf '%s\n' "Overwriting image..."
    rm -rf "$1/kafkacow-image.AppImage"
fi
./AppImageAssistant kafkacow.AppDir ./kafkacow-image.AppImage
