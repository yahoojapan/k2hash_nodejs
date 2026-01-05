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
#include "k2h_keyqueue.h"
#include "k2h_keyqueue_async.h"

using namespace std;

//---------------------------------------------------------
// Emitter
//---------------------------------------------------------
#define	K2HKQ_EMITTER_POS_PUSH				(0)
#define	K2HKQ_EMITTER_POS_POP				(K2HKQ_EMITTER_POS_PUSH		+ 1)
#define	K2HKQ_EMITTER_POS_COUNT				(K2HKQ_EMITTER_POS_POP		+ 1)
#define	K2HKQ_EMITTER_POS_READ				(K2HKQ_EMITTER_POS_COUNT	+ 1)
#define	K2HKQ_EMITTER_POS_REMOVE			(K2HKQ_EMITTER_POS_READ	+ 1)

const char*	stc_k2hkq_emitters[] = {
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
static Napi::Value SetK2hkqEmitterCallback(const Napi::CallbackInfo& info, size_t pos, const char* pemitter)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hKeyQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hKeyQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hKeyQueue* obj = Napi::ObjectWrap<K2hKeyQueue>::Unwrap(info.This().As<Napi::Object>());

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

static Napi::Value UnsetK2hkqEmitterCallback(const Napi::CallbackInfo& info, const char* pemitter)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hKeyQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hKeyQueue* obj = Napi::ObjectWrap<K2hKeyQueue>::Unwrap(info.This().As<Napi::Object>());

	// unset
	bool result = obj->_cbs.Unset(std::string(pemitter));
	return Napi::Boolean::New(env, result);
}

//---------------------------------------------------------
// K2hKeyQueue Class
//---------------------------------------------------------
Napi::FunctionReference	K2hKeyQueue::constructor;

//---------------------------------------------------------
// K2hKeyQueue Methods
//---------------------------------------------------------
K2hKeyQueue::K2hKeyQueue(const Napi::CallbackInfo& info) : Napi::ObjectWrap<K2hKeyQueue>(info), _cbs(), _k2hkeyqueue()
{
}

K2hKeyQueue::~K2hKeyQueue()
{
}

void K2hKeyQueue::Init(Napi::Env env, Napi::Object exports)
{
	Napi::Function funcs = DefineClass(env, "K2hKeyQueue", {
		// DefineClass normally handles the constructor internally. Therefore, there is no need
		// to include a static wrapper New() in the class prototype, which works the same way as
		// when using NAN.
		// For reference, the following example shows how to declare New as a static method.
		// (Registration is not normally required.)
		//
		//	K2hKeyQueue::InstanceMethod("new",		&K2hKeyQueue::New),

		// Prototype for event emitter
		K2hKeyQueue::InstanceMethod("on",			&K2hKeyQueue::On),
		K2hKeyQueue::InstanceMethod("onPush",		&K2hKeyQueue::OnPush),
		K2hKeyQueue::InstanceMethod("onPop",		&K2hKeyQueue::OnPop),
		K2hKeyQueue::InstanceMethod("onCount",		&K2hKeyQueue::OnCount),
		K2hKeyQueue::InstanceMethod("onRead",		&K2hKeyQueue::OnRead),
		K2hKeyQueue::InstanceMethod("onRemove",		&K2hKeyQueue::OnRemove),
		K2hKeyQueue::InstanceMethod("off",			&K2hKeyQueue::Off),
		K2hKeyQueue::InstanceMethod("offPush",		&K2hKeyQueue::OffPush),
		K2hKeyQueue::InstanceMethod("offPop",		&K2hKeyQueue::OffPop),
		K2hKeyQueue::InstanceMethod("offCount",		&K2hKeyQueue::OffCount),
		K2hKeyQueue::InstanceMethod("offRead",		&K2hKeyQueue::OffRead),
		K2hKeyQueue::InstanceMethod("offRemove",	&K2hKeyQueue::OffRemove),

		// Prototype
		K2hKeyQueue::InstanceMethod("init",			&K2hKeyQueue::Initialize),
		K2hKeyQueue::InstanceMethod("push",			&K2hKeyQueue::Push),
		K2hKeyQueue::InstanceMethod("count",		&K2hKeyQueue::Count),
		K2hKeyQueue::InstanceMethod("isEmpty",		&K2hKeyQueue::IsEmpty),
		K2hKeyQueue::InstanceMethod("read",			&K2hKeyQueue::Read),
		K2hKeyQueue::InstanceMethod("pop",			&K2hKeyQueue::Pop),
		K2hKeyQueue::InstanceMethod("remove",		&K2hKeyQueue::Remove),
		K2hKeyQueue::InstanceMethod("dump",			&K2hKeyQueue::Dump)
	});

	constructor = Napi::Persistent(funcs);
	constructor.SuppressDestruct();

	// [NOTE]
	// do NOT do exports.Set("K2hKeyQueue", func) here if InitAll will return createFn.
	//
}

