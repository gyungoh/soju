# soju
Lightweight Korean IME for linux

### Build
1. dependency : x11proto-dev, libxcb1-dev, libxcb-keysyms1-dev and libxcb-util0-dev
1. Download [xcb-imdkit](https://github.com/wengxt/xcb-imdkit)
1. Set the path of xcb-imdkit to IMDKIT_PATH
1. Build
```
Make
```

### Setup
```
$ mkdir -p ~/bin && cp soju "$_"
$ cat > ~/.xsessionrc
QT_IM_MODULE=xim
GTK_IM_MODULE=xim
XMODIFIERS=@im=soju
bin/soju &
^D
```
