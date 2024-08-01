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

SUBSYSTEMS=="usb", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="00??", GROUP="plugdev", MODE="0666"

sudo udevadm control --reload-rules
sudo udevadm trigger

service udev start

https://qiita.com/NWLab/items/04c02cb4dec6291e08fe