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
 * CREATE:   Fri Mar 3 2017
 * REVISION:
 */

#ifndef K2H_SHM_AYNC_H
#define K2H_SHM_AYNC_H

#include <string.h>
#include "k2h_common.h"

//
// AsyncWorker classes for using K2hNode
//

//---------------------------------------------------------
// CreateWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* k2hfile, bool fullmapping, int mask, int cmask, int element_cnt, size_t page)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class CreateAsyncWorker : public Napi::AsyncWorker
{
	public:
		CreateAsyncWorker(const Napi::Function& callback, K2HShm* k2hshm, const std::string& filename, bool isfullmapping, int mask_bitcnt, int cmask_bitcnt, int max_element_cnt, size_t pagesize) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _k2hshm(k2hshm),
			_filename(filename), _isfullmapping(isfullmapping), _mask_bitcnt(mask_bitcnt), _cmask_bitcnt(cmask_bitcnt), _max_element_cnt(max_element_cnt), _pagesize(pagesize), _result(false)
		{
			_callbackRef.Ref();
		}

		~CreateAsyncWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_k2hshm){
				SetError("No object is associated to async worker");
				return;
			}
			if(!_k2hshm->Create(_filename.c_str(), _isfullmapping, _mask_bitcnt, _cmask_bitcnt, _max_element_cnt, _pagesize)){
				SetError(std::string("Failed to create K2HASH file: ") + _filename);	// call SetError method in Napi::AsyncWorker
				return;
			}
			_result = true;
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is null and the second argument is the result.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null(), Napi::Boolean::New(env, _result) });
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ err.Value() });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HShm*					_k2hshm;
		std::string				_filename;
		bool					_isfullmapping;
		int						_mask_bitcnt;
		int						_cmask_bitcnt;
		int						_max_element_cnt;
		size_t					_pagesize;
		bool					_result;
};

//---------------------------------------------------------
// OpenWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* k2hfile, bool readonly, bool create, bool tempfilemode, bool fullmapping, int mask, int cmask, int element_cnt, size_t page)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class OpenAsyncWorker : public Napi::AsyncWorker
{
	public:
		OpenAsyncWorker(const Napi::Function& callback, K2HShm* k2hshm, const std::string& filename, bool isReadOnly, bool isCreate, bool isTempFile, bool isfullmapping, int mask_bitcnt, int cmask_bitcnt, int max_element_cnt, size_t pagesize) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _k2hshm(k2hshm),
			_filename(filename), _isReadOnly(isReadOnly), _isCreate(isCreate), _isTempFile(isTempFile), _isfullmapping(isfullmapping), _mask_bitcnt(mask_bitcnt), _cmask_bitcnt(cmask_bitcnt), _max_element_cnt(max_element_cnt), _pagesize(pagesize), _result(false)
		{
			_callbackRef.Ref();
		}

		~OpenAsyncWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_k2hshm){
				SetError("No object is associated to async worker");
				return;
			}
			if(!_k2hshm->Attach(_filename.c_str(), _isReadOnly, _isCreate, _isTempFile, _isfullmapping, _mask_bitcnt, _cmask_bitcnt, _max_element_cnt, _pagesize)){
				SetError(std::string("Failed to attach(open) k2hash object."));		// call SetError method in Napi::AsyncWorker
				return;
			}
			_result = true;
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is null and the second argument is the result.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null(), Napi::Boolean::New(env, _result) });
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ err.Value() });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HShm*					_k2hshm;
		std::string				_filename;
		bool					_isReadOnly;
		bool					_isCreate;
		bool					_isTempFile;
		bool					_isfullmapping;
		int						_mask_bitcnt;
		int						_cmask_bitcnt;
		int						_max_element_cnt;
		size_t					_pagesize;
		bool					_result;
};

//---------------------------------------------------------
// CloseWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class CloseAsyncWorker : public Napi::AsyncWorker
{
	public:
		CloseAsyncWorker(const Napi::Function& callback, K2HShm* k2hshm) : Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _k2hshm(k2hshm)
		{
			_callbackRef.Ref();
		}

		~CloseAsyncWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_k2hshm){
				SetError("No object is associated to async worker");
				return;
			}
			if(!_k2hshm->Detach()){
				SetError(std::string("Failed to close k2hash object."));	// call SetError method in Napi::AsyncWorker
				return;
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is null and the second argument is the result.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null(), Napi::Boolean::New(env, true) });
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ err.Value() });
			}else{
				// Throw error
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HShm*					_k2hshm;
};

