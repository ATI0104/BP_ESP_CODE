# Photovoltaic system monitoring and control

Author: Attila Kovacs

## Description

This directory holds the source files for the photovoltaic system monitoring and control project.
The ESP directory contains the source files for the ESP32 microcontroller, while the web directory contains the source files for the web server.

## Deployment

### ESP32

Requirements:

- PlatformIO
- ESP32 microcontroller with various components connected based on the schematic

To deploy the ESP32 firmware, open the ESP directory as a PlatformIO project and compile and upload it to the ESP32.

### WEB

Requirements:
- Rapsberry Pi
- ChirpStack Gateway OS

Please read the README.md file in the web directory for more information.

## Installing and configuring Chirpstack Gateway OS

Some configuration is needed for the ChirpStack Gateway OS to work with the ESP32 microcontroller. I couldn't find a way to automate this process, so this needs to be done manually.

- Download the ChirpStack Gateway OS Full image from the [ChirpStack website](https://www.chirpstack.io/docs/chirpstack-gateway-os/install/raspberry-pi.html).
- Using Balena etcher or it's alternative, flash the image to an SD card.
- Insert the SD card into the Raspberry Pi and boot it up.
- Obtain the IP address of the Raspberry Pi.
- Open the web interface by navigating to the IP address of the Raspberry Pi.
- By default the root account is 'root' with an empty password it's recommended to change this.
- In the left sidebar select ChirpStack → Concertatord.

- For the Waveshare SX1303 select SX1302/SX1303:
  - Set the antenna gain to 5dbi
  - Set the shield model to Waveshare - SX1302 LoRaWAN Gateway HAT
  - Set the Channel-plan to your region
  - Enable GNSS
  - Press Save
- Inside the Global configuration:
  - Set the Enabled chipset to SX1302/SX1303
  - Press Save & Apply
- Reboot the Pi by going into System → Reboot
- After the Pi has rebooted open the same web interface again and in the bottom right corner you should see the Gateway ID, save this value somewhere.
- Click on the Chirpstack image, this should open up the ChirpStack web interface.
- In the left sidebar select Gateways and click on Add gateway.
  - Fill out the form. The Gateway ID should be the one you saved earlier. Don't worry about the location it will be updated later.
- In the left sidebar select Device Profiles
  - Give it a name for example ESP32
  - Set the MAC version to LoRaWAN 1.0.3
  - Set the Regional Parameters revision to RP002-1.0.3
  - Set the Expected uplink interval to 1440
- In the left sidebar select the Applications entry and click on Add application
  - Give it a name for example My house
  - Press submit
- Now we are going to replace the ChirpStack concentratord with our modified version:
  - On your machine, where you have the modified concetratord package run `python3 -m http.server`
  - ssh into your Raspberry Pi `ssh root@<IP>`
  - run the following commands:
    - `cd /tmp`
    - `wget http://<your machine IP>:8000/chirpstack-concentratord-sx1302`
    - `mv chirpstack-concentratord-sx1302 /usr/bin/chirpstack-concentratord-sx1302`
- reboot the Raspberry Pi
- After the Raspberry Pi has rebooted, open the ChirpStack web interface again and after a few minutes you should see that the gateway is in the right location, meaning that the GPS is now working.

### Patches

As mentioned in my thesis, I came across an old bug with the Waveshare SX1303 Gateway hat, the 2 patch files can be used to compile a modified version of ChirpStack Concetratord that fixes the GPS synchronization issue. The patch files are located in the patches directory.

The concetratord.patch needs to be applied to the ChirpStack Concentratord source code. [Can be found here](https://github.com/chirpstack/chirpstack-concentratord)

The sx1302_hal.patch can be optionally applied to the SX1302_hal source code. Currently, this patch is not needed because I'm hosting the already patched version of the source code on my GitHub [Orginal can be found here](https://github.com/brocaar/sx1302_hal) ([With the applied patch.](https://github.com/ATI0104/sx1302_hal)).

This fixes the GPS issues with the Waveshare SX1303 Gateway hat (maybe even more gateways).

I'm also including the patched version of ChirpStack Concentratord in the patches/patched_binaries directory.