Napi::Value K2hKeyQueue::New(const Napi::CallbackInfo& info)
{
	if(info.IsConstructCall()){
		// Invoked as constructor: new K2hKeyQueue()
		// DefineClass 経由のコンストラクタが呼ばれているはずなので、ここでは this を返すだけで良い
		return info.This();
	}else{
		// Invoked as plain function K2hKeyQueue(), turn into construct call.
		if(0 < info.Length()){
			return constructor.New({info[0]});
		}else{
			return constructor.New({});
		}
	}
}

// NewInstance( no parameter )
Napi::Object K2hKeyQueue::NewInstance(Napi::Env env)
{
	Napi::EscapableHandleScope scope(env);
	Napi::Object obj = constructor.New({}).As<Napi::Object>();
	return scope.Escape(napi_value(obj)).ToObject();
}

// NewInstance( 1 parameter )
Napi::Object K2hKeyQueue::NewInstance(Napi::Env env, const Napi::Value& arg)
{
	Napi::EscapableHandleScope scope(env);
	Napi::Object obj = constructor.New({ arg }).As<Napi::Object>();
	return scope.Escape(napi_value(obj)).ToObject();
}

Napi::Object K2hKeyQueue::GetInstance(const Napi::CallbackInfo& info)
{
	if(0 < info.Length()){
		return K2hKeyQueue::constructor.New({info[0]});
	}else{
		return K2hKeyQueue::constructor.New({});
	}
}

/// \defgroup nodejs_methods
//@{

