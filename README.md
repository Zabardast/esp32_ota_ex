# esp32_ota_ex
test esp32_ota for an other project ;)

In this test i used 2 esp32-c6 but you can do it with only one and use a normal router as AP.

for this project i ended up using the ESP-IDF plugin in vscode instead on platformio because i was having issues with the partitions of my esp32.

/!\ keep in mind there are some magic values in this example you will need to change for your environment.

### plan v0.1:

I have 2 esp32's (red and black)

red: will be my local AP because i need an AP that is in reach of their tiny antennas.

black: test subject. tcp client. ota client? (can I say it's a client?)

#### the test subject:

the test subject will be a tcp client to my pc's tcp server and send `hello world!` initialy, but after being updated the message will be `OTA Baby!!!`.


### config:


- ```. $HOME/.espidf/v5.4/esp-idf/export.sh``` mind that this is for my system so maybe change it to work with yours.

- idf.py menuconfig --> Partition Table --> Partition Table --> Factory App, two OTA definitions
- idf.py menuconfig --> ESP HTTPS OTA --> Allow HTTP for OTA (...) == true
please consider using certificates with https if using in a at risk environment.

#### Run http upload server

Run `python3 -m http.server 8000` in the directory where you have youre binarie.

```bash
cd .pio/build/esp32-c6-devkitm-1

or

mkdir project_name/upload_build
cp project_name/build/project_name.bin project_name/upload_build/

cd project_name/upload_build

python -m http.server 8000

```
Make sure the bin in the upload_build is "OTA Babay!!!" so you can actually see the difference in the esp-idf monitor.
