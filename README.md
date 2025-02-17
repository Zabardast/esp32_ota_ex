# esp32_ota_ex
test esp32_ota for an other project ;)

### plan v0.1:

I have 2 esp32's (red and black)

red: will be my local AP because i need an AP that is in reach of their tiny antennas.

black: test subject.

#### the test subject:

the test subject will be a tcp client to my pc's tcp server and send `hello world!` initialy, but after being updated the message will be `OTA Baby!!!`.


#### Steps to get OTA:

- MenuConfig --> Partition Table --> Partition Table --> Factory App, two OTA definitions


#### Run http upload server

Run `python -m http.server 8000` in the directory where you have youre binarie.

```bash
cd .pio/build/esp32-c6-devkitm-1

python -m http.server 8000

```
