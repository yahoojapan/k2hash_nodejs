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
// CommonJS Smoke Test
//---------------------------------------------------------
// [Purpose]
//	Verify that the module can be loaded correctly in a CommonJS
//	environment(require) and that its main exports and basic
//	construction/method calls work(smoke test).
//
// [Outline]
//	- Load the project root package with require('../').
//	- Print the module key( Object.keys(mod) ) and verify that
//	  the expected exports (ex, K2hNode, K2hQueue, K2hKeyQueue,
//	  default, etc.) exist.
//	- If K2hNode is exported, attempt to instantiate it with new,
//	  and if the getQueue method exists, call it and verify the
//	  return value.
//	- Catch exceptions and send them to standard error output
//	  (to avoid crashes).
//
// [Expected]
//		module keys: [ 'K2hNode', 'K2hQueue', 'K2hKeyQueue', 'default' ]
//		K2hNode constructed -> typeof: object
//		getQueue returned: K2hQueue
//
// [Meaning of Failure]
//	- require failed
//		Not built / problem with main entry / unable to load
//		native module(.node load error).
//	- exception in new K2hNode()
//		native code initialization error or binding inconsistency.
//	- getQueue not found
//		The name expected by the API is different from the actual
//		implementation.
//---------------------------------------------------------

try{
	const	mod = require('../');		// project root package
	console.log('module keys:', Object.keys(mod));

	if(mod.K2hNode){
		try{
			const	k2hobj = new mod.K2hNode();
			console.log('K2hNode constructed -> typeof:', typeof k2hobj);

			// try to call getQueue if exists (safe-wrapped)
			if(typeof k2hobj.getQueue === 'function'){
				try{
					const	k2hqobj = k2hobj.getQueue();
					console.log('getQueue returned:', k2hqobj && k2hqobj.constructor ? k2hqobj.constructor.name : typeof k2hqobj);
				}catch(err3){
					console.error('getQueue threw:', err3 && err3.message);
					process.exit(1);
				}
			}else{
				console.error('getQueue not available on K2hNode instance');
				process.exit(1);
			}
		}catch(err2){
			console.error('K2hNode constructor threw:', err2 && err2.message);
			process.exit(1);
		}
	}else{
		console.error('K2hNode not exported by module');
		process.exit(1);
	}
}catch(err1){
	console.error('require failed:', err1 && err1.message);
	process.exit(1);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
