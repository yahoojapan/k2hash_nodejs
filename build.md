---
layout: contents
language: en-us
title: Build
short_desc: K2HASH Node.js - K2HASH addon library for Node.js
lang_opp_file: buildja.html
lang_opp_word: To Japanese
prev_url: usage.html
prev_string: Usage
top_url: index.html
top_string: TOP
next_url: developer.html
next_string: Developer
---

# Building
The build method for K2HASH Node.js addon library is explained below.

## 1. Install prerequisites before compiling
- Debian / Ubuntu
```
$ sudo aptitude update
$ sudo aptitude git gcc g++ make gdb dh-make fakeroot dpkg-dev
$ sudo aptitude install nodejs npm
```
- Fedora / CentOS
```
$ sudo yum install git autoconf automake gcc libstdc++-devel gcc-c++ make libtool
$ sudo yum install nodejs npm
```

## 2. K2HASH library
In advance, you need to install the developer package of [K2HASH](https://k2hash.antpick.ax/) library(See [here](https://k2hash.antpick.ax/usage.html) for installation).  
Without installing this package, you can also build the [K2HASH](https://k2hash.antpick.ax/) library(See [here](https://k2hash.antpick.ax/build.html) for the build).

## 3. Clone source codes from Github
```
$ git clone git@github.com:yahoojapan/k2hash_nodejs.gif
$ cd k2hash_nodejs
```

## 4. Building and Testing
Run the build and test using the [npm](https://www.npmjs.com/get-npm) command.
```
$ sudo npm cache clean
$ npm update
$ npm run build
$ npm run test
```
If the building and testing succeed, you can find three ** *.node ** files in the following path. (And a symbolic link **build** under the top directory.)
```
$ ls -la build
lrwxrwxrwx 1 guest users     43 Nov 30 14:00 build -> /home/guest/k2hash_nodejs/src/build

$ ls -l src/build/Release/*.node
total 996
-rwxr-xr-x 1 guest users 563119 Nov 30 14:00 k2hash.node
-rwxr-xr-x 1 guest users 223379 Nov 30 14:00 k2hkeyqueue.node
-rwxr-xr-x 1 guest users 222004 Nov 30 14:00 k2hqueue.node
```
