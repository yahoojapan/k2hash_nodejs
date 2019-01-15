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
#include "k2h_queue.h"
#include "k2h_queue_async.h"

using namespace v8;
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
// Utility macros
//---------------------------------------------------------
#define	SetK2hqEmitterCallback(info, pos, pemitter) \
		{ \
			K2hQueue*	obj = Nan::ObjectWrap::Unwrap<K2hQueue>(info.This()); \
			if(info.Length() <= pos){ \
				Nan::ThrowSyntaxError("No callback is specified."); \
				return; \
			} \
			Nan::Callback* cb = new Nan::Callback(); \
			cb->SetFunction(info[pos].As<v8::Function>()); \
			bool	result = obj->_cbs.Set(pemitter, cb); \
			info.GetReturnValue().Set(Nan::New(result)); \
		}

#define	UnsetK2hqEmitterCallback(info, pemitter) \
		{ \
			K2hQueue*	obj		= Nan::ObjectWrap::Unwrap<K2hQueue>(info.This()); \
			bool		result	= obj->_cbs.Unset(pemitter); \
			info.GetReturnValue().Set(Nan::New(result)); \
		}

//---------------------------------------------------------
// K2hQueue Class
//---------------------------------------------------------
Nan::Persistent<Function>	K2hQueue::constructor;

//---------------------------------------------------------
// K2hQueue Methods
//---------------------------------------------------------
K2hQueue::K2hQueue() : _k2hqueue(), _cbs()
{
}

K2hQueue::~K2hQueue()
{
}

void K2hQueue::Init(void)
{
	// Prepare constructor template
	Local<FunctionTemplate>	tpl = Nan::New<FunctionTemplate>(New); 
	tpl->SetClassName(Nan::New("K2hQueue").ToLocalChecked()); 
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
	constructor.Reset(tpl->GetFunction()); 
}

NAN_METHOD(K2hQueue::New)
{
	if(info.IsConstructCall()){ 
		// Invoked as constructor: new K2hQueue()
		K2hQueue*	obj = new K2hQueue();
		obj->Wrap(info.This()); 
		info.GetReturnValue().Set(info.This()); 
	}else{ 
		// Invoked as plain function K2hQueue(), turn into construct call. 
		const unsigned	argc		= 1;
		Local<Value>	argv[argc]	= {info[0]}; 
		Local<Function>	cons		= Nan::New<Function>(constructor); 
		info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked()); 
	} 
}

NAN_METHOD(K2hQueue::NewInstance)
{
	const unsigned	argc		= 1;
	Local<Value>	argv[argc]	= {info[0]}; 
	Local<Function>	cons		= Nan::New<Function>(constructor); 
	info.GetReturnValue().Set(Nan::NewInstance(cons, argc, argv).ToLocalChecked()); 
}

Local<Object> K2hQueue::GetInstance(Nan::NAN_METHOD_ARGS_TYPE info)
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

NAN_METHOD(K2hQueue::On)
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
	if(NULL == (pemitter = GetNormalizationEmitter(*emitter, stc_k2hq_emitters))){
		string	msg = "Unknown ";
		msg			+= *emitter;
		msg			+= " emitter";
		Nan::ThrowSyntaxError(msg.c_str());
		return;
	}
	// add callback
	SetK2hqEmitterCallback(info, 1, pemitter);
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

