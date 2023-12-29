# UtilityBubble
Utility Bubble is a small window application analogous to “Assistive Ball” in android. It is used for accessing frequently used features of the OS and can be also used for opening recent or most used apps.

## Installation

- Install Qt5.15.6
```sudo apt-get install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools```

- Build Project
```qmake UtilityBubble.pro```
```make```

- On Ubuntu OS
Run as non-root user ```./UtilityBubble```
Run as root user ```sudo ./UtilityBubble``` (MUST FOR TOGGLING CAMERA)
