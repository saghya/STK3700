#!/bin/sh
service udev restart
udevadm control --reload
/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe -command usbipd wsl attach -b 2-2