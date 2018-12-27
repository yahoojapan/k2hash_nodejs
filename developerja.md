---
layout: contents
language: ja
title: Developer
short_desc: K2HASH Node.js - K2HASH addon library for Node.js
lang_opp_file: developer.html
lang_opp_word: To English
prev_url: buildja.html
prev_string: Build
top_url: indexja.html
top_string: TOP
next_url: environmentsja.html
next_string: Environments
---

<!-- -----------------------------------------------------------　-->
# 開発者向け

## [共通](#COMMON)
[同期と非同期](#SYNCHRONOUS)  
[提供されるクラス](#ABOUTCLASS)

## [k2hashクラス](#K2HASHCLASS)
[k2hash::Create()](#K2HASH-CREATE)  
[k2hash::Open()](#K2HASH-OPEN)  
[k2hash::OpenRW()](#K2HASH-OPENRW)  
[k2hash::OpenRO()](#K2HASH-OPENRO)  
[k2hash::OpenTempfile()](#K2HASH-OPENTEMPFILE)  
[k2hash::OpenMem()](#K2HASH-OPENMEM)  
[k2hash::Close()](#K2HASH-CLOSE)  
[k2hash::GetValue()](#K2HASH-GETVALUE)  
[k2hash::GetSubkeys()](#K2HASH-GETSUBKEYS)  
[k2hash::SetValue()](#K2HASH-SETVALUE)  
[k2hash::AddSubkey()](#K2HASH-ADDSUBKEY)  
[k2hash::AddSubkeys()](#K2HASH-ADDSUBKEYS)  
[k2hash::Remove()](#K2HASH-REMOVE)  
[k2hash::RemoveAll()](#K2HASH-REMOVEALL)  
[k2hash::PrintState()](#K2HASH-PRINTSTATE)  
[k2hash::PrintVersion()](#K2HASH-PRINTVERSION)  
[k2hash::DumpHead()](#K2HASH-DUMPHEAD)  
[k2hash::DumpKeytable()](#K2HASH-DUMPKEYTABLE)  
[k2hash::DumpFullKeytable()](#K2HASH-DUMPFULLKEYTABLE)  
[k2hash::DumpElementtable()](#K2HASH-DUMPELEMENTTABLE)  
[k2hash::DumpFull()](#K2HASH-DUMPFULL)  
[k2hash::Transaction()](#K2HASH-TRANSACTION)  
[k2hash::EnableTransaction()](#K2HASH-ENABLETRANSACTION)  
[k2hash::DisableTransaction()](#K2HASH-DISABLETRANSACTION)  
[k2hash::UnsetTransactionThreadPool()](#K2HASH-UNSETTRANSACTIONPOOL)  
[k2hash::GetTransactionThreadPool()](#K2HASH-GETTRANSACTIONPOOL)  
[k2hash::SetTransactionThreadPool()](#K2HASH-SETTRANSACTIONPOOL)  
[k2hash::LoadArchive()](#K2HASH-LOADARCHIVE)  
[k2hash::PutArchive()](#K2HASH-LOADARCHIVE)  
[k2hash::SetCommonAttribute()](#K2HASH-SETCOMMONATTR)  
[k2hash::CleanCommonAttribute()](#K2HASH-CLEANCOMMONATTR)  
[k2hash::AddAttrPluginLib()](#K2HASH-ADDATTRPLUGINLIB)  
[k2hash::AddAttrCryptPass()](#K2HASH-ADDATTRCRYPTPASS)  
[k2hash::GetAttrVersionInfos()](#K2HASH-GETATTRVERINFO)  
[k2hash::GetAttrInfos()](#K2HASH-GETATTRINFOS)  
[k2hash::GetAttrs()](#K2HASH-GETATTRS)  
[k2hash::GetAttrValue()](#K2HASH-GETATTRVALUE)  
[k2hash::AddAttr()](#K2HASH-ADDATTR)  
[k2hash::GetQueue()](#K2HASH-GETQUEUE)  
[k2hash::GetKeyQueue()](#K2HASH-GETKEYQUEUE)  

## [k2hqueueクラス](#K2HQUEUECLASS)
[k2hqueue::Init()](#K2HQUEUE-INIT)  
[k2hqueue::Push()](#K2HQUEUE-PUSH)  
[k2hqueue::Count()](#K2HQUEUE-COUNT)  
[k2hqueue::IsEmpty()](#K2HQUEUE-ISEMPTY)  
[k2hqueue::Read()](#K2HQUEUE-READ)  
[k2hqueue::Pop()](#K2HQUEUE-POP)  
[k2hqueue::Remove()](#K2HQUEUE-REMOVE)  
[k2hqueue::Dump()](#K2HQUEUE-DUMP)  

## [k2hkeyqueueクラス](#K2HKEYQUEUECLASS)
[k2hkeyqueue::Init()](#K2HKEYQUEUE-INIT)  
[k2hkeyqueue::Push()](#K2HKEYQUEUE-PUSH)  
[k2hkeyqueue::Count()](#K2HKEYQUEUE-COUNT)  
[k2hkeyqueue::IsEmpty()](#K2HKEYQUEUE-ISEMPTY)  
[k2hkeyqueue::Read()](#K2HKEYQUEUE-READ)  
[k2hkeyqueue::Pop()](#K2HKEYQUEUE-POP)  
[k2hkeyqueue::Remove()](#K2HKEYQUEUE-REMOVE)  
[k2hkeyqueue::Dump()](#K2HKEYQUEUE-DUMP)  

<!-- -----------------------------------------------------------　-->
***

## <a name="COMMON"> 共通
### <a name="ASYNCHRONOUS"> 同期と非同期
K2HASH Node.js アドオンライブラリのクラスのメソッドは、同期処理とCallback関数を指定できる非同期処理をサポートしています。  
非同期処理をサポートしているメソッドは、callback関数の引数を受け取ることができます。  
また、非同期処理として**on**や**onXXXXX**でイベントハンドラーを指定することもできます。  
callback関数もしくはイベントハンドラーの指定をすることで非同期処理を行うことができます。  

callback関数の引数を指定しない場合やイベントハンドラーの指定を指定しない場合、各々のメソッドは同期処理として動作します。

### <a name="ABOUTCLASS"> 提供されるクラス
K2HASH Node.js アドオンライブラリは、**k2hash**クラスを提供します。  
[K2HASH](https://k2hash.antpick.ax/indexja.html)の機能の**queue**を操作するための**k2hqueue**クラスと**k2hKeyqueue**クラスも提供されています。  
**k2hqueue**クラスと**k2hKeyqueue**クラスのオブジェクトは、**k2hash**クラスのオブジェクトからメソッドを使って取得することができます。

<!-- -----------------------------------------------------------　-->
***

## <a name="K2HASHCLASS"> k2hashクラス
K2HASH Node.js アドオンライブラリの提供するメインクラスです。  
このクラスにより、K2HASHファイルやメモリ上のデータの操作を行うことができます。

クラスを生成するには以下のように指定します。
  ```
  var k2hobj = require('k2hash')();
  ```
  もしくは
  ```
  var k2hobj = new require('k2hash')();
  ```

以下に、K2HASHクラスのメソッドとその説明をします。

### <a name="K2HASH-CREATE"> k2hash::Create()
K2HASHファイルの作成および初期化します。

#### 書式
```
bool Create(String   filename,
            int      maskbitcnt = 2,
            int      cmaskbitcnt = 4,
            int      maxelementcnt = 32,
            int      pagesize = 128,
            Callback cbfunc = null
)
```

#### 引数
- filename  
  K2HASHファイルパスを指定します。
- maskbitcnt  
  K2HASHファイルの初期**mask**ビット数を指定します。
- cmaskbitcnt  
  K2HASHファイルの初期**cmask**ビット数を指定します。
- maxelementcnt  
  K2HASHファイルの初期**element count**数を指定します。
- pagesize  
  K2HASHファイルの**ページサイズ**を指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  k2h.SetValue('key','val');
  
  console_log(k2h.GetValue('key'));
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h', function(error){
      if(null == error){
          k2h.SetValue('key','val');
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('create', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('create', function(error){
      if(null == error){
          k2h.SetValue('key','val');
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  
  k2h.Create('test.k2h');
  ```
- onCreate(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onCreate(function(error){
      if(null == error){
          k2h.SetValue('key','val');
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  
  k2h.Create('test.k2h');
  ```

### <a name="K2HASH-OPEN"> k2hash::Open()
K2HASHファイルをオープンします。  
ファイルが存在しない場合には、新規に作成されます。

#### 書式
```
bool Open(  String   filename,
            bool     readonly = false, 
            bool     removefile = false, 
            bool     fullmap = true, 
            int      maskbitcnt = 2, 
            int      cmaskbitcnt = 4, 
            int      maxelementcnt = 32, 
            int      pagesize = 128,
            Callback cbfunc = null
)
```

#### 引数
- filename  
  K2HASHファイルパスを指定します。
- readonly  
  trueを指定した場合、K2HASHファイルを読み出し専用でオープンします。
- removefile  
  trueを指定した場合、K2HASHファイルを閉じるときにファイルを削除します。
- fullmap  
  K2HASHファイルをオープンしたときに、そのファイル内容をすべてメモリに読み込みます。
- maskbitcnt  
  K2HASHファイルの初期**mask**ビット数を指定します。（ファイルが新規作成される場合のみ影響します。）
- cmaskbitcnt  
  K2HASHファイルの初期**cmask**ビット数を指定します。（ファイルが新規作成される場合のみ影響します。）
- maxelementcnt  
  K2HASHファイルの初期**element count**数を指定します。（ファイルが新規作成される場合のみ影響します。）
- pagesize  
  K2HASHファイルの**ページサイズ**を指定します。（ファイルが新規作成される場合のみ影響します。）
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Open('test.k2h');
  k2h.SetValue('key','val');
  
  console_log(k2h.GetValue('key'));
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Open('test.k2h', function(error){
      if(null == error){
          k2h.SetValue('key','val');
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('open', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('open', function(error){
      if(null == error){
          k2h.SetValue('key','val');
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  
  k2h.Open('test.k2h');
  ```
- onOpen(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onOpen(function(error){
      if(null == error){
          k2h.SetValue('key','val');
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  
  k2h.Open('test.k2h');
  ```

### <a name="K2HASH-OPENRW"> k2hash::OpenRW()
K2HASHファイルを読み書き専用としてオープンします。  

#### 書式
```
bool OpenRW(String   filename,
            bool     fullmap = true, 
            int      maskbitcnt = 2, 
            int      cmaskbitcnt = 4, 
            int      maxelementcnt = 32, 
            int      pagesize = 128,
            Callback cbfunc = null
)
```

#### 引数
- filename  
  K2HASHファイルパスを指定します。
- fullmap  
  K2HASHファイルをオープンしたときに、そのファイル内容をすべてメモリに読み込みます。
- maskbitcnt  
  K2HASHファイルの初期**mask**ビット数を指定します。（ファイルが新規作成される場合のみ影響します。）
- cmaskbitcnt  
  K2HASHファイルの初期**cmask**ビット数を指定します。（ファイルが新規作成される場合のみ影響します。）
- maxelementcnt  
  K2HASHファイルの初期**element count**数を指定します。（ファイルが新規作成される場合のみ影響します。）
- pagesize  
  K2HASHファイルの**ページサイズ**を指定します。（ファイルが新規作成される場合のみ影響します。）
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenRW('test.k2h');
  k2h.SetValue('key','val');
  
  console_log(k2h.GetValue('key'));
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenRW('test.k2h', function(error){
      if(null == error){
          k2h.SetValue('key','val');
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('openRW', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('openRW', function(error){
      if(null == error){
          k2h.SetValue('key','val');
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  
  k2h.OpenRW('test.k2h');
  ```
- onOpenRW(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onOpenRW(function(error){
      if(null == error){
          k2h.SetValue('key','val');
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  
  k2h.OpenRW('test.k2h');
  ```

### <a name="K2HASH-OPENRO"> k2hash::OpenRO()
K2HASHファイルを読みとり専用としてオープンします。  

#### 書式
```
bool OpenRO(String   filename,
            bool     fullmap = true, 
            int      maskbitcnt = 2, 
            int      cmaskbitcnt = 4, 
            int      maxelementcnt = 32, 
            int      pagesize = 128,
            Callback cbfunc = null
)
```

#### 引数
- filename  
  K2HASHファイルパスを指定します。
- fullmap  
  K2HASHファイルをオープンしたときに、そのファイル内容をすべてメモリに読み込みます。
- maskbitcnt  
  K2HASHファイルの初期**mask**ビット数を指定します。（ファイルが新規作成される場合のみ影響します。）
- cmaskbitcnt  
  K2HASHファイルの初期**cmask**ビット数を指定します。（ファイルが新規作成される場合のみ影響します。）
- maxelementcnt  
  K2HASHファイルの初期**element count**数を指定します。（ファイルが新規作成される場合のみ影響します。）
- pagesize  
  K2HASHファイルの**ページサイズ**を指定します。（ファイルが新規作成される場合のみ影響します。）
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenRO('test.k2h');
  
  console_log(k2h.GetValue('key'));
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenRO('test.k2h', function(error){
      if(null == error){
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('openRO', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('openRO', function(error){
      if(null == error){
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  
  k2h.OpenRO('test.k2h');
  ```
- onOpenRO(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onOpenRO(function(error){
      if(null == error){
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  
  k2h.OpenRO('test.k2h');
  ```

### <a name="K2HASH-OPENTEMPFILE"> k2hash::OpenTempfile()
一時K2HASHファイルを作成、初期化、オープンします。  
このファイルはクローズ後に削除されます。

#### 書式
```
bool OpenTempfile(String   filename,
                  bool     fullmap = true, 
                  int      maskbitcnt = 2, 
                  int      cmaskbitcnt = 4, 
                  int      maxelementcnt = 32, 
                  int      pagesize = 128,
                  Callback cbfunc = null
)
```

#### 引数
- filename  
  K2HASHファイルパスを指定します。
- fullmap  
  K2HASHファイルをオープンしたときに、そのファイル内容をすべてメモリに読み込みます。
- maskbitcnt  
  K2HASHファイルの初期**mask**ビット数を指定します。（ファイルが新規作成される場合のみ影響します。）
- cmaskbitcnt  
  K2HASHファイルの初期**cmask**ビット数を指定します。（ファイルが新規作成される場合のみ影響します。）
- maxelementcnt  
  K2HASHファイルの初期**element count**数を指定します。（ファイルが新規作成される場合のみ影響します。）
- pagesize  
  K2HASHファイルの**ページサイズ**を指定します。（ファイルが新規作成される場合のみ影響します。）
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenTempfile('test.k2h');
  k2h.SetValue('key','val');
  
  console_log(k2h.GetValue('key'));
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenTempfile('test.k2h', function(error){
      if(null == error){
          k2h.SetValue('key','val');
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('openTempfile', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('openTempfile', function(error){
      if(null == error){
          k2h.SetValue('key','val');
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  
  k2h.OpenTempfile('test.k2h');
  ```
- onOpenTempfile(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onOpenTempfile(function(error){
      if(null == error){
          k2h.SetValue('key','val');
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  
  k2h.OpenTempfile('test.k2h');
  ```

### <a name="K2HASH-OPENMEM"> k2hash::OpenMem()
オンメモリ上にK2HASHデータを初期化し、そのハンドルをオブジェクトに紐付けします。

#### 書式
```
bool OpenMem(	int      maskbitcnt = 2, 
                int      cmaskbitcnt = 4, 
                int      maxelementcnt = 32, 
                int      pagesize = 128,
                Callback cbfunc = null
)
```

#### 引数
- maskbitcnt  
  オンメモリK2HASHデータの初期**mask**ビット数を指定します。（ファイルが新規作成される場合のみ影響します。）
- cmaskbitcnt  
  オンメモリK2HASHデータの初期**cmask**ビット数を指定します。（ファイルが新規作成される場合のみ影響します。）
- maxelementcnt  
  オンメモリK2HASHデータの初期**element count**数を指定します。（ファイルが新規作成される場合のみ影響します。）
- pagesize  
  オンメモリK2HASHデータの**ページサイズ**を指定します。（ファイルが新規作成される場合のみ影響します。）
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.SetValue('key','val');
  
  console_log(k2h.GetValue('key'));
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem(function(error){
      if(null == error){
          k2h.SetValue('key','val');
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('openMem', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('openMem', function(error){
      if(null == error){
          k2h.SetValue('key','val');
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  ```
- onOpenMem(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onOpenMem(function(error){
      if(null == error){
          k2h.SetValue('key','val');
          console_log(k2h.GetValue('key'));
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  ```

### <a name="K2HASH-CLOSE"> k2hash::Close()
オープンしているK2HASHファイルもしくはオンメモリをクローズします。

#### 書式
```
bool Close(Callback cbfunc = null)
```

#### 引数
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.Close(function(error){
      if(null !== error){
          console_log('close error');
      }
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('close', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('close', function(error){
      if(null !== error){
          console_log('close error');
      }
  });
  
  k2h.OpenMem();
  k2h.Close();
  ```
- onClose(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onClose(function(error){
      if(null !== error){
          console_log('close error');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.Close();
  ```

### <a name="K2HASH-GETVALUE"> k2hash::GetValue()
キーを指定して値を読み出します。

#### 書式
```
[bool/String] GetValue(  String   key,
                         String   subkey = "",
                         bool     attrcheck = true,
                         String   pass = "",
                         Callback cbfunc = null
)
```

#### 引数
- key  
  読み出すキーを指定します。  
  サブキーが指定されている場合には、この値はサブキーの親キーを示します。
- subkey  
  サブキーの値を読み出す場合に指定します。  
  親キー以下にサブキーが存在しない場合にはエラーとなります。
- attrcheck  
  K2HASHの組み込み属性のチェックを行う場合、trueに設定してください。  
  組み込み属性とは、暗号化、履歴、有効期限のことです。
- pass  
  キーが暗号化された値を持つ場合には、複合するためのパスフレーズを指定してください。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error, String value)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
- 同期  
  同期処理の場合、正常終了したときには読み出した**値**が返されます。  
  エラーが発生した場合には、**null**が返されます。
- 非同期（Callback）  
  Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.SetValue('key','val');
  
  var value = k2h.GetValue('key');
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.SetValue('key','val');

  k2h.GetValue('key', function(error, value){
      if(null == error){
          console_log(value);
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('get', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('get', function(error, value){
      if(null == error){
          console_log(value);
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.SetValue('key','val');
  
  k2h.GetValue('key');
  ```
- onGet(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onGet(function(error, value){
      if(null == error){
          console_log(value);
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.SetValue('key','val');
  
  k2h.GetValue('key');
  ```

#### 注意
本メソッドはキーとサブキーを同時に指定することができます。  
通常、キーの値の読み出しであれば、キー名を指定するだけで読み出すことができます。  
キーとサブキーを同時に指定するという意味は、サブキーがキーに紐付けられていることが条件となります。  
もし、サブキーがキーに紐づいていない場合にはこのメソッドは値を読み出さず、エラーを返します。

### <a name="K2HASH-GETSUBKEYS"> k2hash::GetSubkeys()
キーを指定して、キーに紐づいているサブキーのリストを読み出します。

#### 書式
```
[bool/Array] GetSubkeys(String   key,
                        Callback cbfunc = null
)
```

#### 引数
- key  
  サブキーのリストを読み出すキーを指定します。  
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error, Array skeylist)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
- 同期  
  同期処理の場合、正常終了したときには読み出したサブキーのリストを**配列**で返します。
  エラーが発生した場合には、**null**が返されます。
- 非同期（Callback）  
  Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.addSubkey('parentkey', 'subkey', 'subkeyval');
  
  var skeylist = k2h.GetSubkeys('parentkey');
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.addSubkey('parentkey', 'subkey', 'subkeyval');
  
  k2h.GetSubkeys('parentkey', function(error, skeylist){
      if(null == error){
          console_log(skeylist[0]);
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('getSubkeys', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('getSubkeys', function(error, skeylist){
      if(null == error){
          console_log(skeylist[0]);
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.addSubkey('parentkey', 'subkey', 'subkeyval');
  
  k2h.GetSubkeys('parentkey');
  ```
- onGetSubkeys(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onGetSubkeys(function(error, skeylist){
      if(null == error){
          console_log(skeylist[0]);
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.addSubkey('parentkey', 'subkey', 'subkeyval');
  
  k2h.GetSubkeys('parentkey');
  ```

### <a name="K2HASH-SETVALUE"> k2hash::SetValue()
キーを指定して値を設定します。

#### 書式
```
bool SetValue(  String   key,
                String   value,
                String   subkey = null,
                String   pass = "",
                int      expire = 0,
                Callback cbfunc = null
)
```

#### 引数
- key  
  値を設定するキーを指定します。  
  サブキーが指定されている場合には、この値はサブキーの親キーを示します。
- value  
  設定する値を指定します。
- subkey  
  このキーが指定された場合には、この値はサブキー名を示し、サブキーは親キーに紐付けられ、値はサブキーに設定されます。
- pass  
  キー（値）を暗号化する場合にパスフレーズを指定します。
- expire  
  キー（値）に有効期限を設定する場合には、この値に有効期限秒を指定します。  
  0を指定した場合には、有効期限は設定されません。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.SetValue('parentkey', 'subkey', 'val');
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.SetValue('parentkey', 'subkey', 'val', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('set', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('set', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.SetValue('parentkey', 'subkey', 'val');
  ```
- onSet(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onSet(function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.SetValue('parentkey', 'subkey', 'val');
  ```

### <a name="K2HASH-ADDSUBKEY"> k2hash::AddSubkey()
親キーを指定して、サブキーを追加します。

#### 書式
```
bool AddSubkey( String   key,
                String   subkey,
                String   value = null,
                Callback cbfunc = null
)
```

#### 引数
- key  
  サブキーを追加する親キーを指定します。
- subkey  
  親キーに追加するサブキー名を指定します。
- value  
  親キーにサブキーを追加し、かつサブキーの値も設定する場合に指定できます。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.AddSubkey('parentkey', 'subkey');
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.AddSubkey('parentkey', 'subkey', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('addSubkey', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('addSubkey', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.AddSubkey('parentkey', 'subkey');
  ```
- onAddSubkey(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onAddSubkey(function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.AddSubkey('parentkey', 'subkey');
  ```

### <a name="K2HASH-ADDSUBKEYS"> k2hash::AddSubkeys()
親キーを指定して、サブキーのリストを上書きします。

#### 書式
```
bool AddSubkeys(String   key,
                Array    subkeys,
                Callback cbfunc = null
)
```

#### 引数
- key  
  サブキーを追加する親キーを指定します。
- subkeys  
  親キーのサブキーリストを上書きするサブキーリストの配列を指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.AddSubkeys('parentkey', ['subkey1', 'subkey2']);
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.AddSubkeys('parentkey', ['subkey1', 'subkey2'], function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('addSubkeys', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('addSubkeys', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.AddSubkeys('parentkey', ['subkey1', 'subkey2']);
  ```
- onAddSubkeys(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onAddSubkeys(function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.AddSubkeys('parentkey', ['subkey1', 'subkey2']);
  ```

### <a name="K2HASH-REMOVE"> k2hash::Remove()
キーもしくはサブキーを削除します。

#### 書式
```
bool Remove(String   key,
            String   subkey,
            Callback cbfunc = null
)
```

#### 引数
- key  
  削除するキーを指定します。  
  subkeyを指定した場合には、そのサブキーが削除され、このキーにリストされているサブキーがリストから削除されます。  
  この場合、キーは削除されません。
- subkey  
  サブキーを削除する場合に指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.SetValue('parentkey', 'subkey', 'val');
  
  k2h.Remove('parentkey', 'subkey1');
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.SetValue('parentkey', 'subkey', 'val');
  
  k2h.Remove('parentkey', 'subkey', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('remove', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('remove', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.SetValue('parentkey', 'subkey', 'val');
  
  k2h.Remove('parentkey', 'subkey1');
  ```
- onRemove(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onRemove(function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.SetValue('parentkey', 'subkey', 'val');
  
  k2h.Remove('parentkey', 'subkey1');
  ```

#### 注意
本メソッドはキーとサブキーを同時に指定することができます。  
キーのみを指定した場合には、そのキーが削除されます。  
キーとサブキーを指定した場合、サブキーは削除され、キーは削除されません。  
この場合、キーにリストのサブキーリストから指定されたサブキーが削除されます。  
もし、キーのサブキーリストに指定されたサブキーが存在しない場合には、エラーを返します。

### <a name="K2HASH-REMOVEALL"> k2hash::RemoveAll()
キーと、キーに紐付けられているサブキーもすべて削除します。

#### 書式
```
bool RemoveAll(String   key,
               Callback cbfunc = null
)
```

#### 引数
- key  
  削除するキーを指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.SetValue('parentkey', 'subkey', 'val');
  
  k2h.RemoveAll('parentkey');
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.SetValue('parentkey', 'subkey', 'val');
  
  k2h.RemoveAll('parentkey', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('removeAll', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('removeAll', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.SetValue('parentkey', 'subkey', 'val');
  
  k2h.RemoveAll('parentkey');
  ```
- onRemoveAll(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onRemoveAll(function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.SetValue('parentkey', 'subkey', 'val');
  
  k2h.RemoveAll('parentkey');
  ```

### <a name="K2HASH-PRINTSTATE"> k2hash::PrintState()
K2HASHファイルもしくはオンメモリデータの状態（情報）を出力します。

#### 書式
```
bool PrintState(int fd = null)
```

#### 引数
- fd  
  必要であれば、出力用のStreamを指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.PrintState();

k2h.Close();
```

### <a name="K2HASH-PRINTVERSION"> k2hash::PrintVersion()
K2HASHライブラリのバージョン、CopyRightを表示します。

#### 書式
```
bool PrintVersion(int fd = null)
```

#### 引数
- fd  
  必要であれば、出力用のStreamを指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.PrintVersion();
```

### <a name="K2HASH-DUMPHEAD"> k2hash::DumpHead()
K2HASHファイルもしくはオンメモリデータの内部HEADER情報を出力します。

#### 書式
```
bool DumpHead(int fd = null)
```

#### 引数
- fd  
  必要であれば、出力用のStreamを指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.DumpHead();

k2h.Close();
```

### <a name="K2HASH-DUMPKEYTABLE"> k2hash::DumpKeytable()
K2HASHファイルもしくはオンメモリデータの内部キーデータテーブル情報を出力します。

#### 書式
```
bool DumpKeytable(int fd = null)
```

#### 引数
- fd  
  必要であれば、出力用のStreamを指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.DumpKeytable();

k2h.Close();
```

### <a name="K2HASH-DUMPFULLKEYTABLE"> k2hash::DumpFullKeytable()
K2HASHファイルもしくはオンメモリデータの内部キーテーブルの情報をすべて出力します。

#### 書式
```
bool DumpFullKeytable(int fd = null)
```

#### 引数
- fd  
  必要であれば、出力用のStreamを指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.DumpFullKeytable();

k2h.Close();
```

### <a name="K2HASH-DUMPELEMENTTABLE"> k2hash::DumpElementtable()
K2HASHファイルもしくはオンメモリデータの内部エレメントテーブル情報を出力します。

#### 書式
```
bool DumpElementtable(int fd = null)
```

#### 引数
- fd  
  必要であれば、出力用のStreamを指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.DumpElementtable();

k2h.Close();
```

### <a name="K2HASH-DUMPFULL"> k2hash::DumpFull()
K2HASHファイルもしくはオンメモリデータの内部データをすべて出力します。

#### 書式
```
bool DumpFull(int fd = null)
```

#### 引数
- fd  
  必要であれば、出力用のStreamを指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.DumpFull();

k2h.Close();
```

### <a name="K2HASH-TRANSACTION"> k2hash::Transaction()
K2HASHファイルもしくはオンメモリデータに対してトランザクション処理の有効、無効を設定します。

#### 書式
```
bool Transaction(bool   enable,
                 String transfile = "",
                 String prefix = "",
                 String param = "",
                 int    expire = 0
)
```

#### 引数
- enable  
  トランザクション処理の有効、無効を指定します。
- transfile  
  トランザクションを有効とする場合、トランザクション出力をするファイルを指定します。
- prefix  
  トランザクションを有効とする場合、トランザクションデータを蓄積するためのキーのプレフィックスを指定します。
- param  
  トランザクションを有効とする場合、トランザクション処理に応じたパラメータを指定します。
- expire  
  トランザクションを有効とする場合、トランザクションデータの有効期限を指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.Transaction(true, 'my.tr', 'tr_prefix_', '', 10);

k2h.Close();
```

### <a name="K2HASH-ENABLETRANSACTION"> k2hash::EnableTransaction()
K2HASHファイルもしくはオンメモリデータに対してトランザクション処理を有効にします。

#### 書式
```
bool EnableTransaction(String transfile = "",
                       String prefix = "",
                       String param = "",
                       int    expire = 0
)
```

#### 引数
- transfile  
  トランザクション出力をするファイルを指定します。
- prefix  
  トランザクションデータを蓄積するためのキーのプレフィックスを指定します。
- param  
  トランザクション処理に応じたパラメータを指定します。
- expire  
  トランザクションデータの有効期限を指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.EnableTransaction('my.tr', 'tr_prefix_', '', 10);

k2h.Close();
```

### <a name="K2HASH-DISABLETRANSACTION"> k2hash::DisableTransaction()
K2HASHファイルもしくはオンメモリデータに対してトランザクション処理を無効にします。

#### 書式
```
bool DisableTransaction(void)
```

#### 引数
なし

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.DisableTransaction();

k2h.Close();
```

### <a name="K2HASH-UNSETTRANSACTIONPOOL"> k2hash::UnsetTransactionThreadPool()
K2HASHファイルもしくはオンメモリデータに対してトランザクション処理を行うためのスレッドプールをリセットします。

#### 書式
```
bool UnsetTransactionThreadPool(void)
```

#### 引数
なし

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
トランザクションが有効になっている場合は、このメソッドは失敗します。

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.DisableTransaction();
k2h.UnsetTransactionThreadPool();
k2h.EnableTransaction('my.tr', 'tr_prefix_', '', 10);

k2h.Close();
```

### <a name="K2HASH-GETTRANSACTIONPOOL"> k2hash::GetTransactionThreadPool()
K2HASHファイルもしくはオンメモリデータに対してトランザクション処理を行うためのスレッドプールの数を取得します。

#### 書式
```
int GetTransactionThreadPool(void)
```

#### 引数
なし

#### 返り値
スレッドプール数を返します。

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
var pools = k2h.GetTransactionThreadPool();

k2h.Close();
```

### <a name="K2HASH-SETTRANSACTIONPOOL"> k2hash::SetTransactionThreadPool()
K2HASHファイルもしくはオンメモリデータに対してトランザクション処理を行うためのスレッドプールを設定します。

#### 書式
```
bool SetTransactionThreadPool(int count)
```

#### 引数
- count  
  トランザクションスレッドプール数を指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
トランザクションが有効になっている場合は、このメソッドは失敗します。

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.DisableTransaction();
k2h.SetTransactionThreadPool(10);
k2h.EnableTransaction('my.tr', 'tr_prefix_', '', 10);

k2h.Close();
```

### <a name="K2HASH-LOADARCHIVE"> k2hash::LoadArchive()
K2HASHアーカイブファイルからデータをロードします。

#### 書式
```
bool LoadArchive(String   filename,
                 bool     errskip = true,
                 Callback cbfunc = null
)
```

#### 引数
- filename  
  ロードするK2HASHアーカイブファイルを指定します。
- errskip  
  ロード中にエラーが発生した場合、処理を継続するかどうかを指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.LoadArchive('k2hash.ar', false);
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  
  k2h.LoadArchive('k2hash.ar', false, function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('load', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('load', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.LoadArchive('k2hash.ar', false);
  ```
- onLoad(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onLoad(function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.LoadArchive('k2hash.ar', false);
  ```

### <a name="K2HASH-LOADARCHIVE"> k2hash::PutArchive()
K2HASHファイルもしくはオンメモリデータの内容をK2HASHアーカイブファイルとして出力します。

#### 書式
```
bool PutArchive(String   filename,
                 bool     errskip = true,
                 Callback cbfunc = null
)
```

#### 引数
- filename  
  出力先のK2HASHアーカイブファイルを指定します。
- errskip  
  出力中にエラーが発生した場合、処理を継続するかどうかを指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.PutArchive('k2hash.ar', false);
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  
  k2h.PutArchive('k2hash.ar', false, function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('put', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('put', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.PutArchive('k2hash.ar', false);
  ```
- onPut(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onPut(function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.PutArchive('k2hash.ar', false);
  ```

### <a name="K2HASH-SETCOMMONATTR"> k2hash::SetCommonAttribute()
組み込み属性（更新時刻、履歴、有効期限、暗号化、暗号化パスフレーズ）の利用設定をします。

#### 書式
```
bool SetCommonAttribute(int    is_mtime = -1, 
                        int    is_history = -1, 
                        int    is_encrypt = -1, 
                        String passfile = "", 
                        int    is_expire = -1, 
                        int    expire = 0
)
```

#### 引数
- is_mtime  
  更新時刻の設定をします。
  有効(1)、無効(0)、デフォルト・変更なし(-1)の値を指定します。
- is_history  
  履歴の設定をします。
  有効(1)、無効(0)、デフォルト・変更なし(-1)の値を指定します。
- is_encrypt  
  暗号化の設定をします。
  有効(1)、無効(0)、デフォルト・変更なし(-1)の値を指定します。
- passfile  
  暗号化のためのパスフレーズを設定します。
- is_expire  
  有効期限の設定をします。
  有効(1)、無効(0)、デフォルト・変更なし(-1)の値を指定します。
- expire  
  有効期限の設定が有効の場合の秒数を指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.setCommonAttribute(1, 1, 0, 'mypassphrase', 1, 100);

k2h.Close();
```

### <a name="K2HASH-CLEANCOMMONATTR"> k2hash::CleanCommonAttribute()
組み込み属性（更新時刻、履歴、有効期限、暗号化、暗号化パスフレーズ）の状態をデフォルトにします。

#### 書式
```
bool SetCommonAttribute(void)
```

#### 引数
なし

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.cleanCommonAttribute();

k2h.Close();
```

### <a name="K2HASH-ADDATTRPLUGINLIB"> k2hash::AddAttrPluginLib()
カスタム属性ライブラリのロードをします。

#### 書式
```
bool AddAttrPluginLib(String libfile)
```

#### 引数
- libfile  
  カスタム属性ライブラリのファイルパスを指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.addAttrPluginLib('myattrlib.so');

k2h.Close();
```

### <a name="K2HASH-ADDATTRCRYPTPASS"> k2hash::AddAttrCryptPass()
組み込み属性の暗号化のためのパスフレーズを設定します。  
また、組み込み属性の暗号化を有効もしくは無効化します。

#### 書式
```
bool AddAttrCryptPass(String encpass,
                      bool   is_default_encrypt = false
)
```

#### 引数
- encpass  
  組み込み属性の暗号化のためのパスフレーズを指定します。
- is_default_encrypt  
  組み込み属性の暗号化の有効、無効を設定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.k2h.addAttrCryptPass('passphrase', true);

k2h.Close();
```

#### 注意
パスフレーズを指定し、かつ組み込み属性の暗号化を無効とした場合には、キーの更新、書き込みなどにおいて暗号化はなされません。  
キーの読み出しにおいてのみ、指定されたパスフレーズを使い、復号されます。

### <a name="K2HASH-GETATTRVERINFO"> k2hash::GetAttrVersionInfos()
組み込み属性を含めた使用されている属性ライブラリのバージョン情報を出力します。

#### 書式
```
bool GetAttrVersionInfos(int fd = null)
```

#### 引数
- fd  
  必要であれば、出力用のStreamを指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.GetAttrVersionInfos();

k2h.Close();
```

### <a name="K2HASH-GETATTRINFOS"> k2hash::GetAttrInfos()
組み込み属性を含めた使用されている属性ライブラリの情報を出力します。

#### 書式
```
bool GetAttrInfos(int fd = null)
```

#### 引数
- fd  
  必要であれば、出力用のStreamを指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.GetAttrInfos();

k2h.Close();
```

### <a name="K2HASH-GETATTRS"> k2hash::GetAttrs()
キーに設定されている属性名のリストを取得します。

#### 書式
```
Array GetAttrs(String   key,
               Callback cbfunc = null
)
```

#### 引数
- key  
  キー名を指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error, Array attrs)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  var attrnames = k2h.GetAttrs('key');
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  
  k2h.GetAttrs('key', function(error, attrs){
      if(null == error){
          console_log(attrs);
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('getAttrs', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('getAttrs', function(error, attrs){
      if(null == error){
          console_log(attrs);
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.GetAttrs('key');
  ```
- onGetAttrs(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onGetAttrs(function(error, attrs){
      if(null == error){
          console_log(attrs);
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.GetAttrs('key');
  ```

### <a name="K2HASH-GETATTRVALUE"> k2hash::GetAttrValue()
キーに設定されている属性名の値を取得します。

#### 書式
```
Array GetAttrValue(String   key,
                   String   attrkey,
                   Callback cbfunc = null
)
```

#### 引数
- key  
  キー名を指定します。
- attrkey  
  属性名を指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error, String value)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  var attrnames = k2h.GetAttrValue('key', 'attr');
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  
  k2h.GetAttrValue('key', 'attr', function(error, value){
      if(null == error){
          console_log(value);
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('getAttrValue', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('getAttrValue', function(error, value){
      if(null == error){
          console_log(value);
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.GetAttrValue('key', 'attr');
  ```
- onGetAttrValue(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onGetAttrValue(function(error, value){
      if(null == error){
          console_log(value);
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.GetAttrValue('key', 'attr');
  ```

### <a name="K2HASH-ADDATTR"> k2hash::AddAttr()
キーに設定されている属性名の値を設定します。

#### 書式
```
Array AddAttr(String   key,
              String   attrkey,
              String   attrval = null,
              Callback cbfunc = null
)
```

#### 引数
- key  
  キー名を指定します。
- attrkey  
  属性名を指定します。
- attrval  
  設定する属性の値を指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.AddAttr('key', 'attr', 'val');
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  
  k2h.AddAttr('key', 'attr', 'val', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('addAttr', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.on('addAttr', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.AddAttr('key', 'attr', 'val');
  ```
- onAddAttr(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.onAddAttr(function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2h.OpenMem();
  k2h.AddAttr('key', 'attr', 'val');
  ```

### <a name="K2HASH-GETQUEUE"> k2hash::GetQueue()
キュー操作のためのk2hqueueクラスオブジェクトを取得する。

#### 書式
```
object GetQueue(bool   is_fifo = true
              String prefix = null
)
```

#### 引数
- is_fifo  
  取得するk2hQueueオブジェクトのFIFO/LIFOを指定する。
- prefix  
  取得するk2hQueueオブジェクトが取り扱うキューのキーのプレフィックスを指定します。

#### 返り値
正常終了した場合には、**k2hqueueオブジェクト**を返します。  
失敗した場合には、**null**を返します。

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
var k2hq = k2h.GetQueue();

k2h.Close();
```

### <a name="K2HASH-GETKEYQUEUE"> k2hash::GetKeyQueue()
キーキュー操作のためのk2hkeyqueueクラスオブジェクトを取得する。

#### 書式
```
object GetKeyQueue(bool   is_fifo = true
                   String prefix = null
)
```

#### 引数
- is_fifo  
  取得するk2hkeyQueueオブジェクトのFIFO/LIFOを指定する。
- prefix  
  取得するk2hkeyQueueオブジェクトが取り扱うキューのキーのプレフィックスを指定します。

#### 返り値
正常終了した場合には、**k2hkeyqueueオブジェクト**を返します。  
失敗した場合には、**null**を返します。

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
var k2hkq = k2h.GetKeyQueue();

k2h.Close();
```

<!-- -----------------------------------------------------------　-->
***

## <a name="K2HQUEUECLASS"> k2hqueueクラス
K2HASH Node.js アドオンライブラリの提供するメインクラスのk2hashクラスオブジェクトから本クラスオブジェクトを取得することができます。  
このクラスにより、K2HASHファイルやメモリ上のキューのデータの操作を行うことができます。

クラスを生成するには以下のように指定します。
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  var k2hq = k2h.GetQueue();
  
  k2h.Close();
  ```

以下に、K2HQUEUEクラスのメソッドとその説明をします。

### <a name="K2HQUEUE-INIT"> k2hqueue::Init()
k2hqueueオブジェクトを初期化します。

#### 書式
```
bool Init(object k2hhash,
          bool   is_fifo = true,
          String prefix = null,
)
```

#### 引数
- k2hhash  
  本オブジェクトを取得したk2hashオブジェクトを指定します。
- is_fifo  
  キューのFIFO、LIFOを指定します。
- prefix  
  キューで利用するキーのプレフィックスを指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.Create('test.k2h');
var k2hq = k2h.GetQueue();

k2hq.Init(k2h, true, 'queue_prefix_');

k2h.Close();
```

### <a name="K2HQUEUE-PUSH"> k2hqueue::Push()
K2HASHのキューにデータをプッシュします。

#### 書式
```
bool Push(String    data,
          String    pass = "",
          int       expire = 0,
          Callback cbfunc = null
)
```

#### 引数
- data  
  キューに蓄積するデータを指定します。
- pass  
  キュー（キー）が暗号化されている場合に、その暗号化のパスフレーズを指定します。
- expire  
  キューに蓄積するデータの有効期限を指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.Push('data');
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.Push('data', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('push', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.on('push', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2hq.Push('data');
  ```
- onPush(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.onPush(function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2hq.Push('data');
  ```

### <a name="K2HQUEUE-COUNT"> k2hqueue::Count()
K2HASHのキューの蓄積されているデータ数を取得します。

#### 書式
```
int Count(Callback cbfunc = null)
```

#### 引数
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error, int count)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了の場合にはキューのデータ数を返します。失敗およびエラーの場合には、**-1**が返されます。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  var count = k2hq.Count();
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.Count(function(error, count){
      if(null == error){
          console_log(count);
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('count', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.on('count', function(error, count){
      if(null == error){
          console_log(count);
      }
      k2h.Close();
  });
  
  k2hq.Count();
  ```
- onCount(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.onCount(function(error, count){
      if(null == error){
          console_log(count);
      }
      k2h.Close();
  });
  
  k2hq.Count();
  ```

### <a name="K2HQUEUE-ISEMPTY"> k2hqueue::IsEmpty()
K2HASHのキューの蓄積されているデータの存在有無を確認します。

#### 書式
```
bool IsEmpty(void)
```

#### 引数
なし

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.Create('test.k2h');
var k2hq = k2h.GetQueue();
k2hq.Init(k2h, true, 'queue_prefix_');

k2hq.IsEmpty();

k2h.Close();
```

### <a name="K2HQUEUE-READ"> k2hqueue::Read()
K2HASHのキューの蓄積されているデータを取り出さず、ひとつ読み出します。  

#### 書式
```
[bool/String] Read(int      pos = 0,
                   String   pass ="",
                   Callback cbfunc = null
)
```

#### 引数
- pos  
  キューから読み出すデータの位置を指定します。  
  キューの先頭（FIFO、LIFOに依存した）を0として位置を指定します。  
  -1を指定した場合には、キューの最後のデータが読み出されます。
- pass  
  キュー（キー）が暗号化されている場合に、その暗号化のパスフレーズを指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error, String data)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了のときは読み出したデータを返します。失敗のときは**null**を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  var data = k2hq.Read(0);
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.Read(0, function(error, data){
      if(null == error){
          console_log(data);
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('read', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.on('read', function(error, data){
      if(null == error){
          console_log(data);
      }
      k2h.Close();
  });
  
  k2hq.Read(0);
  ```
- onRead(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.onRead(function(error, data){
      if(null == error){
          console_log(count);
      }
      k2h.Close();
  });
  
  k2hq.Read(0);
  ```

### <a name="K2HQUEUE-POP"> k2hqueue::Pop()
K2HASHのキューの蓄積されているデータをひとつ取り出ます。

#### 書式
```
[bool/String] Pop(String   pass ="",
                  Callback cbfunc = null
)
```

#### 引数
- pass  
  キュー（キー）が暗号化されている場合に、その暗号化のパスフレーズを指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error, String data)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了のときは取り出したデータを返します。失敗のときは、**null**を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  var data = k2hq.Pop();
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.Pop(function(error, data){
      if(null == error){
          console_log(data);
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('pop', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.on('pop', function(error, data){
      if(null == error){
          console_log(data);
      }
      k2h.Close();
  });
  
  k2hq.Pop();
  ```
- onPop(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.onPop(function(error, data){
      if(null == error){
          console_log(count);
      }
      k2h.Close();
  });
  
  k2hq.Pop();
  ```

### <a name="K2HQUEUE-REMOVE"> k2hqueue::Remove()
K2HASHのキューの蓄積されているデータを読み出さず、指定した数だけ削除します。

#### 書式
```
int Remove(int      count
           String   pass ="",
           Callback cbfunc = null
)
```

#### 引数
- count  
  削除するキューのデータ数を指定します。
- pass  
  キュー（キー）が暗号化されている場合に、その暗号化のパスフレーズを指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error, int remove_count)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了のときは削除したデータ数を返します。失敗したときは、**-1**を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  var remove_count = k2hq.Remove(5);
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.Remove(5, function(error, remove_count){
      if(null == error){
          console_log(remove_count);
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('remove', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.on('remove', function(error, remove_count){
      if(null == error){
          console_log(remove_count);
      }
      k2h.Close();
  });
  
  k2hq.Remove(5);
  ```
- onRemove(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.onRemove(function(error, remove_count){
      if(null == error){
          console_log(remove_count);
      }
      k2h.Close();
  });
  
  k2hq.Remove(5);
  ```

### <a name="K2HQUEUE-DUMP"> k2hqueue::Dump()
K2HASHのキューの蓄積されているデータを出力します。

#### 書式
```
int Dump(int fd = null)
```

#### 引数
- fd  
  必要であれば、出力用のStreamを指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.Create('test.k2h');
var k2hq = k2h.GetQueue();
k2hq.Init(k2h, true, 'queue_prefix_');

k2hq.Dump();

k2h.Close();
```

<!-- -----------------------------------------------------------　-->
***

## <a name="K2HKEYQUEUECLASS"> k2hkeyqueueクラス
K2HASH Node.js アドオンライブラリの提供するメインクラスのk2hashクラスオブジェクトから本クラスオブジェクトを取得することができます。  
このクラスにより、K2HASHファイルやメモリ上のKEYキューのデータの操作を行うことができます。

クラスを生成するには以下のように指定します。
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  var k2hkq = k2h.GetKeyQueue();
  
  k2h.Close();
  ```

以下に、K2HKEYQUEUEクラスのメソッドとその説明をします。

### <a name="K2HKEYQUEUE-INIT"> k2hkeyqueue::Init()
k2hkeyqueueオブジェクトを初期化します。

#### 書式
```
bool Init(object k2hhash,
          bool   is_fifo = true,
          String prefix = null,
)
```

#### 引数
- k2hhash  
  本オブジェクトを取得したk2hashオブジェクトを指定します。
- is_fifo  
  KEYキューのFIFO、LIFOを指定します。
- prefix  
  KEYキューで利用するキーのプレフィックスを指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.Create('test.k2h');
var k2hkq = k2h.GetKeyQueue();

k2hkq.Init(k2h, true, 'keyqueue_prefix_');

k2h.Close();
```

### <a name="K2HKEYQUEUE-PUSH"> k2hkeyqueue::Push()
K2HASHのKEYキューにデータをプッシュします。

#### 書式
```
bool Push(String    key,
          String    val,
          String    pass = "",
          int       expire = 0,
          Callback cbfunc = null
)
```

#### 引数
- key  
  KEYキューに蓄積するKEY名を指定します。
- val  
  KEYキューに蓄積するKEYのデータを指定します。
- pass  
  KEYキュー（キー）が暗号化されている場合に、その暗号化のパスフレーズを指定します。
- expire  
  KEYキューに蓄積するデータの有効期限を指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.Push('key, 'val');
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.Push('key, 'val', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('push', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.on('push', function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2hkq.Push('key, 'val');
  ```
- onPush(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.onPush(function(error){
      if(null !== error){
          console_log('error occurred');
      }
      k2h.Close();
  });
  
  k2hkq.Push('key, 'val');
  ```

### <a name="K2HKEYQUEUE-COUNT"> k2hkeyqueue::Count()
K2HASHのKEYキューの蓄積されているデータ数を取得します。

#### 書式
```
int Count(Callback cbfunc = null)
```

#### 引数
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error, int count)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了の場合にはキューのデータ数を返します。失敗およびエラーの場合には、**-1**が返されます。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  var count = k2hkq.Count();
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.Count(function(error, count){
      if(null == error){
          console_log(count);
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('count', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.on('count', function(error, count){
      if(null == error){
          console_log(count);
      }
      k2h.Close();
  });
  
  k2hkq.Count();
  ```
- onCount(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.onCount(function(error, count){
      if(null == error){
          console_log(count);
      }
      k2h.Close();
  });
  
  k2hkq.Count();
  ```

### <a name="K2HKEYQUEUE-ISEMPTY"> k2hkeyqueue::IsEmpty()
K2HASHのKEYキューの蓄積されているデータの存在有無を確認します。

#### 書式
```
bool IsEmpty(void)
```

#### 引数
なし

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.Create('test.k2h');
var k2hkq = k2h.GetKeyQueue();
k2hkq.Init(k2h, true, 'keyqueue_prefix_');

k2hkq.IsEmpty();

k2h.Close();
```

### <a name="K2HKEYQUEUE-READ"> k2hkeyqueue::Read()
K2HASHのKEYキューの蓄積されているデータを取り出さず、ひとつ読み出します。  

#### 書式
```
[bool/Array] Read(int      pos = 0,
                  String   pass ="",
                  Callback cbfunc = null
)
```

#### 引数
- pos  
  KEYキューから読み出すデータの位置を指定します。  
  KEYキューの先頭（FIFO、LIFOに依存した）を0として位置を指定します。  
  -1を指定した場合には、KEYキューの最後のデータが読み出されます。
- pass  
  KEYキュー（キー）が暗号化されている場合に、その暗号化のパスフレーズを指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error, Array data)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了のときは読み出したデータの配列を返します。失敗のときは、**null**を返します。  
返される配列は、["キー名", "値"]として設定されます。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  var data = k2hkq.Read(0);
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.Read(0, function(error, data){
      if(null == error){
          console_log(data);
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('read', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.on('read', function(error, data){
      if(null == error){
          console_log(data);
      }
      k2h.Close();
  });
  
  k2hkq.Read(0);
  ```
- onRead(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.onRead(function(error, data){
      if(null == error){
          console_log(count);
      }
      k2h.Close();
  });
  
  k2hkq.Read(0);
  ```

### <a name="K2HKEYQUEUE-POP"> k2hkeyqueue::Pop()
K2HASHのKEYキューの蓄積されているデータをひとつ取り出ます。

#### 書式
```
[bool/Array] Pop(String   pass ="",
                 Callback cbfunc = null
)
```

#### 引数
- pass  
  KEYキュー（キー）が暗号化されている場合に、その暗号化のパスフレーズを指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error, String data)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了のときは読み出したデータの配列を返します。失敗のときは、**null**を返します。  
返される配列は、["キー名", "値"]として設定されます。
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  var data = k2hkq.Pop();
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.Pop(function(error, data){
      if(null == error){
          console_log(data);
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('pop', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.on('pop', function(error, data){
      if(null == error){
          console_log(data);
      }
      k2h.Close();
  });
  
  k2hkq.Pop();
  ```
- onPop(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.onPop(function(error, data){
      if(null == error){
          console_log(count);
      }
      k2h.Close();
  });
  
  k2hkq.Pop();
  ```

### <a name="K2HKEYQUEUE-REMOVE"> k2hkeyqueue::Remove()
K2HASHのKEYキューの蓄積されているデータを読み出さず、指定した数だけ削除します。

#### 書式
```
int Remove(int      count
           String   pass ="",
           Callback cbfunc = null
)
```

#### 引数
- count  
  削除するKEYキューのデータ数を指定します。
- pass  
  KEYキュー（キー）が暗号化されている場合に、その暗号化のパスフレーズを指定します。
- cbfunc  
  本関数呼び出しを非同期で処理するとき、Callback関数を指定します。  
  Callback関数の書式は以下になります。  
  ```
  function(Error error, int remove_count)
  ```
  エラー発生時にはerrorはnull以外の値となります。

#### 返り値
正常終了のときは削除したデータ数を返します。失敗したときは、**-1**を返します。  
Callback関数が指定されている場合には、常に**true**が返されます。

#### サンプル
- 同期  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  var remove_count = k2hkq.Remove(5);
  
  k2h.Close();
  ```
- 非同期（Callback）  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.Remove(5, function(error, remove_count){
      if(null == error){
          console_log(remove_count);
      }
      k2h.Close();
  });
  ```

#### イベントハンドラ
Callback関数を指定せず、イベントハンドラを設定して非同期処理をすることができます。  
以下の2つの方法で非同期処理を記述できます。
- on('remove', Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.on('remove', function(error, remove_count){
      if(null == error){
          console_log(remove_count);
      }
      k2h.Close();
  });
  
  k2hkq.Remove(5);
  ```
- onRemove(Callback cbfunc)  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.onRemove(function(error, remove_count){
      if(null == error){
          console_log(remove_count);
      }
      k2h.Close();
  });
  
  k2hkq.Remove(5);
  ```

### <a name="K2HKEYQUEUE-DUMP"> k2hkeyqueue::Dump()
K2HASHのKEYキューの蓄積されているデータを出力します。

#### 書式
```
int Dump(int fd = null)
```

#### 引数
- fd  
  必要であれば、出力用のStreamを指定します。

#### 返り値
正常終了（**true**）もしくは失敗（**false**）を返します。  

#### サンプル
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.Create('test.k2h');
var k2hkq = k2h.GetKeyQueue();
k2hkq.Init(k2h, true, 'keyqueue_prefix_');

k2hkq.Dump();

k2h.Close();
```

