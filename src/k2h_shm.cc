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

#include "k2h_shm.h"
#include "k2h_shm_async.h"
#include "k2h_queue.h"
#include "k2h_keyqueue.h"

using namespace v8;
using namespace std;

//---------------------------------------------------------
// Symbols
//---------------------------------------------------------
#define K2H_VAL_ATTR_DEFAULT			-1
#define K2H_VAL_ATTR_DISABLE			0
#define K2H_VAL_ATTR_ENABLE				1

//---------------------------------------------------------
// Emitter
//---------------------------------------------------------
#define	K2H_EMITTER_POS_CREATE			(0)
#define	K2H_EMITTER_POS_OPEN			(K2H_EMITTER_POS_CREATE		+ 1)
#define	K2H_EMITTER_POS_OPENRW			(K2H_EMITTER_POS_OPEN		+ 1)
#define	K2H_EMITTER_POS_OPENRO			(K2H_EMITTER_POS_OPENRW		+ 1)
#define	K2H_EMITTER_POS_OPENTEMP		(K2H_EMITTER_POS_OPENRO		+ 1)
#define	K2H_EMITTER_POS_OPENMEM			(K2H_EMITTER_POS_OPENTEMP	+ 1)
#define	K2H_EMITTER_POS_CLOSE			(K2H_EMITTER_POS_OPENMEM	+ 1)
#define	K2H_EMITTER_POS_GET				(K2H_EMITTER_POS_CLOSE		+ 1)
#define	K2H_EMITTER_POS_GETSUBKEYS		(K2H_EMITTER_POS_GET		+ 1)
#define	K2H_EMITTER_POS_GETATTRS		(K2H_EMITTER_POS_GETSUBKEYS	+ 1)
#define	K2H_EMITTER_POS_GETATTRVAL		(K2H_EMITTER_POS_GETATTRS	+ 1)
#define	K2H_EMITTER_POS_SET				(K2H_EMITTER_POS_GETATTRVAL	+ 1)
#define	K2H_EMITTER_POS_ADDSUBKEY		(K2H_EMITTER_POS_SET		+ 1)
#define	K2H_EMITTER_POS_ADDSUBKEYS		(K2H_EMITTER_POS_ADDSUBKEY	+ 1)
#define	K2H_EMITTER_POS_ADDATTR			(K2H_EMITTER_POS_ADDSUBKEYS	+ 1)
#define	K2H_EMITTER_POS_REMOVE			(K2H_EMITTER_POS_ADDATTR	+ 1)
#define	K2H_EMITTER_POS_REMOVEALL		(K2H_EMITTER_POS_REMOVE		+ 1)
#define	K2H_EMITTER_POS_LOAD			(K2H_EMITTER_POS_REMOVEALL	+ 1)
#define	K2H_EMITTER_POS_PUT				(K2H_EMITTER_POS_LOAD		+ 1)

const char*	stc_k2h_emitters[] = {
	"create",
	"open",
	"openRW",
	"openRO",
	"openTempfile",
	"openMem",
	"close",
	"get",
	"getSubkeys",
	"getAttrs",
	"getAttrValue",
	"set",
	"addSubkey",
	"addSubkeys",
	"addAttr",
	"remove",
	"removeAll",
	"load",
	"put",
	NULL
};

//---------------------------------------------------------
// Utility macros
//---------------------------------------------------------
#define	SetK2hEmitterCallback(info, pos, pemitter) \
		{ \
			K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This()); \
			if(info.Length() <= pos){ \
				Nan::ThrowSyntaxError("No callback is specified."); \
				return; \
			} \
			Nan::Callback* cb = new Nan::Callback(); \
			cb->SetFunction(info[pos].As<v8::Function>()); \
			bool	result = obj->_cbs.Set(pemitter, cb); \
			info.GetReturnValue().Set(Nan::New(result)); \
		}

#define	UnsetK2hEmitterCallback(info, pemitter) \
		{ \
			K2hNode*	obj		= Nan::ObjectWrap::Unwrap<K2hNode>(info.This()); \
			bool		result	= obj->_cbs.Unset(pemitter); \
			info.GetReturnValue().Set(Nan::New(result)); \
		}

//---------------------------------------------------------
// K2hNode Class
//---------------------------------------------------------
Nan::Persistent<Function> K2hNode::constructor;

//---------------------------------------------------------
// K2hNode Methods
//---------------------------------------------------------
K2hNode::K2hNode() : _k2hshm(), _cbs()
{
}

K2hNode::~K2hNode()
{
	_k2hshm.Detach();
}

void K2hNode::Init(void)
{
	// Prepare constructor template
	Local<FunctionTemplate>	tpl = Nan::New<FunctionTemplate>(New);
	tpl->SetClassName(Nan::New("K2hNode").ToLocalChecked()); 
	tpl->InstanceTemplate()->SetInternalFieldCount(1); 

	// Prototype for event emitter
	Nan::SetPrototypeMethod(tpl, "on",							On);
	Nan::SetPrototypeMethod(tpl, "onCreate",					OnCreate);
	Nan::SetPrototypeMethod(tpl, "onOpen",						OnOpen);
	Nan::SetPrototypeMethod(tpl, "onOpenRW",					OnOpenRW);
	Nan::SetPrototypeMethod(tpl, "onOpenRO",					OnOpenRO);
	Nan::SetPrototypeMethod(tpl, "onOpenTempfile",				OnOpenTempfile);
	Nan::SetPrototypeMethod(tpl, "onOpenMem",					OnOpenMem);
	Nan::SetPrototypeMethod(tpl, "onClose",						OnClose);
	Nan::SetPrototypeMethod(tpl, "onGet",						OnGet);
	Nan::SetPrototypeMethod(tpl, "onGetSubkeys",				OnGetSubkeys);
	Nan::SetPrototypeMethod(tpl, "onGetAttrs",					OnGetAttrs);
	Nan::SetPrototypeMethod(tpl, "onGetAttrValue",				OnGetAttrValue);
	Nan::SetPrototypeMethod(tpl, "onSet",						OnSet);
	Nan::SetPrototypeMethod(tpl, "onAddSubkey",					OnAddSubkey);
	Nan::SetPrototypeMethod(tpl, "onAddSubkeys",				OnAddSubkeys);
	Nan::SetPrototypeMethod(tpl, "onAddAttr",					OnAddAttr);
	Nan::SetPrototypeMethod(tpl, "onRemove",					OnRemove);
	Nan::SetPrototypeMethod(tpl, "onRemoveAll",					OnRemoveAll);
	Nan::SetPrototypeMethod(tpl, "onLoad",						OnLoad);
	Nan::SetPrototypeMethod(tpl, "onLoadArchive",				OnLoad);
	Nan::SetPrototypeMethod(tpl, "onPut",						OnPut);
	Nan::SetPrototypeMethod(tpl, "onPutArchive",				OnPut);
	Nan::SetPrototypeMethod(tpl, "off",							Off);
	Nan::SetPrototypeMethod(tpl, "offCreate",					OffCreate);
	Nan::SetPrototypeMethod(tpl, "offOpen",						OffOpen);
	Nan::SetPrototypeMethod(tpl, "offOpenRW",					OffOpenRW);
	Nan::SetPrototypeMethod(tpl, "offOpenRO",					OffOpenRO);
	Nan::SetPrototypeMethod(tpl, "offOpenTempfile",				OffOpenTempfile);
	Nan::SetPrototypeMethod(tpl, "offOpenMem",					OffOpenMem);
	Nan::SetPrototypeMethod(tpl, "offClose",					OffClose);
	Nan::SetPrototypeMethod(tpl, "offGet",						OffGet);
	Nan::SetPrototypeMethod(tpl, "offGetSubkeys",				OffGetSubkeys);
	Nan::SetPrototypeMethod(tpl, "offGetAttrs",					OffGetAttrs);
	Nan::SetPrototypeMethod(tpl, "offGetAttrValue",				OffGetAttrValue);
	Nan::SetPrototypeMethod(tpl, "offSet",						OffSet);
	Nan::SetPrototypeMethod(tpl, "offAddSubkey",				OffAddSubkey);
	Nan::SetPrototypeMethod(tpl, "offAddSubkeys",				OffAddSubkeys);
	Nan::SetPrototypeMethod(tpl, "offAddAttr",					OffAddAttr);
	Nan::SetPrototypeMethod(tpl, "offRemove",					OffRemove);
	Nan::SetPrototypeMethod(tpl, "offRemoveAll",				OffRemoveAll);
	Nan::SetPrototypeMethod(tpl, "offLoad",						OffLoad);
	Nan::SetPrototypeMethod(tpl, "offLoadArchive",				OffLoad);
	Nan::SetPrototypeMethod(tpl, "offPut",						OffPut);
	Nan::SetPrototypeMethod(tpl, "offPutArchive",				OffPut);

	// Prototype
	Nan::SetPrototypeMethod(tpl, "create",						Create);
	Nan::SetPrototypeMethod(tpl, "open",						Open);
	Nan::SetPrototypeMethod(tpl, "openRW",						OpenRW);
	Nan::SetPrototypeMethod(tpl, "openRO",						OpenRO);
	Nan::SetPrototypeMethod(tpl, "openTempfile",				OpenTempfile);
	Nan::SetPrototypeMethod(tpl, "openMem",						OpenMem);
	Nan::SetPrototypeMethod(tpl, "close",						Close);

	Nan::SetPrototypeMethod(tpl, "getValue",					GetValue);
	Nan::SetPrototypeMethod(tpl, "getSubkeys",					GetSubkeys);

	Nan::SetPrototypeMethod(tpl, "setValue",					SetValue);
	Nan::SetPrototypeMethod(tpl, "addSubkey",					AddSubkey);
	Nan::SetPrototypeMethod(tpl, "addSubkeys",					AddSubkeys);

	Nan::SetPrototypeMethod(tpl, "remove",						Remove);
	Nan::SetPrototypeMethod(tpl, "removeAll",					RemoveAll);

	Nan::SetPrototypeMethod(tpl, "printState",					PrintState);
	Nan::SetPrototypeMethod(tpl, "printVersion",				PrintVersion);

	Nan::SetPrototypeMethod(tpl, "dumpHead",					DumpHead);
	Nan::SetPrototypeMethod(tpl, "dumpKeytable",				DumpKeytable);
	Nan::SetPrototypeMethod(tpl, "dumpFullKeytable",			DumpFullKeytable);
	Nan::SetPrototypeMethod(tpl, "dumpElementtable",			DumpElementtable);
	Nan::SetPrototypeMethod(tpl, "dumpFull",					DumpFull);

	Nan::SetPrototypeMethod(tpl, "transaction",					Transaction);
	Nan::SetPrototypeMethod(tpl, "enableTransaction",			EnableTransaction);
	Nan::SetPrototypeMethod(tpl, "disableTransaction",			DisableTransaction);

	Nan::SetPrototypeMethod(tpl, "getTransactionThreadPool",	GetTransactionThreadPool);
	Nan::SetPrototypeMethod(tpl, "setTransactionThreadPool",	SetTransactionThreadPool);
	Nan::SetPrototypeMethod(tpl, "unsetTransactionThreadPool",	UnsetTransactionThreadPool);

	Nan::SetPrototypeMethod(tpl, "putArchive",					PutArchive);
	Nan::SetPrototypeMethod(tpl, "loadArchive",					LoadArchive);

	Nan::SetPrototypeMethod(tpl, "getQueue",					getQueue);
	Nan::SetPrototypeMethod(tpl, "getKeyQueue",					getKeyQueue);

	Nan::SetPrototypeMethod(tpl, "setCommonAttribute",			SetCommonAttribute);
	Nan::SetPrototypeMethod(tpl, "cleanCommonAttribute",		CleanCommonAttribute);

	Nan::SetPrototypeMethod(tpl, "addAttrPluginLib",			AddAttrPluginLib);
	Nan::SetPrototypeMethod(tpl, "addAttrCryptPass",			AddAttrCryptPass);

	Nan::SetPrototypeMethod(tpl, "getAttrVersionInfos",			GetAttrVersionInfos);
	Nan::SetPrototypeMethod(tpl, "getAttrInfos",				GetAttrInfos);
	Nan::SetPrototypeMethod(tpl, "getAttrs",					GetAttrs);
	Nan::SetPrototypeMethod(tpl, "getAttrValue",				GetAttrValue);

	Nan::SetPrototypeMethod(tpl, "addAttr",						AddAttr);

	// Reset
	constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(K2hNode::New)
{
	if(info.IsConstructCall()){ 
		// Invoked as constructor: new K2hNode()
		K2hNode* obj = new K2hNode();
		obj->Wrap(info.This()); 
		info.GetReturnValue().Set(info.This()); 
	}else{ 
		// Invoked as plain function K2hNode(), turn into construct call. 
		const int		argc		= 1;
		Local<Value>	argv[argc]	= {info[0]}; 
		Local<Function>	cons		= Nan::New<Function>(constructor); 
		info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked()); 
	}
}

