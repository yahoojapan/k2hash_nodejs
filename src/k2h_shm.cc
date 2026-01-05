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
#include "k2h_shm_async.h"
#include "k2h_queue.h"
#include "k2h_keyqueue.h"

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
// Utility (using StackEmitCB Class)
//---------------------------------------------------------
static Napi::Value SetK2hEmitterCallback(const Napi::CallbackInfo& info, size_t pos, const char* pemitter)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// check parameter
	if(info.Length() <= pos){
		Napi::TypeError::New(env, "No callback is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	if(!info[pos].IsFunction()){
		Napi::TypeError::New(env, "The parameter is not callback function.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	Napi::Function cb = info[pos].As<Napi::Function>();

	// set
	bool result = obj->_cbs.Set(std::string(pemitter), cb);
	return Napi::Boolean::New(env, result);
}

static Napi::Value UnsetK2hEmitterCallback(const Napi::CallbackInfo& info, const char* pemitter)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// unset
	bool result = obj->_cbs.Unset(std::string(pemitter));
	return Napi::Boolean::New(env, result);
}

//---------------------------------------------------------
// K2hNode Class
//---------------------------------------------------------
Napi::FunctionReference	K2hNode::constructor;

//---------------------------------------------------------
// K2hNode Methods
//---------------------------------------------------------
K2hNode::K2hNode(const Napi::CallbackInfo& info) : Napi::ObjectWrap<K2hNode>(info), _cbs(), _k2hshm()
{
	// [NOTE]
	// Perhaps due to an initialization order issue, these
	// k2hash debug environment variable settings don't work.
	// So, load the environment variables and set the debug
	// mode/file settings here.
	//
	const char* k2hdbgmode = std::getenv("K2HDBGMODE");
	const char* k2hdbgfile = std::getenv("K2HDBGFILE");
	if(k2hdbgmode && k2hdbgfile){
		if(0 == strcasecmp(k2hdbgmode, "SLT") || 0 == strcasecmp(k2hdbgmode, "SILENT")){
			k2h_set_debug_level_silent();
		}else if(0 == strcasecmp(k2hdbgmode, "ERR") || 0 == strcasecmp(k2hdbgmode, "ERROR")){
			k2h_set_debug_level_error();
		}else if(0 == strcasecmp(k2hdbgmode, "WARNING") || 0 == strcasecmp(k2hdbgmode, "WARN") || 0 == strcasecmp(k2hdbgmode, "WAN")){
			k2h_set_debug_level_warning();
		}else if(0 == strcasecmp(k2hdbgmode, "INFO") || 0 == strcasecmp(k2hdbgmode, "INF") || 0 == strcasecmp(k2hdbgmode, "MSG")){
			k2h_set_debug_level_message();
		}
		k2h_set_debug_file(k2hdbgfile);		// Ignore any errors that occur.
	}
}

K2hNode::~K2hNode()
{
	_k2hshm.Detach();
}

void K2hNode::Init(Napi::Env env, Napi::Object exports)
{
	Napi::Function funcs = DefineClass(env, "K2hNode", {
		// DefineClass normally handles the constructor internally. Therefore, there is no need
		// to include a static wrapper New() in the class prototype, which works the same way as
		// when using NAN.
		// For reference, the following example shows how to declare New as a static method.
		// (Registration is not normally required.)
		//
		//	K2hNode::InstanceMethod("new", 						&K2hNode::New),

		// Prototype for event emitter
		K2hNode::InstanceMethod("on",							&K2hNode::On),
		K2hNode::InstanceMethod("onCreate",						&K2hNode::OnCreate),
		K2hNode::InstanceMethod("onOpen",						&K2hNode::OnOpen),
		K2hNode::InstanceMethod("onOpenRW",						&K2hNode::OnOpenRW),
		K2hNode::InstanceMethod("onOpenRO",						&K2hNode::OnOpenRO),
		K2hNode::InstanceMethod("onOpenTempfile",				&K2hNode::OnOpenTempfile),
		K2hNode::InstanceMethod("onOpenMem",					&K2hNode::OnOpenMem),
		K2hNode::InstanceMethod("onClose",						&K2hNode::OnClose),
		K2hNode::InstanceMethod("onGet",						&K2hNode::OnGet),
		K2hNode::InstanceMethod("onGetSubkeys",					&K2hNode::OnGetSubkeys),
		K2hNode::InstanceMethod("onGetAttrs",					&K2hNode::OnGetAttrs),
		K2hNode::InstanceMethod("onGetAttrValue",				&K2hNode::OnGetAttrValue),
		K2hNode::InstanceMethod("onSet",						&K2hNode::OnSet),
		K2hNode::InstanceMethod("onAddSubkey",					&K2hNode::OnAddSubkey),
		K2hNode::InstanceMethod("onAddSubkeys",					&K2hNode::OnAddSubkeys),
		K2hNode::InstanceMethod("onAddAttr",					&K2hNode::OnAddAttr),
		K2hNode::InstanceMethod("onRemove",						&K2hNode::OnRemove),
		K2hNode::InstanceMethod("onRemoveAll",					&K2hNode::OnRemoveAll),
		K2hNode::InstanceMethod("onLoad",						&K2hNode::OnLoad),
		K2hNode::InstanceMethod("onLoadArchive",				&K2hNode::OnLoad),
		K2hNode::InstanceMethod("onPut",						&K2hNode::OnPut),
		K2hNode::InstanceMethod("onPutArchive",					&K2hNode::OnPut),
		K2hNode::InstanceMethod("off",							&K2hNode::Off),
		K2hNode::InstanceMethod("offCreate",					&K2hNode::OffCreate),
		K2hNode::InstanceMethod("offOpen",						&K2hNode::OffOpen),
		K2hNode::InstanceMethod("offOpenRW",					&K2hNode::OffOpenRW),
		K2hNode::InstanceMethod("offOpenRO",					&K2hNode::OffOpenRO),
		K2hNode::InstanceMethod("offOpenTempfile",				&K2hNode::OffOpenTempfile),
		K2hNode::InstanceMethod("offOpenMem",					&K2hNode::OffOpenMem),
		K2hNode::InstanceMethod("offClose",						&K2hNode::OffClose),
		K2hNode::InstanceMethod("offGet",						&K2hNode::OffGet),
		K2hNode::InstanceMethod("offGetSubkeys",				&K2hNode::OffGetSubkeys),
		K2hNode::InstanceMethod("offGetAttrs",					&K2hNode::OffGetAttrs),
		K2hNode::InstanceMethod("offGetAttrValue",				&K2hNode::OffGetAttrValue),
		K2hNode::InstanceMethod("offSet",						&K2hNode::OffSet),
		K2hNode::InstanceMethod("offAddSubkey",					&K2hNode::OffAddSubkey),
		K2hNode::InstanceMethod("offAddSubkeys",				&K2hNode::OffAddSubkeys),
		K2hNode::InstanceMethod("offAddAttr",					&K2hNode::OffAddAttr),
		K2hNode::InstanceMethod("offRemove",					&K2hNode::OffRemove),
		K2hNode::InstanceMethod("offRemoveAll",					&K2hNode::OffRemoveAll),
		K2hNode::InstanceMethod("offLoad",						&K2hNode::OffLoad),
		K2hNode::InstanceMethod("offLoadArchive",				&K2hNode::OffLoad),
		K2hNode::InstanceMethod("offPut",						&K2hNode::OffPut),
		K2hNode::InstanceMethod("offPutArchive",				&K2hNode::OffPut),

		// Prototype
		K2hNode::InstanceMethod("create",						&K2hNode::Create),
		K2hNode::InstanceMethod("open",							&K2hNode::Open),
		K2hNode::InstanceMethod("openRW",						&K2hNode::OpenRW),
		K2hNode::InstanceMethod("openRO",						&K2hNode::OpenRO),
		K2hNode::InstanceMethod("openTempfile",					&K2hNode::OpenTempfile),
		K2hNode::InstanceMethod("openMem",						&K2hNode::OpenMem),
		K2hNode::InstanceMethod("close",						&K2hNode::Close),

		K2hNode::InstanceMethod("getValue",						&K2hNode::GetValue),
		K2hNode::InstanceMethod("getSubkeys",					&K2hNode::GetSubkeys),

		K2hNode::InstanceMethod("setValue",						&K2hNode::SetValue),
		K2hNode::InstanceMethod("addSubkey",					&K2hNode::AddSubkey),
		K2hNode::InstanceMethod("addSubkeys",					&K2hNode::AddSubkeys),

		K2hNode::InstanceMethod("remove",						&K2hNode::Remove),
		K2hNode::InstanceMethod("removeAll",					&K2hNode::RemoveAll),

		K2hNode::InstanceMethod("printState",					&K2hNode::PrintState),
		K2hNode::InstanceMethod("printVersion",					&K2hNode::PrintVersion),

		K2hNode::InstanceMethod("dumpHead",						&K2hNode::DumpHead),
		K2hNode::InstanceMethod("dumpKeytable",					&K2hNode::DumpKeytable),
		K2hNode::InstanceMethod("dumpFullKeytable",				&K2hNode::DumpFullKeytable),
		K2hNode::InstanceMethod("dumpElementtable",				&K2hNode::DumpElementtable),
		K2hNode::InstanceMethod("dumpFull",						&K2hNode::DumpFull),

		K2hNode::InstanceMethod("transaction",					&K2hNode::Transaction),
		K2hNode::InstanceMethod("enableTransaction",			&K2hNode::EnableTransaction),
		K2hNode::InstanceMethod("disableTransaction",			&K2hNode::DisableTransaction),

		K2hNode::InstanceMethod("getTransactionThreadPool",		&K2hNode::GetTransactionThreadPool),
		K2hNode::InstanceMethod("setTransactionThreadPool",		&K2hNode::SetTransactionThreadPool),
		K2hNode::InstanceMethod("unsetTransactionThreadPool",	&K2hNode::UnsetTransactionThreadPool),

		K2hNode::InstanceMethod("putArchive",					&K2hNode::PutArchive),
		K2hNode::InstanceMethod("loadArchive",					&K2hNode::LoadArchive),

		K2hNode::InstanceMethod("getQueue",						&K2hNode::getQueue),
		K2hNode::InstanceMethod("getKeyQueue",					&K2hNode::getKeyQueue),

		K2hNode::InstanceMethod("setCommonAttribute",			&K2hNode::SetCommonAttribute),
		K2hNode::InstanceMethod("cleanCommonAttribute",			&K2hNode::CleanCommonAttribute),

		K2hNode::InstanceMethod("addAttrPluginLib",				&K2hNode::AddAttrPluginLib),
		K2hNode::InstanceMethod("addAttrCryptPass",				&K2hNode::AddAttrCryptPass),

		K2hNode::InstanceMethod("getAttrVersionInfos",			&K2hNode::GetAttrVersionInfos),
		K2hNode::InstanceMethod("getAttrInfos",					&K2hNode::GetAttrInfos),
		K2hNode::InstanceMethod("getAttrs",						&K2hNode::GetAttrs),
		K2hNode::InstanceMethod("getAttrValue",					&K2hNode::GetAttrValue),

		K2hNode::InstanceMethod("addAttr",						&K2hNode::AddAttr)
	});

	constructor = Napi::Persistent(funcs);
	constructor.SuppressDestruct();

	// Initialize K2hQueue and K2hKeyQueue classes
	K2hQueue::Init(env, exports);
	K2hKeyQueue::Init(env, exports);

	// [NOTE]
	// do NOT do exports.Set("K2hNode", func) here if InitAll will return createFn.
	//
}

Napi::Value K2hNode::New(const Napi::CallbackInfo& info)
{
	if(info.IsConstructCall()){
		// Invoked as constructor: new K2hNode()
		return info.This();
	}else{
		// Invoked as plain function K2hNode(), turn into construct call.
		return constructor.New({});		// always no arguments
	}
}

// [NOTE]
// The logic for receiving arguments when switching to N-API has been removed.
// This is because the arguments were not used in the first place and did not
// need to be defined.
//
// NewInstance( always no argments )
Napi::Object K2hNode::NewInstance(Napi::Env env)
{
	Napi::EscapableHandleScope scope(env);
	Napi::Object obj = constructor.New({}).As<Napi::Object>();
	return scope.Escape(napi_value(obj)).ToObject();
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

Napi::Value K2hNode::On(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// check
	if(info.Length() < 1){
		Napi::TypeError::New(env, "No handle emitter name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else if(info.Length() < 2){
		Napi::TypeError::New(env, "No callback is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// check emitter name
	std::string emitter  = info[0].ToString().Utf8Value();
	const char* pemitter = GetNormalizationEmitter(emitter.c_str(), stc_k2h_emitters);
	if(!pemitter){
		std::string	msg	= "Unknown ";
		msg				+= emitter;
		msg				+= " emitter";
		Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// add callback
	return SetK2hEmitterCallback(info, 1, pemitter);
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

Napi::Value K2hNode::OnCreate(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_CREATE]);
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

Napi::Value K2hNode::OnOpen(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_OPEN]);
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

Napi::Value K2hNode::OnOpenRW(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_OPENRW]);
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

Napi::Value K2hNode::OnOpenRO(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_OPENRO]);
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

Napi::Value K2hNode::OnOpenTempfile(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_OPENTEMP]);
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

Napi::Value K2hNode::OnOpenMem(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_OPENMEM]);
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

Napi::Value K2hNode::OnClose(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_CLOSE]);
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

