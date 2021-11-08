# FreeTils
## Utils to ease development with Free's STB.

## This tool is already able to do :
- auto detect stb on network
- deploy qml app on stb (revolution & delta)
- show logs

## This tool will be able to :
- launch app with qmlscene
- filter logs
- control app with virtual remote on keyboard
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
- [ ] detect device type (revolution, delta, mini 4k or unsuported)
- [ ] make it work for the mini 4K android via ADB
- [ ] control the app via remote keyboard
- [ ] launch app with qmlscene
- [ ] filter logs
- [ ] see if hot reload is possible ?
- [ ] make home made file server work instead of using PHP internal server
- [ ] make a cmd line version (and a VS code extension ?)

## Setup requirements
- PHP 7 must be installed (internal server is used for file serving, should be temporary)

## Build with
- Qt 6 / C++2a

## Screenshots
![first working version](screenshots/freeTils.png?raw=true "FreeTils")