NAN_METHOD(K2hNode::NewInstance)
{
	const unsigned	argc		= 1;
	Local<Value>	argv[argc]	= {info[0]}; 
	Local<Function>	cons		= Nan::New<Function>(constructor); 
	info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
}

/**
 * @mainpage k2hash_nodejs
 */

/// \defgroup nodejs_methods	the methods for using from node.js
//@{

/**
 * @memberof K2hNode
 * @fn void\
 * On(\
 * 	String	emitter\
 * 	, Callback cbfunc\
 * )
 * @brief	set callback handling
 *
 * @param[in] emitter			Specify emitter name
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::On)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No handle emitter name is specified.");
		return;
	}else if(info.Length() < 2){
		Nan::ThrowSyntaxError("No callback is specified.");
		return;
	}

	// check emitter name
	Nan::Utf8String	emitter(info[0]);
	const char*		pemitter;
	if(NULL == (pemitter = GetNormalizationEmitter(*emitter, stc_k2h_emitters))){
		string	msg = "Unknown ";
		msg			+= *emitter;
		msg			+= " emitter";
		Nan::ThrowSyntaxError(msg.c_str());
		return;
	}
	// add callback
	SetK2hEmitterCallback(info, 1, pemitter);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnCreate(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnCreate)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_CREATE]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnOpen(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnOpen)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_OPEN]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnOpenRW(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnOpenRW)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_OPENRW]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnOpenRO(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnOpenRO)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_OPENRO]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnOpenTempfile(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnOpenTempfile)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_OPENTEMP]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnOpenMem(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnOpenMem)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_OPENMEM]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnClose(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnClose)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_CLOSE]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnGet(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnGet)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_GET]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnGetSubkeys(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnGetSubkeys)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_GETSUBKEYS]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnGetAttrs(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnGetAttrs)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_GETATTRS]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnGetAttrValue(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnGetAttrValue)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_GETATTRVAL]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnSet(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnSet)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_SET]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnAddSubkey(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnAddSubkey)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_ADDSUBKEY]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnAddSubkeys(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnAddSubkeys)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_ADDSUBKEYS]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnAddAttr(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnAddAttr)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_ADDATTR]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnRemove(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnRemove)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_REMOVE]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnRemoveAll(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnRemoveAll)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_REMOVEALL]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnLoad(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnLoad)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_LOAD]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OnPut(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OnPut)
{
	SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_PUT]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * Off(\
 * 	String	emitter\
 * )
 * @brief	set callback handling
 *
 * @param[in] emitter			Specify emitter name
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::Off)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No handle emitter name is specified.");
		return;
	}

	// check emitter name
	Nan::Utf8String	emitter(info[0]);
	const char*		pemitter;
	if(NULL == (pemitter = GetNormalizationEmitter(*emitter, stc_k2h_emitters))){
		string	msg = "Unknown ";
		msg			+= *emitter;
		msg			+= " emitter";
		Nan::ThrowSyntaxError(msg.c_str());
		return;
	}
	// unset callback
	UnsetK2hEmitterCallback(info, pemitter);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffCreate(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffCreate)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_CREATE]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffOpen(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffOpen)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_OPEN]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffOpenRW(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffOpenRW)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_OPENRW]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffOpenRO(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffOpenRO)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_OPENRO]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffOpenTempfile(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffOpenTempfile)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_OPENTEMP]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffOpenMem(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffOpenMem)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_OPENMEM]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffClose(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffClose)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_CLOSE]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffGet(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffGet)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_GET]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffGetSubkeys(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffGetSubkeys)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_GETSUBKEYS]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffGetAttrs(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffGetAttrs)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_GETATTRS]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffGetAttrValue(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffGetAttrValue)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_GETATTRVAL]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffSet(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffSet)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_SET]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffAddSubkey(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffAddSubkey)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_ADDSUBKEY]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffAddSubkeys(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffAddSubkeys)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_ADDSUBKEYS]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffAddAttr(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffAddAttr)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_ADDATTR]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffRemove(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffRemove)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_REMOVE]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffRemoveAll(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffRemoveAll)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_REMOVEALL]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffLoad(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffLoad)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_LOAD]);
}

/**
 * @memberof K2hNode
 * @fn void\
 * OffPut(\
 * )
 * @brief	set callback handling for initializing k2hash object
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::OffPut)
{
	UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_PUT]);
}

/**
 * @memberof K2hNode
 * @fn bool Create(\
 *	String filename\
 * 	, int maskbitcnt=2\
 * 	, int cmaskbitcnt=4\
 * 	, int maxelementcnt=32\
 * 	, int pagesize=128\
 * 	, Callback cbfunc=null\
 * )
 * @brief	Create K2HASH file
 *
 *	If the callback function is specified, or on callback handles for this,
 *  this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] filename		Specify K2HASH file path
 * @param[in] maskbitcnt	Specify initial mask bit count(must over 2).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] cmaskbitcnt	Specify initial collision mask bit count(must over 4).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] maxelementcnt	Specify initial element count(should be over 32).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] pagesize		Specify page size which is data block size(must over 128)
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] cbfunc		callback function.
 *
 * @return return true for success, false for failure
 *
 * @par Usage(Node.js):
 * @code
 * var k2hash = require('bindings')('k2hash') ;
 * var k2h = k2hash() ;
 * k2h.Create('/tmp/zz.k2h') ;
 * k2h.SetValue('key','val') ;
 * console_log( k2h.GetValue('key') ) ;
 * k2h.Close() ;
 * @endcode
 *
 */

