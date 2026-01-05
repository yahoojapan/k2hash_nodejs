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

import resolve	from '@rollup/plugin-node-resolve';
import commonjs	from '@rollup/plugin-commonjs';
import terser	from '@rollup/plugin-terser';
import replace	from '@rollup/plugin-replace';

export default
{
	input:	'build/esm/index.js',
	output: {
		file:		'build/esm/index.mjs',
		format:		'es',
		sourcemap:	true
	},
	plugins: [
		resolve({
			preferBuiltins: true
		}),
		commonjs(),
		replace({
			'process.env.NODE_ENV':	JSON.stringify(process.env.NODE_ENV || 'production'),
			preventAssignment:		true
		}),
		terser()
	],
	external: [
		'bindings',
		'node-addon-api'
	]
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