//---------------------------------------------------------
// GetValueWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* psubkey, bool attrchk, const char* ppass)
// Callback function:	function(string error[, string value])
//
//---------------------------------------------------------
class GetValueAsyncWorker : public Napi::AsyncWorker
{
	public:
		GetValueAsyncWorker(const Napi::Function& callback, K2HShm* k2hshm, const char* pkey, const char* psubkey, bool attrchk, const char* ppass) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _k2hshm(k2hshm),
			_is_key_set(pkey != nullptr), _strkey(pkey ? pkey : ""), _is_skey_set(psubkey != nullptr), _strsubkey(psubkey ? psubkey : ""), _is_attr_check(attrchk), _is_pass_set(ppass != nullptr), _strpass(ppass ? ppass : ""), _presult(nullptr)
		{
			_callbackRef.Ref();
		}

		~GetValueAsyncWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
			K2H_Free(_presult);
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_k2hshm){
				SetError("No object is associated to async worker");
				return;
			}
			if(!_is_key_set){
				SetError("Specified key is empty(null)");
				return;
			}

			// check subkey if specified
			if(_is_skey_set){
				// subkey is specified, thus need to check the key has it.
				bool		found	= false;
				K2HSubKeys*	sk		= _k2hshm->GetSubKeys(_strkey.c_str());
				if(sk){
					strarr_t	strarr;
					sk->StringArray(strarr);
					for(const auto &_str : strarr){
						if(_str == _strsubkey){
							found = true;
							break;
						}
					}
					delete sk;
				}
				if(!found){
					SetError("There is no specified subkey in key subkey list.");
					return;
				}
				_strkey = _strsubkey;
			}

			// get value
			_presult = _k2hshm->Get(_strkey.c_str(), _is_attr_check, (_is_pass_set ? _strpass.c_str() : NULL));
			if(!_presult){
				SetError("Failed to get value from key/subkey or the value is empty(null).");
				return;
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// The first argument is null and the second argument is the result.
			if(!_callbackRef.IsEmpty()){
				Napi::String	jsValue = Napi::String::New(env, (_presult ? _presult : ""), (_presult ? static_cast<size_t>(strlen(_presult)) : 0));
				_callbackRef.Value().Call({ env.Null(), jsValue });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			std::string	msg;
			if(err.Value().IsObject()){
				Napi::Object	obj		= err.Value().As<Napi::Object>();
				Napi::Value		msgval	= obj.Get("message");
				if(msgval.IsString()){
					msg = msgval.As<Napi::String>().Utf8Value();
				}else{
					Napi::String	_str = obj.ToString();
					msg = _str.Utf8Value();
				}
			}else{
				msg = "Unknown error";
			}

			// The first argument is the error message.
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, msg), env.Null() });
			}else{
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HShm*					_k2hshm;
		bool					_is_key_set;
		std::string				_strkey;
		bool					_is_skey_set;
		std::string				_strsubkey;
		bool					_is_attr_check;
		bool					_is_pass_set;
		std::string				_strpass;
		char*					_presult;
};

//---------------------------------------------------------
// GetSubkeysWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey)
// Callback function:	function(string error, array subkeys)
//
//---------------------------------------------------------
class GetSubkeysAsyncWorker : public Napi::AsyncWorker
{
	public:
		GetSubkeysAsyncWorker(const Napi::Function& callback, K2HShm* pobj, const char* pkey) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _k2hshm(pobj), _is_key_set(pkey != nullptr), _strkey(pkey ? pkey : ""), _subkey_array()
		{
			_callbackRef.Ref();
		}