NAN_METHOD(K2hNode::Create)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No file name is specified.");
	}

	K2hNode*		obj				= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	Nan::Utf8String	filename(info[0]);
	bool			isfullmapping	= true;
	int				mask_bitcnt		= K2HShm::MIN_MASK_BITCOUNT;
	int				cmask_bitcnt	= K2HShm::DEFAULT_COLLISION_MASK_BITCOUNT;
	int				max_element_cnt	= K2HShm::DEFAULT_MAX_ELEMENT_CNT;
	size_t			pagesize		= K2HShm::MIN_PAGE_SIZE;
	Nan::Callback*	callback		= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_CREATE]);

	if(1 < info.Length()){
		if(info[1]->IsFunction()){
			if(2 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[1].As<v8::Function>());
		}else{
			isfullmapping	= Nan::To<bool>(info[1]).ToChecked();
		}
	}
	if(2 < info.Length()){
		if(info[2]->IsFunction()){
			if(3 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[2].As<v8::Function>());
		}else{
			mask_bitcnt		= Nan::To<int>(info[2]).ToChecked();
		}
	}
	if(3 < info.Length()){
		if(info[3]->IsFunction()){
			if(4 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[3].As<v8::Function>());
		}else{
			cmask_bitcnt	= Nan::To<int>(info[3]).ToChecked();
		}
	}
	if(4 < info.Length()){
		if(info[4]->IsFunction()){
			if(5 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[4].As<v8::Function>());
		}else{
			max_element_cnt	= Nan::To<int>(info[4]).ToChecked();
		}
	}
	if(5 < info.Length()){
		if(info[5]->IsFunction()){
			if(6 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[5].As<v8::Function>());
		}else{
			pagesize		= Nan::To<int>(info[5]).ToChecked();
		}
	}
	if(6 < info.Length()){
		if(7 < info.Length() || !info[6]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback			= new Nan::Callback(info[6].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new CreateWorker(callback, &(obj->_k2hshm), *filename, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(obj->_k2hshm.Create(*filename, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize)));
	}
}

/**
 * @memberof K2hNode
 * @fn bool Open(\
 *	String filename\
 * 	, bool readonly=false\
 * 	, bool removefile=false\
 * 	, bool fullmap=true\
 * 	, int maskbitcnt=2\
 * 	, int cmaskbitcnt=4\
 * 	, int maxelementcnt=32\
 * 	, int pagesize=128\
 * 	, Callback cbfunc=null\
 * )
 * @brief	Open(attach) K2HASH file
 *
 *	If the callback function is specified, or on callback handles for this,
 *  this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] filename		Specify the K2HASH file path for open.
 * @param[in] readonly		Specify true for read only, false is read/write mode.
 * @param[in] removefile	Specify true for removing K2HASH file automatically when
 *							there is no processes to attaching K2HASH file.
 * @param[in] fullmap		Specify true for full memory mapping. false for mapping only
 *							index area in K2HASH.
 * @param[in] maskbitcnt	Specify initial mask bit count(must over 2).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] cmaskbitcnt	Specify initial collision mask bit count(must over 4).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] maxelementcnt	Specify initial element count(should be over 32).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] pagesize		Specify page size which is data block size(must over 128)
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] cbfunc		callback function.
 *
 * @return return true for success, false for failure
 *
 */

NAN_METHOD(K2hNode::Open)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No file name is specified.");
	}

	K2hNode*		obj				= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	Nan::Utf8String	filename(info[0]);
	bool			isreadonly		= false;
	bool			istempfile		= false;
	bool			isfullmapping	= true;
	int				mask_bitcnt		= K2HShm::MIN_MASK_BITCOUNT;
	int				cmask_bitcnt	= K2HShm::DEFAULT_COLLISION_MASK_BITCOUNT;
	int				max_element_cnt	= K2HShm::DEFAULT_MAX_ELEMENT_CNT;
	size_t			pagesize		= K2HShm::MIN_PAGE_SIZE;
	Nan::Callback*	callback		= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_OPEN]);

	if(1 < info.Length()){
		if(info[1]->IsFunction()){
			if(2 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[1].As<v8::Function>());
		}else{
			isreadonly		= Nan::To<bool>(info[1]).ToChecked();
		}
	}
	if(2 < info.Length()){
		if(info[2]->IsFunction()){
			if(3 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[2].As<v8::Function>());
		}else{
			istempfile		= Nan::To<bool>(info[2]).ToChecked();
		}
	}
	if(3 < info.Length()){
		if(info[3]->IsFunction()){
			if(4 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[3].As<v8::Function>());
		}else{
			isfullmapping	= Nan::To<bool>(info[3]).ToChecked();
		}
	}
	if(4 < info.Length()){
		if(info[4]->IsFunction()){
			if(5 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[4].As<v8::Function>());
		}else{
			mask_bitcnt		= Nan::To<int>(info[4]).ToChecked();
		}
	}
	if(5 < info.Length()){
		if(info[5]->IsFunction()){
			if(6 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[5].As<v8::Function>());
		}else{
			cmask_bitcnt	= Nan::To<int>(info[5]).ToChecked();
		}
	}
	if(6 < info.Length()){
		if(info[6]->IsFunction()){
			if(7 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[6].As<v8::Function>());
		}else{
			max_element_cnt	= Nan::To<int>(info[6]).ToChecked();
		}
	}
	if(7 < info.Length()){
		if(info[7]->IsFunction()){
			if(8 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[7].As<v8::Function>());
		}else{
			pagesize		= Nan::To<int>(info[7]).ToChecked();
		}
	}
	if(8 < info.Length()){
		if(9 < info.Length() || !info[8]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback			= new Nan::Callback(info[8].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new OpenWorker(callback, &(obj->_k2hshm), *filename, isreadonly, false, istempfile, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(obj->_k2hshm.Attach(*filename, isreadonly, false, istempfile, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize)));
	}
}

/**
 * @memberof K2hNode
 * @fn bool OpenRW(\
 *	String filename\
 * 	, bool fullmap=true\
 * 	, int maskbitcnt=2\
 * 	, int cmaskbitcnt=4\
 * 	, int maxelementcnt=32\
 * 	, int pagesize=128\
 * 	, Callback cbfunc=null\
 * )
 * @brief	Open(attach) K2HASH file for read/write mode.
 *
 *	If the callback function is specified, or on callback handles for this,
 *  this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] filename		Specify K2HASH file path
 * @param[in] fullmap		Specify true for full memory mapping. false for mapping only
 *							index area in K2HASH.
 * @param[in] maskbitcnt	Specify initial mask bit count(must over 2).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] cmaskbitcnt	Specify initial collision mask bit count(must over 4).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] maxelementcnt	Specify initial element count(should be over 32).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] pagesize		Specify page size which is data block size(must over 128)
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] cbfunc		callback function.
 *
 * @return return true for success, false for failure
 *
 */

NAN_METHOD(K2hNode::OpenRW)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No file name is specified.");
	}

	K2hNode*		obj				= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	Nan::Utf8String	filename(info[0]);
	bool			isfullmapping	= true;
	int				mask_bitcnt		= K2HShm::MIN_MASK_BITCOUNT;
	int				cmask_bitcnt	= K2HShm::DEFAULT_COLLISION_MASK_BITCOUNT;
	int				max_element_cnt	= K2HShm::DEFAULT_MAX_ELEMENT_CNT;
	size_t			pagesize		= K2HShm::MIN_PAGE_SIZE;
	Nan::Callback*	callback		= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_OPENRW]);

	if(1 < info.Length()){
		if(info[1]->IsFunction()){
			if(2 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[1].As<v8::Function>());
		}else{
			isfullmapping	= Nan::To<bool>(info[1]).ToChecked();
		}
	}
	if(2 < info.Length()){
		if(info[2]->IsFunction()){
			if(3 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[2].As<v8::Function>());
		}else{
			mask_bitcnt		= Nan::To<int>(info[2]).ToChecked();
		}
	}
	if(3 < info.Length()){
		if(info[3]->IsFunction()){
			if(4 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[3].As<v8::Function>());
		}else{
			cmask_bitcnt	= Nan::To<int>(info[3]).ToChecked();
		}
	}
	if(4 < info.Length()){
		if(info[4]->IsFunction()){
			if(5 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[4].As<v8::Function>());
		}else{
			max_element_cnt	= Nan::To<int>(info[4]).ToChecked();
		}
	}
	if(5 < info.Length()){
		if(info[5]->IsFunction()){
			if(6 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[5].As<v8::Function>());
		}else{
			pagesize		= Nan::To<int>(info[5]).ToChecked();
		}
	}
	if(6 < info.Length()){
		if(7 < info.Length() || !info[6]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback			= new Nan::Callback(info[6].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new OpenWorker(callback, &(obj->_k2hshm), *filename, false, false, false, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(obj->_k2hshm.Attach(*filename, false, false, false, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize)));
	}
}

/**
 * @memberof K2hNode
 * @fn bool OpenRO(\
 *	String filename\
 * 	, bool fullmap=true\
 * 	, int maskbitcnt=2\
 * 	, int cmaskbitcnt=4\
 * 	, int maxelementcnt=32\
 * 	, int pagesize=128\
 * 	, Callback cbfunc=null\
 * )
 * @brief	Open(attach) K2HASH file for only read mode.
 *
 *	If the callback function is specified, or on callback handles for this,
 *  this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] filename		Specify K2HASH file path
 * @param[in] fullmap		Specify true for full memory mapping. false for mapping only
 *							index area in K2HASH.
 * @param[in] maskbitcnt	Specify initial mask bit count(must over 2).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] cmaskbitcnt	Specify initial collision mask bit count(must over 4).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] maxelementcnt	Specify initial element count(should be over 32).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] pagesize		Specify page size which is data block size(must over 128)
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] cbfunc		callback function.
 *
 * @return return true for success, false for failure
 *
 */

NAN_METHOD(K2hNode::OpenRO)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No file name is specified.");
	}

	K2hNode*		obj				= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	Nan::Utf8String	filename(info[0]);
	bool			isfullmapping	= true;
	int				mask_bitcnt		= K2HShm::MIN_MASK_BITCOUNT;
	int				cmask_bitcnt	= K2HShm::DEFAULT_COLLISION_MASK_BITCOUNT;
	int				max_element_cnt	= K2HShm::DEFAULT_MAX_ELEMENT_CNT;
	size_t			pagesize		= K2HShm::MIN_PAGE_SIZE;
	Nan::Callback*	callback		= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_OPENRO]);

	if(1 < info.Length()){
		if(info[1]->IsFunction()){
			if(2 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[1].As<v8::Function>());
		}else{
			isfullmapping	= Nan::To<bool>(info[1]).ToChecked();
		}
	}
	if(2 < info.Length()){
		if(info[2]->IsFunction()){
			if(3 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[2].As<v8::Function>());
		}else{
			mask_bitcnt		= Nan::To<int>(info[2]).ToChecked();
		}
	}
	if(3 < info.Length()){
		if(info[3]->IsFunction()){
			if(4 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[3].As<v8::Function>());
		}else{
			cmask_bitcnt	= Nan::To<int>(info[3]).ToChecked();
		}
	}
	if(4 < info.Length()){
		if(info[4]->IsFunction()){
			if(5 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[4].As<v8::Function>());
		}else{
			max_element_cnt	= Nan::To<int>(info[4]).ToChecked();
		}
	}
	if(5 < info.Length()){
		if(info[5]->IsFunction()){
			if(6 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[5].As<v8::Function>());
		}else{
			pagesize		= Nan::To<int>(info[5]).ToChecked();
		}
	}
	if(6 < info.Length()){
		if(7 < info.Length() || !info[6]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback			= new Nan::Callback(info[6].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new OpenWorker(callback, &(obj->_k2hshm), *filename, true, false, false, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(obj->_k2hshm.Attach(*filename, true, false, false, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize)));
	}
}

/**
 * @memberof K2hNode
 * @fn bool OpenTempfile(\
 *	String filename\
 * 	, bool fullmap=true\
 * 	, int maskbitcnt=2\
 * 	, int cmaskbitcnt=4\
 * 	, int maxelementcnt=32\
 * 	, int pagesize=128\
 * 	, Callback cbfunc=null\
 * )
 * @brief	Open(attach) K2HASH file which is like temporary file.
 *			The file is temporary, it means the file is removed automatically when
 *			close it.
 *			If the callback function is specified, or on callback handles for this,
 *			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] filename		Specify K2HASH file path
 * @param[in] fullmap		Specify true for full memory mapping. false for mapping only
 *							index area in K2HASH.
 * @param[in] maskbitcnt	Specify initial mask bit count(must over 2).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] cmaskbitcnt	Specify initial collision mask bit count(must over 4).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] maxelementcnt	Specify initial element count(should be over 32).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] pagesize		Specify page size which is data block size(must over 128)
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] cbfunc		callback function.
 *
 * @return return true for success, false for failure
 *
 */

NAN_METHOD(K2hNode::OpenTempfile)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No file name is specified.");
	}

	K2hNode*		obj				= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	Nan::Utf8String	filename(info[0]);
	bool			isfullmapping	= true;
	int				mask_bitcnt		= K2HShm::MIN_MASK_BITCOUNT;
	int				cmask_bitcnt	= K2HShm::DEFAULT_COLLISION_MASK_BITCOUNT;
	int				max_element_cnt	= K2HShm::DEFAULT_MAX_ELEMENT_CNT;
	size_t			pagesize		= K2HShm::MIN_PAGE_SIZE;
	Nan::Callback*	callback		= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_OPENTEMP]);

	if(1 < info.Length()){
		if(info[1]->IsFunction()){
			if(2 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[1].As<v8::Function>());
		}else{
			isfullmapping	= Nan::To<bool>(info[1]).ToChecked();
		}
	}
	if(2 < info.Length()){
		if(info[2]->IsFunction()){
			if(3 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[2].As<v8::Function>());
		}else{
			mask_bitcnt		= Nan::To<int>(info[2]).ToChecked();
		}
	}
	if(3 < info.Length()){
		if(info[3]->IsFunction()){
			if(4 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[3].As<v8::Function>());
		}else{
			cmask_bitcnt	= Nan::To<int>(info[3]).ToChecked();
		}
	}
	if(4 < info.Length()){
		if(info[4]->IsFunction()){
			if(5 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[4].As<v8::Function>());
		}else{
			max_element_cnt	= Nan::To<int>(info[4]).ToChecked();
		}
	}
	if(5 < info.Length()){
		if(info[5]->IsFunction()){
			if(6 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[5].As<v8::Function>());
		}else{
			pagesize		= Nan::To<int>(info[5]).ToChecked();
		}
	}
	if(6 < info.Length()){
		if(7 < info.Length() || !info[6]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback			= new Nan::Callback(info[6].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new OpenWorker(callback, &(obj->_k2hshm), *filename, false, false, true, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(obj->_k2hshm.Attach(*filename, false, false, true, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize)));
	}
}

/**
 * @memberof K2hNode
 * @fn bool OpenMem(\
 * 	  int maskbitcnt=2\
 * 	, int cmaskbitcnt=4\
 * 	, int maxelementcnt=32\
 * 	, int pagesize=128\
 * 	, Callback cbfunc=null\
 * )
 * @brief	Open(create) K2HASH data on memory.
 *			If the callback function is specified, or on callback handles for this,
 *			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] maskbitcnt	Specify initial mask bit count(must over 2).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] cmaskbitcnt	Specify initial collision mask bit count(must over 4).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] maxelementcnt	Specify initial element count(should be over 32).
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] pagesize		Specify page size which is data block size(must over 128)
 *							If opens existing K2HASH file, this value is ignored and
 *							uses this value which is set in K2HASH file.
 * @param[in] cbfunc		callback function.
 *
 * @return return true for success, false for failure
 *
 */

NAN_METHOD(K2hNode::OpenMem)
{
	K2hNode*		obj				= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	int				mask_bitcnt		= K2HShm::MIN_MASK_BITCOUNT;
	int				cmask_bitcnt	= K2HShm::DEFAULT_COLLISION_MASK_BITCOUNT;
	int				max_element_cnt	= K2HShm::DEFAULT_MAX_ELEMENT_CNT;
	size_t			pagesize		= K2HShm::MIN_PAGE_SIZE;
	Nan::Callback*	callback		= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_OPENMEM]);

	if(1 < info.Length()){
		if(info[1]->IsFunction()){
			if(2 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[1].As<v8::Function>());
		}else{
			mask_bitcnt		= Nan::To<int>(info[1]).ToChecked();
		}
	}
	if(2 < info.Length()){
		if(info[2]->IsFunction()){
			if(3 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[2].As<v8::Function>());
		}else{
			cmask_bitcnt	= Nan::To<int>(info[2]).ToChecked();
		}
	}
	if(3 < info.Length()){
		if(info[3]->IsFunction()){
			if(4 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[3].As<v8::Function>());
		}else{
			max_element_cnt	= Nan::To<int>(info[3]).ToChecked();
		}
	}
	if(4 < info.Length()){
		if(info[4]->IsFunction()){
			if(5 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[4].As<v8::Function>());
		}else{
			pagesize		= Nan::To<int>(info[4]).ToChecked();
		}
	}
	if(5 < info.Length()){
		if(6 < info.Length() || !info[5]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback			= new Nan::Callback(info[5].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new OpenWorker(callback, &(obj->_k2hshm), NULL, false, true, false, true, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(obj->_k2hshm.Attach(NULL, false, true, false, true, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize)));
	}
}

/**
 * @memberof K2hNode
 * @fn bool Close(\
 * 	Callback cbfunc=null\
 * )
 * @brief	Close(detach) the K2HASH file(memory)
 *
 *	If the callback function is specified, or on callback handles for this,
 *	this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] cbfunc	callback function.
 *
 * @return return true for success, false for failure
 *
 * @par Usage(Node.js):
 * @code
 * var k2hash = require('bindings')('k2hash') ;
 * var k2h = k2hash() ;
 * k2h.OpenMem() ;
 * k2h.SetValue('key','val') ;
 * console_log( k2h.GetValue('key') ) ;
 * k2h.Close() ;
 * @endcode
 */

NAN_METHOD(K2hNode::Close)
{
	K2hNode*		obj		= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_CLOSE]);

	if(0 < info.Length()){
		if(1 < info.Length() || !info[0]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback = new Nan::Callback(info[0].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new CloseWorker(callback, &(obj->_k2hshm)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(obj->_k2hshm.Detach()));
	}
}

/**
 * @memberof K2hNode
 * @fn String GetValue(\
 * 	String key\
 * 	, String subkey=""\
 * 	, bool attrcheck=true\
 * 	, String pass=""\
 * 	, Callback cbfunc=null\
 * )
 * @brief	Get value for key.
 *			If the subkey is specified, returns the value for subkey.
 *			Then if the key does not have subkey, any value is returned.
 *			If the callback function is specified, or on callback handles for this,
 *			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key		Specify the key name.
 * @param[in] subkey	Specify the subkey name when you want to get value for it.
 * @param[in] attrcheck	If this parameter is true, enable to check attributes which is presented by builtin.
 * @param[in] pass		Specify optional passphrase if encrypting the key
 * @param[in] cbfunc	callback function.
 *
 * @return	Returns the value. If there is no key(subkey), returns null.
 *			Returns always true when the callback function is specified.
 *
 * @par Usage(Node.js):
 * @code
 * var k2hash = require('bindings')('k2hash') ;
 * var k2h = k2hash() ;
 * k2h.OpenMem() ;
 * k2h.SetValue('key','val') ;
 * console_log( k2h.GetValue('key') ) ;
 * k2h.Close() ;
 * @endcode
 */

NAN_METHOD(K2hNode::GetValue)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No key name is specified.");
		return;
	}

	K2hNode*		obj				= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	string			strkey;
	bool			is_subkey_set	= false;
	string			strsubkey;
	bool			attrchk			= true;
	bool			is_pass_set		= false;
	string			strpass;
	Nan::Callback*	callback		= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_GET]);

	if(info[0]->IsNull()){
		Nan::ThrowSyntaxError("key is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[0]);
		strkey				= std::string(*buf);
	}
	if(1 < info.Length()){
		if(info[1]->IsFunction()){
			if(2 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[1].As<v8::Function>());
		}else if(!info[1]->IsNull()){
			Nan::Utf8String	buf(info[1]);
			strsubkey		= std::string(*buf);
			is_subkey_set	= true;
		}
	}
	if(2 < info.Length()){
		if(info[2]->IsFunction()){
			if(3 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[2].As<v8::Function>());
		}else if(info[2]->IsBoolean()){
			attrchk			= Nan::To<bool>(info[2]).ToChecked();
		}else{
			Nan::ThrowSyntaxError("Third parameter must be boolean.");
			return;
		}
	}
	if(3 < info.Length()){
		if(info[3]->IsFunction()){
			if(4 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[3].As<v8::Function>());
		}else if(!info[3]->IsNull()){
			Nan::Utf8String	buf(info[3]);
			strpass			= std::string(*buf);
			is_pass_set		= true;
		}
	}
	if(4 < info.Length()){
		if(5 < info.Length() || !info[4]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback			= new Nan::Callback(info[4].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new GetValueWorker(callback, &(obj->_k2hshm), strkey.c_str(), (is_subkey_set ? strsubkey.c_str() : NULL), attrchk, (is_pass_set ? strpass.c_str() : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		if(is_subkey_set){
			// subkey is specified, thus need to check the key has it.
			bool		found	= false;
			K2HSubKeys*	sk		= obj->_k2hshm.GetSubKeys(strkey.c_str());
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
				info.GetReturnValue().Set(Nan::Undefined());
				return;
			}
			strkey = strsubkey;
		}

		char*	presult = obj->_k2hshm.Get(strkey.c_str(), attrchk, (is_pass_set ? strpass.c_str() : NULL));
		if(presult){
			info.GetReturnValue().Set(Nan::New<String>(presult).ToLocalChecked());
			K2H_Free(presult);
		}else{
			info.GetReturnValue().Set(Nan::Null());
		}
	}
}

/**
 * @memberof K2hNode
 * @fn Array GetSubkeys(\
 *	String key\
 * 	, Callback cbfunc=null\
 *	)
 *
 * @brief	Get subkey list for key.
 *
 *	If the callback function is specified, or on callback handles for this,
 *	this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key		Specify the key name.
 * @param[in] cbfunc	callback function.
 *
 * @return	Returns the value. If there is no key(subkey), returns null.
 *			Returns always true when the callback function is specified.
 *
 */

NAN_METHOD(K2hNode::GetSubkeys)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No key name is specified.");
		return;
	}

	K2hNode*		obj		= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	string			strkey;
	Nan::Callback*	callback= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_GETSUBKEYS]);

	if(info[0]->IsNull()){
		Nan::ThrowSyntaxError("key is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[0]);
		strkey			= std::string(*buf);
	}
	if(1 < info.Length()){
		if(2 < info.Length() || !info[1]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback		= new Nan::Callback(info[1].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new GetSubkeysWorker(callback, &(obj->_k2hshm), strkey.c_str()));
		info.GetReturnValue().Set(Nan::True());
	}else{
		K2HSubKeys*	sk = obj->_k2hshm.GetSubKeys(strkey.c_str());
		if(!sk){
			info.GetReturnValue().Set(Nan::Null());
			return;
		}
		strarr_t	strarr;
		sk->StringArray(strarr);

		Local<Array>	retarr	= Nan::New<Array>();
		int				pos		= 0 ;
		for(strarr_t::const_iterator iter = strarr.begin(); iter != strarr.end(); ++iter, ++pos){
			Nan::Set(retarr, pos, Nan::New<String>(iter->c_str()).ToLocalChecked());
		}
		delete sk;

		info.GetReturnValue().Set(retarr);
	}
}

/**
 * @memberof K2hNode
 * @fn bool SetValue(\
 * 	  String key\
 * 	, String value\
 * 	, String subkey=NULL\
 * 	, String pass=""\
 * 	, int expire=0\
 * 	, Callback cbfunc=null\
 * )
 * @brief	Set the value to the key.
 *
 *	If the key is existed, the key's value is over wrote by new value.
 *	Then if the key has subkeys, those subkeys is not changed.
 *	If the subkey parameter is specified, new subkey is set in the key and
 *	the value is set to new subkey. Then the subkey is existed, the subkey's
 *	value is over write.
 *	If the callback function is specified, or on callback handles for this,
 *	this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key		Specify the key name
 * @param[in] value		Specify the value
 * @param[in] subkey	Specify the subkey name, if you want to set the value to subkey.
 * @param[in] pass		Specify optional passphrase if encrypting the key
 * @param[in] expire	Specify optional timeout seconds if you need
 * @param[in] cbfunc	callback function.
 *
 * @return return true for success, false for failure
 *
 * @par Usage(Node.js):
 * @code
 * var k2hash = require('bindings')('k2hash') ;
 * var k2h = k2hash() ;
 * k2h.OpenMem() ;
 * k2h.SetValue('key','val') ;
 * console_log( k2h.GetValue('key') ) ;
 * k2h.Close() ;
 * @endcode
 */

NAN_METHOD(K2hNode::SetValue)
{
	if(info.Length() < 2){
		Nan::ThrowSyntaxError("No key name or no value are specified.");
		return;
	}

	K2hNode*		obj			= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	string			strkey;
	bool			is_val_set	= false;
	string			strval;
	bool			is_skey_set	= false;
	string			strsubkey;
	bool			is_pass_set	= false;
	string			strpass;
	time_t			expire		= 0;
	Nan::Callback*	callback	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_SET]);

	if(info[0]->IsNull()){
		Nan::ThrowSyntaxError("key is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[0]);
		strkey					= std::string(*buf);
	}
	if(!info[1]->IsNull()){
		Nan::Utf8String	buf(info[1]);
		strval					= std::string(*buf);
		is_val_set				= true;
	}
	if(2 < info.Length()){
		if(info[2]->IsFunction()){
			if(3 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback			= new Nan::Callback(info[2].As<v8::Function>());
		}else if(!info[2]->IsNull()){
			Nan::Utf8String	buf(info[2]);
			strsubkey			= std::string(*buf);
			is_skey_set			= true;
		}
	}
	if(3 < info.Length()){
		if(info[3]->IsFunction()){
			if(4 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback			= new Nan::Callback(info[3].As<v8::Function>());
		}else if(!info[3]->IsNull()){
			Nan::Utf8String	buf(info[3]);
			strpass				= std::string(*buf);
			is_pass_set			= true;
		}
	}
	if(4 < info.Length()){
		if(info[4]->IsFunction()){
			if(5 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback			= new Nan::Callback(info[4].As<v8::Function>());
		}else if(info[4]->IsInt32()){
			int	nexpire			= Nan::To<int32_t>(info[4]).ToChecked();
			expire				= static_cast<time_t>(nexpire);
		}else{
			Nan::ThrowSyntaxError("Expire parameter must be int32 value.");
			return;
		}
	}
	if(5 < info.Length()){
		if(6 < info.Length() || !info[5]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback				= new Nan::Callback(info[5].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new SetValueWorker(callback, &(obj->_k2hshm), strkey.c_str(), (is_skey_set ? strsubkey.c_str() : NULL), (is_val_set ? strval.c_str() : NULL), (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		if(is_skey_set){
			// subkey is specified
			info.GetReturnValue().Set(Nan::New(obj->_k2hshm.AddSubkey(strkey.c_str(), strsubkey.c_str(), (is_val_set ? strval.c_str() : NULL), (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL))));
		}else{
			// subkey is not specified
			info.GetReturnValue().Set(Nan::New(obj->_k2hshm.Set(strkey.c_str(), (is_val_set ? strval.c_str() : NULL), (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL))));
		}
	}
}

/**
 * @memberof K2hNode
 * @fn bool AddSubkey(\
 *	String key\
 *	, String subkey\
 *	, String value[=option]\
 * 	, Callback cbfunc=null\
 *	)
 *
 * @brief	Added subkey into key
 *
 *	If the value is specified, new subkey has it.
 *	If the callback function is specified, or on callback handles for this,
 *	this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key		Specify the key name
 * @param[in] subkey	Specify added new subkey name
 * @param[in] value		Specify the value for subkey, if you want to set the value to subkey.
 * @param[in] cbfunc	callback function.
 *
 * @return return true for success, false for failure
 *
 */

NAN_METHOD(K2hNode::AddSubkey)
{
	if(info.Length() < 2){
		Nan::ThrowSyntaxError("No key name or no subkey name are specified.");
		return;
	}

	K2hNode*		obj			= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	string			strkey;
	string			strsubkey;
	bool			is_value_set= false;
	string			strvalue;
	Nan::Callback*	callback	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_ADDSUBKEY]);

	if(info[0]->IsNull()){
		Nan::ThrowSyntaxError("key is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[0]);
		strkey				= std::string(*buf);
	}
	if(info[1]->IsNull()){
		Nan::ThrowSyntaxError("subkey is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[1]);
		strsubkey			= std::string(*buf);
	}
	if(2 < info.Length()){
		if(info[2]->IsFunction()){
			if(3 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback		= new Nan::Callback(info[2].As<v8::Function>());
		}else if(!info[2]->IsNull()){
			Nan::Utf8String	buf(info[2]);
			strvalue		= std::string(*buf);
			is_value_set	= true;
		}
	}
	if(3 < info.Length()){
		if(4 < info.Length() || !info[3]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback			= new Nan::Callback(info[3].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new AddSubkeyWorker(callback, &(obj->_k2hshm), strkey.c_str(), strsubkey.c_str(), (is_value_set ? strvalue.c_str() : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(obj->_k2hshm.AddSubkey(strkey.c_str(), strsubkey.c_str(), (is_value_set ? strvalue.c_str() : NULL))));
	}
}

/**
 * @memberof K2hNode
 * @fn bool AddSubkeys(\
 *	String key\
 *	, Array subkeys\
 * 	, Callback cbfunc=null\
 *	)
 *
 * @brief	Added subkey name list to the key.
 *
 *	If the key has subkey list, the subkey list is removed and replaced by new subkey list.
 *	Take care for that the replaced old subkey list, the old subkey is not removed from
 *	K2HASH, it is removed only the key's subkey list.
 *	If the callback function is specified, or on callback handles for this,
 *	this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key		Specify the key name
 * @param[in] subkeys	Specify the subkey list
 * @param[in] cbfunc	callback function.
 *
 * @return return true for success, false for failure
 *
 */

NAN_METHOD(K2hNode::AddSubkeys)
{
	if(info.Length() < 2){
		Nan::ThrowSyntaxError("No key name or no subkeys array are specified.");
		return;
	}

	K2hNode*		obj			= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	string			strkey;
	unsigned char*	bySubkeys	= NULL;
	size_t			skeylen		= 0UL;
	Nan::Callback*	callback	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_ADDSUBKEYS]);

	if(info[0]->IsNull()){
		Nan::ThrowSyntaxError("key is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[0]);
		strkey					= std::string(*buf);
	}
	if(!info[1]->IsArray()){
		Nan::ThrowSyntaxError("Specified subkeys is not array.");
		return;
	}else{
		Local<Array>	inSubkeys= Local<Array>::Cast(info[1]);
		K2HSubKeys		Subkeys;
		for(int pos = 0; pos < static_cast<int>(inSubkeys->Length()); ++pos){
			string		tmpkey;
			{
				Nan::Utf8String	buf(Nan::Get(inSubkeys, pos).ToLocalChecked());
				tmpkey = std::string(*buf);
			}
			if(Subkeys.end() == Subkeys.insert(tmpkey.c_str())){
				// failed to set subkey
				info.GetReturnValue().Set(Nan::False());
				return;
			}
		}
		if(0UL >= Subkeys.size()){
			// there is no subkey
			info.GetReturnValue().Set(Nan::False());
			return;
		}
		// serialize
		if(!Subkeys.Serialize(&bySubkeys, skeylen)){
			info.GetReturnValue().Set(Nan::False());
			return;
		}
	}
	if(2 < info.Length()){
		if(3 < info.Length() || !info[2]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback				= new Nan::Callback(info[2].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new AddSubkeysWorker(callback, &(obj->_k2hshm), strkey.c_str(), bySubkeys, skeylen));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(obj->_k2hshm.ReplaceSubkeys(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, bySubkeys, skeylen)));
	}
	K2H_Free(bySubkeys);
}

/**
 * @memberof K2hNode
 * @fn bool Remove(\
 *	String key\
 *	, String subkey[=option]\
 * 	, Callback cbfunc=null\
 *	)
 *
 * @brief	Remove the key or subkey
 *
 *	If the subkey name is specified, the subkey is removed from the key's subkey
 *	list and the subkey is removed from K2HASH.
 *	If the callback function is specified, or on callback handles for this,
 *	this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key		Specify the key name
 * @param[in] subkey	Specify the subkey name
 * @param[in] cbfunc	callback function.
 *
 * @return return true for success, false for failure
 *
 */

NAN_METHOD(K2hNode::Remove)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No key name is specified.");
		return;
	}

	K2hNode*		obj				= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	string			strkey;
	bool			is_subkey_set	= false;
	string			strsubkey;
	Nan::Callback*	callback		= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_REMOVE]);

	if(info[0]->IsNull()){
		Nan::ThrowSyntaxError("key is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[0]);
		strkey						= std::string(*buf);
	}
	if(1 < info.Length()){
		if(info[1]->IsFunction()){
			if(2 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback				= new Nan::Callback(info[1].As<v8::Function>());
		}else if(!info[1]->IsNull()){
			Nan::Utf8String	buf(info[1]);
			strsubkey				= std::string(*buf);
			is_subkey_set			= true;
		}
	}
	if(2 < info.Length()){
		if(3 < info.Length() || !info[2]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback					= new Nan::Callback(info[2].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new RemoveWorker(callback, &(obj->_k2hshm), strkey.c_str(), (is_subkey_set ? strsubkey.c_str() : NULL), false));
		info.GetReturnValue().Set(Nan::True());
	}else{
		if(is_subkey_set){
			info.GetReturnValue().Set(Nan::New(obj->_k2hshm.Remove(strkey.c_str(), strsubkey.c_str())));
		}else{
			info.GetReturnValue().Set(Nan::New(obj->_k2hshm.Remove(strkey.c_str(), false)));
		}
	}
}

/**
 * @memberof K2hNode
 * @fn bool RemoveAll(\
 *	String key\
 * 	, Callback cbfunc=null\
 *	)
 * @brief	Remove the key and key's all subkeys.
 *
 *	If the key has subkeys, all of the subkeys is removed.
 *	If the callback function is specified, or on callback handles for this,
 *	this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key		Specify the key name
 * @param[in] cbfunc	callback function.
 *
 * @return return true for success, false for failure
 *
 */

NAN_METHOD(K2hNode::RemoveAll)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No key name is specified.");
		return;
	}

	K2hNode*		obj			= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	string			strkey;
	Nan::Callback*	callback	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_REMOVEALL]);

	if(info[0]->IsNull()){
		Nan::ThrowSyntaxError("key is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[0]);
		strkey					= std::string(*buf);
	}
	if(1 < info.Length()){
		if(2 < info.Length() || !info[1]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback				= new Nan::Callback(info[1].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new RemoveWorker(callback, &(obj->_k2hshm), strkey.c_str(), NULL, true));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(obj->_k2hshm.Remove(strkey.c_str(), true)));
	}
}

/**
 * @memberof K2hNode
 * @fn bool PrintState(int fd[=option])
 * @brief	Put K2HASH status
 *
 *	If fd is not specified, the status is put to stdout.
 *
 * @param[in] fd	Specify output stream
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::PrintState)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	if(0 >= info.Length()){
		info.GetReturnValue().Set(Nan::New(
			obj->_k2hshm.PrintState()
		));
	}else{
		int		fd = info[0]->IsInt32() ? Nan::To<int32_t>(info[0]).ToChecked() : -1;
		FILE*	fp = (-1 == fd ? stdout : fdopen(fd, "a"));
		if(!fp){
			Nan::ThrowError("could not open output stream.");
			return;
		}
		bool result = obj->_k2hshm.PrintState(fp);

		// [NOTE]
		// Must flush at here, because nodejs's file descriptor is used for fd.
		// Otherwise, calling flash on nodejs(javascript) is not effected.
		fflush(fp);

		info.GetReturnValue().Set(result);
	}
}

/**
 * @memberof K2hNode
 * @fn bool PrintVersion(int fd[=option])
 * @brief	Put K2HASH library version
 *
 *	If fd is not specified, the status is put to stdout.
 *
 * @param[in] fd	Specify output stream
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::PrintVersion)
{
	int			fd	= (0 < info.Length() && info[0]->IsInt32()) ? Nan::To<int32_t>(info[0]).ToChecked() : -1;
	FILE*		fp	= (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Nan::ThrowError("could not open output stream.");
		return;
	}
	k2h_print_version(fp);

	// [NOTE]
	// Must flush at here, because nodejs's file descriptor is used for fd.
	// Otherwise, calling flash on nodejs(javascript) is not effected.
	fflush(fp);

	info.GetReturnValue().Set(Nan::True());
}

/**
 * @memberof K2hNode
 * @fn bool DumpHead(int fd[=option])
 * @brief	Dump K2HASH head information
 *
 *	If fd is not specified, the status is put to stdout.
 *
 * @param[in] fd	Specify output stream
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::DumpHead)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	int			fd	= (0 < info.Length() && info[0]->IsInt32()) ? Nan::To<int32_t>(info[0]).ToChecked() : -1;
	FILE*		fp	= (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Nan::ThrowError("could not open output stream.");
		return;
	}

	//SetK2hDbgMode(K2HDBG_MSG);

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hshm.Dump(fp, K2HShm::DUMP_HEAD)
	));

	//SetK2hDbgMode(K2HDBG_SILENT);

	// Need to flush stream here!
	fflush(fp) ;

	info.GetReturnValue().Set(Nan::True());
}

/**
 * @memberof K2hNode
 * @fn bool DumpKeytable(int fd[=option])
 * @brief	Dump K2HASH head and tables information
 *
 *	If fd is not specified, the status is put to stdout.
 *
 * @param[in] fd	Specify output stream
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::DumpKeytable)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	int			fd	= (0 < info.Length() && info[0]->IsInt32()) ? Nan::To<int32_t>(info[0]).ToChecked() : -1;
	FILE*		fp	= (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Nan::ThrowError("could not open output stream.");
		return;
	}

	//SetK2hDbgMode(K2HDBG_MSG);

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hshm.Dump(fp, K2HShm::DUMP_KINDEX_ARRAY)
	));

	//SetK2hDbgMode(K2HDBG_SILENT);

	// Need to flush stream here!
	fflush(fp) ;

	info.GetReturnValue().Set(Nan::True());
}

/**
 * @memberof K2hNode
 * @fn bool DumpFullKeytable(int fd[=option])
 * @brief	Dump K2HASH head, tables and collision tables information
 *
 *	If fd is not specified, the status is put to stdout.
 *
 * @param[in] fd	Specify output stream
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::DumpFullKeytable)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	int			fd	= (0 < info.Length() && info[0]->IsInt32()) ? Nan::To<int32_t>(info[0]).ToChecked() : -1;
	FILE*		fp	= (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Nan::ThrowError("could not open output stream.");
		return;
	}

	//SetK2hDbgMode(K2HDBG_MSG);

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hshm.Dump(fp, K2HShm::DUMP_CKINDEX_ARRAY)
	));

	//SetK2hDbgMode(K2HDBG_SILENT);

	// Need to flush stream here!
	fflush(fp) ;

	info.GetReturnValue().Set(Nan::True());
}

/**
 * @memberof K2hNode
 * @fn bool DumpElementtable(int fd[=option])
 * @brief	Dump K2HASH head, tables, collision tables, element data information
 *
 *	If fd is not specified, the status is put to stdout.
 *
 * @param[in] fd	Specify output stream
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::DumpElementtable)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	int			fd	= (0 < info.Length() && info[0]->IsInt32()) ? Nan::To<int32_t>(info[0]).ToChecked() : -1;
	FILE*		fp	= (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Nan::ThrowError("could not open output stream.");
		return;
	}

	//SetK2hDbgMode(K2HDBG_MSG);

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hshm.Dump(fp, K2HShm::DUMP_ELEMENT_LIST)
	));

	//SetK2hDbgMode(K2HDBG_SILENT);

	// Need to flush stream here!
	fflush(fp) ;

	info.GetReturnValue().Set(Nan::True());
}

/**
 * @memberof K2hNode
 * @fn bool DumpFull(int fd[=option])
 * @brief	Dump K2HASH all information(head, tables, collision tables, element data, page data)
 *
 *	If fd is not specified, the status is put to stdout.
 *
 * @param[in] fd	Specify output stream
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::DumpFull)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	int			fd	= (0 < info.Length() && info[0]->IsInt32()) ? Nan::To<int32_t>(info[0]).ToChecked() : -1;
	FILE*		fp	= (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Nan::ThrowError("could not open output stream.");
		return;
	}

	//SetK2hDbgMode(K2HDBG_MSG);

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hshm.Dump(fp, K2HShm::DUMP_PAGE_LIST)
	));

	//SetK2hDbgMode(K2HDBG_SILENT);

	// Need to flush stream here!
	fflush(fp) ;

	info.GetReturnValue().Set(Nan::True());
}

/**
 * @memberof K2hNode
 * @fn bool Transaction(\
 *	bool enable\
 * 	, String transfile=""\
 * 	, String prefix=""\
 * 	, String param=""\
 * 	, int expire=0\
 * )
 * @brief	Disable/Enable transaction
 *
 *	If the transfile is not specified, you must set customized transaction
 *	plugin for your system. The builtin transaction must specify the transaction
 *	file path.
 *	If you need to change the transaction queue key name, you can specify the
 *	prefix for transaction queue key.
 *	You can set the custom transaction parameter by param.
 *	If you specify expire, you can set the expire time(s) for each transaction
 *	data. If expire is set 0, it means that the data is not expired.
 *
 * @param[in] enable 	If true is specified, the transaction is enabled. false for disabled.
 * @param[in] transfile	Specify the transaction file path
 * @param[in] prefix 	Specify the custom transaction queue key name for prefix.
 * @param[in] param 	Specify the custom transaction parameter
 * @param[in] expire	Specify optional timeout seconds for transaction data
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::Transaction)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	if(info.Length() < 1){
		Nan::ThrowSyntaxError("Need to specify first parameter for dis/enable.");
		return;
	}

	bool	enable = Nan::To<bool>(info[0]).ToChecked();
	if(enable){
		std::string	transfile;
		std::string	prefix;
		std::string	param;
		int			expire = (info.Length() < 5 || !info[4]->IsInt32()) ? 0	: Nan::To<int32_t>(info[4]).ToChecked();

		if(2 <= info.Length()){
			Nan::Utf8String	buf(info[1]);
			transfile = std::string(*buf);
		}
		if(3 <= info.Length()){
			Nan::Utf8String	buf(info[2]);
			prefix = std::string(*buf);
		}
		if(4 <= info.Length()){
			Nan::Utf8String	buf(info[3]);
			param = std::string(*buf);
		}

		info.GetReturnValue().Set(Nan::New(
			obj->_k2hshm.EnableTransaction(
				(transfile.empty()	? NULL	: transfile.c_str()),
				(prefix.empty()		? NULL	: reinterpret_cast<const unsigned char*>(prefix.c_str())),
				(prefix.empty()		? 0		: prefix.length()),
				(param.empty()		? NULL	: reinterpret_cast<const unsigned char*>(param.c_str())),
				(param.empty()		? 0		: param.length()),
				(0 >= expire		? NULL	: reinterpret_cast<const time_t*>(&expire))
			)
		));
	}else{
		info.GetReturnValue().Set(Nan::New(
			obj->_k2hshm.DisableTransaction()
		));
	}
}

/**
 * @memberof K2hNode
 * @fn bool EnableTransaction(\
 * 	String transfile=""\
 * 	, String prefix=""\
 * 	, String param=""\
 * 	, int expire=0\
 * )
 * @brief	Enable transaction
 *
 *	see Transaction() function.
 *
 * @param[in] transfile	Specify the transaction file path
 * @param[in] prefix 	Specify the custom transaction queue key name for prefix.
 * @param[in] param 	Specify the custom transaction parameter
 * @param[in] expire	Specify optional timeout seconds for transaction data
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::EnableTransaction)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	std::string	transfile;
	std::string	prefix;
	std::string	param;
	int			expire = (info.Length() < 4 || !info[4]->IsInt32()) ? 0	: Nan::To<int32_t>(info[3]).ToChecked();

	if(1 <= info.Length()){
		Nan::Utf8String	buf(info[0]);
		transfile = std::string(*buf);
	}
	if(2 <= info.Length()){
		Nan::Utf8String	buf(info[1]);
		prefix = std::string(*buf);
	}
	if(3 <= info.Length()){
		Nan::Utf8String	buf(info[2]);
		param = std::string(*buf);
	}

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hshm.EnableTransaction(
			(transfile.empty()	? NULL	: transfile.c_str()),
			(prefix.empty()		? NULL	: reinterpret_cast<const unsigned char*>(prefix.c_str())),
			(prefix.empty()		? 0		: (prefix.length() + 1)),
			(param.empty()		? NULL	: reinterpret_cast<const unsigned char*>(param.c_str())),
			(param.empty()		? 0		: (param.length() + 1)),
			(0 >= expire		? NULL	: reinterpret_cast<const time_t*>(&expire))
		)
	));
}

/**
 * @memberof K2hNode
 * @fn bool DisableTransaction(void)
 * @brief	Disable transaction
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::DisableTransaction)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hshm.DisableTransaction()
	));
}

/**
 * @memberof K2hNode
 * @fn bool UnsetTransactionThreadPool(void)
 * @brief	Clear transaction thread pool
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::UnsetTransactionThreadPool)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hshm.UnsetTransThreadPool()
	));
}

/**
 * @memberof K2hNode
 * @fn int GetTransactionThreadPool(void)
 * @brief	Get transaction thread pool count
 *
 * @return	Return count of transaction thread pool
 */

NAN_METHOD(K2hNode::GetTransactionThreadPool)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hshm.GetTransThreadPool()
	));
}

/**
 * @memberof K2hNode
 * @fn bool SetTransactionThreadPool(int count)
 * @brief	Set the transaction thread count
 *
 * @param[in] count	Specify the count of transaction thread
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::SetTransactionThreadPool)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	if(info.Length() < 1){
		Nan::ThrowSyntaxError("Need to specify first parameter for count of pool.");
		return;
	}

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hshm.SetTransThreadPool(Nan::To<int>(info[0]).ToChecked())
	));
}

/**
 * @memberof K2hNode
 * @fn bool LoadArchive(\
 *	String filename\
 *	, bool errskip=true\
 * 	, Callback cbfunc=null\
 *	)
 * @brief	Load K2HASH data from archive file
 *
 *	If there is same key name in archive file, the key is over wrote.
 *	The archive file is made by PutArchive() function.
 *	If the callback function is specified, or on callback handles for this,
 *	this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] filename	Specify the archive file path
 * @param[in] errskip	If true is specified, this function does not stop when something
 *						error is occured in loading the archive file.
 * @param[in] cbfunc	callback function.
 *
 * @return return true for success, false for failure
 *
 */

NAN_METHOD(K2hNode::LoadArchive)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No file name is specified.");
		return;
	}

	K2hNode*		obj			= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	string			filename;
	bool			errskip		= true;
	Nan::Callback*	callback	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_LOAD]);

	if(info[0]->IsNull()){
		Nan::ThrowSyntaxError("file name is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[0]);
		filename				= std::string(*buf);
	}
	if(1 < info.Length()){
		if(info[1]->IsFunction()){
			if(2 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback			= new Nan::Callback(info[1].As<v8::Function>());
		}else if(info[1]->IsBoolean()){
			errskip				= Nan::To<bool>(info[1]).ToChecked();
		}else{
			Nan::ThrowSyntaxError("Second parameter must be boolean.");
			return;
		}
	}
	if(2 < info.Length()){
		if(3 < info.Length() || !info[2]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback				= new Nan::Callback(info[2].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new ArchiveWorker(callback, &(obj->_k2hshm), filename.c_str(), errskip, true));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(k2h_load_archive(reinterpret_cast<k2h_h>(&obj->_k2hshm), filename.c_str(), errskip)));
	}
}

/**
 * @memberof K2hNode
 * @fn bool PutArchive(\
 *	String filename\
 *	, bool errskip=true\
 * 	, Callback cbfunc=null\
 *	)
 *
 * @brief	Put(Serialize) all of K2HASH data to file.
 *
 *	The archive file which is made by this method can be loaded by LoadArchive()
 *	If the callback function is specified, or on callback handles for this,
 *	this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] filename	Specify the archive file path
 * @param[in] errskip	If true is specified, this function does not stop when something
 *						error is occured in putting the archive file.
 * @param[in] cbfunc	callback function.
 *
 * @return return true for success, false for failure
 *
 */

NAN_METHOD(K2hNode::PutArchive)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No file name is specified.");
		return;
	}

	K2hNode*		obj			= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	string			filename;
	bool			errskip		= true;
	Nan::Callback*	callback	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_PUT]);

	if(info[0]->IsNull()){
		Nan::ThrowSyntaxError("file name is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[0]);
		filename				= std::string(*buf);
	}
	if(1 < info.Length()){
		if(info[1]->IsFunction()){
			if(2 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback			= new Nan::Callback(info[1].As<v8::Function>());
		}else if(info[1]->IsBoolean()){
			errskip				= Nan::To<bool>(info[1]).ToChecked();
		}else{
			Nan::ThrowSyntaxError("Second parameter must be boolean.");
			return;
		}
	}
	if(2 < info.Length()){
		if(3 < info.Length() || !info[2]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback				= new Nan::Callback(info[2].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new ArchiveWorker(callback, &(obj->_k2hshm), filename.c_str(), errskip, false));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(k2h_put_archive(reinterpret_cast<k2h_h>(&obj->_k2hshm), filename.c_str(), errskip)));
	}
}

/**
 * @memberof K2hNode
 * @fn object getQueue(bool is_fifo=true , String prefix[=option])
 * @brief	Get K2hQueue object which accesses this object.
 *
 *	You can specify the type of queue(FIFO/LIFO) and prefix of queue key name.
 *
 * @param[in] is_fifo	true for FIFO type queue, false for LIFO.
 * @param[in] prefix	Specify prefix for the queue key name
 *
 * @return	Returns K2hQueue object instance.
 *
 * @par Usage(Node.js):
 * @code
 * var k2hash = require('bindings')('k2hash') ;
 * var k2h = k2hash() ;
 * k2h.OpenMem() ;
 * var k2hq = k2h.getQueue(false,'++LIFO++') ;
 * k2hq,Push('key','val') ;
 * k2hq.Count() ;
 * k2h.Close() ;
 * @endcode
 */

NAN_METHOD(K2hNode::getQueue)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	K2hQueue::Init();
	Local<Object>	retobj	= K2hQueue::GetInstance(info);
	K2hQueue*		objq	= Nan::ObjectWrap::Unwrap<K2hQueue>(retobj->ToObject(Nan::GetCurrentContext()).ToLocalChecked());
	bool			is_fifo	= info.Length() < 1 ? true : Nan::To<bool>(info[0]).ToChecked();
	std::string		prefix;
	if(2 <= info.Length()){
		Nan::Utf8String	buf(info[1]);
		prefix = std::string(*buf);
	}
	objq->_k2hqueue.Init(&obj->_k2hshm, is_fifo, (prefix.empty() ? NULL : reinterpret_cast<const unsigned char*>(prefix.c_str())), (prefix.empty() ? 0 : (prefix.length() + 1)));

	info.GetReturnValue().Set(retobj);
}

