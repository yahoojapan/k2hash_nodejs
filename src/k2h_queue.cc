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
#include "k2h_queue.h"
#include "k2h_queue_async.h"

using namespace std;

//---------------------------------------------------------
// Emitter
//---------------------------------------------------------
#define	K2HQ_EMITTER_POS_PUSH			(0)
#define	K2HQ_EMITTER_POS_POP			(K2HQ_EMITTER_POS_PUSH	+ 1)
#define	K2HQ_EMITTER_POS_COUNT			(K2HQ_EMITTER_POS_POP	+ 1)
#define	K2HQ_EMITTER_POS_READ			(K2HQ_EMITTER_POS_COUNT	+ 1)
#define	K2HQ_EMITTER_POS_REMOVE			(K2HQ_EMITTER_POS_READ	+ 1)

const char*	stc_k2hq_emitters[] = {
	"push",
	"pop",
	"count",
	"read",
	"remove",
	NULL
};

//---------------------------------------------------------
// Utility (using StackEmitCB Class)
//---------------------------------------------------------
static Napi::Value SetK2hqEmitterCallback(const Napi::CallbackInfo& info, size_t pos, const char* pemitter)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hQueue* obj = Napi::ObjectWrap<K2hQueue>::Unwrap(info.This().As<Napi::Object>());

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

static Napi::Value UnsetK2hqEmitterCallback(const Napi::CallbackInfo& info, const char* pemitter)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hQueue* obj = Napi::ObjectWrap<K2hQueue>::Unwrap(info.This().As<Napi::Object>());

	// unset
	bool result = obj->_cbs.Unset(std::string(pemitter));
	return Napi::Boolean::New(env, result);
}

//---------------------------------------------------------
// K2hQueue Class
//---------------------------------------------------------
Napi::FunctionReference	K2hQueue::constructor;

//---------------------------------------------------------
// K2hQueue Methods
//---------------------------------------------------------
K2hQueue::K2hQueue(const Napi::CallbackInfo& info) : Napi::ObjectWrap<K2hQueue>(info), _cbs(), _k2hqueue()
{
}

K2hQueue::~K2hQueue()
{
}

void K2hQueue::Init(Napi::Env env, Napi::Object exports)
{
	Napi::Function funcs = DefineClass(env, "K2hQueue", {

		// DefineClass normally handles the constructor internally. Therefore, there is no need
		// to include a static wrapper New() in the class prototype, which works the same way as
		// when using NAN.
		// For reference, the following example shows how to declare New as a static method.
		// (Registration is not normally required.)
		//
		//	K2hQueue::InstanceMethod("new",		&K2hQueue::New),

		// Prototype for event emitter
		K2hQueue::InstanceMethod("on",			&K2hQueue::On),
		K2hQueue::InstanceMethod("onPush",		&K2hQueue::OnPush),
		K2hQueue::InstanceMethod("onPop",		&K2hQueue::OnPop),
		K2hQueue::InstanceMethod("onCount",		&K2hQueue::OnCount),
		K2hQueue::InstanceMethod("onRead",		&K2hQueue::OnRead),
		K2hQueue::InstanceMethod("onRemove",	&K2hQueue::OnRemove),
		K2hQueue::InstanceMethod("off",			&K2hQueue::Off),
		K2hQueue::InstanceMethod("offPush",		&K2hQueue::OffPush),
		K2hQueue::InstanceMethod("offPop",		&K2hQueue::OffPop),
		K2hQueue::InstanceMethod("offCount",	&K2hQueue::OffCount),
		K2hQueue::InstanceMethod("offRead",		&K2hQueue::OffRead),
		K2hQueue::InstanceMethod("offRemove",	&K2hQueue::OffRemove),

		// Prototype
		K2hQueue::InstanceMethod("init",		&K2hQueue::Initialize),
		K2hQueue::InstanceMethod("push",		&K2hQueue::Push),
		K2hQueue::InstanceMethod("count",		&K2hQueue::Count),
		K2hQueue::InstanceMethod("isEmpty",		&K2hQueue::IsEmpty),
		K2hQueue::InstanceMethod("read",		&K2hQueue::Read),
		K2hQueue::InstanceMethod("pop",			&K2hQueue::Pop),
		K2hQueue::InstanceMethod("remove",		&K2hQueue::Remove),
		K2hQueue::InstanceMethod("dump",		&K2hQueue::Dump)
	});

	constructor = Napi::Persistent(funcs);
	constructor.SuppressDestruct();

	// [NOTE]
	// do NOT do exports.Set("K2hQueue", func) here if InitAll will return createFn.
	//
}

