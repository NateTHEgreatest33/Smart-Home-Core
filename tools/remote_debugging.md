# Remote Debugging Setup for Raspberry Pi Pico

This document outlines the steps to set up a remote debugging environment for the Raspberry Pi Pico using a Raspberry Pi 3B+ as a debug host and a Mac with VS Code as the client.

There are two primary ways to connect your Pico to the Raspberry Pi for debugging:
1.  **Using the official Raspberry Pi Debug Probe (Recommended)**
2.  **Using the Raspberry Pi's GPIO pins directly**

This guide will cover both.

---

## Part 1: Initial Setup on the Raspberry Pi 3B+

These steps are required for both methods.

### 1.1. Build and Install the Correct OpenOCD

The key to success is to build and install the Raspberry Pi-specific version of OpenOCD, which contains the necessary patches for the Pico.

**1. Install Build Dependencies**
```bash
sudo apt-get update
sudo apt-get install -y automake autoconf build-essential texinfo libtool libftdi-dev libusb-1.0-0-dev
```

**2. Clone OpenOCD Source**
```bash
# It is recommended to clone this into your home directory (~)
git clone https://github.com/raspberrypi/openocd.git --branch rp2040-v0.12.0 --depth=1
```

**3. Build and Install**
```bash
cd openocd # Or wherever you cloned the repository
./bootstrap
./configure --enable-sysfsgpio --enable-bcm2835gpio
make
sudo make install
```

**4. Remove Old OpenOCD (Crucial Step)**
If you had previously installed OpenOCD via `apt-get`, you must remove it.
```bash
sudo apt-get remove openocd
```
To verify, run `which openocd`. It should output `/usr/local/bin/openocd`.

### 1.2. Find the Raspberry Pi's IP Address

You'll need this to connect from your Mac.
```bash
hostname -I
```
Note the IP address (e.g., `192.168.X.X`).

---

## Part 2: Starting the OpenOCD Server

Choose the section that matches your hardware setup.

### Method A: Using the Official Raspberry Pi Debug Probe

This is the most reliable method.

**1. Set USB Permissions (`udev` rule)**
By default, the system will prevent OpenOCD from accessing the USB probe. You must create a `udev` rule to grant permission.

Create a new rule file:
```bash
sudo nano /etc/udev/rules.d/99-pico-debug.rules
```

Add the following content to the file:
```
# Raspberry Pi Pico/Debug Probe
SUBSYSTEM=="usb", ATTR{idVendor}=="2e8a", ATTR{idProduct}=="000c", MODE="0666"
```

Save the file, then apply the new rule:
```bash
sudo udevadm control --reload-rules && sudo udevadm trigger
```
Finally, unplug and replug your Debug Probe into the Raspberry Pi's USB port.

**2. Start the Server**
Connect the Debug Probe to your Pico. The wiring is typically a direct 3-pin JST-SH connection. Then run OpenOCD:
```bash
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg --command "bindto 0.0.0.0"
```

### Method B: Using the Raspberry Pi's GPIO Pins

This method uses the Pi's own GPIO pins as a debugger.

**1. Connect the Wiring**
Carefully connect the GPIO pins on your Raspberry Pi to the SWD pins on your Pico.

| Raspberry Pi 3B+ Pin | Pico Pin         | Description    |
| -------------------- | ---------------- | -------------- |
| GPIO 25 (Pin 22)     | SWCLK (Pin 24)   | SWD Clock      |
| GPIO 24 (Pin 18)     | SWDIO (Pin 25)   | SWD Data I/O   |
| GND (Any Ground Pin) | GND (Any Ground) | Common Ground  |

**2. Start the Server**
```bash
openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg
```

---

## Part 3: On Your Mac (Client Setup)

### 1. Set the Raspberry Pi's IP Address in VS Code

Open the `.vscode/launch.json` file in your project. Find the line that says:

```json
"gdbTarget": "RPI_IP_ADDRESS:3333",
```

Replace `RPI_IP_ADDRESS` with the IP address of your Raspberry Pi.

### 2. Start Debugging

1.  Make sure the OpenOCD server is running on your Raspberry Pi.
2.  In VS Code on your Mac, open the "Run and Debug" view (Shift+Command+D).
3.  Select the "Pico Debug" configuration from the dropdown menu.
4.  Click the green "Start Debugging" button (F5).

VS Code should now connect to the OpenOCD server on your Raspberry Pi, allowing you to debug your Pico application remotely.