/**
 * @memberof K2hNode
 * @fn object getKeyQueue(bool is_fifo=true , String prefix[=option])
 * @brief	Get K2hKeyQueue object which accesses this object.
 *
 *	You can specify the type of queue(FIFO/LIFO) and prefix of queue key name.
 *
 * @param[in] is_fifo	true for FIFO type queue, false for LIFO.
 * @param[in] prefix	Specify prefix for the queue key name
 *
 * @return	Returns K2hKeyQueue object instance.
 *
 * @par Usage(Node.js):
 * @code
 * var k2hash = require('bindings')('k2hash') ;
 * var k2h = k2hash() ;
 * k2h.OpenMem() ;
 * var k2hkq = k2h.getKeyQueue(false,'++LIFO++') ;
 * k2hkq,Push('key','val') ;
 * k2hkq.Count() ;
 * k2h.Close() ;
 * @endcode
 */

NAN_METHOD(K2hNode::getKeyQueue)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	K2hKeyQueue::Init();
	Local<Object>	retobj	= K2hKeyQueue::GetInstance(info);
	K2hKeyQueue*	objq	= Nan::ObjectWrap::Unwrap<K2hKeyQueue>(retobj->ToObject(Nan::GetCurrentContext()).ToLocalChecked());
	bool			is_fifo	= info.Length() < 1 ? true : Nan::To<bool>(info[0]).ToChecked();
	std::string		prefix;
	if(2 <= info.Length()){
		Nan::Utf8String	buf(info[1]);
		prefix = std::string(*buf);
	}
	objq->_k2hkeyqueue.Init(&obj->_k2hshm, is_fifo, (prefix.empty() ? NULL : reinterpret_cast<const unsigned char*>(prefix.c_str())), (prefix.empty() ? 0 : (prefix.length() + 1)));

	info.GetReturnValue().Set(retobj);
}