Napi::Value K2hQueue::New(const Napi::CallbackInfo& info)
{
	if(info.IsConstructCall()){
		// Invoked as constructor: new K2hQueue()
		// DefineClass 経由のコンストラクタが呼ばれているはずなので、ここでは this を返すだけで良い
		return info.This();
	}else{
		// Invoked as plain function K2hQueue(), turn into construct call.
		if(0 < info.Length()){
			return constructor.New({info[0]});
		}else{
			return constructor.New({});
		}
	}
}

// NewInstance( no parameter )
Napi::Object K2hQueue::NewInstance(Napi::Env env)
{
	Napi::EscapableHandleScope scope(env);
	Napi::Object obj = constructor.New({}).As<Napi::Object>();
	return scope.Escape(napi_value(obj)).ToObject();
}

// NewInstance( 1 parameter )
Napi::Object K2hQueue::NewInstance(Napi::Env env, const Napi::Value& arg)
{
	Napi::EscapableHandleScope scope(env);
	Napi::Object obj = constructor.New({ arg }).As<Napi::Object>();
	return scope.Escape(napi_value(obj)).ToObject();
}

Napi::Object K2hQueue::GetInstance(const Napi::CallbackInfo& info)
{
	if(0 < info.Length()){
		return K2hQueue::constructor.New({info[0]});
	}else{
		return K2hQueue::constructor.New({});
	}
}

/// \defgroup nodejs_methods
//@{

