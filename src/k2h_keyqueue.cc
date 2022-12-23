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

using namespace v8;
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
// Utility macros
//---------------------------------------------------------
#define	SetK2hkqEmitterCallback(info, pos, pemitter) \
		{ \
			K2hKeyQueue*	obj = Nan::ObjectWrap::Unwrap<K2hKeyQueue>(info.This()); \
			if(info.Length() <= pos){ \
				Nan::ThrowSyntaxError("No callback is specified."); \
				return; \
			} \
			Nan::Callback* cb = new Nan::Callback(); \
			cb->SetFunction(info[pos].As<v8::Function>()); \
			bool	result = obj->_cbs.Set(pemitter, cb); \
			info.GetReturnValue().Set(Nan::New(result)); \
		}

#define	UnsetK2hkqEmitterCallback(info, pemitter) \
		{ \
			K2hKeyQueue*	obj		= Nan::ObjectWrap::Unwrap<K2hKeyQueue>(info.This()); \
			bool			result	= obj->_cbs.Unset(pemitter); \
			info.GetReturnValue().Set(Nan::New(result)); \
		}

//---------------------------------------------------------
// K2hKeyQueue Class
//---------------------------------------------------------
Nan::Persistent<Function>	K2hKeyQueue::constructor;

//---------------------------------------------------------
// K2hKeyQueue Methods
//---------------------------------------------------------
K2hKeyQueue::K2hKeyQueue() : _k2hkeyqueue(), _cbs()
{
}

K2hKeyQueue::~K2hKeyQueue()
{
}

void K2hKeyQueue::Init(void)
{
	// Prepare constructor template
	Local<FunctionTemplate>	tpl = Nan::New<FunctionTemplate>(New); 
	tpl->SetClassName(Nan::New("K2hKeyQueue").ToLocalChecked()); 
	tpl->InstanceTemplate()->SetInternalFieldCount(1); 

	// Prototype for event emitter
	Nan::SetPrototypeMethod(tpl, "on",			On);
	Nan::SetPrototypeMethod(tpl, "onPush",		OnPush);
	Nan::SetPrototypeMethod(tpl, "onPop",		OnPop);
	Nan::SetPrototypeMethod(tpl, "onCount",		OnCount);
	Nan::SetPrototypeMethod(tpl, "onRead",		OnRead);
	Nan::SetPrototypeMethod(tpl, "onRemove",	OnRemove);
	Nan::SetPrototypeMethod(tpl, "off",			Off);
	Nan::SetPrototypeMethod(tpl, "offPush",		OffPush);
	Nan::SetPrototypeMethod(tpl, "offPop",		OffPop);
	Nan::SetPrototypeMethod(tpl, "offCount",	OffCount);
	Nan::SetPrototypeMethod(tpl, "offRead",		OffRead);
	Nan::SetPrototypeMethod(tpl, "offRemove",	OffRemove);

	// Prototype
	Nan::SetPrototypeMethod(tpl, "init",		Init);
	Nan::SetPrototypeMethod(tpl, "push",		Push);
	Nan::SetPrototypeMethod(tpl, "count",		Count);
	Nan::SetPrototypeMethod(tpl, "isEmpty",		IsEmpty);
	Nan::SetPrototypeMethod(tpl, "read",		Read);
	Nan::SetPrototypeMethod(tpl, "pop",			Pop);
	Nan::SetPrototypeMethod(tpl, "remove",		Remove);
	Nan::SetPrototypeMethod(tpl, "dump",		Dump);

	// Reset
	constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(K2hKeyQueue::New)
{
	if(info.IsConstructCall()){ 
		// Invoked as constructor: new K2hKeyQueue()
		K2hKeyQueue*	obj = new K2hKeyQueue() ;
		obj->Wrap(info.This()); 
		info.GetReturnValue().Set(info.This()); 
	}else{ 
		// Invoked as plain function K2hKeyQueue(), turn into construct call. 
		const unsigned	argc		= 1;
		Local<Value>	argv[argc]	= {info[0]}; 
		Local<Function>	cons		= Nan::New<Function>(constructor); 
		info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked()); 
	} 
}

NAN_METHOD(K2hKeyQueue::NewInstance)
{
	const unsigned	argc		= 1;
	Local<Value>	argv[argc]	= {info[0]}; 
	Local<Function>	cons		= Nan::New<Function>(constructor); 
	info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked());
}

