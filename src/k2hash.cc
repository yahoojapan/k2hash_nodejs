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

#include "k2h_shm.h"

using namespace v8 ;

//---------------------------------------------------------
// k2hash node object
//---------------------------------------------------------
NAN_METHOD(CreateObject)
{
	K2hNode::NewInstance(info);
}

void InitAll(Local<Object> exports, Local<Object> module)
{
	K2hNode::Init();
	Nan::Set(module, Nan::New("exports").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(CreateObject)).ToLocalChecked());
}

NODE_MODULE(k2hash , InitAll)

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: expandtab sw=4 ts=4 fdm=marker
 * vim<600: expandtab sw=4 ts=4
 */
