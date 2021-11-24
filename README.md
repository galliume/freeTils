# FreeTils
## Utils to ease development with Free's STB.

## This tool is already able to do :
- auto detect stb on network
- deploy qml app on stb (revolution & delta)
- launch qml app on your computer without any stb
- show logs

## This tool will be able to :
- control app with virtual remote on keyboard
- filter logs
- maybe hot reload

## STB already supported :
- [x] revolution
- [x] delta

## STB who will be supported :
- [ ] mini 4K

## OS supported :
- [x] linux
- [x] windows
- [x] mac

## Help
- Consult the wiki page [help](https://github.com/galliume/freeTils/wiki/Help)

## @Todo
- [x] auto detect freebox on network
- [x] create a tcpserver to serve project files
- [x] add root project dir selection
- [x] deploy app to the freebox via JsonRpc
- [x] close properly the app and disconnect from stb
- [x] display logs
- [x] cleaning and refactoring
- [x] launch app with qmlscene
- [ ] control the app via remote keyboard
- [ ] detect device type (revolution, delta, mini 4k or unsuported)
- [ ] make it work for the mini 4K android via ADB
- [ ] filter logs
- [ ] see if hot reload is possible ?
- [ ] make home made file server work instead of using PHP internal server
- [ ] make a cmd line version (and a VS code extension ?)

## Setup requirements
- You have to fill your freebox develop id per category and per stb (all given by Free on demand) in the manifest.json (see [help](https://github.com/galliume/freeTils/wiki/Help) section)
- PHP 7 must be installed (internal server is used for file serving, should be temporary)
- To be able to use the QML launcher, the freebox library libfbxqml must be installed in "<rootAppFolder>/vendor/libfbxqml"

## Build with
- Qt 6 / C++2a

## Screenshots
![first working version](screenshots/freeTils.png?raw=true "FreeTils")
