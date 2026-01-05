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
 * CREATE:   Tue 6 Nov 2018
 * REVISION:
 */

'use strict';

import 'mocha';
import * as fs				from 'fs';
import * as k2hashModule	from '..';

const k2hash: any			= (k2hashModule && (k2hashModule as any).default) ? (k2hashModule as any).default : k2hashModule;

// [NOTE] About chai
// In NodeJS 20, this unit test code will be converted to CommonJS by
// tsc and then executed.
// However, in Alpine's NodeJS 20, chai only supports ESM (import),
// resulting in an error.(ESM-only)
// For this reason, we will make sure that only chai calls the native
// import.
//
// If this issue did not exist (NodeJS 20 is no longer supported due
// to its EOL), the code could be simplified as follows:
//		----------------------------------
//		import * as chai from 'chai';
//		const { assert, expect } = chai;
//		before(function(){
//		});
//		----------------------------------
//
let	assert: any;	// assert.chai
let	expect: any;	// expect.chai

//--------------------------------------------------------------
// Common function
//--------------------------------------------------------------
// [NOTE]
// This sleep is high load average, but this script is only test.
//
function sleep(milliseconds: number): void
{
	const	start = new Date().getTime();
	while(true){
		if((new Date().getTime() - start) > milliseconds){
			break;
		}
	}
}

//
// Helper function for calling native dynamic import function
//
// [NOTE]
// This is chai's ESM-only problem.
//
function nativeDynamicImport(specifier: string): Promise<any>
{
	// [NOTE]
	// When using "new Function()", tsc won't convert this contents,
	// which means that Node's import() will be called at runtime
	// even after converting to CommonJS.
	//
	return (new Function('s', 'return import(s)'))(specifier);
}

//--------------------------------------------------------------
// Before in global section
//--------------------------------------------------------------
// [NOTE]
// For chai's ESM-only problem
//
// When importing(requiring) chai in NodeJS 20, native import will
// be attempted even in CommonJS.
// This allows you to import chai, which is ESM-only. If the import
// fails, the require will be retried.
//
before(async function()
{
	// Try import()
	try{
		const	chaiModule	= await nativeDynamicImport('chai');
		const	chai		= (chaiModule && (chaiModule as any).default) ? (chaiModule as any).default : chaiModule;
		assert	= chai.assert;
		expect	= chai.expect;
	}catch(error: any){
		// Retry with require()
		try{
			// eslint-disable-next-line @typescript-eslint/no-var-requires
			const	chai = require('chai');
			assert	= chai.assert;
			expect	= chai.expect;
		}catch(error2: any){
			throw new Error('Failed to load chai via import() and require(): ' + JSON.stringify(error2));
		}
	}
});

//--------------------------------------------------------------
// After in global section
//--------------------------------------------------------------
after(function(){
	// Nothing to do
});

//--------------------------------------------------------------
// BeforeEach in global section
//--------------------------------------------------------------
beforeEach(function(){
	// Nothing to do
});

//--------------------------------------------------------------
// AfterEach in global section
//--------------------------------------------------------------
afterEach(function(){
	// Nothing to do
});

