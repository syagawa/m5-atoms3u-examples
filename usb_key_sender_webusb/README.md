# USB key sender

## settings examples


* Select, Copy and Paste
```json
{"color": "red", "keys": ["press:KEY_LEFT_CTRL,a", "press:KEY_LEFT_CTRL,c", "press:KEY_LEFT_CTRL,v"], "waitSecnds":3}
```

* Login and Open Chrome in Windows
```json
{"color": "red", "keys": ["press:KEY_RETURN", "password", "\n", "open:chrome"], "waitSecnds":3}
```

* Minimize all windws in Windows
```json
{"color": "red", "keys": ["press:KEY_LEFT_GUI,d"], "waitSecnds":3}
```



---

This example is created with reference to https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/examples/USBMSC/USBMSC.ino .


---


sudo cp 99-tinyusb.rules /etc/udev/rules.d/

##SUBSYSTEMS=="usb", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="1001", GROUP="plugdev", MODE="0666"
#SUBSYSTEMS=="tty", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="1001", GROUP="plugdev", MODE="0666"
SUBSYSTEM=="tty", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="1001", GROUP="plugdev", MODE="0666", TAG+="uaccess"

sudo udevadm control --reload-rules
sudo udevadm trigger

sudo usermod -aG plugdev $USER

ls -la /dev/ttyACM0

service udev start

sudo snap set system experimental.hotplug=true

https://qiita.com/NWLab/items/04c02cb4dec6291e08fe
https://qiita.com/frameair/items/596724fc2f3438ea7925

---

start chromium-browser as root

```

sudo cp ~/.Xauthority /root/
sudo su -
mkdir /run/user/0
xhost +SI:localuser:root
sudo -i
export DISPLAY=:0
chromium-browser --no-sandbox


```


If M5AtomS3U is not properly recognized as WebUSB when connected to a PC, remove M5AtomS3U from Device Manager and also remove M5AtomS3U from Chrome's WebUSB settings.
