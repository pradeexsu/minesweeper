# The MineSweeper Game

<p align="center">
<img src="https://raw.githubusercontent.com/sutharp777/minesweeper/master/demo.gif" height="400">
<p>
  
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
