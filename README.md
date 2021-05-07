# minesweeper

![demo](https://raw.githubusercontent.com/sutharp777/minesweeper/master/demo.gif)


## run on you linux OS

### step 1. install wxgtk
```
sudo apt install libwxgtk3.0-gtk3-dev
```
### step 2. go to minesweeper directory and build main.cpp
```
g++ main.cpp `wx-config --cxxflags --libs` -o app
```
### running app
```
./app
```
