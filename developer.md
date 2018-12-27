---
layout: contents
language: en-us
title: Developer
short_desc: K2HASH Node.js - K2HASH addon library for Node.js
lang_opp_file: developerja.html
lang_opp_word: To Japanese
prev_url: build.html
prev_string: Build
top_url: index.html
top_string: TOP
next_url: environments.html
next_string: Environments
---

<!-- -----------------------------------------------------------　-->
# For developer

## [Common](#COMMON)
[Synchronous and Asynchronous](#SYNCHRONOUS)  
[About Classes](#ABOUTCLASS)

## [k2hash Class](#K2HASHCLASS)
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

## [k2hqueue Class](#K2HQUEUECLASS)
[k2hqueue::Init()](#K2HQUEUE-INIT)  
[k2hqueue::Push()](#K2HQUEUE-PUSH)  
[k2hqueue::Count()](#K2HQUEUE-COUNT)  
[k2hqueue::IsEmpty()](#K2HQUEUE-ISEMPTY)  
[k2hqueue::Read()](#K2HQUEUE-READ)  
[k2hqueue::Pop()](#K2HQUEUE-POP)  
[k2hqueue::Remove()](#K2HQUEUE-REMOVE)  
[k2hqueue::Dump()](#K2HQUEUE-DUMP)  

## [k2hkeyqueue Class](#K2HKEYQUEUECLASS)
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

## <a name="COMMON"> Common
### <a name="ASYNCHRONOUS"> Synchronous and Asynchronous
The methods of the classes provided by K2HASH Node.js addon library support synchronous and asynchronous processing by specifying the callback function.  
Methods that support asynchronous processing can accept the arguments of the callback function.  
In addition, these can specify event handlers as **on** or **onXXXXX** as asynchronous processing.  
Developers can perform asynchronous processing using callback functions or event handlers.  
Callback function arguments and event handlers are not specified, these methods act as synchronization processes.

### <a name="ABOUTCLASS"> About Classes
K2HASH Node.js addon library provides **k2hash** class.
The **k2hqueue** class and **k2hKeyqueue** class for manipulating the queue data provided by [K2HASH](https://k2hash.antpick.ax/) are also provided.
Objects of **k2hqueue** class and **k2hKeyqueue** class can be obtained from methods of **k2hash** class object using methods.

<!-- -----------------------------------------------------------　-->
***

## <a name="K2HASHCLASS"> k2hash Class
This is the main class provided by K2HASH Node.js addon library.
This class provides functions for manipulating K2HASH files and data in memory.

A sample to create this class object is shown below.
  ```
  var k2hobj = require('k2hash')();
  ```
  or
  ```
  var k2hobj = new require('k2hash')();
  ```

Below are the methods of the k2hash class and those explanation.

### <a name="K2HASH-CREATE"> k2hash::Create()
This method create and initialize a file of k2hash.

#### Format
```
bool Create(String   filename,
            int      maskbitcnt = 2,
            int      cmaskbitcnt = 4,
            int      maxelementcnt = 32,
            int      pagesize = 128,
            Callback cbfunc = null
)
```

#### Arguments
- filename  
  Specify a file path to k2hash file.
- maskbitcnt  
  Specify initial **mask** bit count(must over 2).  
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- cmaskbitcnt  
  Specify initial **collision mask** bit count(must over 4).  
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- maxelementcnt  
  Specify initial **element** count(should be over 32).  
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- pagesize  
  Specify **page size** which is data block size(must over 128)  
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  k2h.SetValue('key','val');
  
  console_log(k2h.GetValue('key'));
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method opens(attaches) a k2hash file.  
If the k2hash file is not existed, this method creates it.

#### Format
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

#### Arguments
- filename  
  Specify K2HASH file path for open.
- readonly  
  Specify true for read only, false is read/write mode.
- removefile  
  Specify true for removing K2HASH file automatically when there is no processes to attaching K2HASH file.
- fullmap  
  Specify true for full memory mapping. false for mapping only index area in K2HASH.
- maskbitcnt  
  Specify initial **mask** bit count(must over 2).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- cmaskbitcnt  
  Specify initial **collision mask** bit count(must over 4).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- maxelementcnt  
  Specify initial **element** count(should be over 32).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- pagesize  
  Specify **page size** which is data block size(must over 128).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Open('test.k2h');
  k2h.SetValue('key','val');
  
  console_log(k2h.GetValue('key'));
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method opens(attaches) a k2hash file for read/write mode.  

#### Format
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

#### Arguments
- filename  
  Specify K2HASH file path for open.
- fullmap  
  Specify true for full memory mapping. false for mapping only index area in K2HASH.
- maskbitcnt  
  Specify initial **mask** bit count(must over 2).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- cmaskbitcnt  
  Specify initial **collision mask** bit count(must over 4).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- maxelementcnt  
  Specify initial **element** count(should be over 32).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- pagesize  
  Specify **page size** which is data block size(must over 128).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenRW('test.k2h');
  k2h.SetValue('key','val');
  
  console_log(k2h.GetValue('key'));
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method opens(attaches) a k2hash file for read only mode.  

#### Format
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

#### Arguments
- filename  
  Specify K2HASH file path for open.
- fullmap  
  Specify true for full memory mapping. false for mapping only index area in K2HASH.
- maskbitcnt  
  Specify initial **mask** bit count(must over 2).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- cmaskbitcnt  
  Specify initial **collision mask** bit count(must over 4).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- maxelementcnt  
  Specify initial **element** count(should be over 32).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- pagesize  
  Specify **page size** which is data block size(must over 128).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenRO('test.k2h');
  
  console_log(k2h.GetValue('key'));
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method opens(attaches) K2HASH file which is like temporary file.  
The file is temporary, it means the file is removed automatically when close it.  

#### Format
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

#### Arguments
- filename  
  Specify K2HASH file path for open.
- fullmap  
  Specify true for full memory mapping. false for mapping only index area in K2HASH.
- maskbitcnt  
  Specify initial **mask** bit count(must over 2).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- cmaskbitcnt  
  Specify initial **collision mask** bit count(must over 4).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- maxelementcnt  
  Specify initial **element** count(should be over 32).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- pagesize  
  Specify **page size** which is data block size(must over 128).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenTempfile('test.k2h');
  k2h.SetValue('key','val');
  
  console_log(k2h.GetValue('key'));
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method opens(creates) K2HASH data in memory.

#### Format
```
bool OpenMem(	int      maskbitcnt = 2, 
                int      cmaskbitcnt = 4, 
                int      maxelementcnt = 32, 
                int      pagesize = 128,
                Callback cbfunc = null
)
```

#### Arguments
- maskbitcnt  
  Specify initial **mask** bit count(must over 2).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- cmaskbitcnt  
  Specify initial **collision mask** bit count(must over 4).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- maxelementcnt  
  Specify initial **element** count(should be over 32).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- pagesize  
  Specify **page size** which is data block size(must over 128).
  If opens existing K2HASH file, this value is ignored and uses this value which is set in K2HASH file.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.SetValue('key','val');
  
  console_log(k2h.GetValue('key'));
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method closes(detaches) the K2HASH file or data in memory.

#### Format
```
bool Close(Callback cbfunc = null)
```

#### Arguments
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method gets value for key.  
If the subkey is specified, returns the value for subkey.  
Then if the key does not have subkey, any value is returned.

#### Format
```
[bool/String] GetValue(  String   key,
                         String   subkey = "",
                         bool     attrcheck = true,
                         String   pass = "",
                         Callback cbfunc = null
)
```

#### Arguments
- key  
  Specify the key name.
- subkey  
  Specify the subkey name when you want to get value for it.
- attrcheck  
  If this parameter is true, enable to check attributes which is presented by builtin.
- pass  
  Specify optional passphrase if excrypting the key
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error, String value)
  ```
  If an error occurs, error is not null.

#### Return Values
- Synchronous  
  For synchronous processing, it returns the read **value**.  
  If an error occurs, **null** is returned.
- Asynchronous(Callback function)  
  When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.SetValue('key','val');
  
  var value = k2h.GetValue('key');
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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

#### Notes
This method can specify keys and subkeys at the same time.  
If you just read the value of the key, just specify the key name.  
If you specify the key and subkey at the same time, you can read the value only when the subkey is tied to the key. 
If the subkey is not tied to a key, this method can not read the value and returns an error.

### <a name="K2HASH-GETSUBKEYS"> k2hash::GetSubkeys()
This method returns subkey list which are tied to the key.

#### Format
```
[bool/Array] GetSubkeys(String   key,
                        Callback cbfunc = null
)
```

#### Arguments
- key  
  Specify the key name.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error, Array skeylist)
  ```
  If an error occurs, error is not null.

#### Return Values
- Synchronous  
  For success, returns the list of subkeys as **array**.  
  If an error occurs, **null** is returned.
- Asynchronous(Callback function)  
  When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.addSubkey('parentkey', 'subkey', 'subkeyval');
  
  var skeylist = k2h.GetSubkeys('parentkey');
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method sets a value to the key.

#### Format
```
bool SetValue(  String   key,
                String   value,
                String   subkey = null,
                String   pass = "",
                int      expire = 0,
                Callback cbfunc = null
)
```

#### Arguments
- key  
  Spesify the key name.
- value  
  Specify the value.
- subkey  
  Specify the subkey name, if you want to set the value to subkey.
- pass  
  Specify optional passphrase if excrypting the key.
- expire  
  Specify optional timeout seconds if you need.
  If 0 is specified, it means that the key(value) is never expired.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.SetValue('parentkey', 'subkey', 'val');
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method adds the subkey name into parent key's subkey list.

#### Format
```
bool AddSubkey( String   key,
                String   subkey,
                String   value = null,
                Callback cbfunc = null
)
```

#### Arguments
- key  
  Specify the key name.
- subkey  
  Specify added new subkey name.
- value  
  Specify the value for subkey, if you want to set the value to subkey.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.AddSubkey('parentkey', 'subkey');
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method over writes parent key's subkey list.  
If the key has subkey list, the subkey list is removed and replaced by new subkey list.  
Take care for that the replaced old subkey list, the old subkey is not removed from K2HASH, it is removed only the key's subkey list.

#### Format
```
bool AddSubkeys(String   key,
                Array    subkeys,
                Callback cbfunc = null
)
```

#### Arguments
- key  
  Specify the key name.
- subkeys  
  Specify the subkey list.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.AddSubkeys('parentkey', ['subkey1', 'subkey2']);
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method removes the key or subkey.

#### Format
```
bool Remove(String   key,
            String   subkey,
            Callback cbfunc = null
)
```

#### Arguments
- key  
  Specify the key name.  
  If subkey argument is specified, the subkey is removed from the key's subkey list.
- subkey  
  Specify the subkey name.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.SetValue('parentkey', 'subkey', 'val');
  
  k2h.Remove('parentkey', 'subkey1');
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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

#### Notes
This method can specify keys and subkeys at the same time.  
If you specify only the key, that key will be removed.  
If you specify a key and subkey, the subkey is removed and the key is not removed.  
Then, the specified subkey is removed from the key subkey list.  
If the specified subkey does not exist in the subkey list of the key, this method returns an error.

### <a name="K2HASH-REMOVEALL"> k2hash::RemoveAll()
This method removes the key and key's all subkeys.  
If the key has subkeys, all of the subkeys is removed.

#### Format
```
bool RemoveAll(String   key,
               Callback cbfunc = null
)
```

#### Arguments
- key  
  Specify the key name.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.SetValue('parentkey', 'subkey', 'val');
  
  k2h.RemoveAll('parentkey');
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method puts the state of k2hash file or data in memory.

#### Format
```
bool PrintState(int fd = null)
```

#### Arguments
- fd  
  Specify output stream.

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.PrintState();

k2h.Close();
```

### <a name="K2HASH-PRINTVERSION"> k2hash::PrintVersion()
This method puts the version of k2hash libraries.

#### Format
```
bool PrintVersion(int fd = null)
```

#### Arguments
- fd  
  Specify output stream.

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.PrintVersion();
```

### <a name="K2HASH-DUMPHEAD"> k2hash::DumpHead()
This method puts the result of dumpping head table of k2hash data.

#### Format
```
bool DumpHead(int fd = null)
```

#### Arguments
- fd  
  Specify output stream.

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.DumpHead();

k2h.Close();
```

### <a name="K2HASH-DUMPKEYTABLE"> k2hash::DumpKeytable()
This method puts the result of dumpping key tables of k2hash data.

#### Format
```
bool DumpKeytable(int fd = null)
```

#### Arguments
- fd  
  Specify output stream.

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.DumpKeytable();

k2h.Close();
```

### <a name="K2HASH-DUMPFULLKEYTABLE"> k2hash::DumpFullKeytable()
This method puts the result of full dumpping key tables of k2hash data.

#### Format
```
bool DumpFullKeytable(int fd = null)
```

#### Arguments
- fd  
  Specify output stream.

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.DumpFullKeytable();

k2h.Close();
```

### <a name="K2HASH-DUMPELEMENTTABLE"> k2hash::DumpElementtable()
This method puts the result of dumpping all elements data of k2hash data.

#### Format
```
bool DumpElementtable(int fd = null)
```

#### Arguments
- fd  
  Specify output stream.

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.DumpElementtable();

k2h.Close();
```

### <a name="K2HASH-DUMPFULL"> k2hash::DumpFull()
This method puts the result of dumpping all data of k2hash data.

#### Format
```
bool DumpFull(int fd = null)
```

#### Arguments
- fd  
  Specify output stream.

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.DumpFull();

k2h.Close();
```

### <a name="K2HASH-TRANSACTION"> k2hash::Transaction()
This method enables/disables transaction for k2hash.

#### Format
```
bool Transaction(bool   enable,
                 String transfile = "",
                 String prefix = "",
                 String param = "",
                 int    expire = 0
)
```

#### Arguments
- enable  
  If true is specified, the transaction is enabled. false for desabled.
- transfile  
  Specify the transaction file path.
- prefix  
  Specify the custom transaction queue key name for prefix.
- param  
  Specify the custom transaction parameter.
- expire  
  Specify optional timeout seconds for transaction data.

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.Transaction(true, 'my.tr', 'tr_prefix_', '', 10);

k2h.Close();
```

### <a name="K2HASH-ENABLETRANSACTION"> k2hash::EnableTransaction()
This method enables transaction.

#### Format
```
bool EnableTransaction(String transfile = "",
                       String prefix = "",
                       String param = "",
                       int    expire = 0
)
```

#### Arguments
- transfile  
  Specify the transaction file path.
- prefix  
  Specify the custom transaction queue key name for prefix.
- param  
  Specify the custom transaction parameter.
- expire  
  Specify optional timeout seconds for transaction data.

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.EnableTransaction('my.tr', 'tr_prefix_', '', 10);

k2h.Close();
```

### <a name="K2HASH-DISABLETRANSACTION"> k2hash::DisableTransaction()
This method disables transaction.

#### Format
```
bool DisableTransaction(void)
```

#### Arguments
n/a

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.DisableTransaction();

k2h.Close();
```

### <a name="K2HASH-UNSETTRANSACTIONPOOL"> k2hash::UnsetTransactionThreadPool()
This method clears transaction thread pool.

#### Format
```
bool UnsetTransactionThreadPool(void)
```

#### Arguments
n/a

#### Return Values
This method returns success(**true**) or failure(**false**).  
If transactions are enabled, this method will fail.

#### Examples
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
This method returns transaction thread pool count.

#### Format
```
int GetTransactionThreadPool(void)
```

#### Arguments
n/a

#### Return Values
The count of transaction thread pool.

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
var pools = k2h.GetTransactionThreadPool();

k2h.Close();
```

### <a name="K2HASH-SETTRANSACTIONPOOL"> k2hash::SetTransactionThreadPool()
This method sets the thransaction thread count.

#### Format
```
bool SetTransactionThreadPool(int count)
```

#### Arguments
- count  
  Specify the count of thransaction thread.

#### Return Values
This method returns success(**true**) or failure(**false**).  
If transactions are enabled, this method will fail.

#### Examples
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
This method loads K2HASH data from archive file.  
If there is same key name in archive file, the key is over wrote.  
The archive file is made by PutArchive() function.

#### Format
```
bool LoadArchive(String   filename,
                 bool     errskip = true,
                 Callback cbfunc = null
)
```

#### Arguments
- filename  
  Specify the archive file path.
- errskip  
  If true is specified, this function does not stop when something error is occured in loading the archive file.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.LoadArchive('k2hash.ar', false);
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method puts(serializes) all of K2HASH data to file.  
The archive file which is made by this method can be loaded by LoadArchive()

#### Format
```
bool PutArchive(String   filename,
                 bool     errskip = true,
                 Callback cbfunc = null
)
```

#### Arguments
- filename  
  Specify the archive file path.
- errskip  
  If true is specified, this function does not stop when something error is occured in putting the archive file.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.PutArchive('k2hash.ar', false);
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method sets builtin attributes for K2HASH.  
The bultin attributes are mtime/history/expire/encrypt(and pass phrease)

#### Format
```
bool SetCommonAttribute(int    is_mtime = -1, 
                        int    is_history = -1, 
                        int    is_encrypt = -1, 
                        String passfile = "", 
                        int    is_expire = -1, 
                        int    expire = 0
)
```

#### Arguments
- is_mtime  
  Specify flag for mtime, enable(1), disable(0), default(-1).
- is_history  
  Specify flag for history, enable(1), disable(0), default(-1).
- is_encrypt  
  Specify flag for encrypting, enable(1), disable(0), default(-1).
- passfile  
  Specify pass phrase string.
- is_expire  
  Specify flag for expire, enable(1), disable(0), default(-1).
- expire  
  Specify expire seconds.

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.setCommonAttribute(1, 1, 0, 'mypassphrase', 1, 100);

k2h.Close();
```

### <a name="K2HASH-CLEANCOMMONATTR"> k2hash::CleanCommonAttribute()
This method clears bultin attribute setting(to default).

#### Format
```
bool SetCommonAttribute(void)
```

#### Arguments
n/a

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.cleanCommonAttribute();

k2h.Close();
```

### <a name="K2HASH-ADDATTRPLUGINLIB"> k2hash::AddAttrPluginLib()
This method loads custom attribute lugin(library).

#### Format
```
bool AddAttrPluginLib(String libfile)
```

#### Arguments
- libfile  
  Specify the custom attribute library path.

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.addAttrPluginLib('myattrlib.so');

k2h.Close();
```

### <a name="K2HASH-ADDATTRCRYPTPASS"> k2hash::AddAttrCryptPass()
This method sets pass phrase for builtin attribute encrypting.

#### Format
```
bool AddAttrCryptPass(String encpass,
                      bool   is_default_encrypt = false
)
```

#### Arguments
- encpass  
  Specify optional passphrase if excrypting the key.
- is_default_encrypt  
  true is enabled default encrypting..

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.k2h.addAttrCryptPass('passphrase', true);

k2h.Close();
```

#### Notes
Set pass phrase and enable/disable the default encrypting.  
If the default encrypting is disabled with pass phrase, it means that do only decrypting.

### <a name="K2HASH-GETATTRVERINFO"> k2hash::GetAttrVersionInfos()
This method puts Attribute plugin version(with builtin attribute).

#### Format
```
bool GetAttrVersionInfos(int fd = null)
```

#### Arguments
- fd  
  Specify the stream for output.

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.GetAttrVersionInfos();

k2h.Close();
```

### <a name="K2HASH-GETATTRINFOS"> k2hash::GetAttrInfos()
This method puts builtin attribute information.

#### Format
```
bool GetAttrInfos(int fd = null)
```

#### Arguments
- fd  
  Specify the stream for output.

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
k2h.GetAttrInfos();

k2h.Close();
```

### <a name="K2HASH-GETATTRS"> k2hash::GetAttrs()
This method gets attribute keys.  
If the callback function is specified, or on callback handles for this,  
this method works asynchronization and calls callback function at finishing.

#### Format
```
Array GetAttrs(String   key,
               Callback cbfunc = null
)
```

#### Arguments
- key  
  Specify the key name.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error, Array attrs)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  var attrnames = k2h.GetAttrs('key');
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method gets value by attribute key.  
If the callback function is specified, or on callback handles for this, this method works asynchronization and calls callback function at finishing.

#### Format
```
Array GetAttrValue(String   key,
                   String   attrkey,
                   Callback cbfunc = null
)
```

#### Arguments
- key  
  Specify the key name.
- attrkey  
  Specify the attribute key name.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error, String value)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  var attrnames = k2h.GetAttrValue('key', 'attr');
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method adds the attribute to the key.  
If the callback function is specified, or on callback handles for this, this method works asynchronization and calls callback function at finishing.

#### Format
```
Array AddAttr(String   key,
              String   attrkey,
              String   attrval = null,
              Callback cbfunc = null
)
```

#### Arguments
- key  
  Specify the key name.
- attrkey  
  Specify the attribute key name.
- attrval  
  Specify the attribute value.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  k2h.AddAttr('key', 'attr', 'val');
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
Asynchronous processing can be described by the following two implementations.
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
This method returns k2hqueue class object which accesses this object.  
You can specify the type of queue(FIFO/LIFO) and prefix of queue key name.

#### Format
```
object GetQueue(bool   is_fifo = true
              String prefix = null
)
```

#### Arguments
- is_fifo  
  Specify true for FIFO type queue, false for LIFO.
- prefix  
  Specify prefix for the queue key name.

#### Return Values
If success, **k2hqueue class object** is returned.  
If an error occurs, **null** is returned.

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
var k2hq = k2h.GetQueue();

k2h.Close();
```

### <a name="K2HASH-GETKEYQUEUE"> k2hash::GetKeyQueue()
This method returns k2hkeyqueue class object which accesses this object.  
You can specify the type of queue(FIFO/LIFO) and prefix of queue key name.

#### Format
```
object GetKeyQueue(bool   is_fifo = true
                   String prefix = null
)
```

#### Arguments
- is_fifo  
  Specify true for FIFO type queue, false for LIFO.
- prefix  
  Specify prefix for the queue key name.

#### Return Values
If success, **k2hkeyqueue class object** is returned.  
If an error occurs, **null** is returned.

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.OpenMem();
var k2hkq = k2h.GetKeyQueue();

k2h.Close();
```

<!-- -----------------------------------------------------------　-->
***

## <a name="K2HQUEUECLASS"> k2hqueue Class
This class object can be obtained from the k2hash class object of the main class provided by K2HASH Node.js addon library.  
This class allows you to manipulate the queue data in K2HASH files or in memory.

To create this class, specify it as follows.
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  var k2hq = k2h.GetQueue();
  
  k2h.Close();
  ```

Below are the methods of the k2hqueue class and their explanation.

### <a name="K2HQUEUE-INIT"> k2hqueue::Init()
This method initializes k2hqueue object with k2hash object.

#### Format
```
bool Init(object k2hhash,
          bool   is_fifo = true,
          String prefix = null,
)
```

#### Arguments
- k2hhash  
  Specify target k2hash class object.
- is_fifo  
  Specify true for FIFO queue, false for LIFO.
- prefix  
  Specify prefix of queue key name.

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.Create('test.k2h');
var k2hq = k2h.GetQueue();

k2hq.Init(k2h, true, 'queue_prefix_');

k2h.Close();
```

### <a name="K2HQUEUE-PUSH"> k2hqueue::Push()
This method pushs data to queue.  

#### Format
```
bool Push(String    data,
          String    pass = "",
          int       expire = 0,
          Callback cbfunc = null
)
```

#### Arguments
- data  
  Specify the data which is queued.
- pass  
  Specify optional passphrase if excrypting the key.
- expire  
  Specify optional timeout seconds if you need.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  k2hq.Push('data');
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
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
This method gets queue count.

#### Format
```
int Count(Callback cbfunc = null)
```

#### Arguments
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error, int count)
  ```
  If an error occurs, error is not null.

#### Return Values
The count of queue is returned, if something error occurred, returns 0.
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  var count = k2hq.Count();
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
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
This method tests for the queue is empty.

#### Format
```
bool IsEmpty(void)
```

#### Arguments
なし

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
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
This method reads queue data at queue position.  
Not remove the data from queue after reading.  
Take care for pareformance, because k2hash library uses liner search for this function.

#### Format
```
[bool/String] Read(int      pos = 0,
                   String   pass ="",
                   Callback cbfunc = null
)
```

#### Arguments
- pos  
  Specify position of reading in queue.  
  If this value is not specified, the default position is 0.  
  If this value is specified -1, the position means lastest position.
- pass  
  Specify optional passphrase if the key is encrypted.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error, String data)
  ```
  If an error occurs, error is not null.

#### Return Values
Returns the data which are queued.  
If there is no data in queue or something error is occurred, returns **null**.  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  var data = k2hq.Read(0);
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
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
The method pops queue data and removes the data from queue after popping.

#### Format
```
[bool/String] Pop(String   pass ="",
                  Callback cbfunc = null
)
```

#### Arguments
- pass  
  Specify optional passphrase if the key is encrypted.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error, String data)
  ```
  If an error occurs, error is not null.

#### Return Values
Returns the data which are queued.  
If there is no data in queue or something error is occurred, returns **null**.  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  var data = k2hq.Pop();
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
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
This method removes count of queue datas, and not return any datas which are removed.

#### Format
```
int Remove(int      count
           String   pass ="",
           Callback cbfunc = null
)
```

#### Arguments
- count  
  Specify the count for removing..
- pass  
  Specify optional passphrase if the key is encrypted.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error, int remove_count)
  ```
  If an error occurs, error is not null.

#### Return Values
If success, returns removed queue count.  
If failure, returns -1.  
If the count is specified over queuing count, the queue is empty and returns removing count.  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hq = k2h.GetQueue();
  k2hq.Init(k2h, true, 'queue_prefix_');
  
  var remove_count = k2hq.Remove(5);
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
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
This method dumps all queue data.

#### Format
```
int Dump(int fd = null)
```

#### Arguments
- fd  
  Specify the stream for output.

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
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

## <a name="K2HKEYQUEUECLASS"> k2hkeyqueue Class
This class object can be obtained from the k2hash class object of the main class provided by K2HASH Node.js addon library.  
This class allows you to manipulate the key queue data in K2HASH files or in memory.

To create this class, specify it as follows.
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.OpenMem();
  var k2hkq = k2h.GetKeyQueue();
  
  k2h.Close();
  ```

Below are the methods of the k2hkeyqueue class and their explanation.

### <a name="K2HKEYQUEUE-INIT"> k2hkeyqueue::Init()
This method initializes k2hkeyqueue object with k2hash object.

#### Format
```
bool Init(object k2hhash,
          bool   is_fifo = true,
          String prefix = null,
)
```

#### Arguments
- k2hhash  
  Specify target k2hash class object.
- is_fifo  
  Specify true for FIFO queue, false for LIFO.
- prefix  
  Specify prefix of key queue's key name.

#### Return Values
This method returns success(**true**) or failure(**false**).

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.Create('test.k2h');
var k2hkq = k2h.GetKeyQueue();

k2hkq.Init(k2h, true, 'keyqueue_prefix_');

k2h.Close();
```

### <a name="K2HKEYQUEUE-PUSH"> k2hkeyqueue::Push()
This method pushs data to queue.  

#### Format
```
bool Push(String    key,
          String    val,
          String    pass = "",
          int       expire = 0,
          Callback cbfunc = null
)
```

#### Arguments
- key  
  Specify the key which is pushed to queue.
- val  
  Specify the value which is set key.
- pass  
  Specify optional passphrase if excrypting the key.
- expire  
  Specify optional timeout seconds if you need.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error)
  ```
  If an error occurs, error is not null.

#### Return Values
This method returns success(**true**) or failure(**false**).  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  k2hkq.Push('key, 'val');
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
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
This method gets queue count.

#### Format
```
int Count(Callback cbfunc = null)
```

#### Arguments
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error, int count)
  ```
  If an error occurs, error is not null.

#### Return Values
The count of queue is returned, if something error occurred, returns 0.
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  var count = k2hkq.Count();
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
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
This method tests for the queue is empty.

#### Format
```
bool IsEmpty(void)
```

#### Arguments
なし

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
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
This method reads queue data at queue position.  
Not remove the data from queue after reading.  
Take care for pareformance, because k2hash library uses liner search for this function.

#### Format
```
[bool/Array] Read(int      pos = 0,
                  String   pass ="",
                  Callback cbfunc = null
)
```

#### Arguments
- pos  
  Specify position of reading in queue.  
  If this value is not specified, the default position is 0.  
  If this value is specified -1, the position means lastest position.
- pass  
  Specify optional passphrase if the key is encrypted.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error, Array data)
  ```
  If an error occurs, error is not null.

#### Return Values
Returns the array for key and value which are queued. 
If there is no data in queue or something error is occurred, returns **null**.  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  var data = k2hkq.Read(0);
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
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
The method pops queue data and removes the data from queue after popping.

#### Format
```
[bool/Array] Pop(String   pass ="",
                 Callback cbfunc = null
)
```

#### Arguments
- pass  
  Specify optional passphrase if the key is encrypted.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error, String data)
  ```
  If an error occurs, error is not null.

#### Return Values
Returns the array for key and value which are queued. 
If there is no data in queue or something error is occurred, returns **null**.  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  var data = k2hkq.Pop();
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
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
This method removes count of queue datas, and not return any datas which are removed.

#### Format
```
int Remove(int      count
           String   pass ="",
           Callback cbfunc = null
)
```

#### Arguments
- count  
  Specify the count for removing..
- pass  
  Specify optional passphrase if the key is encrypted.
- cbfunc  
  When using this method as asynchronous, the following prototype callback function can be specified.  
  ```
  function(Error error, int remove_count)
  ```
  If an error occurs, error is not null.

#### Return Values
If success, returns removed queue count.  
If failure, returns -1.  
If the count is specified over queuing count, the queue is empty and returns removing count.  
When callback function is specified, **true** value is always returned.

#### Examples
- Synchronous  
  ```
  var k2hash = require('bindings')('k2hash');
  var k2h = k2hash();
  
  k2h.Create('test.k2h');
  var k2hkq = k2h.GetKeyQueue();
  k2hkq.Init(k2h, true, 'keyqueue_prefix_');
  
  var remove_count = k2hkq.Remove(5);
  
  k2h.Close();
  ```
- Asynchronous(Callback function)  
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

#### Event handlers
You can use asynchronous processing by implementing an event handler without using callback function.  
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
This method dumps all queue data.

#### Format
```
int Dump(int fd = null)
```

#### Arguments
- fd  
  Specify the stream for output.

#### Return Values
This method returns success(**true**) or failure(**false**).  

#### Examples
```
var k2hash = require('bindings')('k2hash');
var k2h = k2hash();

k2h.Create('test.k2h');
var k2hkq = k2h.GetKeyQueue();
k2hkq.Init(k2h, true, 'keyqueue_prefix_');

k2hkq.Dump();

k2h.Close();
```

