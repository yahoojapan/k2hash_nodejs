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

#ifndef K2H_SHM_H
#define K2H_SHM_H

#include "k2h_common.h"
#include "k2h_cbs.h"

//---------------------------------------------------------
// K2hNode Class
//---------------------------------------------------------
class K2hNode : public Napi::ObjectWrap<K2hNode>
{
	friend class	K2hQueue;
	friend class	K2hKeyQueue;

	public:
		static void Init(Napi::Env env, Napi::Object exports);
		static Napi::Object NewInstance(Napi::Env env);

		// Constructor / Destructor
		explicit K2hNode(const Napi::CallbackInfo& info);
		~K2hNode();

	private:
		Napi::Value New(const Napi::CallbackInfo& info);

		Napi::Value On(const Napi::CallbackInfo& info);
		Napi::Value OnCreate(const Napi::CallbackInfo& info);
		Napi::Value OnOpen(const Napi::CallbackInfo& info);
		Napi::Value OnOpenRW(const Napi::CallbackInfo& info);
		Napi::Value OnOpenRO(const Napi::CallbackInfo& info);
		Napi::Value OnOpenTempfile(const Napi::CallbackInfo& info);
		Napi::Value OnOpenMem(const Napi::CallbackInfo& info);
		Napi::Value OnClose(const Napi::CallbackInfo& info);
		Napi::Value OnGet(const Napi::CallbackInfo& info);
		Napi::Value OnGetSubkeys(const Napi::CallbackInfo& info);
		Napi::Value OnGetAttrs(const Napi::CallbackInfo& info);
		Napi::Value OnGetAttrValue(const Napi::CallbackInfo& info);
		Napi::Value OnSet(const Napi::CallbackInfo& info);
		Napi::Value OnAddSubkey(const Napi::CallbackInfo& info);
		Napi::Value OnAddSubkeys(const Napi::CallbackInfo& info);
		Napi::Value OnAddAttr(const Napi::CallbackInfo& info);
		Napi::Value OnRemove(const Napi::CallbackInfo& info);
		Napi::Value OnRemoveAll(const Napi::CallbackInfo& info);
		Napi::Value OnLoad(const Napi::CallbackInfo& info);
		Napi::Value OnPut(const Napi::CallbackInfo& info);
		Napi::Value Off(const Napi::CallbackInfo& info);
		Napi::Value OffCreate(const Napi::CallbackInfo& info);
		Napi::Value OffOpen(const Napi::CallbackInfo& info);
		Napi::Value OffOpenRW(const Napi::CallbackInfo& info);
		Napi::Value OffOpenRO(const Napi::CallbackInfo& info);
		Napi::Value OffOpenTempfile(const Napi::CallbackInfo& info);
		Napi::Value OffOpenMem(const Napi::CallbackInfo& info);
		Napi::Value OffClose(const Napi::CallbackInfo& info);
		Napi::Value OffGet(const Napi::CallbackInfo& info);
		Napi::Value OffGetSubkeys(const Napi::CallbackInfo& info);
		Napi::Value OffGetAttrs(const Napi::CallbackInfo& info);
		Napi::Value OffGetAttrValue(const Napi::CallbackInfo& info);
		Napi::Value OffSet(const Napi::CallbackInfo& info);
		Napi::Value OffAddSubkey(const Napi::CallbackInfo& info);
		Napi::Value OffAddSubkeys(const Napi::CallbackInfo& info);
		Napi::Value OffAddAttr(const Napi::CallbackInfo& info);
		Napi::Value OffRemove(const Napi::CallbackInfo& info);
		Napi::Value OffRemoveAll(const Napi::CallbackInfo& info);
		Napi::Value OffLoad(const Napi::CallbackInfo& info);
		Napi::Value OffPut(const Napi::CallbackInfo& info);

		Napi::Value Create(const Napi::CallbackInfo& info);
		Napi::Value Open(const Napi::CallbackInfo& info);
		Napi::Value OpenRW(const Napi::CallbackInfo& info);
		Napi::Value OpenRO(const Napi::CallbackInfo& info);
		Napi::Value OpenTempfile(const Napi::CallbackInfo& info);
		Napi::Value OpenMem(const Napi::CallbackInfo& info);
		Napi::Value Close(const Napi::CallbackInfo& info);

		Napi::Value GetValue(const Napi::CallbackInfo& info);
		Napi::Value GetSubkeys(const Napi::CallbackInfo& info);

		Napi::Value SetValue(const Napi::CallbackInfo& info);
		Napi::Value AddSubkey(const Napi::CallbackInfo& info);
		Napi::Value AddSubkeys(const Napi::CallbackInfo& info);

		Napi::Value Remove(const Napi::CallbackInfo& info);
		Napi::Value RemoveAll(const Napi::CallbackInfo& info);

		Napi::Value PrintState(const Napi::CallbackInfo& info);
		Napi::Value PrintVersion(const Napi::CallbackInfo& info);

		Napi::Value DumpHead(const Napi::CallbackInfo& info);
		Napi::Value DumpKeytable(const Napi::CallbackInfo& info);
		Napi::Value DumpFullKeytable(const Napi::CallbackInfo& info);
		Napi::Value DumpElementtable(const Napi::CallbackInfo& info);
		Napi::Value DumpFull(const Napi::CallbackInfo& info);

		Napi::Value Transaction(const Napi::CallbackInfo& info);
		Napi::Value EnableTransaction(const Napi::CallbackInfo& info);
		Napi::Value DisableTransaction(const Napi::CallbackInfo& info);

		Napi::Value GetTransactionThreadPool(const Napi::CallbackInfo& info);
		Napi::Value SetTransactionThreadPool(const Napi::CallbackInfo& info);
		Napi::Value UnsetTransactionThreadPool(const Napi::CallbackInfo& info);

		Napi::Value PutArchive(const Napi::CallbackInfo& info);
		Napi::Value LoadArchive(const Napi::CallbackInfo& info);

		Napi::Value getQueue(const Napi::CallbackInfo& info);
		Napi::Value getKeyQueue(const Napi::CallbackInfo& info);

		Napi::Value SetCommonAttribute(const Napi::CallbackInfo& info);
		Napi::Value CleanCommonAttribute(const Napi::CallbackInfo& info);

		Napi::Value AddAttrPluginLib(const Napi::CallbackInfo& info);
		Napi::Value AddAttrCryptPass(const Napi::CallbackInfo& info);

		Napi::Value GetAttrVersionInfos(const Napi::CallbackInfo& info);
		Napi::Value GetAttrInfos(const Napi::CallbackInfo& info);
		Napi::Value GetAttrs(const Napi::CallbackInfo& info);
		Napi::Value GetAttrValue(const Napi::CallbackInfo& info);

		Napi::Value AddAttr(const Napi::CallbackInfo& info);

	public:
		// constructor reference
		static Napi::FunctionReference	constructor;

		StackEmitCB	_cbs;

	private:
		K2HShm		_k2hshm;
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