		~GetSubkeysAsyncWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_k2hshm){
				SetError("No object is associated to async worker");
				return;
			}
			if(!_is_key_set){
				SetError("Specified key is empty(null)");
				return;
			}

			// get subkeys
			K2HSubKeys*	sk = _k2hshm->GetSubKeys(_strkey.c_str());
			if(sk){
				_subkey_array.clear();
				sk->StringArray(_subkey_array);
				delete	sk;
				if(0 == _subkey_array.size()){
					SetError("Failed to get subkey because the key does not have any subkey.");
					return;
				}
			}else{
				SetError("Failed to get subkey from key or key does not have any subkey.");
				return;
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				Napi::Array	retarr	= Napi::Array::New(env, _subkey_array.size());
				uint32_t	idx		= 0;
				for(const auto &_str : _subkey_array){
					retarr.Set(idx++, Napi::String::New(env, _str.c_str(), static_cast<size_t>(strlen(_str.c_str()))));
				}
				_callbackRef.Value().Call({ env.Null(), retarr });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			std::string msg;
			if(err.Value().IsObject()){
				Napi::Object	obj	= err.Value().As<Napi::Object>();
				Napi::Value		mv	= obj.Get("message");
				if(mv.IsString()){
					msg = mv.As<Napi::String>().Utf8Value();
				}else{
					msg = err.Message();
				}
			}else{
				msg = err.Message();
			}

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, msg), env.Null() });
			}else{
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HShm*					_k2hshm;
		bool					_is_key_set;
		std::string				_strkey;
		strarr_t				_subkey_array;
};

//---------------------------------------------------------
// GetAttrsWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey)
// Callback function:	function(string error, array attrs)
//
//---------------------------------------------------------
class GetAttrsAsyncWorker : public Napi::AsyncWorker
{
	public:
		GetAttrsAsyncWorker(const Napi::Function& callback, K2HShm* pobj, const char* pkey) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _k2hshm(pobj), _is_key_set(pkey != nullptr), _strkey(pkey ? pkey : ""), _attrs_array()
		{
			_callbackRef.Ref();
		}

		~GetAttrsAsyncWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_k2hshm){
				SetError("No object is associated to async worker");
				return;
			}
			if(!_is_key_set){
				SetError("Specified key is empty(null)");
				return;
			}

			// get attributes
			K2HAttrs*	attrs = _k2hshm->GetAttrs(_strkey.c_str());
			if(attrs){
				_attrs_array.clear();
				attrs->KeyStringArray(_attrs_array);
				delete attrs;
				if(0 == _attrs_array.size()){
					SetError("Failed to get attributes because the key does not have any attribute.");
					return;
				}
			}else{
				SetError("Failed to get attributes from key or key does not have any attribute.");
				return;
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				Napi::Array	retarr	= Napi::Array::New(env, _attrs_array.size());
				uint32_t	idx		= 0;
				for(const auto &_str: _attrs_array){
					retarr.Set(idx++, Napi::String::New(env, _str.c_str(), static_cast<size_t>(strlen(_str.c_str()))));
				}
				_callbackRef.Value().Call({ env.Null(), retarr });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			std::string	msg;
			if(err.Value().IsObject()){
				Napi::Object	obj	= err.Value().As<Napi::Object>();
				Napi::Value		mv	= obj.Get("message");
				if(mv.IsString()){
					msg = mv.As<Napi::String>().Utf8Value();
				}else{
					msg = err.Message();
				}
			}else{
				msg = err.Message();
			}

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, msg), env.Null() });
			}else{
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference _callbackRef;
		K2HShm*					_k2hshm;
		bool					_is_key_set;
		std::string				_strkey;
		strarr_t				_attrs_array;
};

//---------------------------------------------------------
// GetAttrValueWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* pattr)
// Callback function:	function(string error[, string value])
//
//---------------------------------------------------------
class GetAttrValueAsyncWorker : public Napi::AsyncWorker
{
	public:
		GetAttrValueAsyncWorker(const Napi::Function& callback, K2HShm* pobj, const char* pkey, const char* pattr) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _k2hshm(pobj), _is_key_set(pkey != nullptr), _strkey(pkey ? pkey : ""), _is_attr_set(pattr != nullptr), _strattr(pattr ? pattr : ""), _attrval()
		{
			_callbackRef.Ref();
		}

