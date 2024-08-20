# This example does not work, or only partially works.

# USB key sender random

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
