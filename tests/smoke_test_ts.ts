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
// TypeScript Smoke Test
//---------------------------------------------------------
// [Purpose]
//	- Verify that TypeScript type definitions(.d.ts) are written
//	  correctly(pass type checks).
//	  Only compile(type check) without executing(tsc --noEmit).
//	- Verify that type representations(constructible/callable
//	   k2hash, classes within namespaces, etc.) are provided
//	   to users correctly.
//
// [Outline]
//	- Sample code that uses TypeScript import statements to
//	  retrieve types/values from packages and instantiate and
//	  reference types.
//	- Perform type checks only with "tsc --noEmit tests/test_ts.ts".
//	  If no errors are found, determine that the .d.ts is
//	  functioning as expected.
//
// [Expected]
//	- tsc returns no errors(exit code 0).
//	  If there are no errors, the type definitions cover basic APIs.
//
// [Meaning of Failure]
//	- TS2309 / TS2300, etc.(mixing export = and top-level export,
//	  duplicate definitions)
//		Invalid .d.ts structure.
//	- Type mismatch(whether a function can be newed / whether
//	  method overloading is correct, etc.)
//		Discrepancy between the .d.ts signature and the
//		actual implementation.
//	- Module resolution error
//		Invalid "types" field in package.json, inappropriate
//		moduleResolution in tsconfig, etc.
//
// [NOTE]
// This does not verify the runtime behavior of values(no node
// execution). Note that only types are verified.
// This test can also cover whether type-specific imports such
// as "import type { K2hNode } from 'k2hash'" work(depending on
// the ambient module settings).
//
// ex)	tsc --noEmit tests/test_ts.ts
//			or
//		npx tsc --noEmit tests/test_ts.ts
//
//---------------------------------------------------------

import { K2hNode }	from '..';

function main()
{
	const	obj = new K2hNode();
	console.log('TS: constructed K2hNode, typeof:', typeof obj);
}

main();

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
