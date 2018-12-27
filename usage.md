---
layout: contents
language: en-us
title: Usage
short_desc: K2HASH Node.js - K2HASH addon library for Node.js
lang_opp_file: usageja.html
lang_opp_word: To Japanese
prev_url: feature.html
prev_string: Feature
top_url: index.html
top_string: TOP
next_url: build.html
next_string: Build
---

# Usage
Try to check its operation after building K2HASH Node.js addon library.

## 1. Construction of execution environment
First of all, you need to build an execution environment.  
There are two ways to prepare for the environment.
One way is to install the package.
The other way is to build the k2hash nodejs addon library yourself.

### 1.1 Creating an environment using packages
This is a way to create an execution environment using packages.  
The package of k2hash nodejs addon library is in [npm](https://www.npmjs.com/package/k2hash).  

Before installing this package, you will need the k2hash developer package, so please refer to the [k2hash documentation](https://k2hash.antpick.ax/).  
There are two ways to install k2hash.
The first is installing from the package.(Please refer to [k2hash usage document](https://k2hash.antpick.ax/usage.html).)
The other way is to build k2hash by yourself.(Please refer to [k2hash build document](https://k2hash.antpick.ax/build.html).)  
Please create an environment that can use k2hash in any way.  

After installing the k2hash library in advance, install the npm package of the k2hash nodejs addon library as follows.    
For details of the npm command, please refer to [here](https://docs.npmjs.com/misc/index.html#npm1).
```
$ npm install k2hash
```

### 1.2 Build to create an environment
You can build the k2hash nodejs addon library from the [source code](https://github.com/yahoojapan/k2hash_nodejs).  
See [here](https://nodejs.k2hash.antpick.ax/build.html) for how to clone and build the source code github repository.

## 2. Examples
Please create the following file and check whether file creation, key and value writing and reading are possible.
```
var k2hash = require('k2hash');             // import k2hash nodejs addon library

var k2h    = k2hash();
k2h.create('test.k2h', 2, 4, 4, 128);       // create k2hash file for testing

k2h.setValue('testkey', 'testvalue');       // set test key with value
var value = k2h.getValue('testkey');        // get it

console.log(value);
k2h.close();                                // close k2hash file
```

## 3. Importing and Executing
First, import the K2HASH Node.js addon library with `require` to read the library in the JavaScript language.
After that, you can call the class, function, and method provided by K2HASH Node.js addon library.
Please implement these depending on your environment and language(TypeScript etc.).

## 4. Other test
K2HASH Node.js addon library provides unit testing using [Mocha](https://github.com/mochajs/mocha) and [Chai](https://github.com/chaijs/chai).
You can test K2HASH Node.js addon library built by using the [npm](https://www.npmjs.com/get-npm) command as shown below.
```
$ npm run test
```
