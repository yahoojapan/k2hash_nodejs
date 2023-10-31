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

#include <fullock/flckstructure.h>
#include <fullock/flckbaselist.tcc>
#include "k2h_cbs.h"

using namespace v8;
using namespace std;
using namespace fullock;

//---------------------------------------------------------
// StackEmitCB Class
//---------------------------------------------------------
StackEmitCB::StackEmitCB() : lockval(FLCK_NOSHARED_MUTEX_VAL_UNLOCKED)
{
}

StackEmitCB::~StackEmitCB()
{
	while(!flck_trylock_noshared_mutex(&lockval));	// LOCK
	EmitCbsMap.clear();
	flck_unlock_noshared_mutex(&lockval);			// UNLOCK
}

// [NOTE]
// This method does not lock, thus must lock before calling this.
//
Nan::Callback* StackEmitCB::RawFind(const char* pemitname)
{
	string			stremit	= pemitname ? pemitname : "";
	Nan::Callback*	cbfunc	= NULL;
	if(stremit.empty()){
		return cbfunc;
	}
	if(EmitCbsMap.end() != EmitCbsMap.find(stremit)){
		cbfunc = EmitCbsMap[stremit];
	}
	return cbfunc;
}

Nan::Callback* StackEmitCB::Find(const char* pemitname)
{
	while(!flck_trylock_noshared_mutex(&lockval));	// LOCK
	Nan::Callback*	cbfunc = RawFind(pemitname);
	flck_unlock_noshared_mutex(&lockval);			// UNLOCK

	return cbfunc;
}

bool StackEmitCB::Set(const char* pemitname, Nan::Callback* cbfunc)
{
	string	stremit = pemitname ? pemitname : "";
	if(stremit.empty()){
		return false;
	}

	while(!flck_trylock_noshared_mutex(&lockval));	// LOCK

	const Nan::Callback*	oldcbfunc = RawFind(pemitname);
	if(oldcbfunc){
		EmitCbsMap.erase(stremit);
	}
	if(cbfunc){
		EmitCbsMap[stremit] = cbfunc;
	}
	flck_unlock_noshared_mutex(&lockval);			// UNLOCK

	return true;
}

bool StackEmitCB::Unset(const char* pemitname)
{
	string	stremit = pemitname ? pemitname : "";
	if(stremit.empty()){
		return false;
	}

	while(!flck_trylock_noshared_mutex(&lockval));	// LOCK

	const Nan::Callback*	oldcbfunc = RawFind(pemitname);
	if(oldcbfunc){
		EmitCbsMap.erase(stremit);
	}
	flck_unlock_noshared_mutex(&lockval);			// UNLOCK

	return true;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