NAN_METHOD(K2hQueue::OnPush)
{
	SetK2hqEmitterCallback(info, 0, stc_k2hq_emitters[K2HQ_EMITTER_POS_PUSH]);
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

NAN_METHOD(K2hQueue::OnPop)
{
	SetK2hqEmitterCallback(info, 0, stc_k2hq_emitters[K2HQ_EMITTER_POS_POP]);
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

NAN_METHOD(K2hQueue::OnCount)
{
	SetK2hqEmitterCallback(info, 0, stc_k2hq_emitters[K2HQ_EMITTER_POS_COUNT]);
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

NAN_METHOD(K2hQueue::OnRead)
{
	SetK2hqEmitterCallback(info, 0, stc_k2hq_emitters[K2HQ_EMITTER_POS_READ]);
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

NAN_METHOD(K2hQueue::OnRemove)
{
	SetK2hqEmitterCallback(info, 0, stc_k2hq_emitters[K2HQ_EMITTER_POS_REMOVE]);
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

NAN_METHOD(K2hQueue::Off)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No handle emitter name is specified.");
		return;
	}

	// check emitter name
	Nan::Utf8String	emitter(info[0]);
	const char*		pemitter;
	if(NULL == (pemitter = GetNormalizationEmitter(*emitter, stc_k2hq_emitters))){
		string	msg = "Unknown ";
		msg			+= *emitter;
		msg			+= " emitter";
		Nan::ThrowSyntaxError(msg.c_str());
		return;
	}
	// unset callback
	UnsetK2hqEmitterCallback(info, pemitter);
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

NAN_METHOD(K2hQueue::OffPush)
{
	UnsetK2hqEmitterCallback(info, stc_k2hq_emitters[K2HQ_EMITTER_POS_PUSH]);
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

NAN_METHOD(K2hQueue::OffPop)
{
	UnsetK2hqEmitterCallback(info, stc_k2hq_emitters[K2HQ_EMITTER_POS_POP]);
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

NAN_METHOD(K2hQueue::OffCount)
{
	UnsetK2hqEmitterCallback(info, stc_k2hq_emitters[K2HQ_EMITTER_POS_COUNT]);
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

NAN_METHOD(K2hQueue::OffRead)
{
	UnsetK2hqEmitterCallback(info, stc_k2hq_emitters[K2HQ_EMITTER_POS_READ]);
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

NAN_METHOD(K2hQueue::OffRemove)
{
	UnsetK2hqEmitterCallback(info, stc_k2hq_emitters[K2HQ_EMITTER_POS_REMOVE]);
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

NAN_METHOD(K2hQueue::Init)
{
	K2hQueue*	obj		= Nan::ObjectWrap::Unwrap<K2hQueue>(info.This());

	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No k2hash object is specified.");
		return;
	}
	K2hNode*	objNode	= ObjectWrap::Unwrap<K2hNode>(info[0]->ToObject() ) ;
	bool		is_fifo	= (info.Length() < 2 ? true : info[1]->BooleanValue());
	std::string	prefix;
	if(3 <= info.Length()){
		Nan::Utf8String	buf(info[2]);
		prefix = std::string(*buf);
	}

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hqueue.Init(&objNode->_k2hshm, is_fifo, (prefix.empty() ? NULL : reinterpret_cast<const unsigned char*>(prefix.c_str())), (prefix.empty() ? 0UL : (prefix.length() + 1)))
	));
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

NAN_METHOD(K2hQueue::Push)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("No data is specified.");
		return;
	}

	K2hQueue*		obj			= Nan::ObjectWrap::Unwrap<K2hQueue>(info.This());
	std::string		data;
	bool			is_pass_set	= false;
	std::string		pass;
	time_t			expire		= 0;
	Nan::Callback*	callback	= obj->_cbs.Find(stc_k2hq_emitters[K2HQ_EMITTER_POS_PUSH]);

	if(info[0]->IsNull()){
		Nan::ThrowSyntaxError("data is empty.");
		return;
	}else{
		Nan::Utf8String	buf(info[0]);
		data					= std::string(*buf);
	}
	if(1 < info.Length()){
		if(info[1]->IsFunction()){
			if(2 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback			= new Nan::Callback(info[1].As<v8::Function>());
		}else if(!info[1]->IsNull()){
			Nan::Utf8String	buf(info[1]);
			pass				= std::string(*buf);
			is_pass_set			= true;
		}
	}
	if(2 < info.Length()){
		if(info[2]->IsFunction()){
			if(3 < info.Length()){
				Nan::ThrowSyntaxError("Last parameter is not callback function.");
				return;
			}
			callback			= new Nan::Callback(info[2].As<v8::Function>());
		}else if(info[2]->IsInt32()){
			int	nexpire			= info[2]->Int32Value();
			expire				= static_cast<time_t>(nexpire);
		}else{
			Nan::ThrowSyntaxError("Expire parameter must be int32 value.");
			return;
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
		Nan::AsyncQueueWorker(new K2hqPushWorker(callback, &(obj->_k2hqueue), data.c_str(), (is_pass_set ? pass.c_str() : NULL), (expire <= 0 ? NULL : &expire)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(obj->_k2hqueue.Push(reinterpret_cast<const unsigned char*>(data.c_str()), (data.length() + 1), NULL, (is_pass_set ? pass.c_str() : NULL), (expire <= 0 ? NULL : &expire))));
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

NAN_METHOD(K2hQueue::Count)
{
	K2hQueue*		obj		= Nan::ObjectWrap::Unwrap<K2hQueue>(info.This());
	Nan::Callback*	callback= obj->_cbs.Find(stc_k2hq_emitters[K2HQ_EMITTER_POS_COUNT]);

	if(0 < info.Length()){
		if(1 < info.Length() || !info[0]->IsFunction()){
			Nan::ThrowSyntaxError("Last parameter is not callback function.");
			return;
		}
		callback			= new Nan::Callback(info[0].As<v8::Function>());
	}

	// work
	if(callback){
		Nan::AsyncQueueWorker(new K2hqCountWorker(callback, &(obj->_k2hqueue)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		//SetK2hDbgMode(K2HDBG_MSG);
		info.GetReturnValue().Set(Nan::New(obj->_k2hqueue.GetCount()));
	}
}

/**
 * @memberof K2hQueue
 * @fn bool IsEmpty(void)
 * @brief Test for the queue is empty
 *
 * @return returns true if the queue is empty. false for not empty.
 */

NAN_METHOD(K2hQueue::IsEmpty)
{
	K2hQueue*	obj		= Nan::ObjectWrap::Unwrap<K2hQueue>(info.This());

	//SetK2hDbgMode(K2HDBG_MSG);

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hqueue.IsEmpty()
	));
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

NAN_METHOD(K2hQueue::Read)
{
	K2hQueue*		obj			= Nan::ObjectWrap::Unwrap<K2hQueue>(info.This());
	int				pos			= 0;
	bool			is_pass_set	= false;
	string			strpass;
	Nan::Callback*	callback	= obj->_cbs.Find(stc_k2hq_emitters[K2HQ_EMITTER_POS_READ]);

	if(0 < info.Length()){
		if(info[0]->IsNumber()){
			pos					= info[0]->NumberValue();
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
		Nan::AsyncQueueWorker(new K2hqReadWorker(callback, &(obj->_k2hqueue), pos, (is_pass_set ? strpass.c_str() : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		unsigned char*	pdata	= NULL;
		size_t			datalen	= 0;
		bool			result	= obj->_k2hqueue.Read(&pdata, datalen, pos, (is_pass_set ? strpass.c_str() : NULL));
		if(!result || !pdata || 0 == datalen){
			info.GetReturnValue().Set(Nan::Null());
		}else{
			info.GetReturnValue().Set(Nan::New<String>(reinterpret_cast<const char*>(pdata)).ToLocalChecked());
		}
		K2H_Free(pdata);
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

NAN_METHOD(K2hQueue::Pop)
{
	K2hQueue*		obj			= Nan::ObjectWrap::Unwrap<K2hQueue>(info.This());
	bool			is_pass_set	= false;
	string			strpass;
	Nan::Callback*	callback	= obj->_cbs.Find(stc_k2hq_emitters[K2HQ_EMITTER_POS_POP]);

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
		Nan::AsyncQueueWorker(new K2hqPopWorker(callback, &(obj->_k2hqueue), (is_pass_set ? strpass.c_str() : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		unsigned char*	pdata	= NULL;
		size_t			datalen	= 0;
		bool			result	= obj->_k2hqueue.Pop(&pdata, datalen, NULL, (is_pass_set ? strpass.c_str() : NULL));
		if(!result || !pdata || 0 == datalen){
			info.GetReturnValue().Set(Nan::Null());
		}else{
			info.GetReturnValue().Set(Nan::New<String>(reinterpret_cast<const char*>(pdata)).ToLocalChecked());
		}
		K2H_Free(pdata);
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

NAN_METHOD(K2hQueue::Remove)
{
	if(info.Length() < 1){
		Nan::ThrowSyntaxError("Need to specify first parameter for count of queue.");
		return;
	}

	K2hQueue*		obj			= Nan::ObjectWrap::Unwrap<K2hQueue>(info.This());
	int				count		= 0;
	bool			is_pass_set	= false;
	string			strpass;
	Nan::Callback*	callback	= obj->_cbs.Find(stc_k2hq_emitters[K2HQ_EMITTER_POS_REMOVE]);

	if(!info[0]->IsNumber()){
		Nan::ThrowSyntaxError("First parameter must be number.");
		return;
	}else{
		count				= info[0]->NumberValue();
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
		Nan::AsyncQueueWorker(new K2hqRemoveWorker(callback, &(obj->_k2hqueue), count, (is_pass_set ? strpass.c_str() : NULL)));
		info.GetReturnValue().Set(Nan::True());
	}else{
		info.GetReturnValue().Set(Nan::New(obj->_k2hqueue.Remove(count, NULL, NULL, (is_pass_set ? strpass.c_str() : NULL))));
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

NAN_METHOD(K2hQueue::Dump)
{
	K2hQueue*	obj	= Nan::ObjectWrap::Unwrap<K2hQueue>(info.This());

	int			fd	= (0 < info.Length() && info[0]->IsInt32()) ? info[0]->Int32Value() : -1;
	FILE*		fp	= (-1 == fd ? stdout : fdopen(fd, "a"));
	if(!fp){
		Nan::ThrowError("could not open output stream.");
		return;
	}

	info.GetReturnValue().Set(Nan::New(
		obj->_k2hqueue.Dump(fp)
	));

	// Need to flush stream here!
	fflush(fp);

	info.GetReturnValue().Set(Nan::True());
}

//@}

/*
 * VIM modelines
 *
 * vim:set ts=4 fenc=utf-8:
 */