//--------------------------------------------------------------
// Main describe section
//--------------------------------------------------------------
describe('K2HASH', function()
{
	//
	// Before in describe section
	//
	before(function(done: Mocha.Done){
		// Nothing to do
		done();
	});

	//
	// After in describe section
	//
	after(function(){
		// Nothing to do
	});

	//-------------------------------------------------------------------
	// Test create/close
	//-------------------------------------------------------------------
	//
	// k2hash::create(), close() - No Callback
	//
	it('k2hash - k2hash::create(), k2hash::close() - No Callback', function(done: Mocha.Done){
		const k2hfile0: string	= '/tmp/test01_tmp.k2h';		// For all test
		const k2hfile1: string	= '/tmp/test01_tmp_open.k2h';	// For open
		const k2hfile2: string	= '/tmp/test01_tmp_orw.k2h';	// For openRW
		const k2hfile3: string	= '/tmp/test01_tmp_oro.k2h';	// For openRO
		const k2hfile4: string	= '/tmp/test01_tmp_otmp1.k2h';	// For openTempfile
		const k2hfile5: string	= '/tmp/test01_tmp_otmp2.k2h';	// For openTempfile
		const k2hfile6: string	= '/tmp/test01_tmp_otmp3.k2h';	// For openTempfile
		const k2hfile7: string	= '/tmp/test01_tmp_otmp4.k2h';	// For openTempfile
		const k2hfile8: string	= '/tmp/test01_tmp_ar1.k2h';	// For Archive
		const k2hfile9: string	= '/tmp/test01_tmp_queue1.k2h';	// For getQueue
		const k2hfile10: string	= '/tmp/test01_tmp_queue2.k2h';	// For getKeyQueue
		const k2h: any			= k2hash();

		expect(k2h.create(k2hfile0, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		expect(k2h.create(k2hfile1, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		expect(k2h.create(k2hfile2, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		expect(k2h.create(k2hfile3, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		expect(k2h.create(k2hfile4, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		expect(k2h.create(k2hfile5, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		expect(k2h.create(k2hfile6, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		expect(k2h.create(k2hfile7, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		expect(k2h.create(k2hfile8, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		expect(k2h.create(k2hfile9, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		expect(k2h.create(k2hfile10, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hash::create(), close() - on Callback
	//
	it('k2hash - k2hash::create(), k2hash::close() - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_c01.k2h';
		const k2h: any			= k2hash();

		// set on(close) callback
		expect(k2h.on('close', function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// unset
				k2h.off('close');
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set on(create) callback
		expect(k2h.on('create', function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// do close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// unset
				k2h.off('create');
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// do create
		expect(k2h.create(k2hfile, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hash::create(), close() - onCreate/onClose Callback
	//
	it('k2hash - k2hash::create(), k2hash::close() - onCreate/onClose Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_c02.k2h';
		const k2h: any			= k2hash();

		// set on(close) callback
		expect(k2h.onClose(function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// unset
				k2h.offClose();
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set on(create) callback
		expect(k2h.onCreate(function(error?: any)
		{
			try{
				// do close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// unset
				k2h.offCreate();
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// do create
		expect(k2h.create(k2hfile, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hash::create(), close() - inline Callback
	//
	it('k2hash - k2hash::create(), k2hash::close() - inline Callback', function(done: Mocha.Done){
		const k2hfile	= '/tmp/test01_tmp_c03.k2h';
		const k2h: any	= k2hash();

		// create and callback
		expect(k2h.create(k2hfile, 2, 4, 4, 128, function(error?: any)
		{
			expect(error).to.be.null;

			// close and callback
			expect(k2h.close(function(error2: any)
			{
				expect(error2).to.be.null;
				// end of this test case
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test open/close
	//-------------------------------------------------------------------
	//
	// k2hnode::open(), close() - No Callback
	//
	it('k2hash - k2hnode::open(), close() - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_open.k2h';
		const k2h: any			= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::open(), close() - on Callback
	//
	it('k2hash - k2hnode::open(), close() - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_open.k2h';
		const k2h: any			= k2hash();

		// set on(close) callback
		expect(k2h.on('close', function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// unset
				k2h.off('close');
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set on(open) callback
		expect(k2h.on('open', function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// do close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// unset
				k2h.off('open');
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// do open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::open(), close() - onOpen/onClose Callback
	//
	it('k2hash - k2hnode::open(), close() - onOpen/onClose Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_open.k2h';
		const k2h: any			= k2hash();

		// set onClose() callback
		expect(k2h.onClose(function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// unset
				k2h.offClose();
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set onOpen() callback
		expect(k2h.onOpen(function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// do close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// unset
				k2h.offOpen();
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// do open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::open(), close() - inline Callback
	//
	it('k2hash - k2hnode::open(), close() - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_open.k2h';
		const k2h: any			= k2hash();

		// do open() callback
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128, function(error?: any)
		{
			expect(error).to.be.null;

			// do close() callback
			expect(k2h.close(function(error2: any)
			{
				expect(error2).to.be.null;
				// end of this test case
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openRW(), close() - No Callback
	//
	it('k2hash - k2hnode::openRW(), close() - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_orw.k2h';
		const k2h: any			= k2hash();

		expect(k2h.openRW(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::openRW(), close() - on Callback
	//
	it('k2hash - k2hnode::openRW(), close() - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_orw.k2h';
		const k2h: any			= k2hash();

		// set on(close) callback
		expect(k2h.on('close', function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// unset
				k2h.off('close');
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set on(openRW) callback
		expect(k2h.on('openRW', function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// do close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// unset
				k2h.off('openRW');
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// do openRW
		expect(k2h.openRW(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openRW(), close() - onOpenRW/onClose Callback
	//
	it('k2hash - k2hnode::openRW(), close() - onOpenRW/onClose Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_orw.k2h';
		const k2h: any			= k2hash();

		// set onClose() callback
		expect(k2h.onClose(function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// unset
				k2h.offClose();
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set onOpenRW() callback
		expect(k2h.onOpenRW(function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// do close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// unset
				k2h.offOpenRW();
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// do openRW
		expect(k2h.openRW(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openRW(), close() - inline Callback
	//
	it('k2hash - k2hnode::openRW(), close() - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_orw.k2h';
		const k2h: any			= k2hash();

		// do openRW() callback
		expect(k2h.openRW(k2hfile, true, 2, 4, 4, 128, function(error?: any)
		{
			expect(error).to.be.null;

			// do close() callback
			expect(k2h.close(function(error2: any)
			{
				expect(error2).to.be.null;
				// end of this test case
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openRO(), close() - No Callback
	//
	it('k2hash - k2hnode::openRO(), close() - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_oro.k2h';
		const k2h: any			= k2hash();

		expect(k2h.openRO(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::openRO(), close() - on Callback
	//
	it('k2hash - k2hnode::openRO(), close() - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_oro.k2h';
		const k2h: any			= k2hash();

		// set on(close) callback
		expect(k2h.on('close', function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// unset
				k2h.off('close');
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set on(openRO) callback
		expect(k2h.on('openRO', function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// do close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// unset
				k2h.off('openRO');
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// do openRO
		expect(k2h.openRO(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openRO(), close() - onOpenRO/onClose Callback
	//
	it('k2hash - k2hnode::openRO(), close() - onOpenRO/onClose Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_oro.k2h';
		const k2h: any			= k2hash();

		// set onClose() callback
		expect(k2h.onClose(function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// unset
				k2h.offClose();
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set onOpenRO() callback
		expect(k2h.onOpenRO(function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// do close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// unset
				k2h.offOpenRO();
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// do openRO
		expect(k2h.openRO(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openRO(), close() - inline Callback
	//
	it('k2hash - k2hnode::openRO(), close() - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_oro.k2h';
		const k2h: any			= k2hash();

		// do openRO() callback
		expect(k2h.openRO(k2hfile, true, 2, 4, 4, 128, function(error?: any)
		{
			expect(error).to.be.null;

			// do close() callback
			expect(k2h.close(function(error2: any)
			{
				expect(error2).to.be.null;
				// end of this test case
				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openTempfile(), close() - No Callback
	//
	it('k2hash - k2hnode::openTempfile(), close() - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_otmp1.k2h';
		const k2h: any			= k2hash();

		expect(k2h.openTempfile(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::openTempfile(), close() - on Callback
	//
	it('k2hash - k2hnode::openTempfile(), close() - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_otmp2.k2h';
		const k2h: any			= k2hash();

		// set on(close) callback
		expect(k2h.on('close', function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// unset
				k2h.off('close');
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set on(openTempfile) callback
		expect(k2h.on('openTempfile', function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// do close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// unset
				k2h.off('openTempfile');
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// do openTempfile
		expect(k2h.openTempfile(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openTempfile(), close() - onOpenTempfile/onClose Callback
	//
	it('k2hash - k2hnode::openTempfile(), close() - onOpenTempfile/onClose Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_otmp3.k2h';
		const k2h: any			= k2hash();

		// set onClose() callback
		expect(k2h.onClose(function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// unset
				k2h.offClose();
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set onOpenTempfile() callback
		expect(k2h.onOpenTempfile(function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// do close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// unset
				k2h.offOpenTempfile();
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// do openTempfile
		expect(k2h.openTempfile(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openTempfile(), close() - inline Callback
	//
	it('k2hash - k2hnode::openTempfile(), close() - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp_otmp4.k2h';
		const k2h: any			= k2hash();

		// do openTempfile() callback
		expect(k2h.openTempfile(k2hfile, true, 2, 4, 4, 128, function(error?: any)
		{
			try{
				expect(error).to.be.null;

				// do close() callback
				expect(k2h.close(function(error2: any)
				{
					try{
						expect(error2).to.be.null;
						// end of this test case
						done();
					}catch(error){
						done(error);
					}
				})).to.be.a('boolean').to.be.true;
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openMem(), close() - No Callback
	//
	it('k2hash - k2hnode::openMem(), close() - No Callback', function(done: Mocha.Done){
		const k2h: any	= k2hash();

		expect(k2h.openMem(2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::openMem(), close() - on Callback
	//
	it('k2hash - k2hnode::openMem(), close() - on Callback', function(done: Mocha.Done){
		const k2h: any	= k2hash();

		// set on(close) callback
		expect(k2h.on('close', function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// unset
				k2h.off('close');
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set on(openMem) callback
		expect(k2h.on('openMem', function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// do close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// unset
				k2h.off('openMem');
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// do openMem
		expect(k2h.openMem(2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openMem(), close() - onOpenMem/onClose Callback
	//
	it('k2hash - k2hnode::openMem(), close() - onOpenMem/onClose Callback', function(done: Mocha.Done){
		const k2h: any	= k2hash();

		// set onClose callback
		expect(k2h.onClose(function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// unset
				k2h.offClose();
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set onOpenMem() callback
		expect(k2h.onOpenMem(function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// do close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// unset
				k2h.offOpenMem();
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// do openMem
		expect(k2h.openMem(2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openMem(), close() - inline Callback
	//
	it('k2hash - k2hnode::openMem(), close() - inline Callback', function(done: Mocha.Done){
		const k2h: any	= k2hash();

		// do onOpenMem() inline callback
		expect(k2h.openMem(2, 4, 4, 128, function(error?: any)
		{
			try{
				expect(error).to.be.null;

				// do close() inline callback
				expect(k2h.close(function(error2: any)
				{
					try{
						expect(error2).to.be.null;
						// end of this test case
						done();
					}catch(error){
						done(error);
					}
				})).to.be.a('boolean').to.be.true;
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test getValue/setValue
	//-------------------------------------------------------------------
	//
	// k2hnode::open(), setValue(), getValue(), close() - No Callback
	//
	it('k2hash - k2hnode::open(), setValue(), getValue(), close() - No Callback', function(done: Mocha.Done){
		const	k2hfile: string	= '/tmp/test01_tmp.k2h';
		const	k2h: any		= k2hash();
		let		testval: any;

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set/get pattern(only key with no parameter)
		expect(k2h.setValue('setget_test_key_11', 'setget_test_key_val_11')).to.be.a('boolean').to.be.true;
		testval = k2h.getValue('setget_test_key_11');
		expect(testval).to.equal('setget_test_key_val_11');

		// set/get pattern(only key with other empty parameters)
		expect(k2h.setValue('setget_test_key_12', 'setget_test_key_val_12', null, null, 0)).to.be.a('boolean').to.be.true;
		testval = k2h.getValue('setget_test_key_12', null, true, null);
		expect(testval).to.equal('setget_test_key_val_12');

		// set/get pattern(only key with other parameters)
		expect(k2h.setValue('setget_test_key_13', 'setget_test_key_val_13', null, 'mypass', 1)).to.be.a('boolean').to.be.true;
		testval = k2h.getValue('setget_test_key_13', null, true, 'mypass');
		expect(testval).to.equal('setget_test_key_val_13');
		testval = k2h.getValue('setget_test_key_13', null, true, 'ngpass');
		expect(testval).to.not.equal('setget_test_key_val_13');
		sleep(1200);
		testval = k2h.getValue('setget_test_key_13', null, true, 'mypass');
		expect(testval).to.not.equal('setget_test_key_val_13');

		// set/get pattern(subkey with no parameters)
		expect(k2h.setValue('setget_test_key_14', 'setget_test_key_val_14', 'setget_test_subkey_14')).to.be.a('boolean').to.be.true;
		testval = k2h.getValue('setget_test_key_14', 'setget_test_subkey_14');
		expect(testval).to.equal('setget_test_key_val_14');

		// close
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::open(), setValue(), getValue(), close() - on Callback
	//
	it('k2hash - k2hnode::open(), setValue(), getValue(), close() - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set on(set) callback
		expect(k2h.on('set', function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// get
				expect(k2h.getValue('setget_test_key_21')).to.be.a('boolean').to.be.true;
				// unset
				k2h.off('set');
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set on(get) callback
		expect(k2h.on('get', function(error?: any, value?: any)
		{
			try{
				expect(error).to.be.null;
				expect(value).to.equal('setget_test_key_val_21');

				// unset
				k2h.off('get');
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set/get pattern(only key with no parameter)
		expect(k2h.setValue('setget_test_key_21', 'setget_test_key_val_21')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::open(), setValue(), getValue(), close() - onGet/onSet Callback
	//
	it('k2hash - k2hnode::open(), setValue(), getValue(), close() - onGet/onSet Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set onSet() callback
		expect(k2h.onSet(function(error?: any)
		{
			try{
				expect(error).to.be.null;
				// get
				expect(k2h.getValue('setget_test_key_31')).to.be.a('boolean').to.be.true;
				// unset
				k2h.offSet();
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set onGet() callback
		expect(k2h.onGet(function(error?: any, value?: any)
		{
			try{
				expect(error).to.be.null;
				expect(value).to.equal('setget_test_key_val_31');

				// unset
				k2h.offGet();
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set/get pattern(only key with no parameter)
		expect(k2h.setValue('setget_test_key_31', 'setget_test_key_val_31')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::open(), setValue(), getValue(), close() - inline Callback
	//
	it('k2hash - k2hnode::open(), setValue(), getValue(), close() - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set pattern(only key with no parameter)
		expect(k2h.setValue('setget_test_key_41', 'setget_test_key_val_41', function(error?: any)
		{
			try{
				expect(error).to.be.null;

				// get
				expect(k2h.getValue('setget_test_key_41', function(error2?: any, value?: any)
				{
					try{
						expect(error2).to.be.null;
						expect(value).to.equal('setget_test_key_val_41');

						// close
						expect(k2h.close()).to.be.a('boolean').to.be.true;
						// end of this test case
						done();
					}catch(error){
						done(error);
					}
				})).to.be.a('boolean').to.be.true;
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::open(), setValue(), getValue(), close() - special words
	//
	it('k2hash - k2hnode::open(), setValue(), getValue(), close() - special words', function(done: Mocha.Done){
		const keyvals: Record<string, string | null> = {
			setget_test_key_51	: 'fooval',
			setget_test_key_52	: 'barval',
			setget_test_key_53	: '',
			setget_test_key_54	: null,
			'日本語キー'		: '日本語バリュー',
			'aaa諦本語キー'		: 'aaa諦本語バリュー'
		};
		const k2hfile: string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		for(const key in keyvals){
			expect(k2h.setValue(key, keyvals[key])).to.be.a('boolean').to.be.true;
		}
		for(const key in keyvals){
			const testval: any = k2h.getValue(key);
			expect(testval).to.equal(keyvals[key]);
		}

		// close
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//-------------------------------------------------------------------
	// Test addSubkey(s)/getSubkeys
	//-------------------------------------------------------------------
	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - No Callback
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make 2 subkey and 2 key for addsubkey(s)
		expect(k2h.setValue('addsubkeys_test_key_000', 'addsubkeys_test_key_value_000')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('addsubkeys_test_key_010', 'addsubkeys_test_key_value_010')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('addsubkeys_test_key_020', 'addsubkeys_test_key_value_020')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('addsubkeys_test_key_020/addsubkeys_test_subkey_020', 'addsubkeys_test_subkey_value_020')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('addsubkeys_test_key_020/addsubkeys_test_subkey_021', 'addsubkeys_test_subkey_value_021')).to.be.a('boolean').to.be.true;

		// addSubkey
		expect(k2h.addSubkey('addsubkeys_test_key_010',	'addsubkeys_test_key_010/addsubkeys_test_subkey_010', 'addsubkeys_test_subkey_value_010')).to.be.a('boolean').to.be.true;

		// addSubkeys
		const subkeylist: string[] = [
			'addsubkeys_test_key_020/addsubkeys_test_subkey_020',
			'addsubkeys_test_key_020/addsubkeys_test_subkey_021'
		];
		expect(k2h.addSubkeys('addsubkeys_test_key_020', subkeylist)).to.be.a('boolean').to.be.true;

		// getSubkey(error)
		expect(k2h.getSubkeys('addsubkeys_test_key_000')).to.be.null;

		// getSubkey(1 subkey)
		const skeylist: string[] | null = k2h.getSubkeys('addsubkeys_test_key_010');
		expect(skeylist).to.not.be.null;
		expect(skeylist instanceof Array).to.be.a('boolean').to.be.true;
		expect((skeylist as string[]).length).to.equal(1);
		expect((skeylist as string[])[0]).to.equal('addsubkeys_test_key_010/addsubkeys_test_subkey_010');

		// getSubkey(2 subkey)
		const skeylist2: string[] | null = k2h.getSubkeys('addsubkeys_test_key_020');
		expect(skeylist2).to.not.be.null;
		expect(skeylist2 instanceof Array).to.be.a('boolean').to.be.true;
		expect((skeylist2 as string[]).length).to.equal(2);
		expect((skeylist2 as string[])[0]).to.equal('addsubkeys_test_key_020/addsubkeys_test_subkey_020');
		expect((skeylist2 as string[])[1]).to.equal('addsubkeys_test_key_020/addsubkeys_test_subkey_021');

		// close
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - on Callback(error)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - on Callback(error)', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_100', 'addsubkeys_test_key_value_100')).to.be.a('boolean').to.be.true;

		// set on(getSubkeys) callback
		expect(k2h.on('getSubkeys', function(error?: any, skeylist?: string[] | null)
		{
			try{
				expect(error).to.not.be.null;

				// unset
				k2h.off('getSubkeys');
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// getSubkeys
		expect(k2h.getSubkeys('addsubkeys_test_key_100')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - on Callback(1 subkey)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - on Callback(1 subkey)', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_110', 'addsubkeys_test_key_value_110')).to.be.a('boolean').to.be.true;

		// set on(addSubkey) callback
		expect(k2h.on('addSubkey', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// getSubkeys
				expect(k2h.getSubkeys('addsubkeys_test_key_110')).to.be.a('boolean').to.be.true;
				// unset
				k2h.off('addSubkey');
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set on(getSubkeys) callback
		expect(k2h.on('getSubkeys', function(error?: any, skeylist?: string[] | null)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// check
				expect(skeylist).to.not.be.null;
				expect(skeylist).to.not.be.undefined;
				expect(skeylist instanceof Array).to.be.a('boolean').to.be.true;
				expect((skeylist as string[]).length).to.equal(1);
				expect((skeylist as string[])[0]).to.equal('addsubkeys_test_key_110/addsubkeys_test_subkey_110');

				// unset
				k2h.off('getSubkeys');
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// addSubkey
		expect(k2h.addSubkey('addsubkeys_test_key_110', 'addsubkeys_test_key_110/addsubkeys_test_subkey_110', 'addsubkeys_test_subkey_value_110')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - on Callback(2 subkey)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - on Callback(2 subkey)', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_120', 'addsubkeys_test_key_value_120')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('addsubkeys_test_key_120/addsubkeys_test_subkey_120', 'addsubkeys_test_subkey_value_120')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('addsubkeys_test_key_120/addsubkeys_test_subkey_121', 'addsubkeys_test_subkey_value_121')).to.be.a('boolean').to.be.true;

		// set on(addSubkeys) callback
		expect(k2h.on('addSubkeys', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// getSubkeys
				expect(k2h.getSubkeys('addsubkeys_test_key_120')).to.be.a('boolean').to.be.true;
				// unset
				k2h.off('addSubkeys');
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set on(getSubkeys) callback
		expect(k2h.on('getSubkeys', function(error?: any, skeylist?: string[] | null)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// check
				expect(skeylist).to.not.be.null;
				expect(skeylist).to.not.be.undefined;
				expect(skeylist instanceof Array).to.be.a('boolean').to.be.true;
				expect((skeylist as string[]).length).to.equal(2);
				expect((skeylist as string[])[0]).to.equal('addsubkeys_test_key_120/addsubkeys_test_subkey_120');
				expect((skeylist as string[])[1]).to.equal('addsubkeys_test_key_120/addsubkeys_test_subkey_121');

				// unset
				k2h.off('getSubkeys');
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// addSubkeys
		const subkeylist: string[] = [
			'addsubkeys_test_key_120/addsubkeys_test_subkey_120',
			'addsubkeys_test_key_120/addsubkeys_test_subkey_121'
		];
		expect(k2h.addSubkeys('addsubkeys_test_key_120',	subkeylist)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - onGetSubkeys Callback(error)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - onGetSubkeys Callback(error)', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_200', 'addsubkeys_test_key_value_200')).to.be.a('boolean').to.be.true;

		// set onGetSubkeys() callback
		expect(k2h.onGetSubkeys(function(error?: any, skeylist?: string[] | null)
		{
			try{
				expect(error).to.not.be.null;
				// unset
				k2h.offGetSubkeys();
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// getSubkeys
		expect(k2h.getSubkeys('addsubkeys_test_key_200')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - onGet(Add)Subkey(s) Callback(1 subkey)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - onGet(Add)Subkey(s) Callback(1 subkey)', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_210', 'addsubkeys_test_key_value_210')).to.be.a('boolean').to.be.true;

		// set onAddSubkey callback
		expect(k2h.onAddSubkey(function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// getSubkeys
				expect(k2h.getSubkeys('addsubkeys_test_key_210')).to.be.a('boolean').to.be.true;
				// unset
				k2h.offAddSubkey();
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set onGetSubkeys() callback
		expect(k2h.onGetSubkeys(function(error?: any, skeylist?: string[] | null)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// check
				expect(skeylist).to.not.be.null;
				expect(skeylist).to.not.be.undefined;
				expect(skeylist instanceof Array).to.be.a('boolean').to.be.true;
				expect((skeylist as string[]).length).to.equal(1);
				expect((skeylist as string[])[0]).to.equal('addsubkeys_test_key_210/addsubkeys_test_subkey_210');

				// unset
				k2h.offGetSubkeys();
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// addSubkey
		expect(k2h.addSubkey('addsubkeys_test_key_210',	'addsubkeys_test_key_210/addsubkeys_test_subkey_210', 'addsubkeys_test_subkey_value_210')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - onGet(Add)Subkey(s) Callback(2 subkey)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - onGet(Add)Subkey(s) Callback(2 subkey)', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_220', 'addsubkeys_test_key_value_220')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('addsubkeys_test_key_220/addsubkeys_test_subkey_220', 'addsubkeys_test_subkey_value_220')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('addsubkeys_test_key_220/addsubkeys_test_subkey_221', 'addsubkeys_test_subkey_value_221')).to.be.a('boolean').to.be.true;

		// set onAddSubkeys() callback
		expect(k2h.onAddSubkeys(function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// getSubkeys
				expect(k2h.getSubkeys('addsubkeys_test_key_220')).to.be.a('boolean').to.be.true;
				// unset
				k2h.offAddSubkeys();
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set onGetSubkeys() callback
		expect(k2h.onGetSubkeys(function(error?: any, skeylist?: string[] | null)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// check
				expect(skeylist).to.not.be.null;
				expect(skeylist).to.not.be.undefined;
				expect(skeylist instanceof Array).to.be.a('boolean').to.be.true;
				expect((skeylist as string[]).length).to.equal(2);
				expect((skeylist as string[])[0]).to.equal('addsubkeys_test_key_220/addsubkeys_test_subkey_220');
				expect((skeylist as string[])[1]).to.equal('addsubkeys_test_key_220/addsubkeys_test_subkey_221');

				// unset
				k2h.offGetSubkeys();
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// addSubkeys
		const subkeylist: string[] = [
			'addsubkeys_test_key_220/addsubkeys_test_subkey_220',
			'addsubkeys_test_key_220/addsubkeys_test_subkey_221'
		];
		expect(k2h.addSubkeys('addsubkeys_test_key_220',	subkeylist)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - inline Callback(error)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - inline Callback(error)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_300', 'addsubkeys_test_key_value_300')).to.be.a('boolean').to.be.true;

		// getSubkeys
		expect(k2h.getSubkeys('addsubkeys_test_key_300', function(error?: any, skeylist?: string[] | null)
		{
			try{
				expect(error).to.not.be.null;
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - inline Callback(1 subkey)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - inline Callback(1 subkey)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_310', 'addsubkeys_test_key_value_310')).to.be.a('boolean').to.be.true;

		// addSubkey
		expect(k2h.addSubkey('addsubkeys_test_key_310', 'addsubkeys_test_key_310/addsubkeys_test_subkey_310', 'addsubkeys_test_subkey_value_310', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// getSubkeys
				expect(k2h.getSubkeys('addsubkeys_test_key_310', function(error?: any, skeylist?: string[] | null)
				{
					try{
						if(null != error){
							k2h.close();
						}
						expect(error).to.be.null;

						// check
						expect(skeylist).to.not.be.null;
						expect(skeylist).to.not.be.undefined;
						expect(skeylist instanceof Array).to.be.a('boolean').to.be.true;
						expect((skeylist as string[]).length).to.equal(1);
						expect((skeylist as string[])[0]).to.equal('addsubkeys_test_key_310/addsubkeys_test_subkey_310');

						// close
						expect(k2h.close()).to.be.a('boolean').to.be.true;
						// end of this test case
						done();
					}catch(error){
						done(error);
					}
				})).to.be.a('boolean').to.be.true;
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - inline Callback(2 subkey)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - inline Callback(2 subkey)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_320', 'addsubkeys_test_key_value_320')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('addsubkeys_test_key_320/addsubkeys_test_subkey_320', 'addsubkeys_test_subkey_value_320')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('addsubkeys_test_key_320/addsubkeys_test_subkey_321', 'addsubkeys_test_subkey_value_321')).to.be.a('boolean').to.be.true;

		// addSubkeys
		const subkeylist: string[] = [
			'addsubkeys_test_key_320/addsubkeys_test_subkey_320',
			'addsubkeys_test_key_320/addsubkeys_test_subkey_321'
		];

		expect(k2h.addSubkeys('addsubkeys_test_key_320', subkeylist, function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// getSubkeys
				expect(k2h.getSubkeys('addsubkeys_test_key_320', function(error?: any, skeylist?: string[] | null)
				{
					try{
						if(null != error){
							k2h.close();
						}
						expect(error).to.be.null;

						// check
						expect(skeylist).to.not.be.null;
						expect(skeylist).to.not.be.undefined;
						expect(skeylist instanceof Array).to.be.a('boolean').to.be.true;
						expect((skeylist as string[]).length).to.equal(2);
						expect((skeylist as string[])[0]).to.equal('addsubkeys_test_key_320/addsubkeys_test_subkey_320');
						expect((skeylist as string[])[1]).to.equal('addsubkeys_test_key_320/addsubkeys_test_subkey_321');

						// close
						expect(k2h.close()).to.be.a('boolean').to.be.true;
						// end of this test case
						done();
					}catch(error){
						done(error);
					}
				})).to.be.a('boolean').to.be.true;
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test remove/removeAll
	//-------------------------------------------------------------------
	//
	// k2hnode::remove(), removeAll() - No Callback
	//
	it('k2hash - k2hnode::remove(), removeAll() - No Callback', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make 2 key
		expect(k2h.setValue('remove_test_key_000', 'remove_test_key_value_000')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('remove_test_key_010', 'remove_test_key_value_010')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('remove_test_key_020', 'remove_test_key_value_020')).to.be.a('boolean').to.be.true;

		// addSubkey to keys
		expect(k2h.addSubkey('remove_test_key_010', 'remove_test_key_010/remove_test_subkey_010', 'remove_test_subkey_value_010')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_020', 'remove_test_key_020/remove_test_subkey_020', 'remove_test_subkey_value_020')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_020', 'remove_test_key_020/remove_test_subkey_021', 'remove_test_subkey_value_021')).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.remove('remove_test_key_XXX')).to.be.a('boolean').to.be.true;
		expect(k2h.removeAll('remove_test_key_XXX')).to.be.a('boolean').to.be.true;
		expect(k2h.remove('remove_test_key_000', 'remove_test_key_000/remove_test_subkey_000')).to.be.a('boolean').to.be.true;

		// remove(key)
		expect(k2h.remove('remove_test_key_000')).to.be.a('boolean').to.be.true;
		expect(k2h.getValue('remove_test_key_000')).to.be.null;

		// remove(subkey)
		expect(k2h.remove('remove_test_key_010', 'remove_test_key_010/remove_test_subkey_010')).to.be.a('boolean').to.be.true;
		expect(k2h.getValue('remove_test_key_010/remove_test_subkey_010')).to.be.null;
		expect(k2h.getSubkeys('remove_test_key_010')).to.be.null;

		// removeall()
		expect(k2h.removeAll('remove_test_key_020')).to.be.a('boolean').to.be.true;
		expect(k2h.getValue('remove_test_key_020/remove_test_subkey_020')).to.be.null;
		expect(k2h.getValue('remove_test_key_020/remove_test_subkey_021')).to.be.null;
		expect(k2h.getSubkeys('remove_test_key_020')).to.be.null;
		expect(k2h.getValue('remove_test_key_020')).to.be.null;

		// close
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::remove(), removeAll() - on Callback(error 1)
	//
	it('k2hash - k2hnode::remove(), removeAll() - on Callback(error 1)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.on('remove', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// unset
				k2h.off('remove');
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.remove('remove_test_key_XXX')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - on Callback(error 2)
	//
	it('k2hash - k2hnode::remove(), removeAll() - on Callback(error 2)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set on(removeAll) callback
		expect(k2h.on('removeAll', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// unset
				k2h.off('removeAll');
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.removeAll('remove_test_key_XXX')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - on Callback(error 3)
	//
	it('k2hash - k2hnode::remove(), removeAll() - on Callback(error 3)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key
		expect(k2h.setValue('remove_test_key_100', 'remove_test_key_value_100')).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.on('remove', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// unset
				k2h.off('remove');
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.remove('remove_test_key_100', 'remove_test_key_100/remove_test_subkey_100')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - on Callback(key)
	//
	it('k2hash - k2hnode::remove(), removeAll() - on Callback(key)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key
		expect(k2h.setValue('remove_test_key_100', 'remove_test_key_value_100')).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.on('remove', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;
				expect(k2h.getValue('remove_test_key_100')).to.be.null;

				// unset
				k2h.off('remove');
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// remove(key)
		expect(k2h.remove('remove_test_key_100')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - on Callback(subkey)
	//
	it('k2hash - k2hnode::remove(), removeAll() - on Callback(subkey)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key and subkey
		expect(k2h.setValue('remove_test_key_110', 'remove_test_key_value_110')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_110', 'remove_test_key_110/remove_test_subkey_110', 'remove_test_subkey_value_110')).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.on('remove', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;
				expect(k2h.getValue('remove_test_key_110/remove_test_subkey_110')).to.be.null;
				expect(k2h.getSubkeys('remove_test_key_110')).to.be.null;

				// unset
				k2h.off('remove');
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// remove(subkey)
		expect(k2h.remove('remove_test_key_110', 'remove_test_key_110/remove_test_subkey_110')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - on Callback(removeall)
	//
	it('k2hash - k2hnode::remove(), removeAll() - on Callback(removeall)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key and subkey
		expect(k2h.setValue('remove_test_key_120', 'remove_test_key_value_120')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_120', 'remove_test_key_120/remove_test_subkey_120', 'remove_test_subkey_value_120')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_120', 'remove_test_key_120/remove_test_subkey_121', 'remove_test_subkey_value_121')).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.on('removeAll', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				expect(k2h.getValue('remove_test_key_120/remove_test_subkey_120')).to.be.null;
				expect(k2h.getValue('remove_test_key_120/remove_test_subkey_121')).to.be.null;
				expect(k2h.getSubkeys('remove_test_key_120')).to.be.null;
				expect(k2h.getValue('remove_test_key_120')).to.be.null;

				// unset
				k2h.off('removeAll');
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// remove(removeall)
		expect(k2h.removeAll('remove_test_key_120')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - onRemove(All) Callback(error 1)
	//
	it('k2hash - k2hnode::remove(), removeAll() - onRemove(All) Callback(error 1)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.onRemove(function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// unset
				k2h.offRemove();
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.remove('remove_test_key_XXX')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - onRemove(All) Callback(error 2)
	//
	it('k2hash - k2hnode::remove(), removeAll() - onRemove(All) Callback(error 2)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set on(removeAll) callback
		expect(k2h.onRemoveAll(function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// unset
				k2h.offRemoveAll();
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.removeAll('remove_test_key_XXX')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - onRemove(All) Callback(error 3)
	//
	it('k2hash - k2hnode::remove(), removeAll() - onRemove(All) Callback(error 3)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key
		expect(k2h.setValue('remove_test_key_200', 'remove_test_key_value_200')).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.onRemove(function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// unset
				k2h.offRemove();
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.remove('remove_test_key_200', 'remove_test_key_200/remove_test_subkey_200')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - onRemove(All) Callback(key)
	//
	it('k2hash - k2hnode::remove(), removeAll() - onRemove(All) Callback(key)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key
		expect(k2h.setValue('remove_test_key_200', 'remove_test_key_value_200')).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.onRemove(function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;
				expect(k2h.getValue('remove_test_key_200')).to.be.null;

				// unset
				k2h.offRemove();
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// remove(key)
		expect(k2h.remove('remove_test_key_200')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - onRemove(All) Callback(subkey)
	//
	it('k2hash - k2hnode::remove(), removeAll() - onRemove(All) Callback(subkey)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key and subkey
		expect(k2h.setValue('remove_test_key_210', 'remove_test_key_value_210')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_210', 'remove_test_key_210/remove_test_subkey_210', 'remove_test_subkey_value_210')).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.onRemove(function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;
				expect(k2h.getValue('remove_test_key_210/remove_test_subkey_210')).to.be.null;
				expect(k2h.getSubkeys('remove_test_key_210')).to.be.null;

				// unset
				k2h.offRemove();
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// remove(subkey)
		expect(k2h.remove('remove_test_key_210', 'remove_test_key_210/remove_test_subkey_210')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - onRemove(All) Callback(removeall)
	//
	it('k2hash - k2hnode::remove(), removeAll() - onRemove(All) Callback(removeall)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key and subkey
		expect(k2h.setValue('remove_test_key_220', 'remove_test_key_value_220')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_220', 'remove_test_key_220/remove_test_subkey_220', 'remove_test_subkey_value_220')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_220', 'remove_test_key_220/remove_test_subkey_221', 'remove_test_subkey_value_221')).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.onRemoveAll(function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				expect(k2h.getValue('remove_test_key_220/remove_test_subkey_220')).to.be.null;
				expect(k2h.getValue('remove_test_key_220/remove_test_subkey_221')).to.be.null;
				expect(k2h.getSubkeys('remove_test_key_220')).to.be.null;
				expect(k2h.getValue('remove_test_key_220')).to.be.null;

				// unset
				k2h.offRemoveAll();
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// remove(removeall)
		expect(k2h.removeAll('remove_test_key_220')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - inline Callback(error 1)
	//
	it('k2hash - k2hnode::remove(), removeAll() - inline Callback(error 1)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.remove('remove_test_key_XXX', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - inline Callback(error 2)
	//
	it('k2hash - k2hnode::remove(), removeAll() - inline Callback(error 2)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.removeAll('remove_test_key_XXX', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - inline Callback(error 3)
	//
	it('k2hash - k2hnode::remove(), removeAll() - inline Callback(error 3)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key
		expect(k2h.setValue('remove_test_key_300', 'remove_test_key_value_300')).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.remove('remove_test_key_300', 'remove_test_key_300/remove_test_subkey_300', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - inline Callback(key)
	//
	it('k2hash - k2hnode::remove(), removeAll() - inline Callback(key)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key
		expect(k2h.setValue('remove_test_key_300', 'remove_test_key_value_300')).to.be.a('boolean').to.be.true;

		// remove(key)
		expect(k2h.remove('remove_test_key_300', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;
				expect(k2h.getValue('remove_test_key_300')).to.be.null;

				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - inline Callback(subkey)
	//
	it('k2hash - k2hnode::remove(), removeAll() - inline Callback(subkey)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key and subkey
		expect(k2h.setValue('remove_test_key_310', 'remove_test_key_value_310')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_310', 'remove_test_key_310/remove_test_subkey_310', 'remove_test_subkey_value_310')).to.be.a('boolean').to.be.true;

		// remove(subkey)
		expect(k2h.remove('remove_test_key_310', 'remove_test_key_310/remove_test_subkey_310', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;
				expect(k2h.getValue('remove_test_key_310/remove_test_subkey_310')).to.be.null;
				expect(k2h.getSubkeys('remove_test_key_310')).to.be.null;

				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - inline Callback(removeall)
	//
	it('k2hash - k2hnode::remove(), removeAll() - inline Callback(removeall)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key and subkey
		expect(k2h.setValue('remove_test_key_320', 'remove_test_key_value_320')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_320', 'remove_test_key_320/remove_test_subkey_320', 'remove_test_subkey_value_320')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_320', 'remove_test_key_320/remove_test_subkey_321', 'remove_test_subkey_value_321')).to.be.a('boolean').to.be.true;

		// remove(removeall)
		expect(k2h.removeAll('remove_test_key_320', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				expect(k2h.getValue('remove_test_key_320/remove_test_subkey_320')).to.be.null;
				expect(k2h.getValue('remove_test_key_320/remove_test_subkey_321')).to.be.null;
				expect(k2h.getSubkeys('remove_test_key_320')).to.be.null;
				expect(k2h.getValue('remove_test_key_320')).to.be.null;

				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test printState/printVersion/getAttrVersionInfos/getAttrInfos
	//-------------------------------------------------------------------
	//
	// k2hnode::printState(), printVersion(), getAttrVersionInfos(), getAttrInfos()
	//
	it('k2hash - k2hnode::printState(), printVersion(), getAttrVersionInfos(), getAttrInfos()', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		expect(k2h.printState()).to.be.a('boolean').to.be.true;
		expect(k2h.printVersion()).to.be.a('boolean').to.be.true;
		expect(k2h.getAttrVersionInfos()).to.be.a('boolean').to.be.true;
		expect(k2h.getAttrInfos()).to.be.a('boolean').to.be.true;

		const fd: number = fs.openSync('/tmp/result01_tmp.log', 'w');

		expect(k2h.printState(fd)).to.be.a('boolean').to.be.true;
		expect(k2h.printVersion(fd)).to.be.a('boolean').to.be.true;
		expect(k2h.getAttrVersionInfos(fd)).to.be.a('boolean').to.be.true;
		expect(k2h.getAttrInfos(fd)).to.be.a('boolean').to.be.true;

		fs.closeSync(fd);

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//-------------------------------------------------------------------
	// Test dump
	//-------------------------------------------------------------------
	//
	// k2hnode::dumpHead(), dumpKeytable(), dumpFullKeytable(), dumpElementtable(), dumpFull()
	//
	it('k2hash - k2hnode::dumpHead(), dumpKeytable(), dumpFullKeytable(), dumpElementtable(), dumpFull()', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		const fd: number = fs.openSync('/tmp/result01_tmp.log', 'w');

		expect(k2h.dumpHead(fd)).to.be.a('boolean').to.be.true;
		expect(k2h.dumpKeytable(fd)).to.be.a('boolean').to.be.true;
		expect(k2h.dumpFullKeytable(fd)).to.be.a('boolean').to.be.true;
		expect(k2h.dumpElementtable(fd)).to.be.a('boolean').to.be.true;
		expect(k2h.dumpFull(fd)).to.be.a('boolean').to.be.true;

		fs.closeSync(fd);

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//-------------------------------------------------------------------
	// Test transaction
	//-------------------------------------------------------------------
	//
	// k2hnode::transaction(), enableTransaction(), disableTransaction()
	//
	it('k2hash - k2hnode::transaction(), enableTransaction(), disableTransaction()', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		expect(k2h.transaction(true, '/tmp/test01_tmp.tr', 'tr_prefix_', 'dummy_parameter', 10)).to.be.a('boolean').to.be.true;
		expect(k2h.transaction(true)).to.be.a('boolean').to.be.false;
		expect(k2h.transaction(true, '/tmp/test01_tmp.tr')).to.be.a('boolean').to.be.true;
		expect(k2h.transaction(true, '/tmp/test01_tmp.tr', 'tr_prefix_')).to.be.a('boolean').to.be.true;
		expect(k2h.transaction(true, '/tmp/test01_tmp.tr', 'tr_prefix_', 'dummy_parameter')).to.be.a('boolean').to.be.true;
		expect(k2h.transaction(false, '/tmp/test01_tmp.tr', 'tr_prefix_', 'dummy_parameter')).to.be.a('boolean').to.be.true;
		expect(k2h.enableTransaction('/tmp/test01_tmp.tr', 'tr_prefix_', 'dummy_parameter')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('tr_test_key1', 'tr_test_value1')).to.be.a('boolean').to.be.true;
		expect(k2h.disableTransaction()).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('tr_test_key2', 'tr_test_value2')).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::unsetTransactionThreadPool(), setTransactionThreadPool(), getTransactionThreadPool()
	//
	it('k2hash - k2hnode::unsetTransactionThreadPool(), setTransactionThreadPool(), getTransactionThreadPool()', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		expect(k2h.unsetTransactionThreadPool()).to.be.a('boolean').to.be.true;
		expect(k2h.setTransactionThreadPool(5)).to.be.a('boolean').to.be.true;
		expect(k2h.getTransactionThreadPool()).to.equal(5);

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//-------------------------------------------------------------------
	// Test Archive
	//-------------------------------------------------------------------
	//
	// k2hnode::putArchive(), loadArchive() - No Callback
	//
	it('k2hash - k2hnode::putArchive(), loadArchive() - No Callback', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		expect(k2h.putArchive('/tmp/result_archive_tmp00.ar', false)).to.be.a('boolean').to.be.true;
		expect(k2h.loadArchive('/tmp/result_archive_tmp00.ar', false)).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::putArchive(), loadArchive() - on Callback
	//
	it('k2hash - k2hnode::putArchive(), loadArchive() - on Callback', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set on(put) callback
		expect(k2h.on('put', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// load
				expect(k2h.loadArchive('/tmp/result_archive_tmp01.ar', false)).to.be.a('boolean').to.be.true;
				// unset
				k2h.off('put');
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set on(load) callback
		expect(k2h.on('load', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// unset
				k2h.off('load');
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// put
		expect(k2h.putArchive('/tmp/result_archive_tmp01.ar', false)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::putArchive(), loadArchive() - onLoad/onPut Callback
	//
	it('k2hash - k2hnode::putArchive(), loadArchive() - onLoad/onPut Callback', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set onPut callback
		expect(k2h.onPut(function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// load
				expect(k2h.loadArchive('/tmp/result_archive_tmp01.ar', false)).to.be.a('boolean').to.be.true;
				// unset
				k2h.offPut();
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set onLoad callback
		expect(k2h.onLoad(function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// unset
				k2h.offLoad();
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// put
		expect(k2h.putArchive('/tmp/result_archive_tmp01.ar', false)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::putArchive(), loadArchive() - inline Callback
	//
	it('k2hash - k2hnode::putArchive(), loadArchive() - inline Callback', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp_ar1.k2h';
		const k2h: any			= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// put
		expect(k2h.putArchive('/tmp/result_archive_tmp02.ar', false, function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// load
				expect(k2h.loadArchive('/tmp/result_archive_tmp02.ar', false, function(error2: any)
				{
					try{
						if(null != error2){
							k2h.close();
						}
						expect(error2).to.be.null;

						// close
						expect(k2h.close()).to.be.a('boolean').to.be.true;
						// end of this test case
						done();
					}catch(error){
						done(error);
					}
				})).to.be.a('boolean').to.be.true;
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test Common attributes
	//-------------------------------------------------------------------
	//
	// k2hnode::setCommonAttribute(), getAttrVersionInfos(), cleanCommonAttribute()
	//
	it('k2hash - k2hnode::setCommonAttribute(), getAttrVersionInfos(), cleanCommonAttribute()', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		expect(k2h.setCommonAttribute(1, 1, 0, '', 1, 100)).to.be.a('boolean').to.be.true;
		expect(k2h.getAttrVersionInfos()).to.be.a('boolean').to.be.true;
		expect(k2h.cleanCommonAttribute()).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//-------------------------------------------------------------------
	// Test Attrs(common setting)
	//-------------------------------------------------------------------
	//
	// k2hnode::addAttrPluginLib(), addAttrCryptPass()
	//
	it('k2hash - k2hnode::addAttrPluginLib(), addAttrCryptPass()', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		expect(k2h.addAttrPluginLib('libfile_path')).to.be.a('boolean').to.be.false;
		expect(k2h.addAttrCryptPass('passphrase')).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//-------------------------------------------------------------------
	// Test Attrs
	//-------------------------------------------------------------------
	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - No Callback
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - No Callback', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set value
		expect(k2h.setValue('attr_test_key_000', 'attr_test_key_val_000')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('attr_test_key_001', 'attr_test_key_val_001')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('attr_test_key_002', 'attr_test_key_val_002', null, 'mypass')).to.be.a('boolean').to.be.true;

		// set attribute
		expect(k2h.addAttr('attr_test_key_001', 'attr_test_attr_001', 'attr_test_attr_val_001')).to.be.a('boolean').to.be.true;

		// get attributes(error)
		expect(k2h.getAttrs('attr_test_key_XXX')).to.be.null;
		expect(k2h.getAttrs('attr_test_key_000')).to.be.null;

		// get attributes(ok)
		const attrslist1: string[] | null = k2h.getAttrs('attr_test_key_001');
		expect(attrslist1).to.not.be.null;
		expect(attrslist1 instanceof Array).to.be.a('boolean').to.be.true;
		expect((attrslist1 as string[]).length).to.equal(1);
		expect((attrslist1 as string[])[0]).to.equal('attr_test_attr_001');

		const attrslist2: string[] | null = k2h.getAttrs('attr_test_key_002');
		expect(attrslist2).to.not.be.null;
		expect(attrslist2 instanceof Array).to.be.a('boolean').to.be.true;
		expect((attrslist2 as string[]).length).to.equal(1);
		expect((attrslist2 as string[])[0]).to.equal('aes256_cbc_pad.pbkdf2.sha256');

		// get attribute value(error)
		expect(k2h.getAttrValue('attr_test_key_XXX', 'attr_test_attr_XXX')).to.be.null;
		expect(k2h.getAttrValue('attr_test_key_000', 'attr_test_attr_000')).to.be.null;

		// get attribute value(ok)
		expect(k2h.getAttrValue('attr_test_key_001', 'attr_test_attr_001')).to.equal('attr_test_attr_val_001');
		expect(k2h.getAttrValue('attr_test_key_002', 'aes256_cbc_pad.pbkdf2.sha256')).to.not.be.null;

		// close
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - on Callback(error 1)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - on Callback(error 1)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set on(getAttrValue) callback
		expect(k2h.on('getAttrValue', function(error?: any, value?: string | null)
		{
			try{
				expect(error).to.not.be.null;

				// unset
				k2h.off('getAttrValue');
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// get attribute value(error)
		expect(k2h.getAttrValue('attr_test_key_XXX', 'attr_test_attr_XXX')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - on Callback(error 2)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - on Callback(error 2)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set value
		expect(k2h.setValue('attr_test_key_100', 'attr_test_key_val_100')).to.be.a('boolean').to.be.true;

		// set on(getAttrValue) callback
		expect(k2h.on('getAttrValue', function(error?: any, value?: string | null)
		{
			try{
				expect(error).to.not.be.null;

				// unset
				k2h.off('getAttrValue');
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set on(getAttrs) callback
		expect(k2h.on('getAttrs', function(error?: any, attrs?: string[] | null)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// check attributes
				expect(attrs).to.not.be.null;
				expect(attrs).to.not.be.undefined;
				expect(attrs instanceof Array).to.be.a('boolean').to.be.true;
				expect((attrs as string[]).length).to.equal(1);
				expect((attrs as string[])[0]).to.equal('attr_test_attr_100');

				// get attribute value(error)
				expect(k2h.getAttrValue('attr_test_key_100', 'attr_test_attr_XXX')).to.be.a('boolean').to.be.true;
				// unset
				k2h.off('getAttrs');
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set on(addAttr) callback
		expect(k2h.on('addAttr', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// get attributes
				expect(k2h.getAttrs('attr_test_key_100')).to.be.a('boolean').to.be.true;
				// unset
				k2h.off('addAttr');
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set attribute
		expect(k2h.addAttr('attr_test_key_100', 'attr_test_attr_100', 'attr_test_attr_val_100')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - on Callback(ok)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - on Callback(ok)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set value
		expect(k2h.setValue('attr_test_key_101', 'attr_test_key_val_101')).to.be.a('boolean').to.be.true;

		// set on(getAttrValue) callback
		expect(k2h.on('getAttrValue', function(error?: any, value?: string | null)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// check attribute value
				expect(value).to.equal('attr_test_attr_val_101');
				// unset
				k2h.off('getAttrValue');
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set on(getAttrs) callback
		expect(k2h.on('getAttrs', function(error?: any, attrs?: string[] | null)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// check attributes
				expect(attrs).to.not.be.null;
				expect(attrs).to.not.be.undefined;
				expect(attrs instanceof Array).to.be.a('boolean').to.be.true;
				expect((attrs as string[]).length).to.equal(1);
				expect((attrs as string[])[0]).to.equal('attr_test_attr_101');

				// get attribute value
				expect(k2h.getAttrValue('attr_test_key_101', 'attr_test_attr_101')).to.be.a('boolean').to.be.true;
				// unset
				k2h.off('getAttrs');
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set on(addAttr) callback
		expect(k2h.on('addAttr', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// get attributes
				expect(k2h.getAttrs('attr_test_key_101')).to.be.a('boolean').to.be.true;
				// unset
				k2h.off('addAttr');
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set attribute
		expect(k2h.addAttr('attr_test_key_101', 'attr_test_attr_101', 'attr_test_attr_val_101')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - onXXX Callback(error 1)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - onXXX Callback(error 1)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set onGetAttrValue callback
		expect(k2h.onGetAttrValue(function(error?: any, value?: string | null | undefined)
		{
			try{
				expect(error).to.not.be.null;

				// unset
				k2h.offGetAttrValue();
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// get attribute value(error)
		expect(k2h.getAttrValue('attr_test_key_XXX', 'attr_test_attr_XXX')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - onXXX Callback(error 2)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - onXXX Callback(error 2)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set value
		expect(k2h.setValue('attr_test_key_200', 'attr_test_key_val_200')).to.be.a('boolean').to.be.true;

		// set onGetAttrValue callback
		expect(k2h.onGetAttrValue(function(error?: any, value?: string | null | undefined)
		{
			try{
				expect(error).to.not.be.null;

				// unset
				k2h.offGetAttrValue();
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set onGetAttrs callback
		expect(k2h.onGetAttrs(function(error?: any, attrs?: string[] | null)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// check attributes
				expect(attrs).to.not.be.null;
				expect(attrs).to.not.be.undefined;
				expect(attrs instanceof Array).to.be.a('boolean').to.be.true;
				expect((attrs as string[]).length).to.equal(1);
				expect((attrs as string[])[0]).to.equal('attr_test_attr_200');

				// get attribute value(error)
				expect(k2h.getAttrValue('attr_test_key_200', 'attr_test_attr_XXX')).to.be.a('boolean').to.be.true;
				// unset
				k2h.offGetAttrs();
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;


		// set onAddAttr callback
		expect(k2h.onAddAttr(function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// get attributes
				expect(k2h.getAttrs('attr_test_key_200')).to.be.a('boolean').to.be.true;
				// unset
				k2h.off('addAttr');
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set attribute
		expect(k2h.addAttr('attr_test_key_200', 'attr_test_attr_200', 'attr_test_attr_val_200')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - onXXX Callback(ok)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - onXXX Callback(ok)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set value
		expect(k2h.setValue('attr_test_key_201', 'attr_test_key_val_201')).to.be.a('boolean').to.be.true;

		// set onGetAttrValue callback
		expect(k2h.on('getAttrValue', function(error?: any, value?: string | null)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// check attribute value
				expect(value).to.equal('attr_test_attr_val_201');
				// unset
				k2h.offGetAttrValue();
				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set onGetAttrs callback
		expect(k2h.onGetAttrs(function(error?: any, attrs?: string[] | null)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// check attributes
				expect(attrs).to.not.be.null;
				expect(attrs).to.not.be.undefined;
				expect(attrs instanceof Array).to.be.a('boolean').to.be.true;
				expect((attrs as string[]).length).to.equal(1);
				expect((attrs as string[])[0]).to.equal('attr_test_attr_201');

				// get attribute value
				expect(k2h.getAttrValue('attr_test_key_201', 'attr_test_attr_201')).to.be.a('boolean').to.be.true;
				// unset
				k2h.offGetAttrs();
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set onAddAttr callback
		expect(k2h.onAddAttr(function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// get attributes
				expect(k2h.getAttrs('attr_test_key_201')).to.be.a('boolean').to.be.true;
				// unset
				k2h.offAddAttr();
				// DO NOT call done() here
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// set attribute
		expect(k2h.addAttr('attr_test_key_201', 'attr_test_attr_201', 'attr_test_attr_val_201')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - inline Callback(error 1)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - inline Callback(error 1)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get attribute value(error)
		expect(k2h.getAttrValue('attr_test_key_XXX', 'attr_test_attr_XXX', function(error?: any, value?: string | null)
		{
			try{
				expect(error).to.not.be.null;

				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;
				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - inline Callback(error 2)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - inline Callback(error 2)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set value
		expect(k2h.setValue('attr_test_key_300', 'attr_test_key_val_300')).to.be.a('boolean').to.be.true;

		// set attribute
		expect(k2h.addAttr('attr_test_key_300', 'attr_test_attr_300', 'attr_test_attr_val_300', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// get attributes
				expect(k2h.getAttrs('attr_test_key_300', function(error?: any, attrs?: string[] | null)
				{
					try{
						if(null != error){
							k2h.close();
						}
						expect(error).to.be.null;

						// check attributes
						expect(attrs).to.not.be.null;
						expect(attrs).to.not.be.undefined;
						expect(attrs instanceof Array).to.be.a('boolean').to.be.true;
						expect((attrs as string[]).length).to.equal(1);
						expect((attrs as string[])[0]).to.equal('attr_test_attr_300');

						// get attribute value(error)
						expect(k2h.getAttrValue('attr_test_key_300', 'attr_test_attr_XXX', function(error2?: any, value?: string | null)
						{
							try{
								expect(error2).to.not.be.null;

								// close
								expect(k2h.close()).to.be.a('boolean').to.be.true;
								// end of this test case
								done();
							}catch(error){
								done(error);
							}
						})).to.be.a('boolean').to.be.true;
					}catch(error){
						done(error);
					}
				})).to.be.a('boolean').to.be.true;
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - inline Callback(ok)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - inline Callback(ok)', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set value
		expect(k2h.setValue('attr_test_key_301', 'attr_test_key_val_301')).to.be.a('boolean').to.be.true;

		// set attribute
		expect(k2h.addAttr('attr_test_key_301', 'attr_test_attr_301', 'attr_test_attr_val_301', function(error?: any)
		{
			try{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// get attributes
				expect(k2h.getAttrs('attr_test_key_301', function(error?: any, attrs?: string[] | null)
				{
					try{
						if(null != error){
							k2h.close();
						}
						expect(error).to.be.null;

						// check attributes
						expect(attrs).to.not.be.null;
						expect(attrs).to.not.be.undefined;
						expect(attrs instanceof Array).to.be.a('boolean').to.be.true;
						expect((attrs as string[]).length).to.equal(1);
						expect((attrs as string[])[0]).to.equal('attr_test_attr_301');

						// get attribute value
						expect(k2h.getAttrValue('attr_test_key_301', 'attr_test_attr_301', function(error2?: any, value?: string | null)
						{
							try{
								if(null != error2){
									k2h.close();
								}
								expect(error2).to.be.null;

								// check attribute value
								expect(value).to.equal('attr_test_attr_val_301');
								// close
								expect(k2h.close()).to.be.a('boolean').to.be.true;
								// end of this test case
								done();
							}catch(error){
								done(error);
							}
						})).to.be.a('boolean').to.be.true;
					}catch(error){
						done(error);
					}
				})).to.be.a('boolean').to.be.true;
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test get object
	//-------------------------------------------------------------------
	//
	// k2hnode::getQueue()
	//
	it('k2hash - k2hnode::getQueue()', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp_queue1.k2h';
		const k2h: any			= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::getKeyQueue()
	//
	it('k2hash - k2hnode::getKeyQueue()', function(done: Mocha.Done){
		const k2hfile:string	= '/tmp/test01_tmp_queue2.k2h';
		const k2h: any			= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		const k2hkq: any		= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});
});

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