/**
 * @memberof K2hQueue
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

Napi::Value K2hQueue::On(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// check
    if(!info.This().IsObject()||!info.This().As<Napi::Object>().InstanceOf(K2hQueue::constructor.Value())){
        Napi::TypeError::New(env, "Invalid this object(K2hQueue instance)").ThrowAsJavaScriptException();
        return env.Undefined();
    }
	if(info.Length() < 1){
		Napi::TypeError::New(env, "No handle emitter name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else if(info.Length() < 2){
		Napi::TypeError::New(env, "No callback is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// check emitter name
	std::string emitter  = info[0].ToString().Utf8Value();
	const char* pemitter = GetNormalizationEmitter(emitter.c_str(), stc_k2hq_emitters);
	if(!pemitter){
		std::string	msg	= "Unknown ";
		msg				+= emitter;
		msg				+= " emitter";
		Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// add callback
	return SetK2hqEmitterCallback(info, 1, pemitter);
}

/**
 * @memberof K2hQueue
 * @fn void\
 * OnPush(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hqueue object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hQueue::OnPush(const Napi::CallbackInfo& info)
{
	return SetK2hqEmitterCallback(info, 0, stc_k2hq_emitters[K2HQ_EMITTER_POS_PUSH]);
}

/**
 * @memberof K2hQueue
 * @fn void\
 * OnPop(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hqueue object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hQueue::OnPop(const Napi::CallbackInfo& info)
{
	return SetK2hqEmitterCallback(info, 0, stc_k2hq_emitters[K2HQ_EMITTER_POS_POP]);
}

/**
 * @memberof K2hQueue
 * @fn void\
 * OnCount(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hqueue object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hQueue::OnCount(const Napi::CallbackInfo& info)
{
	return SetK2hqEmitterCallback(info, 0, stc_k2hq_emitters[K2HQ_EMITTER_POS_COUNT]);
}

/**
 * @memberof K2hQueue
 * @fn void\
 * OnRead(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hqueue object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hQueue::OnRead(const Napi::CallbackInfo& info)
{
	return SetK2hqEmitterCallback(info, 0, stc_k2hq_emitters[K2HQ_EMITTER_POS_READ]);
}

/**
 * @memberof K2hQueue
 * @fn void\
 * OnRemove(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hqueue object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hQueue::OnRemove(const Napi::CallbackInfo& info)
{
	return SetK2hqEmitterCallback(info, 0, stc_k2hq_emitters[K2HQ_EMITTER_POS_REMOVE]);
}

/**
 * @memberof K2hQueue
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

Napi::Value K2hQueue::Off(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No handle emitter name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// check emitter name
	std::string	emitter  = info[0].ToString().Utf8Value();
	const char*	pemitter = GetNormalizationEmitter(emitter.c_str(), stc_k2hq_emitters);
	if (nullptr == pemitter) {
		std::string msg	= "Unknown ";
		msg				+= emitter;
		msg				+= " emitter";
		Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
		return env.Undefined();
	}
	// unset callback
	return UnsetK2hqEmitterCallback(info, pemitter);
}

/**
 * @memberof K2hQueue
 * @fn void\
 * OffPush(\
 * )
 * @brief	set callback handling for initializing k2hqueue object
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hQueue::OffPush(const Napi::CallbackInfo& info)
{
	return UnsetK2hqEmitterCallback(info, stc_k2hq_emitters[K2HQ_EMITTER_POS_PUSH]);
}

/**
 * @memberof K2hQueue
 * @fn void\
 * OffPop(\
 * )
 * @brief	set callback handling for initializing k2hqueue object
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hQueue::OffPop(const Napi::CallbackInfo& info)
{
	return UnsetK2hqEmitterCallback(info, stc_k2hq_emitters[K2HQ_EMITTER_POS_POP]);
}

/**
 * @memberof K2hQueue
 * @fn void\
 * OffCount(\
 * )
 * @brief	set callback handling for initializing k2hqueue object
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hQueue::OffCount(const Napi::CallbackInfo& info)
{
	return UnsetK2hqEmitterCallback(info, stc_k2hq_emitters[K2HQ_EMITTER_POS_COUNT]);
}

/**
 * @memberof K2hQueue
 * @fn void\
 * OffRead(\
 * )
 * @brief	set callback handling for initializing k2hqueue object
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hQueue::OffRead(const Napi::CallbackInfo& info)
{
	return UnsetK2hqEmitterCallback(info, stc_k2hq_emitters[K2HQ_EMITTER_POS_READ]);
}

/**
 * @memberof K2hQueue
 * @fn void\
 * OffRemove(\
 * )
 * @brief	set callback handling for initializing k2hqueue object
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hQueue::OffRemove(const Napi::CallbackInfo& info)
{
	return UnsetK2hqEmitterCallback(info, stc_k2hq_emitters[K2HQ_EMITTER_POS_REMOVE]);
}

/**
 * @memberof K2hQueue
 * @fn bool Init(\
 * 	object k2hobj\
 *	, bool is_fifo=true\
 * 	, String prefix[=option]\
 * )
 * @brief	Initialize k2hkeyqueue object with K2hNode object
 *
 * Nothing to do(example writing) to K2HASH file after calling Init()
 *
 * @param[in] k2hobj	Specify target K2hNode object
 * @param[in] is_fifo	Specify true for FIFO queue, false for LIFO
 * @param[in] prefix	Specify prefix of queue key name
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hQueue::Initialize(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// check
	if(info.Length() < 1){
		Napi::TypeError::New(env, "No k2hash object is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject()||!info.This().As<Napi::Object>().InstanceOf(K2hQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hQueue* obj = Napi::ObjectWrap<K2hQueue>::Unwrap(info.This().As<Napi::Object>());

	// parse positional optional args
	if(!info[0].IsObject()||!info[0].As<Napi::Object>().InstanceOf(K2hNode::constructor.Value())){
		Napi::TypeError::New(env, "Invalid k2hash object").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* objNode = Napi::ObjectWrap<K2hNode>::Unwrap(info[0].As<Napi::Object>());

	bool		is_fifo = (info.Length() < 2 ? true : info[1].ToBoolean().Value());
	std::string	prefix;
	if(2 < info.Length()){
		prefix = info[2].ToString().Utf8Value();
	}

	// Execute
	bool res = obj->_k2hqueue.Init(&objNode->_k2hshm, is_fifo, (prefix.empty() ? NULL : reinterpret_cast<const unsigned char*>(prefix.c_str())), (prefix.empty() ? 0UL : (prefix.length() + 1)));
	return Napi::Boolean::New(env, res);
}

/**
 * @memberof K2hQueue
 * @fn bool Push(\
 * 	  String	data\
 * 	, String	pass=""\
 * 	, int		expire=0\
 * 	, Callback	cbfunc=null\
 * )
 *
 * @brief	Push data to queue.
 *			If the callback function is specified, or on callback handles for this,
 *  		this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] data		Specify the data which is queued
 * @param[in] pass		Specify optional passphrase if encrypting the key
 * @param[in] expire	Specify optional timeout seconds if you need
 * @param[in] cbfunc	callback function.
 *
 * @return	return true for success, false for failure
 *
 */

