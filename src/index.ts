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
 * CREATE:   Wed 19 Nov 2025
 * REVISION:
 */

//---------------------------------------------------------
// High-level TypeScript wrapper for the native k2hash addon
//
// [NOTE]
// Implementation intentionally uses `any` for the native
// binding because detailed types are provided in the
// hand-written types/index.d.ts which will be included in
// the package.
//
// Lazy-loading, compatibility-preserving implementation for
// the k2hash package.
//	- ensureNative() loads the native binding on first use.
//	- copyPropsToFactory preserves callable default + named
//	  properties.
//	- createLazyProxy provides callable/constructible named
//	  exports.
//
//---------------------------------------------------------
// How to call this module supported(index.js/index.mjs)
//---------------------------------------------------------
//	- CommonJS
//		const k2hash = require('k2hash');
//		const k2h = k2hash();
//
//	- CommonJS(reference constuctor directly)
//		const k2hash = require('k2hash');
//		const K2hNode = k2hash.K2hNode;
//		const k2h = k2hash();
//
//	- TypeScript(1)
//		import k2hash from 'k2hash';
//		const k2h = k2hash();
//
//	- TypeScript(2)
//		import k2hash from 'k2hash';
//		const k2h = new k2hash();
//
//	- TypeScript(3: compatible calling)
//		import k2hash = require('k2hash');
//		const k2h = k2hash();
//
//	- TypeScript(reference constuctor directly)
//		import k2hash from 'k2hash';
//		const K2hNode = k2hash.K2hNode;
//		const k2h = k2hash();
//
//	- ESM(pure ES module files: guaranteed version)
//		import { createRequire } from 'module';
//		const require = createRequire(import.meta.url);
//		const k2hash = require('k2hash');
//		const k2h = k2hash();
//---------------------------------------------------------

type AnyFn = (...args: any[]) => any;

let _native: any = undefined;
let _nativeLoaded = false;

// [NOTE]
// Copy properties from native onto the factory(mirrors original runtime
// normalization)
// We try to preserve descriptors when possible.
//
function copyPropsToFactory(k2hashFactory: AnyFn, factory_native: any)
{
	if(!factory_native || (typeof factory_native !== 'object' && typeof factory_native !== 'function')){
		return;
	}

	try{
		Object.getOwnPropertyNames(factory_native).forEach((name) => {
			if(name === 'prototype'){
				return;
			}
			try{
				const	desc = Object.getOwnPropertyDescriptor(factory_native, name);
				if(desc){
					Object.defineProperty(k2hashFactory, name, desc);
				}
			}catch{
				try{
					(k2hashFactory as any)[name] = factory_native[name];
				}catch{
					// ignore
				}
			}
		});

		Object.getOwnPropertySymbols(factory_native).forEach((sym) => {
			try{
				const	desc = Object.getOwnPropertyDescriptor(factory_native, sym as any);
				if(desc){
					Object.defineProperty(k2hashFactory, sym as any, desc);
				}
			}catch{
				try{
					(k2hashFactory as any)[sym as any] = factory_native[sym as any];
				}catch{
					// ignore
				}
			}
		});
	}catch{
		// ignore odd cases(native undefined or primitive)
	}

	try{
		if(factory_native && factory_native.K2hNode){
			(k2hashFactory as any).K2hNode = factory_native.K2hNode;
		}
		if(factory_native && factory_native.K2hQueue){
			(k2hashFactory as any).K2hQueue = factory_native.K2hQueue;
		}
		if(factory_native && factory_native.K2hKeyQueue){
			(k2hashFactory as any).K2hKeyQueue = factory_native.K2hKeyQueue;
		}
	}catch{
		// ignore
	}
}

// [NOTE]
// Load native on first need. Also call copyPropsToFactory once after
// loading.
// require('bindings') may throw if native not present at build/test time
// eslint-disable-next-line @typescript-eslint/no-var-requires
//
function ensureNative(): any
{
	if(_nativeLoaded){
		return _native;
	}
	_nativeLoaded = true;

	try{
		const bindings = require('bindings');
		_native = bindings('k2hash');
	}catch{
		_native = undefined;
	}

	// [NOTE]
	// If we successfully loaded native, copy properties onto the factory so
	//  default export retains the same shape as before (callable + named
	//  properties).
	//
	if(_native){
		try{
			// [NOTE]
			// k2hashFactory is hoisted(function declaration), safe to
			// reference here
			copyPropsToFactory(k2hashFactory as AnyFn, _native);
		}catch{
			// swallow copy errors to preserve robustness
		}
	}
	return _native;
}

// [NOTE]
// Default factory that mirrors previous behavior
//
function k2hashFactory(...args: any[]): any
{
	const _factory_native = ensureNative();
	if(typeof _factory_native === 'function'){
		try{
			const retobj = (_factory_native as AnyFn).apply(null, args);
			if(retobj !== undefined){
				return retobj;
			}
		}catch{
			// fallthrough to constructor fallback
		}
	}

	if(_factory_native && typeof _factory_native.K2hNode === 'function'){
		try{
			return new _factory_native.K2hNode(...args);
		}catch{
			// fallthrough
		}
	}

	// fallback: return native itself (could be an object with properties)
	return _factory_native;
}