/**
 * @memberof K2hNode
 * @fn bool SetCommonAttribute(\
 * 	  int is_mtime=K2H_ATTR_DEFAULT\
 * 	, int is_history=K2H_ATTR_DEFAULT\
 * 	, int is_encrypt=K2H_ATTR_DEFAULT\
 * 	, String passfile=""\
 * 	, int is_expire=K2H_ATTR_DEFAULT\
 * 	, int expire=0\
 * )
 * @brief	Set builtin attributes for K2HASH.
 *
 *	The builtin attributes are mtime/history/expire/encrypt(and pass phrase)
 *
 * @param[in] is_mtime		Specify flag for mtime, enable(1), disable(0), default(-1)
 * @param[in] is_history	Specify flag for history, enable(1), disable(0), default(-1)
 * @param[in] is_encrypt	Specify flag for encrypting, enable(1), disable(0), default(-1)
 * @param[in] passfile		Specify pass phrase string.
 * @param[in] is_expire		Specify flag for expire, enable(1), disable(0), default(-1)
 * @param[in] expire 		Specify expire seconds
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::SetCommonAttribute)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	const bool	istrue		= true;
	const bool	isfalse		= false;
	const bool* pismtime	= (info.Length() < 1 && !info[0]->IsInt32())						? NULL		:
								K2H_VAL_ATTR_ENABLE == Nan::To<int32_t>(info[0]).ToChecked()	? &istrue	:
								K2H_VAL_ATTR_DISABLE == Nan::To<int32_t>(info[0]).ToChecked()	? &isfalse	: NULL;
	const bool* pishistory	= (info.Length() < 2 && !info[1]->IsInt32())						? NULL		:
								K2H_VAL_ATTR_ENABLE == Nan::To<int32_t>(info[1]).ToChecked()	? &istrue	:
								K2H_VAL_ATTR_DISABLE == Nan::To<int32_t>(info[1]).ToChecked()	? &isfalse	: NULL;
	const bool* pisencrypt	= (info.Length() < 3 && !info[2]->IsInt32())						? NULL		:
								K2H_VAL_ATTR_ENABLE == Nan::To<int32_t>(info[2]).ToChecked()	? &istrue	:
								K2H_VAL_ATTR_DISABLE == Nan::To<int32_t>(info[2]).ToChecked()	? &isfalse	: NULL;
	bool		is_expire	= (info.Length() < 5 && !info[4]->IsInt32())						? false		:
								K2H_VAL_ATTR_ENABLE == Nan::To<int32_t>(info[4]).ToChecked()	? true		: false;

	std::string	pass;
	if(4 <= info.Length()){
		Nan::Utf8String	buf(info[3]);
		pass = std::string(*buf);
	}
	time_t		expire;
	time_t*		pexpire = NULL;
	if(is_expire && 6 <= info.Length() && info[5]->IsNumber()){
		expire	= reinterpret_cast<time_t>(info[5]->IntegerValue(Nan::GetCurrentContext()).ToChecked());
		pexpire	= &expire;
	}
	info.GetReturnValue().Set(Nan::New(
		obj->_k2hshm.SetCommonAttribute(pismtime, pisencrypt, (pass.empty() ? NULL : pass.c_str()), pishistory, pexpire)
	));
}

/**
 * @memberof K2hNode
 * @fn bool ClearCommonAttribute()
 * @brief	Clear builtin attribute setting(to default)
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::CleanCommonAttribute)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hshm.CleanCommonAttribute()
	));
}

/**
 * @memberof K2hNode
 * @fn bool AddAttrPluginLib(String libfile)
 * @brief	Set custom attribute plugin(library)
 *
 * @param[in] libfile	Specify the custom attribute library path
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::AddAttrPluginLib)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No library file path is specified.");
		return;
	}
	Nan::Utf8String	libfile(info[0]);

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hshm.AddAttrPluginLib(*libfile)
	));
}

/**
 * @memberof K2hNode
 * @fn bool AddAttrCryptPass(String encpass, bool is_default_encrypt=false)
 * @brief	Set pass phrase for builtin attribute encrypting
 *
 *	Set pass phrase and enable/disable the default encrypting
 *	If the default encrypting is disabled with pass phrase, it means that
 *	do only decrypting.
 *
 * @param[in] encpass				Specify optional passphrase if encrypting the key
 * @param[in] is_default_encrypt	true is enabled default encrypting.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::AddAttrCryptPass)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No pass phrase is specified.");
		return;
	}
	Nan::Utf8String	libfile(info[0]);
	bool			is_default_encrypt = (2 <= info.Length() && true == Nan::To<bool>(info[1]).ToChecked());

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hshm.AddAttrCryptPass(*libfile, is_default_encrypt)
	));
}

/**
 * @memberof K2hNode
 * @fn bool GetAttrVersionInfos(int fd[=option])
 * @brief	Puts Attribute plugin version(with builtin attribute)
 *
 * @param[in] fd	Specify the stream for output.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::GetAttrVersionInfos)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	int			fd	= (0 < info.Length() && info[0]->IsInt32()) ? Nan::To<int32_t>(info[0]).ToChecked() : -1;
	FILE*		fp	= (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Nan::ThrowError("could not open output stream.");
		return;
	}

	strarr_t	verinfos;
	if(!obj->_k2hshm.GetAttrVersionInfos(verinfos)){
		info.GetReturnValue().Set(Nan::False());
		return;
	}

	fprintf(fp, "K2HASH attribute libraries:");
	for(strarr_t::const_iterator iter = verinfos.begin(); iter != verinfos.end(); ++iter){
		fprintf(fp, "  %s\n", iter->c_str());
	}

	// Need to flush stream here!
	fflush(fp) ;

	info.GetReturnValue().Set(Nan::True());
}

/**
 * @memberof K2hNode
 * @fn bool GetAttrInfos(int fd[=option])
 * @brief	Puts builtin attribute information
 *
 * @param[in] fd	Specify the stream for output.
 *
 * @return return true for success, false for failure
 */