		~GetAttrValueAsyncWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_k2hshm){
				SetError("No object is associated to async worker");
				return;
			}
			if(!_is_key_set){
				SetError("Specified key is empty(null)");
				return;
			}
			if(!_is_attr_set){
				SetError("Specified attribute key is empty(null)");
				return;
			}

			// get attributes
			K2HAttrs*	attrs = _k2hshm->GetAttrs(_strkey.c_str());
			if(attrs){
				bool	is_found = false;
				for(K2HAttrs::iterator iter = attrs->begin(); iter != attrs->end(); ++iter){
					if(0UL == iter->keylength || !iter->pkey){
						continue;
					}
					if(iter->keylength != static_cast<size_t>(_strattr.length() + 1)){
						continue;
					}
					if(0 == memcmp(iter->pkey, _strattr.c_str(), iter->keylength)){
						// found
						if(0 < iter->vallength && iter->pval){
							_attrval.assign(reinterpret_cast<const char*>(iter->pval), iter->vallength);
						}else{
							SetError("Failed to get attribute value because the attr does not have any value.");
						}
						is_found = true;
						break;
					}
				}
				delete attrs;
				if(!is_found){
					SetError("Failed to get attribute value because the attr does not have any value.");
				}
			}else{
				SetError("Failed to get attribute value from key or key does not have attribute.");
				return;
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				Napi::String	jsVal = Napi::String::New(env, _attrval.c_str(), static_cast<size_t>(strlen(_attrval.c_str())));
				_callbackRef.Value().Call({ env.Null(), jsVal });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// Extract error message
			std::string	msg;
			if(err.Value().IsObject()){
				Napi::Object	obj	= err.Value().As<Napi::Object>();
				Napi::Value		mv	= obj.Get("message");
				if(mv.IsString()){
					msg = mv.As<Napi::String>().Utf8Value();
				}else{
					msg = err.Message();
				}
			}else{
				msg = err.Message();
			}

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, msg) });
			}else{
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HShm*					_k2hshm;
		bool					_is_key_set;
		std::string				_strkey;
		bool					_is_attr_set;
		std::string				_strattr;
		std::string				_attrval;
};

//---------------------------------------------------------
// SetValueWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* psubkey, const char* pval, const char* ppass, const time_t* pexpire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class SetValueAsyncWorker : public Napi::AsyncWorker
{
	public:
		SetValueAsyncWorker(const Napi::Function& callback, K2HShm* pobj, const char* pkey, const char* psubkey, const char* pval, const char* ppass, const time_t* p_expire) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _k2hshm(pobj),
			_is_key_set(pkey != nullptr), _strkey(pkey ? pkey : ""), _is_skey_set(psubkey != nullptr), _strsubkey(psubkey ? psubkey : ""), _is_val_set(pval != nullptr), _strval(pval ? pval : ""), _is_pass_set(ppass != nullptr), _strpass(ppass ? ppass : ""), _expire(p_expire ? *p_expire : 0)
		{
			_callbackRef.Ref();
		}

		~SetValueAsyncWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_k2hshm){
				SetError("No object is associated to async worker");
				return;
			}
			if(!_is_key_set){
				SetError("Specified key is empty(null)");
				return;
			}

			bool	result = false;
			if(_is_skey_set){
				// subkey is specified
				result = _k2hshm->AddSubkey(_strkey.c_str(), _strsubkey.c_str(), (_is_val_set ? _strval.c_str() : NULL), (_is_pass_set ? _strpass.c_str() : NULL), (_expire > 0 ? &_expire : NULL));
			}else{
				// subkey is not specified
				result = _k2hshm->Set(_strkey.c_str(), (_is_val_set ? _strval.c_str() : NULL), (_is_pass_set ? _strpass.c_str() : NULL), (_expire > 0 ? &_expire : NULL));
			}
			if(!result){
				SetError("Failed to set key/subkey and value.");
				return;
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			std::string	msg = err.Message();
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, msg) });
			}else{
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HShm*					_k2hshm;
		bool					_is_key_set;
		std::string				_strkey;
		bool					_is_skey_set;
		std::string				_strsubkey;
		bool					_is_val_set;
		std::string				_strval;
		bool					_is_pass_set;
		std::string				_strpass;
		time_t					_expire;
};

