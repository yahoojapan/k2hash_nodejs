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

#ifndef K2H_KEYQUEUE_AYNC_H
#define K2H_KEYQUEUE_AYNC_H

#include "k2h_common.h"

//
// AsyncWorker classes for using K2hKeyQueue
//

//---------------------------------------------------------
// K2hkqPushWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HKeyQueue* pobj, const char* pkey, const char* pval, const char* ppass, const time_t* pexpire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class K2hkqPushAsyncWorker : public Napi::AsyncWorker
{
	public:
		K2hkqPushAsyncWorker(const Napi::Function& callback, K2HKeyQueue* pobj, const char* pkey, const char* pval, const char* ppass, const time_t* pexpire) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _pk2hkqobj(pobj),
			_is_key_set(NULL != pkey), _strkey(pkey ? pkey : ""), _is_val_set(NULL != pval), _strval(pval ? pval : ""), _is_pass_set(NULL != ppass), _strpass(ppass ? ppass : ""), _expire(pexpire ? *pexpire : 0)
		{
			_callbackRef.Ref();
		}

		~K2hkqPushAsyncWorker()
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Runs on worker thread
		void Execute() override
		{
			if(!_pk2hkqobj){
				SetError("No object is associated to async worker");
				return;
			}
			if(!_is_key_set || !_is_val_set){
				SetError("Specified key or value is empty(null)");
				return;
			}

			// push
			if(!_pk2hkqobj->Push(reinterpret_cast<const unsigned char*>(_strkey.c_str()), (_strkey.length() + 1), reinterpret_cast<const unsigned char*>(_strval.c_str()), (_strval.length() + 1), (_is_pass_set ? _strpass.c_str() : NULL), (_expire > 0 ? &_expire : NULL))){
				SetError("Failed to push data for k2hkeyqueue.");
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
		K2HKeyQueue*			_pk2hkqobj;
		bool					_is_key_set;
		std::string				_strkey;
		bool					_is_val_set;
		std::string				_strval;
		bool					_is_pass_set;
		std::string				_strpass;
		time_t					_expire;
};

//---------------------------------------------------------
// K2hkqCountWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HKeyQueue* pobj)
// Callback function:	function(string error[, int count])
//
//---------------------------------------------------------
class K2hkqCountAsyncWorker : public Napi::AsyncWorker
{
	public:
		K2hkqCountAsyncWorker(const Napi::Function& callback, K2HKeyQueue* pobj) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _pk2hkqobj(pobj), _rescount(0)
		{
			_callbackRef.Ref();
		}

		~K2hkqCountAsyncWorker()
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Runs on worker thread
		void Execute() override
		{
			if(!_pk2hkqobj){
				SetError("No object is associated to async worker");
				return;
			}

			// count
			_rescount = static_cast<int>(_pk2hkqobj->GetCount());
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
		K2HKeyQueue*			_pk2hkqobj;
		int						_rescount;
};

//---------------------------------------------------------
// K2hkqReadWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HKeyQueue* pobj, int pos, const char* ppass)
// Callback function:	function(string error[, string key, string value])
//
//---------------------------------------------------------
class K2hkqReadAsyncWorker : public Napi::AsyncWorker
{
	public:
		K2hkqReadAsyncWorker(const Napi::Function& callback, K2HKeyQueue* pobj, int pos, const char* ppass) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _pk2hkqobj(pobj), _getpos(pos), _is_pass_set(NULL != ppass), _strpass(ppass ? ppass : ""), _reskey(), _resval()
		{
			_callbackRef.Ref();
		}

		~K2hkqReadAsyncWorker()
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Runs on the worker thread
		void Execute() override
		{
			if(!_pk2hkqobj){
				SetError("No object is associated to async worker");
				return;
			}
			if(0 > _getpos){
				SetError("Specified position is under zero.");
				return;
			}

			unsigned char*	pkey	= NULL;
			unsigned char*	pval	= NULL;
			size_t			keylen	= 0;
			size_t			vallen	= 0;
			bool			result	= _pk2hkqobj->Read(&pkey, keylen, &pval, vallen, _getpos, (_is_pass_set ? _strpass.c_str() : NULL));
			if(!result || !pkey || 0 == keylen){
				SetError("Failed to read data for K2HKeyQueue.");
			}else{
				_reskey.assign(reinterpret_cast<const char*>(pkey), static_cast<size_t>(strlen(reinterpret_cast<const char*>(pkey))));
				if(pval && 0 < vallen){
					_resval.assign(reinterpret_cast<const char*>(pval), static_cast<size_t>(strlen(reinterpret_cast<const char*>(pval))));
				}
			}
			K2H_Free(pkey);
			K2H_Free(pval);
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(_callbackRef && _callbackRef.Value().IsFunction()){
				_callbackRef.Call({ env.Null(), Napi::String::New(env, _reskey.c_str(), static_cast<size_t>(strlen(_reskey.c_str()))), Napi::String::New(env, _resval.c_str(), static_cast<size_t>(strlen(_resval.c_str()))) });
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
		K2HKeyQueue*			_pk2hkqobj;
		int						_getpos;
		bool					_is_pass_set;
		std::string				_strpass;
		std::string				_reskey;
		std::string				_resval;
};

//---------------------------------------------------------
// K2hkqPopWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HKeyQueue* pobj, const char* ppass)
// Callback function:	function(string error[, string key, string value])
//
//---------------------------------------------------------
class K2hkqPopAsyncWorker : public Napi::AsyncWorker
{
	public:
		K2hkqPopAsyncWorker(const Napi::Function& callback, K2HKeyQueue* pobj, const char* ppass) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _pk2hkqobj(pobj), _is_pass_set(NULL != ppass), _strpass(ppass ? ppass : ""), _reskey(), _resval()
		{
			_callbackRef.Ref();
		}

		~K2hkqPopAsyncWorker()
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Runs on the worker thread
		void Execute() override
		{
			if(!_pk2hkqobj){
				SetError("No object is associated to async worker");
				return;
			}

			unsigned char*	pkey	= NULL;
			unsigned char*	pval	= NULL;
			size_t			keylen	= 0;
			size_t			vallen	= 0;
			bool			result	= _pk2hkqobj->Pop(&pkey, keylen, &pval, vallen, (_is_pass_set ? _strpass.c_str() : NULL));
			if(!result || !pkey || 0 == keylen){
				SetError("Failed to pop data for K2HKeyQueue.");
			}else{
				_reskey.assign(reinterpret_cast<const char*>(pkey), static_cast<size_t>(strlen(reinterpret_cast<const char*>(pkey))));
				if(pval && 0 < vallen){
					_resval.assign(reinterpret_cast<const char*>(pval), static_cast<size_t>(strlen(reinterpret_cast<const char*>(pval))));
				}
			}
			K2H_Free(pkey);
			K2H_Free(pval);
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(_callbackRef && _callbackRef.Value().IsFunction()){
				_callbackRef.Call({ env.Null(), Napi::String::New(env, _reskey.c_str(), static_cast<size_t>(strlen(_reskey.c_str()))), Napi::String::New(env, _resval.c_str(), static_cast<size_t>(strlen(_resval.c_str()))) });
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
		K2HKeyQueue*			_pk2hkqobj;
		bool					_is_pass_set;
		std::string				_strpass;
		std::string				_reskey;
		std::string				_resval;
};

//---------------------------------------------------------
// K2hkqRemoveWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HKeyQueue* pobj, int count, const char* ppass)
// Callback function:	function(string error[, int removed_queue_count])
//
//---------------------------------------------------------
class K2hkqRemoveAsyncWorker : public Napi::AsyncWorker
{
	public:
		K2hkqRemoveAsyncWorker(const Napi::Function& callback, K2HKeyQueue* pobj, int count, const char* ppass) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _pk2hkqobj(pobj), _rmcount(count), _is_pass_set(NULL != ppass), _strpass(ppass ? ppass : ""), _removed_count(0)
		{
			_callbackRef.Ref();
		}

		~K2hkqRemoveAsyncWorker()
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Runs on worker thread
		void Execute() override
		{
			if(!_pk2hkqobj){
				SetError("No object is associated to async worker");
				return;
			}
			if(_rmcount <= 0){
				SetError("Specified remove count is under 1.");
				return;
			}

			// remove
			_removed_count = _pk2hkqobj->Remove(_rmcount, NULL, NULL, (_is_pass_set ? _strpass.c_str() : NULL));
			if(-1 == _removed_count){
				SetError("Failed to remove queue for K2HKeyQueue.");
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
		K2HKeyQueue*			_pk2hkqobj;
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
