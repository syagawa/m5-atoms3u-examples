# USB key sender random

## settings examples


* Output random number 0-100
```json
{"color":"red", "randomMode":"", "randomMin":0, "randomMax":100, "seedPort":14}

```
* Output uuid and prefix and suffix
```json
{"color":"red", "keyboardLayout":"ja", "randomMode":"uuid", "randomMin":0, "randomMax":100, "prefix":"{\"uuid\":\"","suffix":"\"}", "seedPort":14}
```


---

This example is created with reference to https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/examples/USBMSC/USBMSC.ino .