//---------------------------------------------------------
// AddSubkeyWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* psubkey, const char* pval)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class AddSubkeyAsyncWorker : public Napi::AsyncWorker
{
	public:
		AddSubkeyAsyncWorker(const Napi::Function& callback, K2HShm* pobj, const char* pkey, const char* psubkey, const char* pval) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _k2hshm(pobj), _is_key_set(pkey != nullptr), _strkey(pkey ? pkey : ""), _is_skey_set(psubkey != nullptr), _strsubkey(psubkey ? psubkey : ""), _is_val_set(pval != nullptr), _strval(pval ? pval : "")
		{
			_callbackRef.Ref();
		}

		~AddSubkeyAsyncWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_k2hshm){
				SetError("No object is associated to async worker");
				return;
			}
			if(!_is_key_set){
				SetError("Specified key is empty(null)");
				return;
			}
			if(!_is_skey_set){
				SetError("Specified subkey is empty(null)");
				return;
			}

			// add subkey
			if(!_k2hshm->AddSubkey(_strkey.c_str(), _strsubkey.c_str(), (_is_val_set ? _strval.c_str() : NULL))){
				SetError("Failed to set subkey and value to key.");
				return;
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			std::string	msg = err.Message();
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, msg) });
			}else{
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HShm*					_k2hshm;
		bool					_is_key_set;
		std::string				_strkey;
		bool					_is_skey_set;
		std::string				_strsubkey;
		bool					_is_val_set;
		std::string				_strval;
};

//---------------------------------------------------------
// AddSubkeysWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const unsigned char* psubkeys, size_t skey_length)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class AddSubkeysAsyncWorker : public Napi::AsyncWorker
{
	public:
		AddSubkeysAsyncWorker(const Napi::Function& callback, K2HShm* pobj, const char* pkey, const unsigned char* psubkeys, size_t skey_length) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _k2hshm(pobj), _is_key_set(pkey != nullptr), _strkey(pkey ? pkey : ""), _bySubkeys(nullptr), _skeylen(0), _alloc_error(false)
		{
			if(psubkeys && 0UL < skey_length){
				if(nullptr != (_bySubkeys = reinterpret_cast<unsigned char*>(malloc(skey_length)))){
					memcpy(_bySubkeys, psubkeys, skey_length);
					_skeylen = skey_length;
				}else{
					// could not allocate memory.
					_alloc_error = true;
				}
			}
			_callbackRef.Ref();
		}

		~AddSubkeysAsyncWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
			K2H_Free(_bySubkeys);
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_k2hshm){
				SetError("No object is associated to async worker");
				return;
			}
			if(!_is_key_set){
				SetError("Specified key is empty(null)");
				return;
			}
			if(_alloc_error){
				SetError("Could not allocate memory");
				return;
			}

			// add subkeys
			if(!_k2hshm->ReplaceSubkeys(reinterpret_cast<const unsigned char*>(_strkey.c_str()), _strkey.length() + 1, _bySubkeys, _skeylen)){
				SetError("Failed to replace subkeys to key.");
				return;
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			std::string msg = err.Message();
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, msg) });
			}else{
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HShm*					_k2hshm;
		bool					_is_key_set;
		std::string				_strkey;
		unsigned char*			_bySubkeys;
		size_t					_skeylen;
		bool					_alloc_error;
};

//---------------------------------------------------------
// AddAttrWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* pattrkey, const char* pattrval)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class AddAttrAsyncWorker : public Napi::AsyncWorker
{
	public:
		AddAttrAsyncWorker(const Napi::Function& callback, K2HShm* pobj, const char* pkey, const char* pattrkey, const char* pattrval) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _k2hshm(pobj), _is_key_set(pkey != nullptr), _strkey(pkey ? pkey : ""), _is_attr_set(pattrkey != nullptr), _strattr(pattrkey ? pattrkey : ""), _is_val_set(pattrval != nullptr), _strval(pattrval ? pattrval : "")
		{
			_callbackRef.Ref();
		}

		~AddAttrAsyncWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_k2hshm){
				SetError("No object is associated to async worker");
				return;
			}
			if(!_is_key_set){
				SetError("Specified key is empty(null)");
				return;
			}
			if(!_is_attr_set){
				SetError("Specified attribute name is empty(null)");
				return;
			}

			// add attribute
			if(!_k2hshm->AddAttr(_strkey.c_str(), _strattr.c_str(), (_is_val_set ? _strval.c_str() : NULL))){
				SetError("Failed to set attribute and value to key.");
				return;
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			// Extract message
			std::string	msg;
			if(err.Value().IsObject()){
				Napi::Object	obj	= err.Value().As<Napi::Object>();
				Napi::Value		mv	= obj.Get("message");
				if(mv.IsString()){
					msg = mv.As<Napi::String>().Utf8Value();
				}else{
					msg = err.Message();
				}
			}else{
				msg = err.Message();
			}

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, msg) });
			}else{
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HShm*					_k2hshm;
		bool					_is_key_set;
		std::string				_strkey;
		bool					_is_attr_set;
		std::string				_strattr;
		bool					_is_val_set;
		std::string				_strval;
};