NAN_METHOD(K2hNode::GetAttrInfos)
{
	K2hNode*	obj = Nan::ObjectWrap::Unwrap<K2hNode>(info.This());

	int			fd	= (0 < info.Length() && info[0]->IsInt32()) ? Nan::To<int32_t>(info[0]).ToChecked() : -1;
	FILE*		fp	= (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Nan::ThrowError("could not open output stream.");
		return;
	}

	std::stringstream	ss;
	obj->_k2hshm.GetAttrInfos(ss);
	fprintf(fp, "%s\n", ss.str().c_str());

	// Need to flush stream here!
	fflush(fp) ;

	info.GetReturnValue().Set(Nan::True());
}

/**
 * @memberof K2hNode
 * @fn Array GetAttrs(\
 *	String key\
 * 	, Callback cbfunc=null\
 *	)
 *
 * @brief	Get attribute keys
 *			If the callback function is specified, or on callback handles for this,
 *			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key	Specify the key name
 * @param[in] cbfunc	callback function.
 *
 * @return	If there are attributes, returns attribute names list. If not, returns null.
 *			Returns always true when the callback function is specified.
 *
 */

NAN_METHOD(K2hNode::GetAttrs)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No key name is specified.");
		return;
	}

	K2hNode*		obj		= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	string			strkey;
	Nan::Callback*	callback= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_GETATTRS]);

	if(info[0]->IsNull()){
		Nan::ThrowSyntaxError("key is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[0]);
		strkey				= std::string(*buf);
	}
	if(1 < info.Length()){
		if(2 < info.Length() || !info[1]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback			= new Nan::Callback(info[1].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new GetAttrsWorker(callback, &(obj->_k2hshm), strkey.c_str()));
		info.GetReturnValue().Set(Nan::True());
	}else{
		K2HAttrs*		attrs	= obj->_k2hshm.GetAttrs(strkey.c_str());
		if(!attrs){
			info.GetReturnValue().Set(Nan::Null());
			return;
		}
		Local<Array>	retarr	= Nan::New<Array>();
		int				pos		= 0;
		for(K2HAttrs::iterator iter = attrs->begin(); iter != attrs->end(); ++iter, ++pos){
			if(0UL == iter->keylength || !iter->pkey){
				continue;
			}
			Nan::Set(retarr, pos, Nan::New<String>(reinterpret_cast<const char*>(iter->pkey)).ToLocalChecked());
		}
		delete attrs;

		info.GetReturnValue().Set(retarr);
	}
}

/**
 * @memberof K2hNode
 * @fn Array GetAttrValue(\
 *	String key\
 *	, String attrkey\
 * 	, Callback cbfunc=null\
 *	)
 *
 * @brief	Get value by attribute key
 *			If the callback function is specified, or on callback handles for this,
 *			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key		Specify the key name
 * @param[in] attrkey	Specify the attribute key name
 * @param[in] cbfunc	callback function.
 *
 * @return	Returns the value. If there is no key(subkey), returns null.
 *			Returns always true when the callback function is specified.
 *
 */

NAN_METHOD(K2hNode::GetAttrValue)
{
	if(info.Length() < 2){
		Nan::ThrowSyntaxError("No key name or no attribute name are specified.");
		return;
	}

	K2hNode*		obj		= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	string			strkey;
	string			strattr;
	Nan::Callback*	callback= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_GETATTRVAL]);

	if(info[0]->IsNull()){
		Nan::ThrowSyntaxError("key is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[0]);
		strkey				= std::string(*buf);
	}
	if(info[1]->IsNull()){
		Nan::ThrowSyntaxError("attr key is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[1]);
		strattr				= std::string(*buf);
	}
	if(2 < info.Length()){
		if(3 < info.Length() || !info[2]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback			= new Nan::Callback(info[2].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new GetAttrValueWorker(callback, &(obj->_k2hshm), strkey.c_str(), strattr.c_str()));
		info.GetReturnValue().Set(Nan::True());
	}else{
		K2HAttrs*	attrs = obj->_k2hshm.GetAttrs(strkey.c_str());
		if(!attrs){
			info.GetReturnValue().Set(Nan::Null());
			return;
		}
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
					info.GetReturnValue().Set(Nan::New<String>(reinterpret_cast<const char*>(iter->pval)).ToLocalChecked());
				}else{
					info.GetReturnValue().Set(Nan::Null());
				}
				is_found = true;
				break;
			}
		}
		delete attrs;
		if(!is_found){
			// not found
			info.GetReturnValue().Set(Nan::Null());
		}
	}
}