/**
 * @memberof K2hKeyQueue
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

Napi::Value K2hKeyQueue::On(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// check
	if(!info.This().IsObject()||!info.This().As<Napi::Object>().InstanceOf(K2hKeyQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hKeyQueue instance)").ThrowAsJavaScriptException();
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
	std::string	emitter  = info[0].ToString().Utf8Value();
	const char*	pemitter = GetNormalizationEmitter(emitter.c_str(), stc_k2hkq_emitters);
	if(!pemitter){
		std::string	msg	= "Unknown ";
		msg				+= emitter;
		msg				+= " emitter";
		Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// add callback
	return SetK2hkqEmitterCallback(info, 1, pemitter);
}

/**
 * @memberof K2hKeyQueue
 * @fn void\
 * OnPush(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hkeyqueue object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hKeyQueue::OnPush(const Napi::CallbackInfo& info)
{
	return SetK2hkqEmitterCallback(info, 0, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_PUSH]);
}

/**
 * @memberof K2hKeyQueue
 * @fn void\
 * OnPop(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hkeyqueue object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hKeyQueue::OnPop(const Napi::CallbackInfo& info)
{
	return SetK2hkqEmitterCallback(info, 0, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_POP]);
}

/**
 * @memberof K2hKeyQueue
 * @fn void\
 * OnCount(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hkeyqueue object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hKeyQueue::OnCount(const Napi::CallbackInfo& info)
{
	return SetK2hkqEmitterCallback(info, 0, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_COUNT]);
}

/**
 * @memberof K2hKeyQueue
 * @fn void\
 * OnRead(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hkeyqueue object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hKeyQueue::OnRead(const Napi::CallbackInfo& info)
{
	return SetK2hkqEmitterCallback(info, 0, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_READ]);
}

/**
 * @memberof K2hKeyQueue
 * @fn void\
 * OnRemove(\
 * 	Callback cbfunc\
 * )
 * @brief	set callback handling for initializing k2hkeyqueue object
 *
 * @param[in] cbfunc			callback function.
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hKeyQueue::OnRemove(const Napi::CallbackInfo& info)
{
	return SetK2hkqEmitterCallback(info, 0, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_REMOVE]);
}

/**
 * @memberof K2hKeyQueue
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

Napi::Value K2hKeyQueue::Off(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "No handle emitter name is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// check emitter name
	std::string	emitter  = info[0].ToString().Utf8Value();
	const char*	pemitter = GetNormalizationEmitter(emitter.c_str(), stc_k2hkq_emitters);
	if (nullptr == pemitter) {
		std::string msg	= "Unknown ";
		msg				+= emitter;
		msg				+= " emitter";
		Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
		return env.Undefined();
	}
	// unset callback
	return UnsetK2hkqEmitterCallback(info, pemitter);
}

/**
 * @memberof K2hKeyQueue
 * @fn void\
 * OffPush(\
 * )
 * @brief	set callback handling for initializing k2hkeyqueue object
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hKeyQueue::OffPush(const Napi::CallbackInfo& info)
{
	return UnsetK2hkqEmitterCallback(info, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_PUSH]);
}

/**
 * @memberof K2hKeyQueue
 * @fn void\
 * OffPop(\
 * )
 * @brief	set callback handling for initializing k2hkeyqueue object
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hKeyQueue::OffPop(const Napi::CallbackInfo& info)
{
	return UnsetK2hkqEmitterCallback(info, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_POP]);
}

/**
 * @memberof K2hKeyQueue
 * @fn void\
 * OffCount(\
 * )
 * @brief	set callback handling for initializing k2hkeyqueue object
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hKeyQueue::OffCount(const Napi::CallbackInfo& info)
{
	return UnsetK2hkqEmitterCallback(info, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_COUNT]);
}

/**
 * @memberof K2hKeyQueue
 * @fn void\
 * OffRead(\
 * )
 * @brief	set callback handling for initializing k2hkeyqueue object
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hKeyQueue::OffRead(const Napi::CallbackInfo& info)
{
	return UnsetK2hkqEmitterCallback(info, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_READ]);
}

/**
 * @memberof K2hKeyQueue
 * @fn void\
 * OffRemove(\
 * )
 * @brief	set callback handling for initializing k2hkeyqueue object
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hKeyQueue::OffRemove(const Napi::CallbackInfo& info)
{
	return UnsetK2hkqEmitterCallback(info, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_REMOVE]);
}

/**
 * @memberof K2hKeyQueue
 * @fn bool Init(\
 * 	Object k2hobj\
 * 	, bool is_fifo=true\
 * 	, String prefix[=option]\
 * )
 * @brief Initialize k2hkeyqueue object with K2hNode object
 *
 * Nothing to do(example writing) to K2HASH file after calling Init()
 *
 * @param[in] k2hobj	Specify target K2hNode object
 * @param[in] is_fifo	Specify true for FIFO queue, false for LIFO
 * @param[in] prefix	Specify prefix of queue key name
 *
 * @return return true for success, false for failure
 */

