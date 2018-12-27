---
layout: contents
language: ja
title: Usage
short_desc: K2HASH Node.js - K2HASH addon library for Node.js
lang_opp_file: usage.html
lang_opp_word: To English
prev_url: featureja.html
prev_string: Feature
top_url: indexja.html
top_string: TOP
next_url: buildja.html
next_string: Build
---

# 使い方
K2HASH Node.jsをビルドした後で、動作確認をしてみます。

## 1. 実行環境の構築
まず、実行環境を構築します。  
構築は、パッケージから構築する方法と、k2hash nodejs addonライブラリをビルドして利用する方法があります。

### 1.1 パッケージを利用して環境を構築
パッケージを利用して実行環境を構築する方法を説明します。  
k2hash nodejs addonライブラリのパッケージは、[npm](https://www.npmjs.com/package/k2hash)にあります。  

このパッケージをインストールする前に、k2hashの開発者パッケージが必要となりますので、k2hashの[ドキュメント](https://k2hash.antpick.ax/indexja.html)を参照してください。  
k2hashをインストールする方法も、パッケージからのインストールと、k2hashを自身でビルドする方法がありますので、[使い方](https://k2hash.antpick.ax/usageja.html)もしくは[ビルド](https://k2hash.antpick.ax/buildja.html)のドキュメントを参照してください。  

事前にk2hashライブラリをインストールした後、k2hash nodejs addonライブラリのnpmパッケージは、以下のようにインストールします。  
npmコマンドの詳細については[こちら](https://docs.npmjs.com/misc/index.html#npm1)を参照してください。  
```
$ npm install k2hash
```
### 1.2 ビルドして環境を構築
ソースコードからk2hash nodejs addonライブラリをビルドすることができます。  
ソースコードのgithubリポジトリをcloneし、ビルドする方法は[こちら](https://node.k2hash.antpick.ax/buildja.html)を参照してください。

## 2. サンプル
以下のようなファイルを作成し、ファイル作成、キーと値の書き込み、読み出しができるか確認してみてください。  
```
var k2hash = require('k2hash');             // import k2hash nodejs addon library

var k2h    = k2hash();
k2h.create('test.k2h', 2, 4, 4, 128);       // create k2hash file for testing

k2h.setValue('testkey', 'testvalue');       // set test key with value
var value = k2h.getValue('testkey');        // get it

console.log(value);
k2h.close();                                // close k2hash file
```

## 3. ロード・実行
基本として、JavaScript言語でライブラリを読み込むための`require`でK2HASH Node.js アドオンライブラリを読み込みます。  
その後は、K2HASH Node.js アドオンライブラリが提供するクラス・関数・メソッドを呼び出してください。  
具体的には、環境や言語（TypeScriptなど）に応じて利用してください。

## 4. その他のテスト  
K2HASH Node.jsでは、[Mocha](https://github.com/mochajs/mocha)と[Chai](https://github.com/chaijs/chai)を使ったテストを実行できます。  
以下のように[npm](https://www.npmjs.com/get-npm)コマンドを使い、作成したK2HASH Node.jsをテストすることができます。  
```
$ npm run test
```
