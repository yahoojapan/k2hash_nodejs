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

#ifndef K2H_QUEUE_H
#define K2H_QUEUE_H

#include "k2h_common.h"
#include "k2h_cbs.h"

//---------------------------------------------------------
// K2hQueue Class
//---------------------------------------------------------
class K2hQueue : public Nan::ObjectWrap
{
	friend class	K2hNode;

	public:
		static void Init(void);
		static NAN_METHOD(NewInstance);
		static v8::Local<v8::Object> GetInstance(Nan::NAN_METHOD_ARGS_TYPE info);

	private:
		explicit K2hQueue();
		~K2hQueue();

		static NAN_METHOD(New);

		static NAN_METHOD(On);
		static NAN_METHOD(OnPush);
		static NAN_METHOD(OnPop);
		static NAN_METHOD(OnCount);
		static NAN_METHOD(OnRead);
		static NAN_METHOD(OnRemove);
		static NAN_METHOD(Off);
		static NAN_METHOD(OffPush);
		static NAN_METHOD(OffPop);
		static NAN_METHOD(OffCount);
		static NAN_METHOD(OffRead);
		static NAN_METHOD(OffRemove);

		static NAN_METHOD(Init);
		static NAN_METHOD(IsEmpty);
		static NAN_METHOD(Count);
		static NAN_METHOD(Read);
		static NAN_METHOD(Push);
		static NAN_METHOD(Pop);
		static NAN_METHOD(Remove);
		static NAN_METHOD(Dump);

	private:
		static Nan::Persistent<v8::Function>	constructor;
		K2HQueue								_k2hqueue;
		StackEmitCB								_cbs;
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