Local<Object> K2hKeyQueue::GetInstance(Nan::NAN_METHOD_ARGS_TYPE info)
{
	Nan::EscapableHandleScope	scope;

	const unsigned	argc		= 1;
	Local<Value>	argv[argc]	= {info[0]}; 
	Local<Function>	cons		= Nan::New<Function>(constructor); 
	Local<Object> 	instance	= Nan::NewInstance(cons, argc, argv).ToLocalChecked();

	return scope.Escape(instance);
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

NAN_METHOD(K2hKeyQueue::On)
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
	if(NULL == (pemitter = GetNormalizationEmitter(*emitter, stc_k2hkq_emitters))){
		string	msg = "Unknown ";
		msg			+= *emitter;
		msg			+= " emitter";
		Nan::ThrowSyntaxError(msg.c_str());
		return;
	}
	// add callback
	SetK2hkqEmitterCallback(info, 1, pemitter);
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

NAN_METHOD(K2hKeyQueue::OnPush)
{
	SetK2hkqEmitterCallback(info, 0, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_PUSH]);
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

NAN_METHOD(K2hKeyQueue::OnPop)
{
	SetK2hkqEmitterCallback(info, 0, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_POP]);
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

NAN_METHOD(K2hKeyQueue::OnCount)
{
	SetK2hkqEmitterCallback(info, 0, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_COUNT]);
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

NAN_METHOD(K2hKeyQueue::OnRead)
{
	SetK2hkqEmitterCallback(info, 0, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_READ]);
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

NAN_METHOD(K2hKeyQueue::OnRemove)
{
	SetK2hkqEmitterCallback(info, 0, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_REMOVE]);
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

NAN_METHOD(K2hKeyQueue::Off)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No handle emitter name is specified.");
		return;
	}

	// check emitter name
	Nan::Utf8String	emitter(info[0]);
	const char*		pemitter;
	if(NULL == (pemitter = GetNormalizationEmitter(*emitter, stc_k2hkq_emitters))){
		string	msg = "Unknown ";
		msg			+= *emitter;
		msg			+= " emitter";
		Nan::ThrowSyntaxError(msg.c_str());
		return;
	}
	// unset callback
	UnsetK2hkqEmitterCallback(info, pemitter);
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

NAN_METHOD(K2hKeyQueue::OffPush)
{
	UnsetK2hkqEmitterCallback(info, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_PUSH]);
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

NAN_METHOD(K2hKeyQueue::OffPop)
{
	UnsetK2hkqEmitterCallback(info, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_POP]);
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

NAN_METHOD(K2hKeyQueue::OffCount)
{
	UnsetK2hkqEmitterCallback(info, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_COUNT]);
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

NAN_METHOD(K2hKeyQueue::OffRead)
{
	UnsetK2hkqEmitterCallback(info, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_READ]);
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

NAN_METHOD(K2hKeyQueue::OffRemove)
{
	UnsetK2hkqEmitterCallback(info, stc_k2hkq_emitters[K2HKQ_EMITTER_POS_REMOVE]);
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
NAN_METHOD(K2hKeyQueue::Init)
{
	K2hKeyQueue*	obj	= Nan::ObjectWrap::Unwrap<K2hKeyQueue>(info.This());

	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No k2hash object is specified.");
		return;
	}
	K2hNode*	objNode	= ObjectWrap::Unwrap<K2hNode>(info[0]->ToObject(Nan::GetCurrentContext()).ToLocalChecked());
	bool		is_fifo	= (info.Length() < 2 ? true : Nan::To<bool>(info[1]).ToChecked());
	std::string	prefix;
	if(3 <= info.Length()){
		Nan::Utf8String	buf(info[2]);
		prefix = std::string(*buf);
	}

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hkeyqueue.Init(&objNode->_k2hshm, is_fifo, (prefix.empty() ? NULL : reinterpret_cast<const unsigned char*>(prefix.c_str())), (prefix.empty() ? 0UL : (prefix.length() + 1)))
	));
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

