#!/bin/sh

chmod +x /wxq/all/auto.sh

chmod +x /wxq/all/my_project
echo "Qt project is ok"

mkdir /usr/share/fonts
cp -r /wxq/all/truetype /usr/share/fonts/
echo "fonts is ok"

mv /wxq/all/librockchip_mpp.so.0 /usr/lib/librockchip_mpp.so.0
echo "lib is ok"

cp /wxq/all/weston.ini /etc/xdg/weston/weston.ini
echo "weston.ini replaced"

cp /wxq/all/rcS /etc/init.d/
echo "rcS replaced"

echo "auto config done, please reboot!"