Napi::Value K2hQueue::Push(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No data is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject()||!info.This().As<Napi::Object>().InstanceOf(K2hQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hQueue* obj = Napi::ObjectWrap<K2hQueue>::Unwrap(info.This().As<Napi::Object>());

	std::string	data;
	bool		is_pass_set	= false;
	std::string	pass;
	time_t		expire		= 0;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2hq_emitters[K2HQ_EMITTER_POS_PUSH]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(info[0].IsNull()){
		Napi::TypeError::New(env, "data is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		data = info[0].ToString().Utf8Value();
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
			pass			= info[1].ToString().Utf8Value();
			is_pass_set		= true;
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
		}else if(info[2].IsNumber()){
			int	nexpire		= info[2].ToNumber().Int32Value();
			expire			= static_cast<time_t>(nexpire);
		}else{
			Napi::TypeError::New(env, "Expire parameter must be int32 value.").ThrowAsJavaScriptException();
			return env.Undefined();
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
		K2hqPushAsyncWorker* worker = new K2hqPushAsyncWorker(maybeCallback, &(obj->_k2hqueue), data.c_str(), (is_pass_set ? pass.c_str() : NULL), (expire <= 0 ? NULL : &expire));
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool res = obj->_k2hqueue.Push(reinterpret_cast<const unsigned char*>(data.c_str()), (data.length() + 1), NULL, (is_pass_set ? pass.c_str() : NULL), (expire <= 0 ? NULL : &expire));
		return Napi::Boolean::New(env, res);
	}
}

/**
 * @memberof K2hQueue
 * @fn int Count(\
 * 	Callback cbfunc=null\
 *	)
 *
 * @brief	Get queue count
 *			If the callback function is specified, or on callback handles for this,
 *  		this method works asynchronization and calls callback function at finishing.
 *
 *			Take care for performance, because k2hash library uses liner search for this
 *			function.
 *
 * @param[in] cbfunc	callback function.
 *
 * @return	Returns count of queue, if something error occurred, returns 0.
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hQueue::Count(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject()||!info.This().As<Napi::Object>().InstanceOf(K2hQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hQueue* obj = Napi::ObjectWrap<K2hQueue>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2hq_emitters[K2HQ_EMITTER_POS_COUNT]);
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
		K2hqCountAsyncWorker* worker = new K2hqCountAsyncWorker(maybeCallback, &(obj->_k2hqueue));
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		uint64_t cnt = obj->_k2hqueue.GetCount();
		return Napi::Number::New(env, static_cast<double>(cnt));
	}
}

/**
 * @memberof K2hQueue
 * @fn bool IsEmpty(void)
 * @brief Test for the queue is empty
 *
 * @return returns true if the queue is empty. false for not empty.
 */

Napi::Value K2hQueue::IsEmpty(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject()||!info.This().As<Napi::Object>().InstanceOf(K2hQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hQueue* obj = Napi::ObjectWrap<K2hQueue>::Unwrap(info.This().As<Napi::Object>());

	// Execute
	bool res = obj->_k2hqueue.IsEmpty();
	return Napi::Boolean::New(env, res);
}

/**
 * @memberof K2hQueue
 * @fn String Read(\
 *	int pos=0\
 *	, String pass=""\
 * 	, Callback cbfunc=null\
 *	)
 *
 * @brief	Read queue data at queue position.
 *			Not remove the data from queue after reading.
 *			If the callback function is specified, or on callback handles for this,
 *  		this method works asynchronization and calls callback function at finishing.
 *
 *			Take care for performance, because k2hash library uses liner search for this
 *			function.
 *
 * @param[in] pos		Specify position of reading in queue.
 *						If this value is not specified, the default position is 0.
 *						If this value is specified -1, the position means lastest position.
 * @param[in] pass		Specify optional passphrase if the key is encrypted
 * @param[in] cbfunc	callback function.
 *
 * @return	Returns the data which are queued. 
 *			If there is no data in queue or something error is occurred, returns null.
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hQueue::Read(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject()||!info.This().As<Napi::Object>().InstanceOf(K2hQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hQueue* obj = Napi::ObjectWrap<K2hQueue>::Unwrap(info.This().As<Napi::Object>());

	int			pos			= 0;
	bool		is_pass_set	= false;
	std::string	strpass;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2hq_emitters[K2HQ_EMITTER_POS_READ]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(0 < info.Length()){
		if(info[0].IsNumber()){
			pos = info[0].ToNumber().Int32Value();
		}else if(info[0].IsString()){
			strpass		= info[0].ToString().Utf8Value();
			is_pass_set	= true;
		}else if(info[0].IsFunction()){
			if(1 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[0].As<Napi::Function>();
			hasCallback		= true;
		}else{
			Napi::TypeError::New(env, "Unknown first parameter.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
	}

	if(1 < info.Length()){
		if(info[1].IsString() && !is_pass_set){
			strpass		= info[1].ToString().Utf8Value();
			is_pass_set	= true;
		}else if(info[1].IsFunction()){
			if(2 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[1].As<Napi::Function>();
			hasCallback		= true;
		}else{
			Napi::TypeError::New(env, "Unknown second parameter.").ThrowAsJavaScriptException();
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
		K2hqReadAsyncWorker* worker = new K2hqReadAsyncWorker(maybeCallback, &(obj->_k2hqueue), pos, (is_pass_set ? strpass.c_str() : NULL));
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		unsigned char*	pdata	= NULL;
		size_t			datalen	= 0;
		bool			result	= obj->_k2hqueue.Read(&pdata, datalen, pos, (is_pass_set ? strpass.c_str() : NULL));
		if(!result || !pdata || 0 == datalen){
			K2H_Free(pdata);
			return env.Null();
		}else{
			Napi::String ret = Napi::String::New(env, reinterpret_cast<const char*>(pdata), static_cast<size_t>(strlen(reinterpret_cast<const char*>(pdata))));
			K2H_Free(pdata);
			return ret;
		}
	}
}

/**
 * @memberof K2hQueue
 * @fn String Pop(\
 *	String pass=""\
 * 	, Callback cbfunc=null\
 *	)
 *
 * @brief	Pop queue data.
 *			Remove the data from queue after pop.
 *			If the callback function is specified, or on callback handles for this,
 *  		this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] pass		Specify optional passphrase if the key is encrypted
 * @param[in] cbfunc	callback function.
 *
 * @return	Returns the data which are queued. 
 *			If there is no data in queue or something error is occurred, returns null.
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hQueue::Pop(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject()||!info.This().As<Napi::Object>().InstanceOf(K2hQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hQueue* obj = Napi::ObjectWrap<K2hQueue>::Unwrap(info.This().As<Napi::Object>());

	bool		is_pass_set = false;
	std::string	strpass;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2hq_emitters[K2HQ_EMITTER_POS_POP]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(0 < info.Length()){
		if(info[0].IsString()){
			strpass		= info[0].ToString().Utf8Value();
			is_pass_set	= true;
		}else if(info[0].IsFunction()){
			if(1 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[0].As<Napi::Function>();
			hasCallback		= true;
		}else{
			Napi::TypeError::New(env, "Unknown first parameter.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
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
		K2hqPopAsyncWorker* worker = new K2hqPopAsyncWorker(maybeCallback, &(obj->_k2hqueue), (is_pass_set ? strpass.c_str() : NULL));
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		unsigned char*	pdata	= NULL;
		size_t			datalen	= 0;
		bool			result	= obj->_k2hqueue.Pop(&pdata, datalen, NULL, (is_pass_set ? strpass.c_str() : NULL));
		if(!result || !pdata || 0 == datalen){
			K2H_Free(pdata);
			return env.Null();
		}else{
			Napi::String ret = Napi::String::New(env, reinterpret_cast<const char*>(pdata), static_cast<size_t>(strlen(reinterpret_cast<const char*>(pdata))));
			K2H_Free(pdata);
			return ret;
		}
	}
}

/**
 * @memberof K2hQueue
 * @fn String Remove(\
 *	int count\
 *	, String pass=""\
 * 	, Callback cbfunc=null\
 *	)
 *
 * @brief	Remove count of queue datas
 *			Not return any datas which are removed.
 *			If the count is specified over queuing count, the queue is empty and returns true.
 *			If the callback function is specified, or on callback handles for this,
 *  		this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] count		Specify the count for removing.
 * @param[in] pass		Specify optional passphrase if the key is encrypted
 * @param[in] cbfunc	callback function.
 *
 * @return	Return removed queue count. if failure, returns -1
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hQueue::Remove(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "Need to specify first parameter for count of queue.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject()||!info.This().As<Napi::Object>().InstanceOf(K2hQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hQueue* obj = Napi::ObjectWrap<K2hQueue>::Unwrap(info.This().As<Napi::Object>());

	int			count		= 0;
	bool		is_pass_set	= false;
	std::string	strpass;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2hq_emitters[K2HQ_EMITTER_POS_REMOVE]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(!info[0].IsNumber()){
		Napi::TypeError::New(env, "First parameter must be number.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		count = info[0].ToNumber().Int32Value();
		if(count <= 0){
			Napi::TypeError::New(env, "Remove queue count must be over 0.").ThrowAsJavaScriptException();
			return env.Undefined();
		}
	}

	if(1 < info.Length()){
		if(info[1].IsString()){
			strpass		= info[1].ToString().Utf8Value();
			is_pass_set	= true;
		}else if(info[1].IsFunction()){
			if(2 < info.Length()){
				Napi::TypeError::New(env, "Last parameter is not callback function.").ThrowAsJavaScriptException();
				return env.Undefined();
			}
			maybeCallback	= info[1].As<Napi::Function>();
			hasCallback		= true;
		}else{
			Napi::TypeError::New(env, "Unknown second parameter.").ThrowAsJavaScriptException();
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
		K2hqRemoveAsyncWorker* worker = new K2hqRemoveAsyncWorker(maybeCallback, &(obj->_k2hqueue), count, (is_pass_set ? strpass.c_str() : NULL));
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		int	res = obj->_k2hqueue.Remove(count, NULL, NULL, (is_pass_set ? strpass.c_str() : NULL));
		return Napi::Number::New(env, static_cast<double>(res));
	}
}

/**
 * @memberof K2hQueue
 * @fn bool Dump(int fd[=option])
 * @brief 	Dump queue data
 *
 * If fd is not specified, stdout is used for the output stream.
 * This method os for debugging.
 *
 * @param[in] fd	Specify the stream for output.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hQueue::Dump(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject()||!info.This().As<Napi::Object>().InstanceOf(K2hQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hQueue* obj = Napi::ObjectWrap<K2hQueue>::Unwrap(info.This().As<Napi::Object>());

	int		fd = (0 < info.Length() && info[0].IsNumber()) ? info[0].ToNumber().Int32Value() : -1;
	FILE*	fp = (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Napi::Error::New(env, "could not open output stream.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	obj->_k2hqueue.Dump(fp);

	// Need to flush stream here!
	fflush(fp);

	return Napi::Boolean::New(env, true);
}

//@}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: expandtab sw=4 ts=4 fdm=marker
 * vim<600: expandtab sw=4 ts=4
 */
