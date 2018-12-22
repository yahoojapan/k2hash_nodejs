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
 * the license file that was distributed with this source code.
 *
 * AUTHOR:   Taku Ishihara
 * CREATE:   Tue 10 Mar 2015
 * REVISION:
 */

#ifndef K2H_SHM_H
#define K2H_SHM_H

#include "k2h_common.h"
#include "k2h_cbs.h"

//---------------------------------------------------------
// K2hNode Class
//---------------------------------------------------------
class K2hNode : public Nan::ObjectWrap
{
	friend class	K2hQueue;
	friend class	K2hKeyQueue;

	public:
		static void Init(void);
		static NAN_METHOD(NewInstance);

	private:
		explicit K2hNode(void);
		~K2hNode();

		static NAN_METHOD(New);

		static NAN_METHOD(On);
		static NAN_METHOD(OnCreate);
		static NAN_METHOD(OnOpen);
		static NAN_METHOD(OnOpenRW);
		static NAN_METHOD(OnOpenRO);
		static NAN_METHOD(OnOpenTempfile);
		static NAN_METHOD(OnOpenMem);
		static NAN_METHOD(OnClose);
		static NAN_METHOD(OnGet);
		static NAN_METHOD(OnGetSubkeys);
		static NAN_METHOD(OnGetAttrs);
		static NAN_METHOD(OnGetAttrValue);
		static NAN_METHOD(OnSet);
		static NAN_METHOD(OnAddSubkey);
		static NAN_METHOD(OnAddSubkeys);
		static NAN_METHOD(OnAddAttr);
		static NAN_METHOD(OnRemove);
		static NAN_METHOD(OnRemoveAll);
		static NAN_METHOD(OnLoad);
		static NAN_METHOD(OnPut);
		static NAN_METHOD(Off);
		static NAN_METHOD(OffCreate);
		static NAN_METHOD(OffOpen);
		static NAN_METHOD(OffOpenRW);
		static NAN_METHOD(OffOpenRO);
		static NAN_METHOD(OffOpenTempfile);
		static NAN_METHOD(OffOpenMem);
		static NAN_METHOD(OffClose);
		static NAN_METHOD(OffGet);
		static NAN_METHOD(OffGetSubkeys);
		static NAN_METHOD(OffGetAttrs);
		static NAN_METHOD(OffGetAttrValue);
		static NAN_METHOD(OffSet);
		static NAN_METHOD(OffAddSubkey);
		static NAN_METHOD(OffAddSubkeys);
		static NAN_METHOD(OffAddAttr);
		static NAN_METHOD(OffRemove);
		static NAN_METHOD(OffRemoveAll);
		static NAN_METHOD(OffLoad);
		static NAN_METHOD(OffPut);

		static NAN_METHOD(Create);
		static NAN_METHOD(Open);
		static NAN_METHOD(OpenRW);
		static NAN_METHOD(OpenRO);
		static NAN_METHOD(OpenTempfile);
		static NAN_METHOD(OpenMem);
		static NAN_METHOD(Close);

		static NAN_METHOD(GetValue);
		static NAN_METHOD(GetSubkeys);

		static NAN_METHOD(SetValue);
		static NAN_METHOD(AddSubkey);
		static NAN_METHOD(AddSubkeys);

		static NAN_METHOD(Remove);
		static NAN_METHOD(RemoveAll);

		static NAN_METHOD(PrintState);
		static NAN_METHOD(PrintVersion);

		static NAN_METHOD(DumpHead);
		static NAN_METHOD(DumpKeytable);
		static NAN_METHOD(DumpFullKeytable);
		static NAN_METHOD(DumpElementtable);
		static NAN_METHOD(DumpFull);

		static NAN_METHOD(Transaction);
		static NAN_METHOD(EnableTransaction);
		static NAN_METHOD(DisableTransaction);

		static NAN_METHOD(GetTransactionThreadPool);
		static NAN_METHOD(SetTransactionThreadPool);
		static NAN_METHOD(UnsetTransactionThreadPool);

		static NAN_METHOD(PutArchive);
		static NAN_METHOD(LoadArchive);

		static NAN_METHOD(getQueue);
		static NAN_METHOD(getKeyQueue);

		static NAN_METHOD(SetCommonAttribute);
		static NAN_METHOD(CleanCommonAttribute);

		static NAN_METHOD(AddAttrPluginLib);
		static NAN_METHOD(AddAttrCryptPass);

		static NAN_METHOD(GetAttrVersionInfos);
		static NAN_METHOD(GetAttrInfos);
		static NAN_METHOD(GetAttrs);
		static NAN_METHOD(GetAttrValue);

		static NAN_METHOD(AddAttr);

	private:
		static Nan::Persistent<v8::Function>	constructor;
		K2HShm									_k2hshm;
		StackEmitCB								_cbs;
};

#endif

/*
 * VIM modelines
 *
 * vim:set ts=4 fenc=utf-8:
 */
