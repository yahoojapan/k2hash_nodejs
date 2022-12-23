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
class K2hkqPushWorker : public Nan::AsyncWorker
{
	public:
		K2hkqPushWorker(Nan::Callback* callback, K2HKeyQueue* pobj, const char* pkey, const char* pval, const char* ppass, const time_t* pexpire) :
			Nan::AsyncWorker(callback), pk2hkqobj(pobj),
			is_key_set(false), strkey(pkey ? pkey : ""), is_val_set(false), strval(pval ? pval : ""), is_pass_set(false), strpass(ppass ? ppass : ""), expire(pexpire ? *pexpire : 0)
		{
			is_key_set	= (NULL != pkey);
			is_val_set	= (NULL != pval);
			is_pass_set	= (NULL != ppass);
		}
		~K2hkqPushWorker() {}

		void Execute()
		{
			if(!pk2hkqobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(!is_key_set || !is_val_set){
				Nan::ReferenceError("Specified key or value is empty(null)");
				return;
			}

			// push
			if(!pk2hkqobj->Push(reinterpret_cast<const unsigned char*>(strkey.c_str()), (strkey.length() + 1), reinterpret_cast<const unsigned char*>(strval.c_str()), (strval.length() + 1), (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL))){
				// set error
				this->SetErrorMessage("Failed to push data for k2hkeyqueue.");
			}
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2HKeyQueue*	pk2hkqobj;
		bool			is_key_set;
		std::string		strkey;
		bool			is_val_set;
		std::string		strval;
		bool			is_pass_set;
		std::string		strpass;
		time_t			expire;
};

//---------------------------------------------------------
// K2hkqCountWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HKeyQueue* pobj)
// Callback function:	function(string error[, int count])
//
//---------------------------------------------------------
class K2hkqCountWorker : public Nan::AsyncWorker
{
	public:
		K2hkqCountWorker(Nan::Callback* callback, K2HKeyQueue* pobj) : Nan::AsyncWorker(callback), pk2hkqobj(pobj), rescount(0) {}
		~K2hkqCountWorker() {}

		void Execute()
		{
			if(!pk2hkqobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}

			// count
			rescount = pk2hkqobj->GetCount();

			// [NOTE]
			// This execution does not return error at any case.
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 2;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null(), Nan::New(rescount) };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			// This method does not call anytime.
			//
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2HKeyQueue*	pk2hkqobj;
		int				rescount;
};

//---------------------------------------------------------
// K2hkqReadWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HKeyQueue* pobj, int pos, const char* ppass)
// Callback function:	function(string error[, string key, string value])
//
//---------------------------------------------------------
class K2hkqReadWorker : public Nan::AsyncWorker
{
	public:
		K2hkqReadWorker(Nan::Callback* callback, K2HKeyQueue* pobj, int pos, const char* ppass) : Nan::AsyncWorker(callback), pk2hkqobj(pobj), getpos(pos), is_pass_set(NULL != ppass), strpass(ppass ? ppass : ""), reskey(""), resval("") {}
		~K2hkqReadWorker() {}

		void Execute()
		{
			if(!pk2hkqobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(0 > getpos){
				Nan::ReferenceError("Specified position is under zero.");
				return;
			}

			// read
			unsigned char*	pkey	= NULL;
			unsigned char*	pval	= NULL;
			size_t			keylen	= 0;
			size_t			vallen	= 0;
			bool			result	= pk2hkqobj->Read(&pkey, keylen, &pval, vallen, getpos, (is_pass_set ? strpass.c_str() : NULL));
			if(!result || !pkey || 0 == keylen){
				// set error
				this->SetErrorMessage("Failed to read data for K2HKeyQueue.");
			}else{
				// copy result
				reskey = std::string(reinterpret_cast<const char*>(pkey), keylen);
				if(pval && 0 < vallen){
					resval = std::string(reinterpret_cast<const char*>(pval), vallen);
				}
			}
			K2H_Free(pkey);
			K2H_Free(pval);
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 3;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null(), Nan::New<v8::String>(reskey.c_str()).ToLocalChecked(), Nan::New<v8::String>(resval.c_str()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2HKeyQueue*	pk2hkqobj;
		int				getpos;
		bool			is_pass_set;
		std::string		strpass;
		std::string		reskey;
		std::string		resval;
};

//---------------------------------------------------------
// K2hkqPopWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HKeyQueue* pobj, const char* ppass)
// Callback function:	function(string error[, string key, string value])
//
//---------------------------------------------------------
class K2hkqPopWorker : public Nan::AsyncWorker
{
	public:
		K2hkqPopWorker(Nan::Callback* callback, K2HKeyQueue* pobj, const char* ppass) : Nan::AsyncWorker(callback), pk2hkqobj(pobj), is_pass_set(NULL != ppass), strpass(ppass ? ppass : ""), reskey(""), resval("") {}
		~K2hkqPopWorker() {}

		void Execute()
		{
			if(!pk2hkqobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}

			// pop
			unsigned char*	pkey	= NULL;
			unsigned char*	pval	= NULL;
			size_t			keylen	= 0;
			size_t			vallen	= 0;
			bool			result	= pk2hkqobj->Pop(&pkey, keylen, &pval, vallen, (is_pass_set ? strpass.c_str() : NULL));
			if(!result || !pkey || 0 == keylen){
				// set error
				this->SetErrorMessage("Failed to pop data for K2HKeyQueue.");
			}else{
				// copy result
				reskey = std::string(reinterpret_cast<const char*>(pkey), keylen);
				if(pval && 0 < vallen){
					resval = std::string(reinterpret_cast<const char*>(pval), vallen);
				}
			}
			K2H_Free(pkey);
			K2H_Free(pval);
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 3;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null(), Nan::New<v8::String>(reskey.c_str()).ToLocalChecked(), Nan::New<v8::String>(resval.c_str()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2HKeyQueue*	pk2hkqobj;
		bool			is_pass_set;
		std::string		strpass;
		std::string		reskey;
		std::string		resval;
};

//---------------------------------------------------------
// K2hkqRemoveWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HKeyQueue* pobj, int count, const char* ppass)
// Callback function:	function(string error[, int removed_queue_count])
//
//---------------------------------------------------------
class K2hkqRemoveWorker : public Nan::AsyncWorker
{
	public:
		K2hkqRemoveWorker(Nan::Callback* callback, K2HKeyQueue* pobj, int count, const char* ppass) : Nan::AsyncWorker(callback), pk2hkqobj(pobj), rmcount(count), is_pass_set(NULL != ppass), strpass(ppass ? ppass : ""), removed_count(0) {}
		~K2hkqRemoveWorker() {}

		void Execute()
		{
			if(!pk2hkqobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(0 >= rmcount){
				Nan::ReferenceError("Specified remove count is under 1.");
				return;
			}

			// remove
			removed_count = pk2hkqobj->Remove(rmcount, NULL, NULL, (is_pass_set ? strpass.c_str() : NULL));
			if(-1 == removed_count){
				// set error
				this->SetErrorMessage("Failed to remove queue for K2HKeyQueue.");
			}
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 2;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null(), Nan::New(removed_count) };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
		}

        void HandleErrorCallback()
        {
			Nan::HandleScope		scope;
			const int				argc		= 1;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2HKeyQueue*	pk2hkqobj;
		int				rmcount;
		bool			is_pass_set;
		std::string		strpass;
		int				removed_count;
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
