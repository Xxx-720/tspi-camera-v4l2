#!/bin/sh

chmod +x /wxq/my__config/auto.sh

mv /wxq/my__config/my_project /my_project
chmod +x /my_project
echo "Qt project is ok"

mkdir /usr/share/fonts
cp -r /wxq/my__config/truetype /usr/share/fonts/
echo "fonts is ok"

mv /wxq/my__config/librockchip_mpp.so.0 /usr/lib/librockchip_mpp.so.0
echo "lib is ok"

cp /wxq/my__config/weston.ini /etc/xdg/weston/weston.ini
echo "weston.ini replaced"

cp /wxq/my__config/rcS /etc/init.d/
echo "rcS replaced"

echo "auto config done, please reboot!"


