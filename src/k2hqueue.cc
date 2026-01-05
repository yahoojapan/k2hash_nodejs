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

#include "k2h_queue.h"

//---------------------------------------------------------
// k2hqueue node object
//---------------------------------------------------------
Napi::Value CreateObject(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(0 < info.Length()){
		return K2hQueue::NewInstance(env, info[0]);
	}else{
		return K2hQueue::NewInstance(env);
	}
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports)
{
	// Register class / create constructor (store in K2hQueue::constructor)
	K2hQueue::Init(env, exports);

	// Create factory function to be assigned to module.exports
	Napi::Function createFn = Napi::Function::New(env, CreateObject, "k2hqueue");

	// Attach constructor for compatibility (require('k2hqueue').K2hQueue)
	createFn.Set("K2hQueue", K2hQueue::constructor.Value());

	// Return factory function as module.exports
	return createFn;
}

NODE_API_MODULE(k2hqueue, InitAll)

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: expandtab sw=4 ts=4 fdm=marker
 * vim<600: expandtab sw=4 ts=4
 */
