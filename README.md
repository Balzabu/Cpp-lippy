# C++lippy
![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Windows 11](https://img.shields.io/badge/Windows%2011-%230079d5.svg?style=for-the-badge&logo=Windows%2011&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)

C++lippy is a Cross-Platform cryptocurrency clipper inspired by a old project I made years ago: [Clippy](https://github.com/balzabu/Clippy)!
<br/>
It currently supports three popular cryptocurrencies(BTC,ETH and XRP), but others can be easily added.
<br/>

## Features
- Automatically monitors clipboard content for cryptocurrency addresses.
- Modifies clipboard content to replace cryptocurrency addresses with default hard-coded values.
- Supports Bitcoin (BTC), Ethereum (ETH), and Ripple (XRP).
- Cross-platform, supporting Windows and Linux.

## FUD
The output binaries, as the time of publishing, are totally FUD.
They only get detected by a not-so-famous AV called Bkav Pro.

## How does it work?

### Windows

If not already installed, the application will install itself on Windows, ensuring it runs silently in the background.
<br/>
During the installation the app will clone itself to `%APPDATA%` and create a new registry key in `HKEY_CURRENT_USER\\Software\\`.
<br/>
The `registry key` is used to execute the new binary at startup and detect if the installation has already been done or not. 

### Linux

If not already installed, the application will install itself on Linux.
It requires root (superuser) permissions for installation.
</br>
During the installation the app wil clone itself to `/usr/local/bin` and creates a new `.desktop` file inside `$HOME/.config/autoplay/`.
It automatically tries to extract the $HOME of the user launching the script, avoiding trying to install it into `/root` due to the `sudo` permissions being required.
<br/>
The `.desktop` file is used to execute the new binary at startup and detect if the installation has already been done or not. 

## Build

Clone the repository and edit the code to include your addresses and names for the registry keys, desktop files, etc.
Then compile the code using the provided build system.
</br>
Follow the platform-specific to build correctly:


**REMEMBER TO SPECIFY STD >= C++17**

### Windows (to build .exe)

```bash
# Compile and run the application on Windows
g++ -std=c++17 main.cpp -o clippy.exe -lole32 -lshell32 -luuid -mwindows
./clippy.exe
```

### Linux

```bash
# Compile and run the application on Linux
g++ -std=c++17 main.cpp -o clippy
sudo ./clippy
```

## Disclaimer
The code published in this GitHub Repository must not be used with malicious intentions.</br>Proof-of-Concepts and tools are shared for educational purposes only;
**any malicious use will not hold the author responsible.**
