#!/bin/sh
echo "==========install boot up tools============="
cp bootup /etc/init.d/
cp bootup.sh /home/tony/
cp runtest /home/tony/

chmod 755 /etc/init.d/bootup
chmod 755 /home/tony/bootup.sh
chmod 755 /home/tony/runtest
update-rc.d bootup defaults