// [NOTE]
// Returns a callable & constructible Proxy that forwards to native export
//
function createLazyProxy(name: string): AnyFn
{
	let _cached: any = undefined;

	function loadActual()
	{
		if(_cached !== undefined){
			return _cached;
		}
		const _lazy_native	= ensureNative();
		const _actual		= _lazy_native && _lazy_native[name] ? _lazy_native[name] : undefined;
		if(!_actual){
			throw new Error("Native export " + JSON.stringify(name) + " is not available");
		}
		_cached = _actual;
		return _cached;
	}

	const target = function (this: any, ...args: any[])
	{
		const _actual = loadActual();
		return _actual.apply(this, args);
	};

	const handler: ProxyHandler<any> = {
		apply(_t, thisArg, args) {
			const _actual = loadActual();
			return _actual.apply(thisArg, args);
		},
		construct(_t, args, newTarget) {
			const _actual = loadActual();
			return Reflect.construct(_actual, args, newTarget);
		},
		get(_t, prop, receiver) {
			const _actual = loadActual();
			return Reflect.get(_actual, prop, receiver);
		},
		set(_t, prop, value, receiver) {
			const _actual = loadActual();
			return Reflect.set(_actual, prop, value, receiver);
		},
		has(_t, prop) {
			const _actual = loadActual();
			return prop in _actual;
		},
		ownKeys(_t) {
			const _actual = loadActual();
			return Reflect.ownKeys(_actual);
		},
		getOwnPropertyDescriptor(_t, prop) {
			const _actual = loadActual();
			return Object.getOwnPropertyDescriptor(_actual, prop) || undefined;
		},
		getPrototypeOf(_t) {
			const _actual = loadActual();
			return Object.getPrototypeOf(_actual);
		},
		setPrototypeOf(_t, proto) {
			const _actual = loadActual();
			return Object.setPrototypeOf(_actual, proto);
		},
		defineProperty(_t, prop, descriptor) {
			const _actual = loadActual();
			return Reflect.defineProperty(_actual, prop, descriptor);
		},
		deleteProperty(_t, prop) {
			const _actual = loadActual();
			return Reflect.deleteProperty(_actual, prop);
		}
	};

	return new Proxy(target as AnyFn, handler) as AnyFn;
}

//
// Export named proxies to keep compatibility with existing consumers
//
// Named convenience exports (runtime values).
// Types are provided by types/index.d.ts.
//
export const K2hNode		= createLazyProxy('K2hNode');
export const K2hQueue		= createLazyProxy('K2hQueue');
export const K2hKeyQueue	= createLazyProxy('K2hKeyQueue');

export default k2hashFactory;

//
// Compatibility normalization for CommonJS output
//
// [NOTE]
// When TypeScript/packager emits CommonJS, it can produce "module.exports = { default: ..., ... }"
// which causes ESM consumers to get an object whose "default" is the callable factory.
// Normalize module.exports so that "require(...)" yields a callable function while
// preserving named properties.
// This runs only in CommonJS environments and is intentionally defensive.
//
declare const module: any;

try{
	if(typeof module !== 'undefined' && module && module.exports){
		const	own_module_export = module.exports as any;

		//
		// If module.exports is exactly { default: fn }, replace module.exports with fn.
		//
		if(	own_module_export													&&
			typeof own_module_export === 'object'								&&
			Object.prototype.hasOwnProperty.call(own_module_export, 'default')	&&
			typeof own_module_export.default === 'function'						&&
			Object.keys(own_module_export).length === 1							)
		{
			module.exports = own_module_export.default;

		}else if(own_module_export && typeof own_module_export === 'object' && typeof own_module_export.default === 'function'){
			//
			// If default is a function but other named exports exist,
			// prefer a top-level callable while preserving named props.
			//
			try{
				const	defaultFn = own_module_export.default as Function;

				// If top-level is not already callable, make it so.
				if(typeof own_module_export !== 'function'){
					// create a callable wrapper that forwards to defaultFn
					const	callable = function(this: any, ...args: any[]){
						return defaultFn.apply(this, args);
					};

					// copy named properties from original exports except 'default'
					Object.keys(own_module_export).forEach((key) => {
						if(key === 'default'){
							return;
						}
						try{
							(callable as any)[key] = own_module_export[key];
						}catch{
							// ignore
						}
					});

					// preserve reference to original module
					try{
						(callable as any).__orig_module = own_module_export;
					}catch{
						// ignore
					}

					// replace module.exports with callable that also carries named props
					module.exports = callable;
				}
			}catch{
				// swallow normalization errors - not critical
			}
		}
	}
}catch{
	// ignore
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