//---------------------------------------------------------
// RemoveWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* psubkey, bool is_all)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class RemoveAsyncWorker : public Napi::AsyncWorker
{
	public:
		RemoveAsyncWorker(const Napi::Function& callback, K2HShm* pobj, const char* pkey, const char* psubkey, bool is_all) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _k2hshm(pobj), _is_key_set(pkey != nullptr), _strkey(pkey ? pkey : ""), _is_skey_set(psubkey != nullptr), _strsubkey(psubkey ? psubkey : ""), _is_remove_all(is_all)
		{
			if(_is_remove_all){
				_strsubkey.clear();
				_is_skey_set = false;
			}
			_callbackRef.Ref();
		}

		~RemoveAsyncWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_k2hshm){
				SetError("No object is associated to async worker");
				return;
			}
			if(!_is_key_set){
				SetError("Specified key is empty(null)");
				return;
			}

			bool	result = false;
			if(_is_remove_all){
				result = _k2hshm->Remove(_strkey.c_str(), true);
			}else if(_is_skey_set){
				result = _k2hshm->Remove(_strkey.c_str(), _strsubkey.c_str());
			}else{
				result = _k2hshm->Remove(_strkey.c_str(), false);
			}
			if(!result){
				SetError("Failed to remove key (and subkey).");
				return;
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			std::string	msg = err.Message();
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, msg) });
			}else{
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HShm*					_k2hshm;
		bool					_is_key_set;
		std::string				_strkey;
		bool					_is_skey_set;
		std::string				_strsubkey;
		bool					_is_remove_all;
};

//---------------------------------------------------------
// ArchiveWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* file, bool is_error_skip, bool is_load)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class ArchiveAsyncWorker : public Napi::AsyncWorker
{
	public:
		ArchiveAsyncWorker(const Napi::Function& callback, K2HShm* pobj, const char* file, bool is_error_skip, bool is_load) :
			Napi::AsyncWorker(callback), _callbackRef(Napi::Persistent(callback)), _k2hshm(pobj), _is_file_set(file != nullptr), _strfile(file ? file : ""), _errskip(is_error_skip), _is_load_type(is_load)
		{
			_callbackRef.Ref();
		}

		~ArchiveAsyncWorker() override
		{
			if(_callbackRef){
				_callbackRef.Unref();
				_callbackRef.Reset();
			}
		}

		// Run on worker thread
		void Execute() override
		{
			if(!_k2hshm){
				SetError("No object is associated to async worker");
				return;
			}
			if(!_is_file_set){
				SetError("Specified file name is empty(null)");
				return;
			}

			if(_is_load_type){
				// load archive
				if(!k2h_load_archive(reinterpret_cast<k2h_h>(_k2hshm), _strfile.c_str(), _errskip)){
					SetError("Failed to load archive file.");
					return;
				}
			}else{
				// put archive
				if(!k2h_put_archive(reinterpret_cast<k2h_h>(_k2hshm), _strfile.c_str(), _errskip)){
					SetError("Failed to put archive file.");
					return;
				}
			}
		}

		// handler for success
		void OnOK() override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ env.Null() });
			}else{
				Napi::TypeError::New(env, "Internal error in async worker").ThrowAsJavaScriptException();
			}
		}

		// handler for failure (by calling SetError)
		void OnError(const Napi::Error& err) override
		{
			Napi::Env env = Env();
			Napi::HandleScope scope(env);

			std::string msg = err.Message();
			if(!_callbackRef.IsEmpty()){
				_callbackRef.Value().Call({ Napi::String::New(env, msg) });
			}else{
				err.ThrowAsJavaScriptException();
			}
		}

	private:
		Napi::FunctionReference	_callbackRef;
		K2HShm*					_k2hshm;
		bool					_is_file_set;
		std::string				_strfile;
		bool					_errskip;
		bool					_is_load_type;
};

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: expandtab sw=4 ts=4 fdm=marker
 * vim<600: expandtab sw=4 ts=4
 */