Napi::Value K2hNode::OnGet(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_GET]);
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

Napi::Value K2hNode::OnGetSubkeys(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_GETSUBKEYS]);
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

Napi::Value K2hNode::OnGetAttrs(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_GETATTRS]);
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

Napi::Value K2hNode::OnGetAttrValue(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_GETATTRVAL]);
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

Napi::Value K2hNode::OnSet(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_SET]);
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

Napi::Value K2hNode::OnAddSubkey(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_ADDSUBKEY]);
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

Napi::Value K2hNode::OnAddSubkeys(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_ADDSUBKEYS]);
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

Napi::Value K2hNode::OnAddAttr(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_ADDATTR]);
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

Napi::Value K2hNode::OnRemove(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_REMOVE]);
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

Napi::Value K2hNode::OnRemoveAll(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_REMOVEALL]);
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

Napi::Value K2hNode::OnLoad(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_LOAD]);
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

Napi::Value K2hNode::OnPut(const Napi::CallbackInfo& info)
{
	return SetK2hEmitterCallback(info, 0, stc_k2h_emitters[K2H_EMITTER_POS_PUT]);
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

Napi::Value K2hNode::Off(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No handle emitter name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// check emitter name
	std::string	emitter  = info[0].ToString().Utf8Value();
	const char*	pemitter = GetNormalizationEmitter(emitter.c_str(), stc_k2h_emitters);
	if (nullptr == pemitter) {
		std::string msg	= "Unknown ";
		msg				+= emitter;
		msg				+= " emitter";
		Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
		return env.Undefined();
	}
	// unset callback
	return UnsetK2hEmitterCallback(info, pemitter);
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

Napi::Value K2hNode::OffCreate(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_CREATE]);
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

Napi::Value K2hNode::OffOpen(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_OPEN]);
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

Napi::Value K2hNode::OffOpenRW(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_OPENRW]);
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

Napi::Value K2hNode::OffOpenRO(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_OPENRO]);
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

Napi::Value K2hNode::OffOpenTempfile(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_OPENTEMP]);
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

Napi::Value K2hNode::OffOpenMem(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_OPENMEM]);
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

Napi::Value K2hNode::OffClose(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_CLOSE]);
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

Napi::Value K2hNode::OffGet(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_GET]);
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

Napi::Value K2hNode::OffGetSubkeys(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_GETSUBKEYS]);
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

Napi::Value K2hNode::OffGetAttrs(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_GETATTRS]);
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

Napi::Value K2hNode::OffGetAttrValue(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_GETATTRVAL]);
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

Napi::Value K2hNode::OffSet(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_SET]);
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

Napi::Value K2hNode::OffAddSubkey(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_ADDSUBKEY]);
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

