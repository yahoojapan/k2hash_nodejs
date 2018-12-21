/*
 * K2HASH
 *
 * Copyright 2015 Yahoo! JAPAN corporation.
 *
 * K2HASH is key-valuew store base libraries.
 * K2HASH is made for the purpose of the construction of
 * original KVS system and the offer of the library.
 * The characteristic is this KVS library which Key can
 * layer. And can support multi-processing and multi-thread,
 * and is provided safely as available KVS.
 *
 * For the full copyright and license information, please view
 * the LICENSE file that was distributed with this source code.
 *
 * AUTHOR:   nakatani@yahoo-corp.jp
 * CREATE:   Fri Mar 3 2017
 * REVISION:
 */

#ifndef K2H_CBS_H
#define K2H_CBS_H

#include "k2h_common.h"

//---------------------------------------------------------
// Typedefs
//---------------------------------------------------------
typedef std::map<std::string, Nan::Callback*>	cbsmap;

//---------------------------------------------------------
// StackEmitCB Class
//---------------------------------------------------------
class StackEmitCB
{
	protected:
		cbsmap			EmitCbsMap;
		volatile int	lockval;				// lock variable for mapping

	protected:
		Nan::Callback* RawFind(const char* pemitname);

	public:
		StackEmitCB();
		virtual ~StackEmitCB();

		Nan::Callback* Find(const char* pemitname);
		bool Set(const char* pemitname, Nan::Callback* cbfunc);
		bool Unset(const char* pemitname);
};

//---------------------------------------------------------
// Utility
//---------------------------------------------------------
inline const char* GetNormalizationEmitter(const char* emitter, const char*	emitters[])
{
	if(!emitter){
		return NULL;
	}
	for(const char** ptmp = &emitters[0]; ptmp && *ptmp; ++ptmp){
		if(0 == strcasecmp(*ptmp, emitter)){
			return *ptmp;
		}
	}
	return NULL;
}

#endif

/*
 * VIM modelines
 *
 * vim:set ts=4 fenc=utf-8:
 */