Napi::Value K2hKeyQueue::Initialize(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// check
	if(info.Length() < 1){
		Napi::TypeError::New(env, "No k2hash object is specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject()||!info.This().As<Napi::Object>().InstanceOf(K2hKeyQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hKeyQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hKeyQueue* obj = Napi::ObjectWrap<K2hKeyQueue>::Unwrap(info.This().As<Napi::Object>());

	// parse positional optional args
	if(!info[0].IsObject()){
		Napi::TypeError::New(env, "First parameter must be a k2hnode object").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hNode* objNode = Napi::ObjectWrap<K2hNode>::Unwrap(info[0].As<Napi::Object>());

	bool		is_fifo = (info.Length() < 2 ? true : info[1].ToBoolean().Value());
	std::string	prefix;
	if(2 < info.Length()){
		prefix = info[2].ToString().Utf8Value();
	}

	// Execute
	bool res = obj->_k2hkeyqueue.Init(&objNode->_k2hshm, is_fifo, (prefix.empty() ? NULL : reinterpret_cast<const unsigned char*>(prefix.c_str())), (prefix.empty() ? 0UL : (prefix.length() + 1)));
	return Napi::Boolean::New(env, res);
}

/**
 * @memberof K2hKeyQueue
 * @fn bool Push(\
 * 	  String	key\
 * 	, String	val\
 * 	, String	pass=""\
 * 	, int		expire=0\
 * 	, Callback cbfunc=null\
 * )
 *
 * @brief	Set key and value to K2HASH and push it to queue.
 *			If the callback function is specified, or on callback handles for this,
 *  		this method works asynchronization and calls callback function at finishing.
 *
 * @param[in] key		Specify the key which is pushed to queue
 * @param[in] val		Specify the value which is set key
 * @param[in] pass		Specify optional passphrase if encrypting the key
 * @param[in] expire	Specify optional timeout seconds if you need
 * @param[in] cbfunc	callback function.
 *
 * @return return true for success, false for failure
 *
 */

Napi::Value K2hKeyQueue::Push(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 2){
		Napi::TypeError::New(env, "No key name or no value are specified.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject()||!info.This().As<Napi::Object>().InstanceOf(K2hKeyQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hKeyQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hKeyQueue* obj = Napi::ObjectWrap<K2hKeyQueue>::Unwrap(info.This().As<Napi::Object>());

	std::string	strkey;
	std::string	strval;
	bool		is_pass_set	= false;
	std::string	strpass;
	bool		is_exp_set	= false;
	time_t		expire		= 0;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2hkq_emitters[K2HKQ_EMITTER_POS_PUSH]);
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
		Napi::TypeError::New(env, "value is empty.").ThrowAsJavaScriptException();
		return env.Undefined();
	}else{
		strval = info[1].ToString().Utf8Value();
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
			int	nexire	= info[2].ToNumber().Int32Value();
			expire		= static_cast<time_t>(nexire);
			is_exp_set	= true;
		}else if(info[2].IsString()){
			strpass		= info[2].ToString().Utf8Value();
			is_pass_set	= true;
		}else{
			Napi::TypeError::New(env, "Unknown third parameter.").ThrowAsJavaScriptException();
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
		}else if(info[3].IsNumber() && !is_exp_set){
			int	nexire	= info[3].ToNumber().Int32Value();
			expire		= static_cast<time_t>(nexire);
			is_exp_set	= true;
		}else{
			Napi::TypeError::New(env, "Unknown forth parameter.").ThrowAsJavaScriptException();
			return env.Undefined();
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
		// Queue async worker
		K2hkqPushAsyncWorker* worker = new K2hkqPushAsyncWorker(maybeCallback, &(obj->_k2hkeyqueue), strkey.c_str(), strval.c_str(), (is_pass_set ? strpass.c_str() : NULL), (is_exp_set && expire > 0 ? &expire : NULL));
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		bool res = obj->_k2hkeyqueue.Push(reinterpret_cast<const unsigned char*>(strkey.c_str()), (strkey.length() + 1), reinterpret_cast<const unsigned char*>(strval.c_str()), (strval.length() + 1), (is_pass_set ? strpass.c_str() : NULL), (is_exp_set && expire > 0 ? &expire : NULL));
		return Napi::Boolean::New(env, res);
	}
}

/**
 * @memberof K2hKeyQueue
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

Napi::Value K2hKeyQueue::Count(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject()||!info.This().As<Napi::Object>().InstanceOf(K2hKeyQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hKeyQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hKeyQueue* obj = Napi::ObjectWrap<K2hKeyQueue>::Unwrap(info.This().As<Napi::Object>());

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2hkq_emitters[K2HKQ_EMITTER_POS_COUNT]);
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
		K2hkqCountAsyncWorker* worker = new K2hkqCountAsyncWorker(maybeCallback, &(obj->_k2hkeyqueue));
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		uint64_t cnt = obj->_k2hkeyqueue.GetCount();
		return Napi::Number::New(env, static_cast<double>(cnt));
	}
}

/**
 * @memberof K2hKeyQueue
 * @fn bool IsEmpty(void)
 * @brief Test for the queue is empty
 *
 * @return returns true if the queue is empty. false for not empty.
 */

Napi::Value K2hKeyQueue::IsEmpty(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hKeyQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hKeyQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hKeyQueue* obj = Napi::ObjectWrap<K2hKeyQueue>::Unwrap(info.This().As<Napi::Object>());

	// Execute
	bool res = obj->_k2hkeyqueue.IsEmpty();
	return Napi::Boolean::New(env, res);
}

/**
 * @memberof K2hKeyQueue
 * @fn Array Read(\
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
 * @return	Returns the array for key and value which are queued. 
 *			If there is no data in queue or something error is occurred, returns null.
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hKeyQueue::Read(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hKeyQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hKeyQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hKeyQueue* obj = Napi::ObjectWrap<K2hKeyQueue>::Unwrap(info.This().As<Napi::Object>());

	int			pos			= 0;
	bool		is_pass_set	= false;
	std::string	strpass;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2hkq_emitters[K2HKQ_EMITTER_POS_READ]);
	if(emitterCbRef){
		maybeCallback	= emitterCbRef->Value();
		hasCallback		= true;
	}

	// parse positional optional args
	if(0 < info.Length()){
		if(info[0].IsNumber()){
			pos	= info[0].ToNumber().Int32Value();
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
		K2hkqReadAsyncWorker* worker = new K2hkqReadAsyncWorker(maybeCallback, &(obj->_k2hkeyqueue), pos, (is_pass_set ? strpass.c_str() : NULL));
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		unsigned char*	pkey	= NULL;
		unsigned char*	pval	= NULL;
		size_t			keylen	= 0;
		size_t			vallen	= 0;
		bool			result	= obj->_k2hkeyqueue.Read(&pkey, keylen, &pval, vallen, pos, (is_pass_set ? strpass.c_str() : NULL));
		if(!result || !pkey || 0 == keylen){
			K2H_Free(pkey);
			K2H_Free(pval);
			return env.Null();
		}else{
			Napi::Array	kvarr = Napi::Array::New(env, 2);

			kvarr.Set((uint32_t)0, Napi::String::New(env, std::string(reinterpret_cast<const char*>(pkey), static_cast<size_t>(strlen(reinterpret_cast<const char*>(pkey))))));
			if(pval && 0 < vallen){
				kvarr.Set((uint32_t)1, Napi::String::New(env, std::string(reinterpret_cast<const char*>(pval), static_cast<size_t>(strlen(reinterpret_cast<const char*>(pval))))));
			}else{
				kvarr.Set((uint32_t)1, Napi::String::New(env, ""));
			}
			K2H_Free(pkey);
			K2H_Free(pval);
			return kvarr;
		}
	}
}

/**
 * @memberof K2hKeyQueue
 * @fn Array Pop(\
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
 * @return	Returns the array for key and value which are queued. 
 *			If there is no data in queue or something error is occurred, returns null.
 *			Returns always true when the callback function is specified.
 *
 */

Napi::Value K2hKeyQueue::Pop(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hKeyQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hKeyQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hKeyQueue* obj = Napi::ObjectWrap<K2hKeyQueue>::Unwrap(info.This().As<Napi::Object>());

	bool		is_pass_set = false;
	std::string	strpass;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2hkq_emitters[K2HKQ_EMITTER_POS_POP]);
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
		K2hkqPopAsyncWorker* worker = new K2hkqPopAsyncWorker(maybeCallback, &(obj->_k2hkeyqueue), (is_pass_set ? strpass.c_str() : NULL));
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		unsigned char*	pkey	= NULL;
		unsigned char*	pval	= NULL;
		size_t			keylen	= 0;
		size_t			vallen	= 0;
		bool			result	= obj->_k2hkeyqueue.Pop(&pkey, keylen, &pval, vallen, (is_pass_set ? strpass.c_str() : NULL));
		if(!result || !pkey || 0 == keylen){
			K2H_Free(pkey);
			K2H_Free(pval);
			return env.Null();
		}else{
			Napi::Array	kvarr = Napi::Array::New(env, 2);

			kvarr.Set((uint32_t)0, Napi::String::New(env, std::string(reinterpret_cast<const char*>(pkey), static_cast<size_t>(strlen(reinterpret_cast<const char*>(pkey))))));
			if(pval && 0 < vallen){
				kvarr.Set((uint32_t)1, Napi::String::New(env, std::string(reinterpret_cast<const char*>(pval), static_cast<size_t>(strlen(reinterpret_cast<const char*>(pval))))));
			}else{
				kvarr.Set((uint32_t)1, Napi::String::New(env, ""));
			}
			K2H_Free(pkey);
			K2H_Free(pval);
			return kvarr;
		}
	}
}

