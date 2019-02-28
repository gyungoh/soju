# soju
Lightweight Korean IME for linux

### Build
1. Download [xcb-imdkit](https://github.com/wengxt/xcb-imdkit)
1. Set the path of xcb-imdkit to IMDKIT_PATH
1. Build
```
Make
```

### Setup
```
$ mkdir -p ~/bin && cp soju "$_"
$ cat > ~/.xinputrc
QT_IM_MODULE=xim
XMODIFIERS=@im=soju
GTK_IM_MODULE=xim
bin/soju &
^D
```
