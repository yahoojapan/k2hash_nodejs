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

#include <napi.h>
#include "k2h_shm.h"

//---------------------------------------------------------
// k2hash node object
//---------------------------------------------------------
// [NOTE]
// The logic for receiving arguments when switching to N-API has been removed.
// This is because the arguments were not used in the first place and did not
// need to be defined.
//
Napi::Value CreateObject(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();
	return K2hNode::NewInstance(env);	// always no arguments.
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports)
{
	// Class registration (creating a constructor)
	K2hNode::Init(env, exports); // この中で constructor を作って Persistent に保存している想定

	// Create a factory function that returns module.exports
	Napi::Function createFn = Napi::Function::New(env, CreateObject, "k2hash");

	// Allow to use "require('k2hash').K2hNode"
	createFn.Set("K2hNode", K2hNode::constructor.Value());

	// Replace module.exports with this function (does not break existing "require('k2hash')()".)
	return createFn;
}

NODE_API_MODULE(k2hash, InitAll)

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: expandtab sw=4 ts=4 fdm=marker
 * vim<600: expandtab sw=4 ts=4
 */