/**
 * @memberof K2hKeyQueue
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

Napi::Value K2hKeyQueue::Remove(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	if(info.Length() < 1){
		Napi::TypeError::New(env, "Need to specify first parameter for queue count.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hKeyQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hKeyQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hKeyQueue* obj = Napi::ObjectWrap<K2hKeyQueue>::Unwrap(info.This().As<Napi::Object>());

	int			count		= 0;
	bool		is_pass_set	= false;
	std::string	strpass;

	// initial callback comes from emitter map if set
	Napi::Function				maybeCallback;
	bool						hasCallback		= false;
	Napi::FunctionReference*	emitterCbRef	= obj->_cbs.Find(stc_k2hkq_emitters[K2HKQ_EMITTER_POS_REMOVE]);
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
		K2hkqRemoveAsyncWorker* worker = new K2hkqRemoveAsyncWorker(maybeCallback, &(obj->_k2hkeyqueue), count, (is_pass_set ? strpass.c_str() : NULL));
		worker->Queue();
		return Napi::Boolean::New(env, true);
	}else{
		int result = obj->_k2hkeyqueue.Remove(count, NULL, NULL, (is_pass_set ? strpass.c_str() : NULL));
		return Napi::Number::New(env, static_cast<double>(result));
	}
}

/**
 * @memberof K2hKeyQueue
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

Napi::Value K2hKeyQueue::Dump(const Napi::CallbackInfo& info)
{
	Napi::Env env = info.Env();

	// Unwrap
	if(!info.This().IsObject() || !info.This().As<Napi::Object>().InstanceOf(K2hKeyQueue::constructor.Value())){
		Napi::TypeError::New(env, "Invalid this object(K2hKeyQueue instance)").ThrowAsJavaScriptException();
		return env.Undefined();
	}
	K2hKeyQueue* obj = Napi::ObjectWrap<K2hKeyQueue>::Unwrap(info.This().As<Napi::Object>());

	int		fd = (0 < info.Length() && info[0].IsNumber()) ? info[0].ToNumber().Int32Value() : -1;
	FILE*	fp = (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Napi::Error::New(env, "could not open output stream.").ThrowAsJavaScriptException();
		return env.Undefined();
	}

	obj->_k2hkeyqueue.Dump(fp);

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
