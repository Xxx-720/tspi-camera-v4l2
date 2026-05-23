#!/bin/sh

chmod +x /wxq/all/auto.sh

chmod +x /wxq/all/my_project
echo "Qt project is ok"

mkdir /usr/share/fonts
cp -r /wxq/all/truetype /usr/share/fonts/
echo "fonts is ok"

cp /wxq/all/weston.ini /etc/xdg/weston/weston.ini
echo "weston.ini replaced"

cp /wxq/all/rcS /etc/init.d/
echo "rcS replaced"

echo "auto config done, please reboot!"


