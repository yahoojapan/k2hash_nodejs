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
 * AUTHOR:   Taku Ishihara
 * CREATE:   Tue 10 Mar 2015
 * REVISION:
 */

#ifndef K2H_KEYQUEUE_H
#define K2H_KEYQUEUE_H

#include "k2h_common.h"
#include "k2h_cbs.h"

//---------------------------------------------------------
// K2hKeyQueue Class
//---------------------------------------------------------
class K2hKeyQueue : public Napi::ObjectWrap<K2hKeyQueue>
{
	friend class	K2hNode;

	public:
		static void Init(Napi::Env env, Napi::Object exports);
		static Napi::Object NewInstance(Napi::Env env);
		static Napi::Object NewInstance(Napi::Env env, const Napi::Value& arg);

		static Napi::Object GetInstance(const Napi::CallbackInfo& info);

		// Constructor / Destructor
		explicit K2hKeyQueue(const Napi::CallbackInfo& info);
		~K2hKeyQueue();

	private:
		Napi::Value New(const Napi::CallbackInfo& info);

		Napi::Value On(const Napi::CallbackInfo& info);
		Napi::Value OnPush(const Napi::CallbackInfo& info);
		Napi::Value OnPop(const Napi::CallbackInfo& info);
		Napi::Value OnCount(const Napi::CallbackInfo& info);
		Napi::Value OnRead(const Napi::CallbackInfo& info);
		Napi::Value OnRemove(const Napi::CallbackInfo& info);

		Napi::Value Off(const Napi::CallbackInfo& info);
		Napi::Value OffPush(const Napi::CallbackInfo& info);
		Napi::Value OffPop(const Napi::CallbackInfo& info);
		Napi::Value OffCount(const Napi::CallbackInfo& info);
		Napi::Value OffRead(const Napi::CallbackInfo& info);
		Napi::Value OffRemove(const Napi::CallbackInfo& info);

		Napi::Value Initialize(const Napi::CallbackInfo& info);
		Napi::Value IsEmpty(const Napi::CallbackInfo& info);
		Napi::Value Count(const Napi::CallbackInfo& info);
		Napi::Value Read(const Napi::CallbackInfo& info);
		Napi::Value Push(const Napi::CallbackInfo& info);
		Napi::Value Pop(const Napi::CallbackInfo& info);
		Napi::Value Remove(const Napi::CallbackInfo& info);
		Napi::Value Dump(const Napi::CallbackInfo& info);

	public:
		// constructor reference
		static Napi::FunctionReference	constructor;

		StackEmitCB	_cbs;

	private:
		K2HKeyQueue	_k2hkeyqueue;
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