Napi::Value K2hNode::OffAddSubkeys(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_ADDSUBKEYS]);
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

Napi::Value K2hNode::OffAddAttr(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_ADDATTR]);
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

Napi::Value K2hNode::OffRemove(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_REMOVE]);
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

Napi::Value K2hNode::OffRemoveAll(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_REMOVEALL]);
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

Napi::Value K2hNode::OffLoad(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_LOAD]);
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

Napi::Value K2hNode::OffPut(const Napi::CallbackInfo& info)
{
	return UnsetK2hEmitterCallback(info, stc_k2h_emitters[K2H_EMITTER_POS_PUT]);
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
 * k2h.SetValue('key', 'val') ;
 * console_log( k2h.GetValue('key') ) ;
 * k2h.Close() ;
 * @endcode
 *
 */

Napi::Value K2hNode::Create(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// check
	if(info.Length() < 1){
		Napi::TypeError::New(env, "No file name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// Variables with default value for optional parameters
	bool	isfullmapping	= true;
	int		mask_bitcnt		= K2HShm::MIN_MASK_BITCOUNT;
	int		cmask_bitcnt	= K2HShm::DEFAULT_COLLISION_MASK_BITCOUNT;
	int		max_element_cnt	= K2HShm::DEFAULT_MAX_ELEMENT_CNT;
	size_t	pagesize		= K2HShm::MIN_PAGE_SIZE;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_CREATE]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// info[0] : Required
	if(info[0].IsNull() || info[0].IsUndefined()){
		Napi::TypeError::New(env, "file name is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	std::string filename = info[0].ToString().Utf8Value();

	// info[1]
	if(1 < info.Length()){
		if(info[1].IsFunction()){
			if(2 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[1].As<Napi::Function>();
			hasCallback		= true;
		}else{
			isfullmapping = info[1].ToBoolean();
		}
	}

	// info[2]
	if(2 < info.Length()){
		if(info[2].IsFunction()){
			if(3 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[2].As<Napi::Function>();
			hasCallback		= true;
		}else{
			if(!info[2].IsNumber()){
				Napi::TypeError::New(env, "Third parameter must be int.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			mask_bitcnt = info[2].As<Napi::Number>().Int32Value();
		}
	}

	// info[3]
	if(3 < info.Length()){
		if(info[3].IsFunction()){
			if(4 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[3].As<Napi::Function>();
			hasCallback		= true;
		}else{
			if(!info[3].IsNumber()){
				Napi::TypeError::New(env, "Fourth parameter must be int.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			cmask_bitcnt = info[3].As<Napi::Number>().Int32Value();
		}
	}

	// info[4]
	if(4 < info.Length()){
		if(info[4].IsFunction()){
			if(5 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[4].As<Napi::Function>();
			hasCallback		= true;
		}else{
			if(!info[4].IsNumber()){
				Napi::TypeError::New(env, "Fifth parameter must be int.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			max_element_cnt = info[4].As<Napi::Number>().Int32Value();
		}
	}

	// info[5]
	if(5 < info.Length()){
		if(info[5].IsFunction()){
			if(6 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[5].As<Napi::Function>();
			hasCallback		= true;
		}else{
			if(!info[5].IsNumber()){
				Napi::TypeError::New(env, "Sixth parameter must be int.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			pagesize = static_cast<size_t>(info[5].As<Napi::Number>().Int64Value());
		}
	}

	// info[6]
	if(6 < info.Length()){
		if(7 < info.Length() || !info[6].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[6].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		// Create worker and Queue it
		CreateAsyncWorker* worker = new CreateAsyncWorker(maybeCallback, &(obj->_k2hshm), filename, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool result = obj->_k2hshm.Create(filename.c_str(), isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize);
		return Napi::Boolean::New(env, result);
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

Napi::Value K2hNode::Open(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No file name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string	filename		= info[0].ToString().Utf8Value();
	bool		isreadonly		= false;
	bool		istempfile		= false;
	bool		isfullmapping	= true;
	int			mask_bitcnt		= K2HShm::MIN_MASK_BITCOUNT;
	int			cmask_bitcnt	= K2HShm::DEFAULT_COLLISION_MASK_BITCOUNT;
	int			max_element_cnt	= K2HShm::DEFAULT_MAX_ELEMENT_CNT;
	size_t		pagesize		= K2HShm::MIN_PAGE_SIZE;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_OPEN]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(1 < info.Length()){
		if(info[1].IsFunction()){
			if(2 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[1].As<Napi::Function>();
			hasCallback		= true;
		}else{
			isreadonly		= info[1].ToBoolean().Value();
		}
	}

	if(2 < info.Length()){
		if(info[2].IsFunction()){
			if(3 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[2].As<Napi::Function>();
			hasCallback		= true;
		} else {
			istempfile		= info[2].ToBoolean().Value();
		}
	}

	if(3 < info.Length()){
		if(info[3].IsFunction()){
			if(4 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[3].As<Napi::Function>();
			hasCallback		= true;
		} else {
			isfullmapping	= info[3].ToBoolean().Value();
		}
	}

	if(4 < info.Length()){
		if(info[4].IsFunction()){
			if(5 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[4].As<Napi::Function>();
			hasCallback		= true;
		}else{
			mask_bitcnt		= info[4].ToNumber().Int32Value();
		}
	}

	if(5 < info.Length()){
		if(info[5].IsFunction()){
			if(6 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[5].As<Napi::Function>();
			hasCallback		= true;
		}else{
			cmask_bitcnt	= info[5].ToNumber().Int32Value();
		}
	}

	if(6 < info.Length()){
		if(info[6].IsFunction()){
			if(7 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[6].As<Napi::Function>();
			hasCallback		= true;
		}else{
			max_element_cnt	= info[6].ToNumber().Int32Value();
		}
	}

	if(7 < info.Length()){
		if(info[7].IsFunction()){
			if(8 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[7].As<Napi::Function>();
			hasCallback		= true;
		}else{
			pagesize		= static_cast<size_t>(info[7].ToNumber().Int64Value());
		}
	}

	if(8 < info.Length()){
		if(9 < info.Length() || !info[8].IsFunction()) {
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[8].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		OpenAsyncWorker* worker = new OpenAsyncWorker(maybeCallback, &(obj->_k2hshm), filename, isreadonly, false, istempfile, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool result = obj->_k2hshm.Attach(filename.c_str(), isreadonly, false, istempfile, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize);
		return Napi::Boolean::New(env, result);
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

Napi::Value K2hNode::OpenRW(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No file name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string filename		= info[0].ToString().Utf8Value();
	bool		isfullmapping	= true;
	int			mask_bitcnt		= K2HShm::MIN_MASK_BITCOUNT;
	int			cmask_bitcnt	= K2HShm::DEFAULT_COLLISION_MASK_BITCOUNT;
	int			max_element_cnt	= K2HShm::DEFAULT_MAX_ELEMENT_CNT;
	size_t		pagesize		= K2HShm::MIN_PAGE_SIZE;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_OPENRW]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(1 < info.Length()){
		if(info[1].IsFunction()){
			if(2 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[1].As<Napi::Function>();
			hasCallback		= true;
		}else{
			isfullmapping	= info[1].ToBoolean().Value();
		}
	}

	if(2 < info.Length()){
		if(info[2].IsFunction()){
			if(3 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[2].As<Napi::Function>();
			hasCallback		= true;
		}else{
			mask_bitcnt		= info[2].ToNumber().Int32Value();
		}
	}

	if(3 < info.Length()){
		if(info[3].IsFunction()){
			if(4 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[3].As<Napi::Function>();
			hasCallback		= true;
		}else{
			cmask_bitcnt	= info[3].ToNumber().Int32Value();
		}
	}

	if(4 < info.Length()){
		if(info[4].IsFunction()){
			if(5 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[4].As<Napi::Function>();
			hasCallback		= true;
		}else{
			max_element_cnt	= info[4].ToNumber().Int32Value();
		}
	}

	if(5 < info.Length()){
		if(info[5].IsFunction()){
			if(6 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[5].As<Napi::Function>();
			hasCallback		= true;
		}else{
			pagesize		= static_cast<size_t>(info[5].ToNumber().Int64Value());
		}
	}

	if(6 < info.Length()){
		if(7 < info.Length() || !info[6].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[6].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		OpenAsyncWorker* worker = new OpenAsyncWorker(maybeCallback, &(obj->_k2hshm), filename, false, false, false, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool result = obj->_k2hshm.Attach(filename.c_str(), false, false, false, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize);
		return Napi::Boolean::New(env, result);
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

Napi::Value K2hNode::OpenRO(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No file name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string	filename		= info[0].ToString().Utf8Value();
	bool		isfullmapping	= true;
	int			mask_bitcnt		= K2HShm::MIN_MASK_BITCOUNT;
	int			cmask_bitcnt	= K2HShm::DEFAULT_COLLISION_MASK_BITCOUNT;
	int			max_element_cnt	= K2HShm::DEFAULT_MAX_ELEMENT_CNT;
	size_t		pagesize		= K2HShm::MIN_PAGE_SIZE;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_OPENRO]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(1 < info.Length()){
		if(info[1].IsFunction()){
			if(2 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[1].As<Napi::Function>();
			hasCallback		= true;
		}else{
			isfullmapping	= info[1].ToBoolean().Value();
		}
	}

	if(2 < info.Length()){
		if(info[2].IsFunction()){
			if(3 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[2].As<Napi::Function>();
			hasCallback		= true;
		}else{
			mask_bitcnt		= info[2].ToNumber().Int32Value();
		}
	}

	if(3 < info.Length()){
		if(info[3].IsFunction()){
			if(4 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[3].As<Napi::Function>();
			hasCallback		= true;
		}else{
			cmask_bitcnt	= info[3].ToNumber().Int32Value();
		}
	}

	if(4 < info.Length()){
		if(info[4].IsFunction()){
			if(5 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[4].As<Napi::Function>();
			hasCallback		= true;
		}else{
			max_element_cnt	= info[4].ToNumber().Int32Value();
		}
	}

	if(5 < info.Length()){
		if(info[5].IsFunction()){
			if(6 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[5].As<Napi::Function>();
			hasCallback		= true;
		}else{
			pagesize		= static_cast<size_t>(info[5].ToNumber().Int64Value());
		}
	}

	if(6 < info.Length()){
		if(7 < info.Length() || !info[6].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[6].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		OpenAsyncWorker* worker = new OpenAsyncWorker(maybeCallback, &(obj->_k2hshm), filename, true, false, false, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool result = obj->_k2hshm.Attach(filename.c_str(), true, false, false, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize);
		return Napi::Boolean::New(env, result);
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

Napi::Value K2hNode::OpenTempfile(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No file name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string	filename		= info[0].ToString().Utf8Value();
	bool		isfullmapping	= true;
	int			mask_bitcnt		= K2HShm::MIN_MASK_BITCOUNT;
	int			cmask_bitcnt	= K2HShm::DEFAULT_COLLISION_MASK_BITCOUNT;
	int			max_element_cnt	= K2HShm::DEFAULT_MAX_ELEMENT_CNT;
	size_t		pagesize		= K2HShm::MIN_PAGE_SIZE;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_OPENTEMP]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(1 < info.Length()){
		if(info[1].IsFunction()){
			if(2 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[1].As<Napi::Function>();
			hasCallback		= true;
		}else{
			isfullmapping	= info[1].ToBoolean().Value();
		}
	}

	if(2 < info.Length()){
		if(info[2].IsFunction()){
			if(3 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[2].As<Napi::Function>();
			hasCallback		= true;
		}else{
			mask_bitcnt		= info[2].ToNumber().Int32Value();
		}
	}

	if(3 < info.Length()){
		if(info[3].IsFunction()){
			if(4 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[3].As<Napi::Function>();
			hasCallback		= true;
		}else{
			cmask_bitcnt	= info[3].ToNumber().Int32Value();
		}
	}

	if(4 < info.Length()){
		if(info[4].IsFunction()){
			if(5 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[4].As<Napi::Function>();
			hasCallback		= true;
		}else{
			max_element_cnt	= info[4].ToNumber().Int32Value();
		}
	}

	if(5 < info.Length()){
		if(info[5].IsFunction()){
			if(6 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[5].As<Napi::Function>();
			hasCallback		= true;
		}else{
			pagesize		= static_cast<size_t>(info[5].ToNumber().Int64Value());
		}
	}

	if(6 < info.Length()){
		if(7 < info.Length() || !info[6].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[6].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		OpenAsyncWorker* worker = new OpenAsyncWorker(maybeCallback, &(obj->_k2hshm), filename, false, false, true, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool result = obj->_k2hshm.Attach(filename.c_str(), false, false, true, isfullmapping, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize);
		return Napi::Boolean::New(env, result);
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

Napi::Value K2hNode::OpenMem(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	int		mask_bitcnt		= K2HShm::MIN_MASK_BITCOUNT;
	int		cmask_bitcnt	= K2HShm::DEFAULT_COLLISION_MASK_BITCOUNT;
	int		max_element_cnt	= K2HShm::DEFAULT_MAX_ELEMENT_CNT;
	size_t	pagesize		= K2HShm::MIN_PAGE_SIZE;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_OPENMEM]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(0 < info.Length()){
		if(info[0].IsFunction()){
			if(1 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[0].As<Napi::Function>();
			hasCallback		= true;
		}else{
			mask_bitcnt		= info[0].ToNumber().Int32Value();
		}
	}

	if(1 < info.Length()){
		if(info[1].IsFunction()){
			if(2 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[1].As<Napi::Function>();
			hasCallback		= true;
		}else{
			cmask_bitcnt	= info[1].ToNumber().Int32Value();
		}
	}

	if(2 < info.Length()){
		if(info[2].IsFunction()){
			if(3 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[2].As<Napi::Function>();
			hasCallback		= true;
		}else{
			max_element_cnt	= info[2].ToNumber().Int32Value();
		}
	}

	if(3 < info.Length()){
		if(info[3].IsFunction()){
			if(4 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[3].As<Napi::Function>();
			hasCallback		= true;
		}else{
			pagesize		= static_cast<size_t>(info[3].ToNumber().Int64Value());
		}
	}

	if(4 < info.Length()){
		if(5 < info.Length() || !info[4].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[4].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		OpenAsyncWorker* worker = new OpenAsyncWorker(maybeCallback, &(obj->_k2hshm), std::string(""), false, true, false, true, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool result = obj->_k2hshm.Attach(nullptr, false, true, false, true, mask_bitcnt, cmask_bitcnt, max_element_cnt, pagesize);
		return Napi::Boolean::New(env, result);
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
 * k2h.SetValue('key', 'val') ;
 * console_log( k2h.GetValue('key') ) ;
 * k2h.Close() ;
 * @endcode
 */

Napi::Value K2hNode::Close(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_CLOSE]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(0 < info.Length()){
		if(1 < info.Length() || !info[0].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[0].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		CloseAsyncWorker* worker = new CloseAsyncWorker(maybeCallback, &(obj->_k2hshm));
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool result = obj->_k2hshm.Detach();
		return Napi::Boolean::New(env, result);
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
 * k2h.SetValue('key', 'val') ;
 * console_log( k2h.GetValue('key') ) ;
 * k2h.Close() ;
 * @endcode
 */

Napi::Value K2hNode::GetValue(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No key name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string	strkey;
	bool		is_subkey_set	= false;
	std::string	strsubkey;
	bool		attrchk			= true;
	bool		is_pass_set		= false;
	std::string	strpass;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_GET]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(info[0].IsNull()){
		Napi::TypeError::New(env, "key is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		strkey = info[0].ToString().Utf8Value();
	}

	if(1 < info.Length()){
		if(info[1].IsFunction()){
			if(2 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[1].As<Napi::Function>();
			hasCallback		= true;
		}else if(!info[1].IsNull()){
			strsubkey		= info[1].ToString().Utf8Value();
			is_subkey_set	= true;
		}
	}

	if(2 < info.Length()){
		if(info[2].IsFunction()){
			if(3 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[2].As<Napi::Function>();
			hasCallback		= true;
		}else if(info[2].IsBoolean()){
			attrchk			= info[2].ToBoolean().Value();
		}else{
			Napi::TypeError::New(env, "Third parameter must be boolean.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
	}

	if(3 < info.Length()){
		if(info[3].IsFunction()){
			if(4 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[3].As<Napi::Function>();
			hasCallback		= true;
		}else if(!info[3].IsNull()){
			strpass			= info[3].ToString().Utf8Value();
			is_pass_set		= true;
		}
	}

	if(4 < info.Length()){
		if(5 < info.Length() || !info[4].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[4].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		GetValueAsyncWorker* worker = new GetValueAsyncWorker(maybeCallback, &(obj->_k2hshm), strkey.c_str(), (is_subkey_set ? strsubkey.c_str() : NULL), attrchk, (is_pass_set ? strpass.c_str() : NULL));
		worker->Queue();
		return Napi::Boolean::New(env, true);
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
				return env.Undefined();
			}
			strkey = strsubkey;
		}

		char*	presult = obj->_k2hshm.Get(strkey.c_str(), attrchk, (is_pass_set ? strpass.c_str() : NULL));
		if(presult){
			Napi::String result = Napi::String::New(env, presult, static_cast<size_t>(strlen(presult)));
			K2H_Free(presult);
			return result;
		}else{
			return env.Null();
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

Napi::Value K2hNode::GetSubkeys(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No key name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string	strkey;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_GETSUBKEYS]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(info[0].IsNull()){
		Napi::TypeError::New(env, "key is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		strkey = info[0].ToString().Utf8Value();
	}

	if(1 < info.Length()){
		if(2 < info.Length() || !info[1].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[1].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		GetSubkeysAsyncWorker* worker = new GetSubkeysAsyncWorker(maybeCallback, &(obj->_k2hshm), strkey.c_str());
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		K2HSubKeys*	sk = obj->_k2hshm.GetSubKeys(strkey.c_str());
		if(!sk){
			return env.Null();
		}
		strarr_t	strarr;
		sk->StringArray(strarr);

		Napi::Array	retarr	= Napi::Array::New(env, static_cast<size_t>(strarr.size()));
		uint32_t	pos		= 0;
		for(strarr_t::const_iterator iter = strarr.begin(); iter != strarr.end(); ++iter, ++pos){
			retarr.Set(pos, Napi::String::New(env, iter->c_str(), static_cast<size_t>(strlen(iter->c_str()))));
		}
		delete sk;

		return retarr;
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
 * k2h.SetValue('key', 'val') ;
 * console_log( k2h.GetValue('key') ) ;
 * k2h.Close() ;
 * @endcode
 */

Napi::Value K2hNode::SetValue(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 2){
		Napi::TypeError::New(env, "No key name or no value are specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string	strkey;
	bool		is_val_set = false;
	std::string	strval;
	bool		is_skey_set = false;
	std::string	strsubkey;
	bool		is_pass_set = false;
	std::string	strpass;
	time_t		expire = 0;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_SET]);
	if(emitterCbRef){
		maybeCallback = emitterCbRef->Value();
		hasCallback = true;
	}

	// parse positional optional args
	if(info[0].IsNull()){
		Napi::TypeError::New(env, "key is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		strkey = info[0].ToString().Utf8Value();
	}

	if(!info[1].IsNull()){
		strval		= info[1].ToString().Utf8Value();
		is_val_set	= true;
	}

	if(2 < info.Length()){
		if(info[2].IsFunction()){
			if(3 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[2].As<Napi::Function>();
			hasCallback		= true;
		}else if(!info[2].IsNull()){
			strsubkey	= info[2].ToString().Utf8Value();
			is_skey_set	= true;
		}
	}

	if(3 < info.Length()){
		if(info[3].IsFunction()){
			if(4 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[3].As<Napi::Function>();
			hasCallback		= true;
		}else if(!info[3].IsNull()){
			strpass		= info[3].ToString().Utf8Value();
			is_pass_set	= true;
		}
	}

	if(4 < info.Length()){
		if(info[4].IsFunction()){
			if(5 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[4].As<Napi::Function>();
			hasCallback		= true;
		}else if(info[4].IsNumber()){
			int32_t	nexpire	= info[4].ToNumber().Int32Value();
			expire			= static_cast<time_t>(nexpire);
		}else{
			Napi::TypeError::New(env, "Expire parameter must be int32 value.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
	}

	if(5 < info.Length()){
		if(6 < info.Length() || !info[5].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[5].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		SetValueAsyncWorker* worker = new SetValueAsyncWorker(maybeCallback, &(obj->_k2hshm), strkey.c_str(), (is_skey_set ? strsubkey.c_str() : NULL), (is_val_set ? strval.c_str() : NULL), (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL));
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool result;
		if(is_skey_set){
			// subkey is specified
			result = obj->_k2hshm.AddSubkey(strkey.c_str(), strsubkey.c_str(), (is_val_set ? strval.c_str() : NULL), (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL));
		}else{
			// subkey is not specified
			result = obj->_k2hshm.Set(strkey.c_str(), (is_val_set ? strval.c_str() : NULL), (is_pass_set ? strpass.c_str() : NULL), (expire > 0 ? &expire : NULL));
		}
		return Napi::Boolean::New(env, result);
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

Napi::Value K2hNode::AddSubkey(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 2){
		Napi::TypeError::New(env, "No key name or no subkey name are specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string	strkey;
	std::string	strsubkey;
	bool		is_value_set = false;
	std::string	strvalue;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_ADDSUBKEY]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(info[0].IsNull()){
		Napi::TypeError::New(env, "key is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		strkey = info[0].ToString().Utf8Value();
	}

	if(info[1].IsNull()){
		Napi::TypeError::New(env, "subkey is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		strsubkey = info[1].ToString().Utf8Value();
	}

	if(2 < info.Length()){
		if(info[2].IsFunction()){
			if(3 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[2].As<Napi::Function>();
			hasCallback		= true;
		}else if(!info[2].IsNull()){
			strvalue		= info[2].ToString().Utf8Value();
			is_value_set	= true;
		}
	}

	if(3 < info.Length()){
		if(4 < info.Length() || !info[3].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[3].As<Napi::Function>();
		hasCallback		= true;
	}

	// work
	if(hasCallback){
		AddSubkeyAsyncWorker* worker = new AddSubkeyAsyncWorker(maybeCallback, &(obj->_k2hshm), strkey.c_str(), strsubkey.c_str(), (is_value_set ? strvalue.c_str() : NULL));
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool result = obj->_k2hshm.AddSubkey(strkey.c_str(), strsubkey.c_str(), (is_value_set ? strvalue.c_str() : NULL));
		return Napi::Boolean::New(env, result);
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

Napi::Value K2hNode::AddSubkeys(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 2){
		Napi::TypeError::New(env, "No key name or no subkeys array are specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string		strkey;
	unsigned char*	bySubkeys	= NULL;
	size_t			skeylen		= 0UL;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_ADDSUBKEYS]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// initial callback comes from emitter map if set
	if(info[0].IsNull()){
		Napi::TypeError::New(env, "key is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		strkey = info[0].ToString().Utf8Value();
	}

	if(!info[1].IsArray()){
		Napi::TypeError::New(env, "Specified subkeys is not array.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		Napi::Array	inSubkeys	= info[1].As<Napi::Array>();
		K2HSubKeys	Subkeys;
		uint32_t	len			= inSubkeys.Length();
		for(uint32_t pos = 0; pos < len; ++pos){
			std::string tmpkey = inSubkeys.Get(pos).ToString().Utf8Value();
			if(Subkeys.end() == Subkeys.insert(tmpkey.c_str())){
				// failed to set subkey
				K2H_Free(bySubkeys);
				return Napi::Boolean::New(env, false);
			}
		}
		if(0UL >= Subkeys.size()){
			// there is no subkey
			K2H_Free(bySubkeys);
			return Napi::Boolean::New(env, false);
		}
		// serialize
		if(!Subkeys.Serialize(&bySubkeys, skeylen)){
			K2H_Free(bySubkeys);
			return Napi::Boolean::New(env, false);
		}
	}

	if(2 < info.Length()){
		if(3 < info.Length() || !info[2].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			K2H_Free(bySubkeys);
			return env.Undefined();
		}
		maybeCallback	= info[2].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		AddSubkeysAsyncWorker* worker = new AddSubkeysAsyncWorker(maybeCallback, &(obj->_k2hshm), strkey.c_str(), bySubkeys, skeylen);
		worker->Queue();
		K2H_Free(bySubkeys);
		return Napi::Boolean::New(env, true);
	}else{
		bool result = obj->_k2hshm.ReplaceSubkeys(reinterpret_cast<const unsigned char*>(strkey.c_str()), strkey.length() + 1, bySubkeys, skeylen);
		K2H_Free(bySubkeys);
		return Napi::Boolean::New(env, result);
	}
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

Napi::Value K2hNode::Remove(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No key name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string	strkey;
	bool		is_subkey_set = false;
	std::string	strsubkey;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_REMOVE]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(info[0].IsNull()){
		Napi::TypeError::New(env, "key is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		strkey = info[0].ToString().Utf8Value();
	}

	if(1 < info.Length()){
		if(info[1].IsFunction()){
			if(2 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[1].As<Napi::Function>();
			hasCallback		= true;
		}else if(!info[1].IsNull()){
			strsubkey		= info[1].ToString().Utf8Value();
			is_subkey_set	= true;
		}
	}

	if(2 < info.Length()){
		if(3 < info.Length() || !info[2].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[2].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		RemoveAsyncWorker* worker = new RemoveAsyncWorker(maybeCallback, &(obj->_k2hshm), strkey.c_str(), (is_subkey_set ? strsubkey.c_str() : NULL), false);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool result;
		if(is_subkey_set){
			result = obj->_k2hshm.Remove(strkey.c_str(), strsubkey.c_str());
		}else{
			result = obj->_k2hshm.Remove(strkey.c_str(), false);
		}
		return Napi::Boolean::New(env, result);
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

Napi::Value K2hNode::RemoveAll(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No key name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string	strkey;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_REMOVEALL]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(info[0].IsNull()){
		Napi::TypeError::New(env, "key is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		strkey = info[0].ToString().Utf8Value();
	}

	if(1 < info.Length()){
		if(2 < info.Length() || !info[1].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[1].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		RemoveAsyncWorker* worker = new RemoveAsyncWorker(maybeCallback, &(obj->_k2hshm), strkey.c_str(), NULL, true);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool result = obj->_k2hshm.Remove(strkey.c_str(), true);
		return Napi::Boolean::New(env, result);
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

Napi::Value K2hNode::PrintState(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// Execute
	if(info.Length() <= 0){
		bool result = obj->_k2hshm.PrintState();
		return Napi::Boolean::New(env, result);
	}else{
		int		fd = info[0].IsNumber() ? info[0].ToNumber().Int32Value() : -1;
		FILE*	fp = (-1 == fd ? stdout : fdopen(fd, "a"));
		if(!fp){
			Napi::Error::New(env, "could not open output stream.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		bool result = obj->_k2hshm.PrintState(fp);

		// [NOTE]
		// Must flush at here, because nodejs's file descriptor is used for fd.
		// Otherwise, calling flush on nodejs(javascript) is not effected.
		fflush(fp);

		return Napi::Boolean::New(env, result);
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

Napi::Value K2hNode::PrintVersion(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Execute
	int		fd = (0 < info.Length() && info[0].IsNumber()) ? info[0].ToNumber().Int32Value() : -1;
	FILE*	fp = (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Napi::Error::New(env, "could not open output stream.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	k2h_print_version(fp);

	// [NOTE]
	// Must flush at here, because nodejs's file descriptor is used for fd.
	// Otherwise, calling flush on nodejs(javascript) is not effected.
	fflush(fp);

	return Napi::Boolean::New(env, true);
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

Napi::Value K2hNode::DumpHead(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// Execute
	int		fd = (0 < info.Length() && info[0].IsNumber()) ? info[0].ToNumber().Int32Value() : -1;
	FILE*	fp = (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Napi::Error::New(env, "could not open output stream.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	//SetK2hDbgMode(K2HDBG_MSG);
	obj->_k2hshm.Dump(fp, K2HShm::DUMP_HEAD);
	//SetK2hDbgMode(K2HDBG_SILENT);

	// Need to flush stream here!
	fflush(fp);

	return Napi::Boolean::New(env, true);
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

Napi::Value K2hNode::DumpKeytable(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// Execute
	int		fd = (0 < info.Length() && info[0].IsNumber()) ? info[0].ToNumber().Int32Value() : -1;
	FILE*	fp = (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Napi::Error::New(env, "could not open output stream.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	//SetK2hDbgMode(K2HDBG_MSG);
	obj->_k2hshm.Dump(fp, K2HShm::DUMP_KINDEX_ARRAY);
	//SetK2hDbgMode(K2HDBG_SILENT);

	// Need to flush stream here!
	fflush(fp);

	return Napi::Boolean::New(env, true);
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

Napi::Value K2hNode::DumpFullKeytable(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// Execute
	int		fd = (0 < info.Length() && info[0].IsNumber()) ? info[0].ToNumber().Int32Value() : -1;
	FILE*	fp = (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Napi::Error::New(env, "could not open output stream.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	//SetK2hDbgMode(K2HDBG_MSG);
	obj->_k2hshm.Dump(fp, K2HShm::DUMP_CKINDEX_ARRAY);
	//SetK2hDbgMode(K2HDBG_SILENT);

	// Need to flush stream here!
	fflush(fp);

	return Napi::Boolean::New(env, true);
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

Napi::Value K2hNode::DumpElementtable(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// Execute
	int		fd = (0 < info.Length() && info[0].IsNumber()) ? info[0].ToNumber().Int32Value() : -1;
	FILE*	fp = (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Napi::Error::New(env, "could not open output stream.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	//SetK2hDbgMode(K2HDBG_MSG);
	obj->_k2hshm.Dump(fp, K2HShm::DUMP_ELEMENT_LIST);
	//SetK2hDbgMode(K2HDBG_SILENT);

	// Need to flush stream here!
	fflush(fp);

	return Napi::Boolean::New(env, true);
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

Napi::Value K2hNode::DumpFull(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// Execute
	int		fd = (0 < info.Length() && info[0].IsNumber()) ? info[0].ToNumber().Int32Value() : -1;
	FILE*	fp = (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Napi::Error::New(env, "could not open output stream.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	//SetK2hDbgMode(K2HDBG_MSG);
	obj->_k2hshm.Dump(fp, K2HShm::DUMP_PAGE_LIST);
	//SetK2hDbgMode(K2HDBG_SILENT);

	// Need to flush stream here!
	fflush(fp);

	return Napi::Boolean::New(env, true);
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

Napi::Value K2hNode::Transaction(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "Need to specify first parameter for dis/enable.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// parse positional optional args
	bool	enable = info[0].ToBoolean().Value();

	// Execute
	if(enable){
		std::string	transfile;
		std::string	prefix;
		std::string	param;
		int			expire = (info.Length() < 5 || !info[4].IsNumber()) ? 0 : info[4].ToNumber().Int32Value();

		if(1 < info.Length()){
			transfile = info[1].ToString().Utf8Value();
		}
		if(2 < info.Length()){
			prefix = info[2].ToString().Utf8Value();
		}
		if(3 < info.Length()){
			param = info[3].ToString().Utf8Value();
		}

		// cppcheck-suppress unmatchedSuppression
		// cppcheck-suppress unreadVariable
		time_t			expire_tt	= 0;
		const time_t*	pexpire		= NULL;
		if(0 < expire){
			expire_tt	= static_cast<time_t>(expire);
			pexpire		= &expire_tt;
		}

		bool	res = obj->_k2hshm.EnableTransaction(
						(transfile.empty() ? NULL : transfile.c_str()),
						(prefix.empty() ? NULL : reinterpret_cast<const unsigned char*>(prefix.c_str())),
						(prefix.empty() ? 0 : prefix.length()),
						(param.empty() ? NULL : reinterpret_cast<const unsigned char*>(param.c_str())),
						(param.empty() ? 0 : param.length()),
						(pexpire)
					);
		return Napi::Boolean::New(env, res);
	}else{
		bool	res = obj->_k2hshm.DisableTransaction();
		return Napi::Boolean::New(env, res);
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

Napi::Value K2hNode::EnableTransaction(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string		transfile;
	std::string		prefix;
	std::string		param;

	// cppcheck-suppress unmatchedSuppression
	// cppcheck-suppress unreadVariable
	time_t			expire_tt = 0;
	const time_t*	pexpire = NULL;

	// parse positional optional args
	if(0 < info.Length()){
		transfile = info[0].ToString().Utf8Value();
	}
	if(1 < info.Length()){
		prefix = info[1].ToString().Utf8Value();
	}
	if(2 < info.Length()){
		param = info[2].ToString().Utf8Value();
	}
	if(3 < info.Length()&&info[3].IsNumber()){
		int	nexpire = info[3].ToNumber().Int32Value();
		if(0 < nexpire){
			expire_tt	= static_cast<time_t>(nexpire);
			pexpire		= &expire_tt;
		}
	}

	// Execute
	bool	res = obj->_k2hshm.EnableTransaction(
					(transfile.empty() ? NULL : transfile.c_str()),
					(prefix.empty() ? NULL : reinterpret_cast<const unsigned char*>(prefix.c_str())),
					(prefix.empty() ? 0 : (prefix.length() + 1)),
					(param.empty() ? NULL : reinterpret_cast<const unsigned char*>(param.c_str())),
					(param.empty() ? 0 : (param.length() + 1)),
					pexpire
				);
	return Napi::Boolean::New(env, res);
}

/**
 * @memberof K2hNode
 * @fn bool DisableTransaction(void)
 * @brief	Disable transaction
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hNode::DisableTransaction(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// Execute
	bool res = obj->_k2hshm.DisableTransaction();
	return Napi::Boolean::New(env, res);
}

/**
 * @memberof K2hNode
 * @fn bool UnsetTransactionThreadPool(void)
 * @brief	Clear transaction thread pool
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hNode::UnsetTransactionThreadPool(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// Execute
	bool res = obj->_k2hshm.UnsetTransThreadPool();
	return Napi::Boolean::New(env, res);
}

/**
 * @memberof K2hNode
 * @fn int GetTransactionThreadPool(void)
 * @brief	Get transaction thread pool count
 *
 * @return	Return count of transaction thread pool
 */

Napi::Value K2hNode::GetTransactionThreadPool(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// Execute
	int res = obj->_k2hshm.GetTransThreadPool();
	return Napi::Number::New(env, res);
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

Napi::Value K2hNode::SetTransactionThreadPool(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "Need to specify first parameter for count of pool.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// parse positional optional args
	int	count	= info[0].ToNumber().Int32Value();

	// Execute
	bool	res	= obj->_k2hshm.SetTransThreadPool(count);
	return Napi::Boolean::New(env, res);
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

Napi::Value K2hNode::LoadArchive(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No file name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string	filename;
	bool		errskip = true;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_LOAD]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(info[0].IsNull()){
		Napi::TypeError::New(env, "file name is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		filename = info[0].ToString().Utf8Value();
	}

	if(1 < info.Length()){
		if(info[1].IsFunction()){
			if(2 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[1].As<Napi::Function>();
			hasCallback		= true;
		}else if(info[1].IsBoolean()){
			errskip			= info[1].ToBoolean().Value();
		}else{
			Napi::TypeError::New(env, "Second parameter must be boolean.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
	}

	if(2 < info.Length()){
		if(3 < info.Length() || !info[2].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[2].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		ArchiveAsyncWorker* worker = new ArchiveAsyncWorker(maybeCallback, &(obj->_k2hshm), filename.c_str(), errskip, true);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool res = k2h_load_archive(reinterpret_cast<k2h_h>(&obj->_k2hshm), filename.c_str(), errskip);
		return Napi::Boolean::New(env, res);
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

Napi::Value K2hNode::PutArchive(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No file name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string	filename;
	bool		errskip = true;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_PUT]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(info[0].IsNull()){
		Napi::TypeError::New(env, "file name is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		filename = info[0].ToString().Utf8Value();
	}

	if(1 < info.Length()){
		if(info[1].IsFunction()){
			if(2 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[1].As<Napi::Function>();
			hasCallback		= true;
		}else if(info[1].IsBoolean()){
			errskip			= info[1].ToBoolean().Value();
		}else{
			Napi::TypeError::New(env, "Second parameter must be boolean.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
	}

	if(2 < info.Length()){
		if(3 < info.Length() || !info[2].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[2].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		ArchiveAsyncWorker* worker = new ArchiveAsyncWorker(maybeCallback, &(obj->_k2hshm), filename.c_str(), errskip, false);
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool res = k2h_put_archive(reinterpret_cast<k2h_h>(&obj->_k2hshm), filename.c_str(), errskip);
		return Napi::Boolean::New(env, res);
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
 * var k2hq = k2h.getQueue(false, '++LIFO++') ;
 * k2hq,Push('key', 'val') ;
 * k2hq.Count() ;
 * k2h.Close() ;
 * @endcode
 */

Napi::Value K2hNode::getQueue(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// Initialize queue
	Napi::Object	retobj	= K2hQueue::GetInstance(info);
	K2hQueue*		objq	= Napi::ObjectWrap<K2hQueue>::Unwrap(retobj.As<Napi::Object>());

	// parse positional optional args
	bool		is_fifo = info.Length() < 1 ? true : info[0].ToBoolean().Value();
	std::string	prefix;
	if(2 <= info.Length()){
		prefix = info[1].ToString().Utf8Value();
	}

	// Execute
	objq->_k2hqueue.Init(&obj->_k2hshm, is_fifo, (prefix.empty() ? NULL : reinterpret_cast<const unsigned char*>(prefix.c_str())), (prefix.empty() ? 0 : (prefix.length() + 1)));
	return retobj;
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
 * k2hkq,Push('key', 'val') ;
 * k2hkq.Count() ;
 * k2h.Close() ;
 * @endcode
 */

Napi::Value K2hNode::getKeyQueue(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// Initialize queue
	Napi::Object	retobj	= K2hKeyQueue::GetInstance(info);
	K2hKeyQueue*	objq	= Napi::ObjectWrap<K2hKeyQueue>::Unwrap(retobj.As<Napi::Object>());

	// parse positional optional args
	bool		is_fifo = info.Length() < 1 ? true : info[0].ToBoolean().Value();
	std::string	prefix;
	if(2 <= info.Length()){
		prefix = info[1].ToString().Utf8Value();
	}

	// Execute
	objq->_k2hkeyqueue.Init(&obj->_k2hshm, is_fifo, (prefix.empty() ? NULL : reinterpret_cast<const unsigned char*>(prefix.c_str())), (prefix.empty() ? 0 : (prefix.length() + 1)));
	return retobj;
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

Napi::Value K2hNode::SetCommonAttribute(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// parse positional optional args
	const bool	istrue	= true;
	const bool	isfalse	= false;

	const bool*	pismtime = nullptr;
	if(0 < info.Length() && info[0].IsNumber()){
		int32_t	val = info[0].ToNumber().Int32Value();
		if(K2H_VAL_ATTR_ENABLE == val){
			pismtime = &istrue;
		}else if(K2H_VAL_ATTR_DISABLE == val){
			pismtime = &isfalse;
		}
	}

	const bool*	pishistory = nullptr;
	if(1 < info.Length() && info[1].IsNumber()){
		int32_t	val = info[1].ToNumber().Int32Value();
		if(K2H_VAL_ATTR_ENABLE == val){
			pishistory = &istrue;
		}else if(K2H_VAL_ATTR_DISABLE == val){
			pishistory = &isfalse;
		}
	}

	const bool*	pisencrypt = nullptr;
	if(2 < info.Length() && info[2].IsNumber()){
		int32_t	val = info[2].ToNumber().Int32Value();
		if(K2H_VAL_ATTR_ENABLE == val){
			pisencrypt = &istrue;
		}else if(K2H_VAL_ATTR_DISABLE == val){
			pisencrypt = &isfalse;
		}
	}

	std::string	pass;
	if(3 < info.Length() && !info[3].IsUndefined() && !info[3].IsNull()){
		pass = info[3].ToString().Utf8Value();
	}

	bool	is_expire = false;
	if(4 < info.Length() && info[4].IsNumber()){
		int32_t	val	= info[4].ToNumber().Int32Value();
		is_expire	= (K2H_VAL_ATTR_ENABLE == val);
	}

	// cppcheck-suppress unmatchedSuppression
	// cppcheck-suppress unreadVariable
	time_t	expire	= 0;
	time_t*	pexpire	= nullptr;
	if(is_expire && 5 < info.Length() && info[5].IsNumber()){
		int64_t	val	= info[5].ToNumber().Int64Value();
		expire		= static_cast<time_t>(val);
		pexpire		= &expire;
	}

	// Execute
	bool res = obj->_k2hshm.SetCommonAttribute(pismtime, pisencrypt, (pass.empty() ? NULL : pass.c_str()), pishistory, pexpire);
	return Napi::Boolean::New(env, res);
}

/**
 * @memberof K2hNode
 * @fn bool ClearCommonAttribute()
 * @brief	Clear builtin attribute setting(to default)
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hNode::CleanCommonAttribute(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if (!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())) {
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// Execute
	bool res = obj->_k2hshm.CleanCommonAttribute();
	return Napi::Boolean::New(env, res);
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

Napi::Value K2hNode::AddAttrPluginLib(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No library file path is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// parse positional optional args
	if(info[0].IsNull()){
		Napi::TypeError::New(env, "library file path is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	std::string	libfile = info[0].ToString().Utf8Value();

	// Execute
	bool res = obj->_k2hshm.AddAttrPluginLib(libfile.c_str());
	return Napi::Boolean::New(env, res);
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

Napi::Value K2hNode::AddAttrCryptPass(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No pass phrase is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// parse positional optional args
	if(info[0].IsNull()){
		Napi::TypeError::New(env, "pass phrase is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	std::string	pass				= info[0].ToString().Utf8Value();
	bool		is_default_encrypt	= (2 <= info.Length() && info[1].ToBoolean().Value());

	// Execute
	bool res = obj->_k2hshm.AddAttrCryptPass(pass.c_str(), is_default_encrypt);
	return Napi::Boolean::New(env, res);
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

Napi::Value K2hNode::GetAttrVersionInfos(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// parse positional optional args
	int		fd = (0 < info.Length() && info[0].IsNumber()) ? info[0].ToNumber().Int32Value() : -1;
	FILE*	fp = (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Napi::Error::New(env, "could not open output stream.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Execute
	strarr_t	verinfos;
	if(!obj->_k2hshm.GetAttrVersionInfos(verinfos)){
		return Napi::Boolean::New(env, false);
	}

	fprintf(fp, "K2HASH attribute libraries:");
	for(strarr_t::const_iterator iter = verinfos.begin(); iter != verinfos.end(); ++iter){
		fprintf(fp, "  %s\n",iter->c_str());
	}

	// Need to flush stream here!
	fflush(fp);

	return Napi::Boolean::New(env, true);
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

Napi::Value K2hNode::GetAttrInfos(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	// parse positional optional args
	int		fd = (0 < info.Length() && info[0].IsNumber()) ? info[0].ToNumber().Int32Value() : -1;
	FILE*	fp = (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Napi::Error::New(env, "could not open output stream.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Execute
	std::stringstream	ss;
	obj->_k2hshm.GetAttrInfos(ss);
	fprintf(fp, "%s\n", ss.str().c_str());

	// Need to flush stream here!
	fflush(fp);

	return Napi::Boolean::New(env, true);
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

Napi::Value K2hNode::GetAttrs(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No key name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject()||!info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string	strkey;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_GETATTRS]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(info[0].IsNull()){
		Napi::TypeError::New(env, "key is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		strkey = info[0].ToString().Utf8Value();
	}

	if(1 < info.Length()){
		if(2 < info.Length() || !info[1].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[1].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		GetAttrsAsyncWorker* worker = new GetAttrsAsyncWorker(maybeCallback, &(obj->_k2hshm), strkey.c_str());
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		K2HAttrs*	attrs = obj->_k2hshm.GetAttrs(strkey.c_str());
		if(!attrs){
			return env.Null();
		}
		Napi::Array	retarr	= Napi::Array::New(env);
		uint32_t	pos		= 0;
		for(K2HAttrs::iterator iter = attrs->begin(); iter != attrs->end(); ++iter){
			if(0UL == iter->keylength || !iter->pkey){
				continue;
			}
			retarr.Set(pos++, Napi::String::New(env, reinterpret_cast<const char*>(iter->pkey), static_cast<size_t>(strlen(reinterpret_cast<const char*>(iter->pkey)))));
		}
		delete attrs;
		return retarr;
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

Napi::Value K2hNode::GetAttrValue(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 2){
		Napi::TypeError::New(env, "No key name or no attribute name are specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string	strkey;
	std::string	strattr;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_GETATTRVAL]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(info[0].IsNull()){
		Napi::TypeError::New(env, "key is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		strkey = info[0].ToString().Utf8Value();
	}

	if(info[1].IsNull()){
		Napi::TypeError::New(env, "attr key is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		strattr = info[1].ToString().Utf8Value();
	}

	if(2 < info.Length()){
		if(3 < info.Length() || !info[2].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[2].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		GetAttrValueAsyncWorker* worker = new GetAttrValueAsyncWorker(maybeCallback, &(obj->_k2hshm), strkey.c_str(), strattr.c_str());
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		K2HAttrs*	attrs = obj->_k2hshm.GetAttrs(strkey.c_str());
		if(!attrs){
			return env.Null();
		}

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
					Napi::Value val = Napi::String::New(env, reinterpret_cast<const char*>(iter->pval), static_cast<size_t>(strlen(reinterpret_cast<const char*>(iter->pval))));
					delete attrs;
					return val;
				}else{
					delete attrs;
					return env.Null();
				}
				break;	// not effort
			}
		}
		delete attrs;
		return env.Null();
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

Napi::Value K2hNode::AddAttr(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 2){
		Napi::TypeError::New(env, "No key name or no attribute name/value are specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject()||!info.This().As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hNode instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* obj = Napi::ObjectWrap<K2hNode>::Unwrap(info.This().As<Napi::Object>());

	std::string	strkey;
	std::string	strattr;
	bool		is_val_set = false;
	std::string	strval;


	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2h_emitters[K2H_EMITTER_POS_ADDATTR]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(info[0].IsNull()){
		Napi::TypeError::New(env, "key is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		strkey = info[0].ToString().Utf8Value();
	}

	if(info[1].IsNull()){
		Napi::TypeError::New(env, "attribute name is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		strattr = info[1].ToString().Utf8Value();
	}

	if(2 < info.Length()){
		if(info[2].IsFunction()){
			if(3 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[2].As<Napi::Function>();
			hasCallback		= true;
		}else if(!info[2].IsNull()){
			strval		= info[2].ToString().Utf8Value();
			is_val_set	= true;
		}
	}

	if(3 < info.Length()){
		if(4 < info.Length() || !info[3].IsFunction()){
			Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
		maybeCallback	= info[3].As<Napi::Function>();
		hasCallback		= true;
	}

	// Execute
	if(hasCallback){
		AddAttrAsyncWorker* worker = new AddAttrAsyncWorker(maybeCallback, &(obj->_k2hshm), strkey.c_str(), strattr.c_str(), (is_val_set ? strval.c_str() : NULL));
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool res = obj->_k2hshm.AddAttr(strkey.c_str(), strattr.c_str(), (is_val_set ? strval.c_str() : NULL));
		return Napi::Boolean::New(env, res);
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
