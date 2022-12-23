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
class K2hqPushWorker : public Nan::AsyncWorker
{
	public:
		K2hqPushWorker(Nan::Callback* callback, K2HQueue* pobj, const char* pdata, const char* ppass, const time_t* pexpire) : Nan::AsyncWorker(callback), pk2hqobj(pobj), is_data_set(false), strdata(pdata ? pdata : ""), is_pass_set(false), strpass(ppass ? ppass : ""), expire(pexpire ? *pexpire : 0)
		{
			is_data_set	= (NULL != pdata);
			is_pass_set	= (NULL != ppass);
		}
		~K2hqPushWorker() {}

		void Execute()
		{
			if(!pk2hqobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(!is_data_set){
				Nan::ReferenceError("Specified data is empty(null)");
				return;
			}

			// push
			if(!pk2hqobj->Push(reinterpret_cast<const unsigned char*>(strdata.c_str()), (strdata.length() + 1), NULL, (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL))){
				// set error
				this->SetErrorMessage("Failed to push data for k2hqueue.");
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
		K2HQueue*	pk2hqobj;
		bool		is_data_set;
		std::string	strdata;
		bool		is_pass_set;
		std::string	strpass;
		time_t		expire;
};

//---------------------------------------------------------
// K2hqCountWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HQueue* pobj)
// Callback function:	function(string error, int count)
//
//---------------------------------------------------------
class K2hqCountWorker : public Nan::AsyncWorker
{
	public:
		K2hqCountWorker(Nan::Callback* callback, K2HQueue* pobj) : Nan::AsyncWorker(callback), pk2hqobj(pobj), rescount(0) {}
		~K2hqCountWorker() {}

		void Execute()
		{
			if(!pk2hqobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}

			// count
			rescount = pk2hqobj->GetCount();

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
		K2HQueue*	pk2hqobj;
		int			rescount;
};

//---------------------------------------------------------
// K2hqReadWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HQueue* pobj, int pos, const char* ppass)
// Callback function:	function(string error[, string data])
//
//---------------------------------------------------------
class K2hqReadWorker : public Nan::AsyncWorker
{
	public:
		K2hqReadWorker(Nan::Callback* callback, K2HQueue* pobj, int pos, const char* ppass) : Nan::AsyncWorker(callback), pk2hqobj(pobj), getpos(pos), is_pass_set(NULL != ppass), strpass(ppass ? ppass : ""), resdata("") {}
		~K2hqReadWorker() {}

		void Execute()
		{
			if(!pk2hqobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(0 > getpos){
				Nan::ReferenceError("Specified position is under zero.");
				return;
			}

			// read
			unsigned char*	pdata	= NULL;
			size_t			datalen	= 0;
			bool			result	= pk2hqobj->Read(&pdata, datalen, getpos, (is_pass_set ? strpass.c_str() : NULL));
			if(!result || !pdata || 0 == datalen){
				// set error
				this->SetErrorMessage("Failed to read data for k2hqueue.");
			}else{
				// copy result
				resdata = std::string(reinterpret_cast<const char*>(pdata), datalen);
			}
			K2H_Free(pdata);
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 2;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null(), Nan::New<v8::String>(resdata.c_str()).ToLocalChecked() };

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
		K2HQueue*	pk2hqobj;
		int			getpos;
		bool		is_pass_set;
		std::string	strpass;
		std::string	resdata;
};

//---------------------------------------------------------
// K2hqPopWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HQueue* pobj, const char* ppass)
// Callback function:	function(string error[, string data])
//
//---------------------------------------------------------
class K2hqPopWorker : public Nan::AsyncWorker
{
	public:
		K2hqPopWorker(Nan::Callback* callback, K2HQueue* pobj, const char* ppass) : Nan::AsyncWorker(callback), pk2hqobj(pobj), is_pass_set(NULL != ppass), strpass(ppass ? ppass : ""), resdata("") {}
		~K2hqPopWorker() {}

		void Execute()
		{
			if(!pk2hqobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}

			// pop
			unsigned char*	pdata	= NULL;
			size_t			datalen	= 0;
			bool			result	= pk2hqobj->Pop(&pdata, datalen, NULL, (is_pass_set ? strpass.c_str() : NULL));
			if(!result || !pdata || 0 == datalen){
				// set error
				this->SetErrorMessage("Failed to pop data for k2hqueue.");
			}else{
				// copy result
				resdata = std::string(reinterpret_cast<const char*>(pdata), datalen);
			}
			K2H_Free(pdata);
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 2;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null(), Nan::New<v8::String>(resdata.c_str()).ToLocalChecked() };

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
		K2HQueue*	pk2hqobj;
		bool		is_pass_set;
		std::string	strpass;
		std::string	resdata;
};

//---------------------------------------------------------
// K2hqRemoveWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HQueue* pobj, int count, const char* ppass)
// Callback function:	function(string error[, int removed_queue_count])
//
//---------------------------------------------------------
class K2hqRemoveWorker : public Nan::AsyncWorker
{
	public:
		K2hqRemoveWorker(Nan::Callback* callback, K2HQueue* pobj, int count, const char* ppass) : Nan::AsyncWorker(callback), pk2hqobj(pobj), rmcount(count), is_pass_set(NULL != ppass), strpass(ppass ? ppass : ""), removed_count(0) {}
		~K2hqRemoveWorker() {}

		void Execute()
		{
			if(!pk2hqobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(0 >= rmcount){
				Nan::ReferenceError("Specified remove count is under 1.");
				return;
			}

			// remove
			removed_count = pk2hqobj->Remove(rmcount, NULL, NULL, (is_pass_set ? strpass.c_str() : NULL));
			if(-1 == removed_count){
				// set error
				this->SetErrorMessage("Failed to remove queue for k2hqueue.");
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
		K2HQueue*	pk2hqobj;
		int			rmcount;
		bool		is_pass_set;
		std::string	strpass;
		int			removed_count;
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
