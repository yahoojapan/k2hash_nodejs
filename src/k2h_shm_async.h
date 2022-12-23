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
class CreateWorker : public Nan::AsyncWorker
{
	public:
		CreateWorker(Nan::Callback* callback, K2HShm* pobj, const char* k2hfile, bool fullmapping, int mask, int cmask, int element_cnt, size_t page) :
			Nan::AsyncWorker(callback), pk2hobj(pobj),
			strfile(k2hfile ? k2hfile : ""), isfullmapping(fullmapping), mask_bitcnt(mask), cmask_bitcnt(cmask), max_element_cnt(element_cnt), pagesize(page)
		{}
		~CreateWorker() {}

		void Execute()
		{
			if(!pk2hobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(!pk2hobj->Create(strfile.c_str(), isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize)){
				// set error
				this->SetErrorMessage("Failed to initialize k2hash object.");
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
		K2HShm*		pk2hobj;
		std::string	strfile;
		bool		isfullmapping;
		int			mask_bitcnt;
		int			cmask_bitcnt;
		int			max_element_cnt;
		size_t		pagesize;
};

//---------------------------------------------------------
// OpenWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* k2hfile, bool readonly, bool create, bool tempfilemode, bool fullmapping, int mask, int cmask, int element_cnt, size_t page)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class OpenWorker : public Nan::AsyncWorker
{
	public:
		OpenWorker(Nan::Callback* callback, K2HShm* pobj, const char* k2hfile, bool readonly, bool create, bool tempfilemode, bool fullmapping, int mask, int cmask, int element_cnt, size_t page) :
			Nan::AsyncWorker(callback), pk2hobj(pobj),
			strfile(k2hfile ? k2hfile : ""), isReadOnly(readonly), isCreate(create), isTempFile(tempfilemode), isfullmapping(fullmapping), mask_bitcnt(mask), cmask_bitcnt(cmask), max_element_cnt(element_cnt), pagesize(page)
		{}
		~OpenWorker() {}

		void Execute()
		{
			if(!pk2hobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(!pk2hobj->Attach(strfile.c_str(), isReadOnly, isCreate, isTempFile, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize)){
				// set error
				this->SetErrorMessage("Failed to attach(open) k2hash object.");
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
		K2HShm*		pk2hobj;
		std::string	strfile;
		bool		isReadOnly;
		bool		isCreate;
		bool		isTempFile;
		bool		isfullmapping;
		int			mask_bitcnt;
		int			cmask_bitcnt;
		int			max_element_cnt;
		size_t		pagesize;
};

//---------------------------------------------------------
// CloseWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class CloseWorker : public Nan::AsyncWorker
{
	public:
		CloseWorker(Nan::Callback* callback, K2HShm* pobj) : Nan::AsyncWorker(callback), pk2hobj(pobj) {}
		~CloseWorker() {}

		void Execute()
		{
			if(!pk2hobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(!pk2hobj->Detach()){
				// set error
				this->SetErrorMessage("Failed to close k2hash object.");
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
		K2HShm*		pk2hobj;
};

//---------------------------------------------------------
// GetValueWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* psubkey, bool attrchk, const char* ppass)
// Callback function:	function(string error[, string value])
//
//---------------------------------------------------------
class GetValueWorker : public Nan::AsyncWorker
{
	public:
		GetValueWorker(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* psubkey, bool attrchk, const char* ppass) :
			Nan::AsyncWorker(callback), pk2hobj(pobj),
			is_key_set(false), strkey(pkey ? pkey : ""), is_skey_set(false), strsubkey(psubkey ? psubkey : ""), is_attr_check(attrchk), is_pass_set(false), strpass(ppass ? ppass : ""), presult(NULL)
		{
			is_key_set	= (NULL != pkey);
			is_skey_set	= (NULL != psubkey);
			is_pass_set	= (NULL != ppass);
		}
		~GetValueWorker()
		{
			K2H_Free(presult);
		}

		void Execute()
		{
			if(!pk2hobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(!is_key_set){
				Nan::ReferenceError("Specified key is empty(null)");
				return;
			}

			// check subkey if specified
			if(is_skey_set){
				// subkey is specified, thus need to check the key has it.
				bool		found	= false;
				K2HSubKeys*	sk		= pk2hobj->GetSubKeys(strkey.c_str());
				if(sk){
					strarr_t	strarr;
					sk->StringArray(strarr);
					for(strarr_t::const_iterator iter = strarr.begin(); iter != strarr.end(); ++iter){
						if(0 == strcmp(iter->c_str(), strsubkey.c_str())){
							found = true;
							break;
						}
					}
					delete sk;
				}
				if(!found){
					// set error
					this->SetErrorMessage("There is no specified subkey in key subkey list.");
					return;
				}
				strkey = strsubkey;
			}

			// get value
			presult = pk2hobj->Get(strkey.c_str(), is_attr_check, (is_pass_set ? strpass.c_str() : NULL));
			if(!presult){
				// set error
				this->SetErrorMessage("Failed to get value from key/subkey or the value is empty(null).");
				return;
			}
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 2;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null(), Nan::New<v8::String>(presult).ToLocalChecked() };

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
			const int				argc		= 2;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked(), Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2HShm*		pk2hobj;
		bool		is_key_set;
		std::string	strkey;
		bool		is_skey_set;
		std::string	strsubkey;
		bool		is_attr_check;
		bool		is_pass_set;
		std::string	strpass;
		char*		presult;
};

//---------------------------------------------------------
// GetSubkeysWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey)
// Callback function:	function(string error, array subkeys)
//
//---------------------------------------------------------
class GetSubkeysWorker : public Nan::AsyncWorker
{
	public:
		GetSubkeysWorker(Nan::Callback* callback, K2HShm* pobj, const char* pkey) : Nan::AsyncWorker(callback), pk2hobj(pobj), is_key_set(false), strkey(pkey ? pkey : "")
		{
			is_key_set	= (NULL != pkey);
		}
		~GetSubkeysWorker() {}

		void Execute()
		{
			if(!pk2hobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(!is_key_set){
				Nan::ReferenceError("Specified key is empty(null)");
				return;
			}

			// get subkey
			K2HSubKeys*	sk = pk2hobj->GetSubKeys(strkey.c_str());
			if(sk){
				subkey_array.clear();
				sk->StringArray(subkey_array);
				delete sk;
				if(0 == subkey_array.size()){
					// set error
					this->SetErrorMessage("Failed to get subkey because the key does not have any subkey.");
					return;
				}
			}else{
				// set error
				this->SetErrorMessage("Failed to get subkey from key or key does not have any subkey.");
				return;
			}
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			v8::Local<v8::Array>	retarr		= Nan::New<v8::Array>();
			int						pos			= 0 ;
			for(strarr_t::const_iterator iter = subkey_array.begin(); iter != subkey_array.end(); ++iter, ++pos){
				Nan::Set(retarr, pos, Nan::New<v8::String>(iter->c_str()).ToLocalChecked());
			}
			const int				argc		= 2;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null(), retarr };

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
			const int				argc		= 2;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked(), Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2HShm*		pk2hobj;
		bool		is_key_set;
		std::string	strkey;
		strarr_t	subkey_array;
};

//---------------------------------------------------------
// GetAttrsWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey)
// Callback function:	function(string error, array attrs)
//
//---------------------------------------------------------
class GetAttrsWorker : public Nan::AsyncWorker
{
	public:
		GetAttrsWorker(Nan::Callback* callback, K2HShm* pobj, const char* pkey) : Nan::AsyncWorker(callback), pk2hobj(pobj), is_key_set(false), strkey(pkey ? pkey : "")
		{
			is_key_set	= (NULL != pkey);
		}
		~GetAttrsWorker() {}

		void Execute()
		{
			if(!pk2hobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(!is_key_set){
				Nan::ReferenceError("Specified key is empty(null)");
				return;
			}

			// get attributes
			K2HAttrs*	attrs = pk2hobj->GetAttrs(strkey.c_str());
			if(attrs){
				attrs_array.clear();
				attrs->KeyStringArray(attrs_array);
				delete attrs;
				if(0 == attrs_array.size()){
					// set error
					this->SetErrorMessage("Failed to get attributes because the key does not have any attribute.");
					return;
				}
			}else{
				// get error
				this->SetErrorMessage("Failed to get attributes from key or key does not have any attribute.");
				return;
			}
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			v8::Local<v8::Array>	retarr		= Nan::New<v8::Array>();
			int						pos			= 0 ;
			for(strarr_t::const_iterator iter = attrs_array.begin(); iter != attrs_array.end(); ++iter, ++pos){
				Nan::Set(retarr, pos, Nan::New<v8::String>(iter->c_str()).ToLocalChecked());
			}
			const int				argc		= 2;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null(), retarr };

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
			const int				argc		= 2;
			v8::Local<v8::Value>	argv[argc]	= { Nan::New<v8::String>(this->ErrorMessage()).ToLocalChecked(), Nan::Null() };

			if(callback){
				callback->Call(argc, argv);
			}else{
				Nan::ThrowSyntaxError("Internal error in async worker");
				return;
			}
        }

	private:
		K2HShm*		pk2hobj;
		bool		is_key_set;
		std::string	strkey;
		strarr_t	attrs_array;
};

//---------------------------------------------------------
// GetAttrValueWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* pattr)
// Callback function:	function(string error[, string value])
//
//---------------------------------------------------------
class GetAttrValueWorker : public Nan::AsyncWorker
{
	public:
		GetAttrValueWorker(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* pattr) : Nan::AsyncWorker(callback), pk2hobj(pobj), is_key_set(false), strkey(pkey ? pkey : ""), is_attr_set(false), strattr(pattr ? pattr : "")
		{
			is_key_set	= (NULL != pkey);
			is_attr_set	= (NULL != pattr);
		}
		~GetAttrValueWorker() {}

		void Execute()
		{
			if(!pk2hobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(!is_key_set){
				Nan::ReferenceError("Specified key is empty(null)");
				return;
			}
			if(!is_attr_set){
				Nan::ReferenceError("Specified attribute key is empty(null)");
				return;
			}

			// get attributes
			K2HAttrs*	attrs = pk2hobj->GetAttrs(strkey.c_str());
			if(attrs){
				bool	is_found = false;
				for(K2HAttrs::iterator iter = attrs->begin(); iter != attrs->end(); ++iter){
					if(0UL == iter->keylength || !iter->pkey){
						continue;
					}
					if(iter->keylength != static_cast<size_t>(strattr.length() + 1)){
						continue;
					}
					if(0 == memcmp(iter->pkey, strattr.c_str(), iter->keylength)){
						// found
						if(0 < iter->vallength && iter->pval){
							attrval = std::string(reinterpret_cast<const char*>(iter->pval), iter->vallength);
						}else{
							this->SetErrorMessage("Failed to get attribute value because the attr does not have any value.");
						}
						is_found	= true;
						break;
					}
				}
				delete attrs;
				if(!is_found){
					// not found
					this->SetErrorMessage("Failed to get attribute value because the attr does not have any value.");
				}
			}else{
				// get error
				this->SetErrorMessage("Failed to get attribute value from key or key does not have attribute.");
				return;
			}
		}

		void HandleOKCallback()
		{
			Nan::HandleScope		scope;
			const int				argc		= 2;
			v8::Local<v8::Value>	argv[argc]	= { Nan::Null(), Nan::New<v8::String>(attrval.c_str()).ToLocalChecked() };

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
		K2HShm*			pk2hobj;
		bool			is_key_set;
		std::string		strkey;
		bool			is_attr_set;
		std::string		strattr;
		std::string		attrval;
};

//---------------------------------------------------------
// SetValueWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* psubkey, const char* pval, const char* ppass, const time_t* pexpire)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class SetValueWorker : public Nan::AsyncWorker
{
	public:
		SetValueWorker(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* psubkey, const char* pval, const char* ppass, const time_t* pexpire) :
			Nan::AsyncWorker(callback), pk2hobj(pobj),
			is_key_set(false), strkey(pkey ? pkey : ""), is_skey_set(false), strsubkey(psubkey ? psubkey : ""), is_val_set(false), strval(pval ? pval : ""), is_pass_set(false), strpass(ppass ? ppass : ""), expire(pexpire ? *pexpire : 0)
		{
			is_key_set	= (NULL != pkey);
			is_skey_set	= (NULL != psubkey);
			is_val_set	= (NULL != pval);
			is_pass_set	= (NULL != ppass);
		}
		~SetValueWorker() {}

		void Execute()
		{
			if(!pk2hobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(!is_key_set){
				Nan::ReferenceError("Specified key is empty(null)");
				return;
			}

			bool	result;
			if(is_skey_set){
				// subkey is specified
				result = pk2hobj->AddSubkey(strkey.c_str(), strsubkey.c_str(), (is_val_set ? strval.c_str() : NULL), (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL));
			}else{
				// subkey is not specified
				result = pk2hobj->Set(strkey.c_str(), (is_val_set ? strval.c_str() : NULL), (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL));
			}
			if(!result){
				// set error
				this->SetErrorMessage("Failed to set key/subkey and value.");
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
		K2HShm*		pk2hobj;
		bool		is_key_set;
		std::string	strkey;
		bool		is_skey_set;
		std::string	strsubkey;
		bool		is_val_set;
		std::string	strval;
		bool		is_pass_set;
		std::string	strpass;
		time_t		expire;
};

//---------------------------------------------------------
// AddSubkeyWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* psubkey, const char* pval)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class AddSubkeyWorker : public Nan::AsyncWorker
{
	public:
		AddSubkeyWorker(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* psubkey, const char* pval) :
			Nan::AsyncWorker(callback), pk2hobj(pobj),
			is_key_set(false), strkey(pkey ? pkey : ""), is_skey_set(false), strsubkey(psubkey ? psubkey : ""), is_val_set(false), strval(pval ? pval : "")
		{
			is_key_set	= (NULL != pkey);
			is_skey_set	= (NULL != psubkey);
			is_val_set	= (NULL != pval);
		}
		~AddSubkeyWorker() {}

		void Execute()
		{
			if(!pk2hobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(!is_key_set){
				Nan::ReferenceError("Specified key is empty(null)");
				return;
			}
			if(!is_skey_set){
				Nan::ReferenceError("Specified subkey is empty(null)");
				return;
			}

			// add subkey
			if(!pk2hobj->AddSubkey(strkey.c_str(), strsubkey.c_str(), (is_val_set ? strval.c_str() : NULL))){
				// set error
				this->SetErrorMessage("Failed to set subkey and value to key.");
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
		K2HShm*		pk2hobj;
		bool		is_key_set;
		std::string	strkey;
		bool		is_skey_set;
		std::string	strsubkey;
		bool		is_val_set;
		std::string	strval;
};

//---------------------------------------------------------
// AddSubkeysWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const unsigned char* psubkeys, size_t skey_length)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class AddSubkeysWorker : public Nan::AsyncWorker
{
	public:
		AddSubkeysWorker(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const unsigned char* psubkeys, size_t skey_length) : Nan::AsyncWorker(callback), pk2hobj(pobj), is_key_set(false), strkey(pkey ? pkey : ""), bySubkeys(NULL), skeylen(0), alloc_error(false)
		{
			if(psubkeys && 0UL < skey_length){
				if(NULL != (bySubkeys = reinterpret_cast<unsigned char*>(malloc(skey_length)))){
					memcpy(bySubkeys, psubkeys, skey_length);
					skeylen = skey_length;
				}else{
					// could not allocate memory.
					alloc_error = true;
				}
			}
			is_key_set	= (NULL != pkey);
		}
		~AddSubkeysWorker()
		{
			K2H_Free(bySubkeys);
		}

		void Execute()
		{
			if(!pk2hobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(!is_key_set){
				Nan::ReferenceError("Specified key is empty(null)");
				return;
			}
			if(alloc_error){
				Nan::ReferenceError("Could not allocate memory");
				return;
			}

			// add subkeys
			if(!pk2hobj->ReplaceSubkeys(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, bySubkeys, skeylen)){
				// set error
				this->SetErrorMessage("Failed to replace subkeys to key.");
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
		K2HShm*			pk2hobj;
		bool			is_key_set;
		std::string		strkey;
		unsigned char*	bySubkeys;
		size_t			skeylen;
		bool			alloc_error;
};

//---------------------------------------------------------
// AddAttrWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* pattrkey, const char* pattrval)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class AddAttrWorker : public Nan::AsyncWorker
{
	public:
		AddAttrWorker(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* pattrkey, const char* pattrval) :
			Nan::AsyncWorker(callback), pk2hobj(pobj),
			is_key_set(false), strkey(pkey ? pkey : ""), is_attr_set(false), strattr(pattrkey ? pattrkey : ""), is_val_set(false), strval(pattrval ? pattrval : "")
		{
			is_key_set	= (NULL != pkey);
			is_attr_set	= (NULL != pattrkey);
			is_val_set	= (NULL != pattrval);
		}
		~AddAttrWorker() {}

		void Execute()
		{
			if(!pk2hobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(!is_key_set){
				Nan::ReferenceError("Specified key is empty(null)");
				return;
			}
			if(!is_attr_set){
				Nan::ReferenceError("Specified attribute name is empty(null)");
				return;
			}

			// add attribute
			if(!pk2hobj->AddAttr(strkey.c_str(), strattr.c_str(), (is_val_set ? strval.c_str() : NULL))){
				// set error
				this->SetErrorMessage("Failed to set attribute and value to key.");
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
		K2HShm*		pk2hobj;
		bool		is_key_set;
		std::string	strkey;
		bool		is_attr_set;
		std::string	strattr;
		bool		is_val_set;
		std::string	strval;
};

//---------------------------------------------------------
// RemoveWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* psubkey, bool is_all)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class RemoveWorker : public Nan::AsyncWorker
{
	public:
		RemoveWorker(Nan::Callback* callback, K2HShm* pobj, const char* pkey, const char* psubkey, bool is_all) : Nan::AsyncWorker(callback), pk2hobj(pobj), is_key_set(false), strkey(pkey ? pkey : ""), is_skey_set(false), strsubkey(psubkey ? psubkey : ""), is_remove_all(is_all)
		{
			is_key_set		= (NULL != pkey);
			if(is_remove_all){
				strsubkey.clear();
			}else{
				is_skey_set	= (NULL != psubkey);
			}
		}
		~RemoveWorker() {}

		void Execute()
		{
			if(!pk2hobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(!is_key_set){
				Nan::ReferenceError("Specified key is empty(null)");
				return;
			}

			// remove
			bool	result;
			if(is_remove_all){
				result = pk2hobj->Remove(strkey.c_str(), true);
			}else if(is_skey_set){
				result = pk2hobj->Remove(strkey.c_str(), strsubkey.c_str());
			}else{
				result = pk2hobj->Remove(strkey.c_str(), false);
			}
			if(!result){
				// set error
				this->SetErrorMessage("Failed to remove key (and subkey).");
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
		K2HShm*		pk2hobj;
		bool		is_key_set;
		std::string	strkey;
		bool		is_skey_set;
		std::string	strsubkey;
		bool		is_remove_all;
};

//---------------------------------------------------------
// ArchiveWorker class
//
// Constructor:			constructor(Nan::Callback* callback, K2HShm* pobj, const char* file, bool is_error_skip, bool is_load)
// Callback function:	function(string error)
//
//---------------------------------------------------------
class ArchiveWorker : public Nan::AsyncWorker
{
	public:
		ArchiveWorker(Nan::Callback* callback, K2HShm* pobj, const char* file, bool is_error_skip, bool is_load) : Nan::AsyncWorker(callback), pk2hobj(pobj), is_file_set(false), strfile(file ? file : ""), errskip(is_error_skip), is_load_type(is_load)
		{
			is_file_set = (NULL != file);
		}
		~ArchiveWorker() {}

		void Execute()
		{
			if(!pk2hobj){
				Nan::ReferenceError("No object is associated to async worker");
				return;
			}
			if(!is_file_set){
				Nan::ReferenceError("Specified file name is empty(null)");
				return;
			}

			if(is_load_type){
				// load archive
				if(!k2h_load_archive(reinterpret_cast<k2h_h>(pk2hobj), strfile.c_str(), errskip)){
					// set error
					this->SetErrorMessage("Failed to load archive file.");
				}
			}else{
				// put archive
				if(!k2h_put_archive(reinterpret_cast<k2h_h>(pk2hobj), strfile.c_str(), errskip)){
					// set error
					this->SetErrorMessage("Failed to put archive file.");
				}
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
		K2HShm*		pk2hobj;
		bool		is_file_set;
		std::string	strfile;
		bool		errskip;
		bool		is_load_type;
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
