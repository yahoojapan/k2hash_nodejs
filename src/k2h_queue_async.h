/*
 * K2HASH
 *
 * Copyright 2015 Yahoo Japan Corporation.
 *
 * K2HASH is key-valuew store base libraries.
 * K2HASH is made for the purpose of the construction of
 * original KVS system and the offer of the library.
 * The characteristic is this KVS library which Key can
 * layer. And can support multi-processing and multi-thread,
 * and is provided safely as available KVS.
 *
 * For the full copyright and license information, please view
 * the license file that was distributed with this source code.
 *
 * AUTHOR:   Takeshi Nakatani
 * CREATE:   Tue Mar 7 2017
 * REVISION:
 */

#ifndef K2H_QUEUE_AYNC_H
#define K2H_QUEUE_AYNC_H

#include "k2h_common.h"

//
// AsyncWorker classes for using K2hQueue
//

//---------------------------------------------------------
// K2hqPushWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HQueue* pobj, const char* pdata, const char* ppass, const time_t* pexpire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class K2hqPushAsyncWorker : public Napi::AsyncWorker
{
	public:
		K2hqPushAsyncWorker(const Napi::Function& callback, K2HQueue* pobj, const char* pdata, const char* ppass, const time_t* pexpire) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _pk2hqobj(pobj), _is_data_set(nullptr != pdata), _strdata(pdata ? pdata : ""), _is_pass_set(nullptr != ppass), _strpass(ppass ? ppass : ""), _expire(pexpire ? *pexpire : 0)
		{
			_callbackRef.Ref();
		}

		~K2hqPushAsyncWorker()
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Runs on worker thread
		void Execute() override
		{
			if(!_pk2hqobj){
				SetError("No object is associated to async worker");
				return;
			}
			if(!_is_data_set){
				SetError("Specified data is empty(null)");
				return;
			}

			// push
			if(!_pk2hqobj->Push(reinterpret_cast<const unsigned char*>(_strdata.c_str()), (_strdata.length() + 1), NULL, (_is_pass_set ? _strpass.c_str() : NULL), (_expire > 0 ? &_expire : NULL))){
				SetError("Failed to push data for k2hqueue.");
				return;
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(_callbackRef && _callbackRef.Value().IsFunction()){
				_callbackRef.Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(_callbackRef && _callbackRef.Value().IsFunction()){
				_callbackRef.Call({ Napi::String::New(env, err.Message()) });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HQueue*				_pk2hqobj;
		bool					_is_data_set;
		std::string				_strdata;
		bool					_is_pass_set;
		std::string				_strpass;
		time_t					_expire;
};

//---------------------------------------------------------
// K2hqCountWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HQueue* pobj)
// Callback function:	function(string error, int count)
//
//---------------------------------------------------------
class K2hqCountAsyncWorker : public Napi::AsyncWorker
{
	public:
		K2hqCountAsyncWorker(const Napi::Function& callback, K2HQueue* pobj) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _pk2hqobj(pobj), _rescount(0)
		{
			_callbackRef.Ref();
		}

		~K2hqCountAsyncWorker()
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Runs on worker thread
		void Execute() override
		{
			if(NULL == _pk2hqobj){
				// Report an error so OnError will be invoked.
				SetError("No object is associated to async worker");
				return;
			}

			// count
			_rescount = static_cast<int>(_pk2hqobj->GetCount());
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(_callbackRef && _callbackRef.Value().IsFunction()){
				_callbackRef.Call({ env.Null(), Napi::Number::New(env, _rescount) });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(_callbackRef && _callbackRef.Value().IsFunction()){
				_callbackRef.Call({ Napi::String::New(env, err.Message()) });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HQueue*				_pk2hqobj;
		int						_rescount;
};

//---------------------------------------------------------
// K2hqReadWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HQueue* pobj, int pos, const char* ppass)
// Callback function:	function(string error[, string data])
//
//---------------------------------------------------------
class K2hqReadAsyncWorker : public Napi::AsyncWorker
{
	public:
		K2hqReadAsyncWorker(const Napi::Function& callback, K2HQueue* pobj, int pos, const char* ppass) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _pk2hqobj(pobj), _getpos(pos), _is_pass_set(NULL != ppass), _strpass(ppass ? ppass : ""), _resdata()
		{
			_callbackRef.Ref();
		}

		~K2hqReadAsyncWorker()
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Runs on worker thread
		void Execute() override
		{
			if(NULL == _pk2hqobj){
				SetError("No object is associated to async worker");
				return;
			}
			if(_getpos < 0){
				SetError("Specified position is under zero.");
				return;
			}

			// read
			unsigned char*	pdata	= NULL;
			size_t			datalen	= 0;
			bool			result	= _pk2hqobj->Read(&pdata, datalen, _getpos, (_is_pass_set ? _strpass.c_str() : NULL));
			if(!result || !pdata || 0 == datalen){
				SetError("Failed to read data for k2hqueue.");
			}else{
				_resdata.assign(reinterpret_cast<const char*>(pdata), static_cast<size_t>(strlen(reinterpret_cast<const char*>(pdata))));
			}
			K2H_Free(pdata);
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(_callbackRef && _callbackRef.Value().IsFunction()){
				Napi::String	val = Napi::String::New(env, _resdata.c_str(), static_cast<size_t>(strlen(_resdata.c_str())));
				_callbackRef.Call({ env.Null(), val });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(_callbackRef && _callbackRef.Value().IsFunction()){
				_callbackRef.Call({ Napi::String::New(env, err.Message()) });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HQueue*				_pk2hqobj;
		int						_getpos;
		bool					_is_pass_set;
		std::string				_strpass;
		std::string				_resdata;
};

//---------------------------------------------------------
// K2hqPopWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HQueue* pobj, const char* ppass)
// Callback function:	function(string error[, string data])
//
//---------------------------------------------------------
class K2hqPopAsyncWorker : public Napi::AsyncWorker
{
	public:
		K2hqPopAsyncWorker(const Napi::Function& callback, K2HQueue* pobj, const char* ppass) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _pk2hqobj(pobj), _is_pass_set(NULL != ppass), _strpass(ppass ? ppass : ""), _resdata()
		{
			_callbackRef.Ref();
		}

		~K2hqPopAsyncWorker()
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Runs on worker thread
		void Execute() override
		{
			if(!_pk2hqobj){
				SetError("No object is associated to async worker");
				return;
			}

			// pop
			unsigned char*	pdata	= NULL;
			size_t			datalen	= 0;
			bool			result	= _pk2hqobj->Pop(&pdata, datalen, NULL, (_is_pass_set ? _strpass.c_str() : NULL));
			if(!result || !pdata || 0 == datalen){
				SetError("Failed to pop data for k2hqueue.");
			}else{
				_resdata.assign(reinterpret_cast<const char*>(pdata), static_cast<size_t>(strlen(reinterpret_cast<const char*>(pdata))));
			}
			K2H_Free(pdata);
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(_callbackRef && _callbackRef.Value().IsFunction()){
				Napi::String	val = Napi::String::New(env, _resdata.c_str(), static_cast<size_t>(strlen(_resdata.c_str())));
				_callbackRef.Call({ env.Null(), val });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(_callbackRef && _callbackRef.Value().IsFunction()){
				_callbackRef.Call({ Napi::String::New(env, err.Message()) });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HQueue*				_pk2hqobj;
		bool					_is_pass_set;
		std::string				_strpass;
		std::string				_resdata;
};

//---------------------------------------------------------
// K2hqRemoveWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HQueue* pobj, int count, const char* ppass)
// Callback function:	function(string error[, int removed_queue_count])
//
//---------------------------------------------------------
class K2hqRemoveAsyncWorker : public Napi::AsyncWorker
{
	public:
		K2hqRemoveAsyncWorker(const Napi::Function& callback, K2HQueue* pobj, int count, const char* ppass) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _pk2hqobj(pobj), _rmcount(count), _is_pass_set(NULL != ppass), _strpass(ppass ? ppass : ""), _removed_count(0)
		{
			_callbackRef.Ref();
		}

		~K2hqRemoveAsyncWorker()
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Runs on worker thread
		void Execute() override
		{
			if(!_pk2hqobj){
				SetError("No object is associated to async worker");
				return;
			}
			if(_rmcount <= 0){
				SetError("Specified remove count is under 1.");
				return;
			}

			// remove
			_removed_count = _pk2hqobj->Remove(_rmcount, NULL, NULL, (_is_pass_set ? _strpass.c_str() : NULL));
			if(-1 == _removed_count){
				SetError("Failed to remove queue for k2hqueue.");
				return;
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(_callbackRef && _callbackRef.Value().IsFunction()){
				_callbackRef.Call({ env.Null(), Napi::Number::New(env, static_cast<double>(_removed_count)) });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(_callbackRef && _callbackRef.Value().IsFunction()){
				_callbackRef.Call({ Napi::String::New(env, err.Message()) });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HQueue*				_pk2hqobj;
		int						_rmcount;
		bool					_is_pass_set;
		std::string				_strpass;
		int						_removed_count;
};

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