/**
 * @memberof K2hNode
 * @fn bool AddAttr(\
 * 	  String key\
 * 	, String attrkey\
 * 	, String attrval=null\
 * 	, Callback cbfunc=null\
 * )
 *
 * @brief	Add the attribute to the key
 *			If the callback function is specified, or on callback handles for this,
 *			this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key		Specify the key name
 * @param[in] attrkey	Specify the attribute key name
 * @param[in] attrval	Specify the attribute value
 * @param[in] cbfunc	callback function.
 *
 * @return return true for success, false for failure
 *
 */

NAN_METHOD(K2hNode::AddAttr)
{
	if(info.Length() < 3){
		Nan::ThrowSyntaxError("No key name or no attribute name/value are specified.");
		return;
	}

	K2hNode*		obj			= Nan::ObjectWrap::Unwrap<K2hNode>(info.This());
	string			strkey;
	string			strattr;
	bool			is_val_set	= false;
	string			strval;
	Nan::Callback*	callback	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_ADDATTR]);

	if(info[0]->IsNull()){
		Nan::ThrowSyntaxError("key is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[0]);
		strkey					= std::string(*buf);
	}
	if(info[1]->IsNull()){
		Nan::ThrowSyntaxError("attribute name is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[1]);
		strattr					= std::string(*buf);
	}
	if(2 < info.Length()){
		if(info[2]->IsFunction()){
			if(3 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback			= new Nan::Callback(info[2].As<v8::Function>());
		}else if(!info[2]->IsNull()){
			Nan::Utf8String	buf(info[2]);
			strval				= std::string(*buf);
			is_val_set			= true;
		}
	}
	if(3 < info.Length()){
		if(4 < info.Length() || !info[3]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback				= new Nan::Callback(info[3].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new AddAttrWorker(callback, &(obj->_k2hshm), strkey.c_str(), strattr.c_str(), (is_val_set ? strval.c_str() : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(obj->_k2hshm.AddAttr(strkey.c_str(), strattr.c_str(), (is_val_set ? strval.c_str() : NULL))));
	}
}

//@}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
