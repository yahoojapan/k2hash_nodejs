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

//--------------------------------------------------------------
// Warning Suppression Wrapper
//--------------------------------------------------------------
// This file is a wrapper that enables or disables the 
// "--experimental-loader warning when running tests as follows:
//
//	$ node --loader esm.mjs --experimental-specifier-resolution=node mocha --extensions ts unit_k2hash.ts
//	  (node:XXXX) ExperimentalWarning: `--experimental-loader` may be removed in the future; instead use `register()`:
//
//--------------------------------------------------------------

import { register } from 'node:module';
import { pathToFileURL } from 'node:url';

register(new URL('../node_modules/ts-node/esm.mjs', import.meta.url), pathToFileURL('./'));

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