NAN_METHOD(K2hKeyQueue::Push)
{
	if(info.Length() < 2){
		Nan::ThrowSyntaxError("No key name or no value are specified.");
		return;
	}

	K2hKeyQueue*	obj			= Nan::ObjectWrap::Unwrap<K2hKeyQueue>(info.This());
	string			strkey;
	string			strval;
	bool			is_pass_set	= false;
	string			strpass;
	bool			is_exp_set	= false;
	time_t			expire		= 0;
	Nan::Callback*	callback	= obj->_cbs.Find(stc_k2hkq_emitters[K2HKQ_EMITTER_POS_PUSH]);

	if(info[0]->IsNull()){
		Nan::ThrowSyntaxError("key is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[0]);
		strkey					= std::string(*buf);
	}
	if(info[1]->IsNull()){
		Nan::ThrowSyntaxError("value is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[1]);
		strval					= std::string(*buf);
	}
	if(2 < info.Length()){
		if(info[2]->IsFunction()){
			if(3 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback			= new Nan::Callback(info[2].As<v8::Function>());
		}else if(info[2]->IsInt32()){
			int	nexpire			= Nan::To<int32_t>(info[2]).ToChecked();
			expire				= static_cast<time_t>(nexpire);
			is_exp_set			= true;
		}else if(info[2]->IsString()){
			Nan::Utf8String	buf(info[2]);
			strpass				= std::string(*buf);
			is_pass_set			= true;
		}else{
			Nan::ThrowSyntaxError("Unknown third parameter.");
			return;
		}
	}
	if(3 < info.Length()){
		if(info[3]->IsFunction()){
			if(4 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback			= new Nan::Callback(info[3].As<v8::Function>());
		}else if(info[3]->IsInt32() && !is_exp_set){
			int	nexpire			= Nan::To<int32_t>(info[3]).ToChecked();
			expire				= static_cast<time_t>(nexpire);
		}else{
			Nan::ThrowSyntaxError("Unknown forth parameter.");
			return;
		}
	}
	if(4 < info.Length()){
		if(5 < info.Length() || !info[4]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback				= new Nan::Callback(info[4].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new K2hkqPushWorker(callback, &(obj->_k2hkeyqueue), strkey.c_str(), strval.c_str(), (is_pass_set ? strpass.c_str() : NULL), (expire <= 0 ? NULL : &expire)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(
			obj->_k2hkeyqueue.Push(
				reinterpret_cast<const unsigned char*>(strkey.c_str()),
				(strkey.length() + 1), 
				reinterpret_cast<const unsigned char*>(strval.c_str()),
				(strval.length() + 1),
				(is_pass_set ? strpass.c_str() : NULL),
				(expire <= 0 ? NULL : &expire)
			)
		));
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

NAN_METHOD(K2hKeyQueue::Count)
{
	K2hKeyQueue*	obj		= Nan::ObjectWrap::Unwrap<K2hKeyQueue>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_k2hkq_emitters[K2HKQ_EMITTER_POS_COUNT]);

	if(0 < info.Length()){
		if(1 < info.Length() || !info[0]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback			= new Nan::Callback(info[0].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new K2hkqCountWorker(callback, &(obj->_k2hkeyqueue)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		//SetK2hDbgMode(K2HDBG_MSG);
		info.GetReturnValue().Set(Nan::New(obj->_k2hkeyqueue.GetCount()));
	}
}

/**
 * @memberof K2hKeyQueue
 * @fn bool IsEmpty(void)
 * @brief Test for the queue is empty
 *
 * @return returns true if the queue is empty. false for not empty.
 */

NAN_METHOD(K2hKeyQueue::IsEmpty)
{
	K2hKeyQueue*	obj	= Nan::ObjectWrap::Unwrap<K2hKeyQueue>(info.This());

	//SetK2hDbgMode(K2HDBG_MSG);

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hkeyqueue.IsEmpty()
	));
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

NAN_METHOD(K2hKeyQueue::Read)
{
	K2hKeyQueue*	obj			= Nan::ObjectWrap::Unwrap<K2hKeyQueue>(info.This());
	int				pos			= 0;
	bool			is_pass_set	= false;
	string			strpass;
	Nan::Callback*	callback	= obj->_cbs.Find(stc_k2hkq_emitters[K2HKQ_EMITTER_POS_READ]);

	if(0 < info.Length()){
		if(info[0]->IsNumber()){
			pos					= Nan::To<int>(info[0]).ToChecked();
		}else if(info[0]->IsString()){
			Nan::Utf8String	buf(info[0]);
			strpass				= std::string(*buf);
			is_pass_set			= true;
		}else if(info[0]->IsFunction()){
			if(1 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback			= new Nan::Callback(info[0].As<v8::Function>());
		}else{
			Nan::ThrowSyntaxError("Unknown first parameter.");
			return;
		}
	}
	if(1 < info.Length()){
		if(info[1]->IsString() && !is_pass_set){
			Nan::Utf8String	buf(info[1]);
			strpass				= std::string(*buf);
			is_pass_set			= true;
		}else if(info[1]->IsFunction()){
			if(2 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback			= new Nan::Callback(info[1].As<v8::Function>());
		}else{
			Nan::ThrowSyntaxError("Unknown second parameter.");
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
		Nan::AsyncQueueWorker(new K2hkqReadWorker(callback, &(obj->_k2hkeyqueue), pos, (is_pass_set ? strpass.c_str() : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		unsigned char*	pkey	= NULL;
		unsigned char*	pval	= NULL;
		size_t			keylen	= 0;
		size_t			vallen	= 0;
		bool			result	= obj->_k2hkeyqueue.Read(&pkey, keylen, &pval, vallen, pos, (is_pass_set ? strpass.c_str() : NULL));
		if(!result || !pkey || 0 == keylen){
			info.GetReturnValue().Set(Nan::Null());
		}else{
			Local<Array>	kvarr = Nan::New<Array>(2);
			Nan::Set(kvarr, 0, Nan::New<String>(reinterpret_cast<const char*>(pkey)).ToLocalChecked());
			if(pval && 0 < vallen){
				Nan::Set(kvarr, 1, Nan::New<String>(reinterpret_cast<const char*>(pval)).ToLocalChecked());
			}else{
				Nan::Set(kvarr, 1, Nan::EmptyString());
			}
			info.GetReturnValue().Set(kvarr);
		}
		K2H_Free(pkey);
		K2H_Free(pval);
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

NAN_METHOD(K2hKeyQueue::Pop)
{
	K2hKeyQueue*	obj			= Nan::ObjectWrap::Unwrap<K2hKeyQueue>(info.This());
	bool			is_pass_set	= false;
	string			strpass;
	Nan::Callback*	callback	= obj->_cbs.Find(stc_k2hkq_emitters[K2HKQ_EMITTER_POS_POP]);

	if(0 < info.Length()){
		if(info[0]->IsString()){
			Nan::Utf8String	buf(info[0]);
			strpass				= std::string(*buf);
			is_pass_set			= true;
		}else if(info[0]->IsFunction()){
			if(1 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback			= new Nan::Callback(info[0].As<v8::Function>());
		}else{
			Nan::ThrowSyntaxError("Unknown first parameter.");
			return;
		}
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
		Nan::AsyncQueueWorker(new K2hkqPopWorker(callback, &(obj->_k2hkeyqueue), (is_pass_set ? strpass.c_str() : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		unsigned char*	pkey	= NULL;
		unsigned char*	pval	= NULL;
		size_t			keylen	= 0;
		size_t			vallen	= 0;
		bool			result	= obj->_k2hkeyqueue.Pop(&pkey, keylen, &pval, vallen, (is_pass_set ? strpass.c_str() : NULL));
		if(!result || !pkey || 0 == keylen){
			info.GetReturnValue().Set(Nan::Null());
		}else{
			Local<Array>	kvarr = Nan::New<Array>(2);
			Nan::Set(kvarr, 0, Nan::New<String>(reinterpret_cast<const char*>(pkey)).ToLocalChecked());
			if(pval && 0 < vallen){
				Nan::Set(kvarr, 1, Nan::New<String>(reinterpret_cast<const char*>(pval)).ToLocalChecked());
			}else{
				Nan::Set(kvarr, 1, Nan::EmptyString());
			}
			info.GetReturnValue().Set(kvarr);
		}
		K2H_Free(pkey);
		K2H_Free(pval);
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

NAN_METHOD(K2hKeyQueue::Remove)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("Need to specify first parameter for queue count.");
		return;
	}

	K2hKeyQueue*	obj			= Nan::ObjectWrap::Unwrap<K2hKeyQueue>(info.This());
	int				count		= 0;
	bool			is_pass_set	= false;
	string			strpass;
	Nan::Callback*	callback	= obj->_cbs.Find(stc_k2hkq_emitters[K2HKQ_EMITTER_POS_REMOVE]);

	if(!info[0]->IsNumber()){
		Nan::ThrowSyntaxError("First parameter must be number.");
		return;
	}else{
		count					= Nan::To<int>(info[0]).ToChecked();
		if(count <= 0){
			Nan::ThrowSyntaxError("Remove queue count must be over 0.");
			return;
		}
	}
	if(1 < info.Length()){
		if(info[1]->IsString()){
			Nan::Utf8String	buf(info[1]);
			strpass				= std::string(*buf);
			is_pass_set			= true;
		}else if(info[1]->IsFunction()){
			if(2 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback			= new Nan::Callback(info[1].As<v8::Function>());
		}else{
			Nan::ThrowSyntaxError("Unknown second parameter.");
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
		Nan::AsyncQueueWorker(new K2hkqRemoveWorker(callback, &(obj->_k2hkeyqueue), count, (is_pass_set ? strpass.c_str() : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(obj->_k2hkeyqueue.Remove(count, NULL, NULL, (is_pass_set ? strpass.c_str() : NULL))));
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

NAN_METHOD(K2hKeyQueue::Dump)
{
	K2hKeyQueue*	obj	= Nan::ObjectWrap::Unwrap<K2hKeyQueue>(info.This());

	int				fd	= (0 < info.Length() && info[0]->IsInt32()) ? Nan::To<int32_t>(info[0]).ToChecked() : -1;
	FILE*			fp	= (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Nan::ThrowError("could not open output stream.");
		return;
	}

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hkeyqueue.Dump(fp)
	));

	// Need to flush stream here!
	fflush(fp);

	info.GetReturnValue().Set(Nan::True());
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
