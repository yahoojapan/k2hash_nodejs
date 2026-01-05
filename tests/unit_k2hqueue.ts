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
describe('K2HQUEUE', function()
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
	// k2hqueue k2hash::create(), close()
	//
	it('k2hqueue - k2hash::create() / k2hash::close()', function(done: Mocha.Done){
		const k2hfile1: string	= '/tmp/test02_tmp.k2h';
		const k2hfile2: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		expect(k2h.create(k2hfile1, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;
		expect(k2h.create(k2hfile2, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//-------------------------------------------------------------------
	// Test getQueue
	//-------------------------------------------------------------------
	//
	// k2hqueue k2hash::getQueue()
	//
	it('k2hqueue - k2hash::getQueue()', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//-------------------------------------------------------------------
	// Test k2hqueue::Push(FIFO)
	//-------------------------------------------------------------------
	//
	// k2hqueue::Push(FIFO nopass) - No Callback
	//
	it('k2hqueue - k2hqueue::Push(FIFO nopass) - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_000_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		const values: string[]	= [
			'data1',
			'data2',
			'data3'
		];
		let	qcount: number	= 0;
		for(const val in values){
			expect(k2hq.push(values[val])).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hq.count()).to.equal(qcount);
		}
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hq.pop()).to.equal('data1');
		expect(k2hq.pop('dummypass')).to.equal('data2');
		expect(k2hq.count()).to.equal(1);
		expect(k2hq.pop()).to.equal('data3');
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// close
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hqueue::Push(FIFO pass) - No Callback
	//
	it('k2hqueue - k2hqueue::Push(FIFO pass) - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_001_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		const	values: string[] = [
			'data1',
			'data2',
			'data3'
		];
		let	qcount: number	= 0;
		for(const val in values){
			expect(k2hq.push(values[val], 'mypass')).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hq.count()).to.equal(qcount);
		}
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hq.pop('mypass')).to.equal('data1');
		expect(k2hq.pop()).to.be.null;
		expect(k2hq.count()).to.equal(1);
		expect(k2hq.pop('mypass')).to.equal('data3');
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// close
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hqueue::Push(FIFO nopass) - on Callback
	//
	it('k2hqueue - k2hqueue::Push(FIFO nopass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_010_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(push) callback
		expect(k2hq.on('push', function(error?: any)
		{
			try{
				expect(error).to.be.null;

				// check queue
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;
				expect(k2hq.count()).to.equal(1);
				expect(k2hq.pop()).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('push');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Push(FIFO pass) - on Callback
	//
	it('k2hqueue - k2hqueue::Push(FIFO pass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_011_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(push) callback
		expect(k2hq.on('push', function(error?: any)
		{
			try{
				expect(error).to.be.null;

				// check queue
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;
				expect(k2hq.count()).to.equal(1);
				expect(k2hq.pop('mypass')).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('push');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Push(FIFO nopass) - onPush Callback
	//
	it('k2hqueue - k2hqueue::Push(FIFO nopass) - onPush Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_020_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty(), 'k2hqueue::Push(FIFO nopass) - onPush Callback : isEmpty() returns false');

		// set onPush callback
		expect(k2hq.onPush(function(error?: any)
		{
			try{
				expect(error).to.be.null;

				// check queue
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;
				expect(k2hq.count()).to.equal(1);
				expect(k2hq.pop()).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offPush();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Push(FIFO pass) - onPush Callback
	//
	it('k2hqueue - k2hqueue::Push(FIFO pass) - onPush Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_021_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onPush callback
		expect(k2hq.onPush(function(error?: any)
		{
			try{
				expect(error).to.be.null;

				// check queue
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;
				expect(k2hq.count()).to.equal(1);
				expect(k2hq.pop('mypass')).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offPush();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Push(FIFO nopass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Push(FIFO nopass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_030_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1', function(error?: any)
		{
			try{
				expect(error).to.be.null;

				// check queue
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;
				expect(k2hq.count()).to.equal(1);
				expect(k2hq.pop()).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Push(FIFO pass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Push(FIFO pass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_031_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass', function(error?: any)
		{
			try{
				expect(error).to.be.null;

				// check queue
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;
				expect(k2hq.count()).to.equal(1);
				expect(k2hq.pop('mypass')).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offPush();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hqueue::Pop(FIFO)
	//-------------------------------------------------------------------
	//
	// k2hqueue::Pop(FIFO nopass) - No Callback : this case is already tested at push.
	// k2hqueue::Pop(FIFO pass) - No Callback   : this case is already tested at push.
	//

	//
	// k2hqueue::Pop(FIFO nopass) - on Callback
	//
	it('k2hqueue - k2hqueue::Pop(FIFO nopass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_110_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(pop) callback
		expect(k2hq.on('pop', function(error?: any, data?: [string, string] | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('pop');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// pop queue(no pass)
		expect(k2hq.pop()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Pop(FIFO pass) - on Callback
	//
	it('k2hqueue - k2hqueue::Pop(FIFO pass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_111_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(pop) callback
		expect(k2hq.on('pop', function(error?: any, data?: [string, string] | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('pop');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// pop queue(pass)
		expect(k2hq.pop('mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Pop(FIFO nopass) - onPop Callback
	//
	it('k2hqueue - k2hqueue::Pop(FIFO nopass) - onPop Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_120_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onPop callback
		expect(k2hq.onPop(function(error?: any, data?: [string, string] | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offPop();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// pop queue(no pass)
		expect(k2hq.pop()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Pop(FIFO pass) - onPop Callback
	//
	it('k2hqueue - k2hqueue::Pop(FIFO pass) - onPop Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_121_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onPop callback
		expect(k2hq.onPop(function(error?: any, data?: [string, string] | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offPop();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// pop queue(pass)
		expect(k2hq.pop('mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Pop(FIFO nopass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Pop(FIFO nopass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_130_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// pop queue(no pass)
		expect(k2hq.pop(function(error?: any, data?: [string, string] | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Pop(FIFO pass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Pop(FIFO pass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_131_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// pop queue(pass)
		expect(k2hq.pop('mypass', function(error?: any, data?: [string, string] | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hqueue::Read(FIFO)
	//-------------------------------------------------------------------
	//
	// k2hqueue::Read(FIFO nopass) - No Callback
	//
	it('k2hqueue - k2hqueue::Read(FIFO nopass) - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_200_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		const values: string[]	= [
			'data1',
			'data2'
		];
		let	qcount: number	= 0;
		for(const val in values){
			expect(k2hq.push(values[val])).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hq.count()).to.equal(qcount);
		}
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hq.read()).to.equal('data1');
		expect(k2hq.read('dummypass')).to.equal('data1');
		expect(k2hq.read(1)).to.equal('data2');
		expect(k2hq.count()).to.equal(2);
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		expect(k2h.close()).to.be.a('boolean').to.be.true;
		done();
	});

	//
	// k2hqueue::Read(FIFO pass) - No Callback
	//
	it('k2hqueue - k2hqueue::Read(FIFO pass) - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_201_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		const values: string[]	= [
			'data1',
			'data2'
		];
		let	qcount: number	= 0;
		for(const val in values){
			expect(k2hq.push(values[val], 'mypass')).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hq.count()).to.equal(qcount);
		}
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hq.read('mypass')).to.equal('data1');
		expect(k2hq.read()).to.be.null;
		expect(k2hq.read(1, 'mypass')).to.equal('data2');
		expect(k2hq.count()).to.equal(2);
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		expect(k2h.close()).to.be.a('boolean').to.be.true;
		done();
	});

	//
	// k2hqueue::Read(FIFO nopass) - on Callback
	//
	it('k2hqueue - k2hqueue::Read(FIFO nopass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_210_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(read) callback
		expect(k2hq.on('read', function(error?: any, data?: [string, string] | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('read');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// read queue(no pass)
		expect(k2hq.read()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Read(FIFO pass) - on Callback
	//
	it('k2hqueue - k2hqueue::Read(FIFO pass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_211_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(read) callback
		expect(k2hq.on('read', function(error?: any, data?: [string, string] | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('read');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// read queue(pass)
		expect(k2hq.read('mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Read(FIFO nopass) - onRead Callback
	//
	it('k2hqueue - k2hqueue::Read(FIFO nopass) - onRead Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_220_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onRead callback
		expect(k2hq.onRead(function(error?: any, data?: [string, string] | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offRead();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// read queue(no pass)
		expect(k2hq.read()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Read(FIFO pass) - onRead Callback
	//
	it('k2hqueue - k2hqueue::Read(FIFO pass) - onRead Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_221_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onRead callback
		expect(k2hq.onRead(function(error?: any, data?: [string, string] | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offRead();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// read queue(pass)
		expect(k2hq.read('mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Read(FIFO nopass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Read(FIFO nopass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_230_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// read queue(no pass)
		expect(k2hq.read(function(error?: any, data?: [string, string] | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Read(FIFO pass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Read(FIFO pass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_231_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// read queue(pass)
		expect(k2hq.read('mypass', function(error?: any, data?: [string, string] | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hqueue::Remove(FIFO)
	//-------------------------------------------------------------------
	//
	// k2hqueue::Remove(FIFO nopass) - No Callback
	//
	it('k2hqueue - k2hqueue::Remove(FIFO nopass) - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_300_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		const values: string[]	= [
			'data1',
			'data2',
			'data3'
		];
		let	qcount: number	= 0;
		for(const val in values){
			expect(k2hq.push(values[val])).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hq.count()).to.equal(qcount);
		}
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hq.remove(1)).to.equal(1);
		expect(k2hq.count()).to.equal(2);
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;
		expect(k2hq.remove(2)).to.equal(2);
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;
		done();
	});

	//
	// k2hqueue::Remove(FIFO pass) - No Callback
	//
	it('k2hqueue - k2hqueue::Remove(FIFO pass) - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_301_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		const values: string[]	= [
			'data1',
			'data2',
			'data3'
		];
		let	qcount: number	= 0;
		for(const val in values){
			expect(k2hq.push(values[val], 'mypass')).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hq.count()).to.equal(qcount);
		}
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hq.remove(1, 'mypass')).to.equal(1);
		expect(k2hq.count()).to.equal(2);
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;
		expect(k2hq.remove(2, 'mypass')).to.equal(2);
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;
		done();
	});

	//
	// k2hqueue::Remove(FIFO nopass) - on Callback
	//
	it('k2hqueue - k2hqueue::Remove(FIFO nopass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_310_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2hq.on('remove', function(error?: any, remove_count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(remove_count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('remove');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// remove queue(no pass)
		expect(k2hq.remove(1)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Remove(FIFO pass) - on Callback
	//
	it('k2hqueue - k2hqueue::Remove(FIFO pass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_311_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2hq.on('remove', function(error?: any, remove_count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(remove_count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('remove');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// remove queue(pass)
		expect(k2hq.remove(1, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Remove(FIFO nopass) - onRemove Callback
	//
	it('k2hqueue - k2hqueue::Remove(FIFO nopass) - onRemove Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_320_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onRemove callback
		expect(k2hq.onRemove(function(error?: any, remove_count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(remove_count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offRemove();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// remove queue(no pass)
		expect(k2hq.remove(1)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Remove(FIFO pass) - onRemove Callback
	//
	it('k2hqueue - k2hqueue::Remove(FIFO pass) - onRemove Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_321_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onRemove callback
		expect(k2hq.onRemove(function(error?: any, remove_count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(remove_count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offRemove();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// remove queue(pass)
		expect(k2hq.remove(1, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Remove(FIFO nopass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Remove(FIFO nopass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_330_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// remove queue(no pass)
		expect(k2hq.remove(1, function(error?: any, remove_count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(remove_count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Remove(FIFO pass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Remove(FIFO pass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_331_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// remove queue(pass)
		expect(k2hq.remove(1, 'mypass', function(error?: any, remove_count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(remove_count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hqueue::Count(FIFO)
	//-------------------------------------------------------------------
	//
	// k2hqueue::Count(FIFO nopass) - No Callback
	//
	it('k2hqueue - k2hqueue::Count(FIFO nopass) - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_400_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		const values: string[]	= [
			'data1',
			'data2',
			'data3'
		];
		let	qcount: number	= 0;
		for(const val in values){
			expect(k2hq.push(values[val])).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hq.count()).to.equal(qcount);
		}
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hq.count()).to.equal(3);

		expect(k2h.close()).to.be.a('boolean').to.be.true;
		done();
	});

	//
	// k2hqueue::Count(FIFO pass) - No Callback
	//
	it('k2hqueue - k2hqueue::Count(FIFO pass) - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_401_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		const values: string[]	= [
			'data1',
			'data2',
			'data3'
		];
		let	qcount: number	= 0;
		for(const val in values){
			expect(k2hq.push(values[val], 'mypass')).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hq.count()).to.equal(qcount);
		}
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hq.count()).to.equal(3);

		expect(k2h.close()).to.be.a('boolean').to.be.true;
		done();
	});

	//
	// k2hqueue::Count(FIFO nopass) - on Callback
	//
	it('k2hqueue - k2hqueue::Count(FIFO nopass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_410_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(count) callback
		expect(k2hq.on('count', function(error?: any, count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('count');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// count queue(no pass)
		expect(k2hq.count()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Count(FIFO pass) - on Callback
	//
	it('k2hqueue - k2hqueue::Count(FIFO pass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_411_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(count) callback
		expect(k2hq.on('count', function(error?: any, count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('count');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// count queue(pass)
		expect(k2hq.count()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Count(FIFO nopass) - onCount Callback
	//
	it('k2hqueue - k2hqueue::Count(FIFO nopass) - onCount Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_420_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onCount Callback
		expect(k2hq.onCount(function(error?: any, count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offCount();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// count queue(no pass)
		expect(k2hq.count()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Count(FIFO pass) - onCount Callback
	//
	it('k2hqueue - k2hqueue::Count(FIFO pass) - onCount Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_421_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onCount Callback
		expect(k2hq.onCount(function(error?: any, count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offCount();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// count queue(pass)
		expect(k2hq.count()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Count(FIFO nopass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Count(FIFO nopass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_430_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// count queue(no pass)
		expect(k2hq.count(function(error?: any, count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Count(FIFO pass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Count(FIFO pass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_431_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// count queue(pass)
		expect(k2hq.count(function(error?: any, count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hqueue::Dump(FIFO)
	//-------------------------------------------------------------------
	//
	// k2hqueue::Dump(FIFO)
	//
	it('k2hqueue - k2hqueue::Dump(FIFO)', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test02_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, true, '_queue_fifo_530_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// dump
		const fd: number = fs.openSync('/tmp/result_queue_02_tmp.log', 'w');
		expect(k2hq.dump(fd)).to.be.a('boolean').to.be.true;
		fs.closeSync(fd);

		expect(k2h.close()).to.be.a('boolean').to.be.true;
		done();
	});

	//-------------------------------------------------------------------
	// Test k2hqueue::Push(LIFO)
	//-------------------------------------------------------------------
	//
	// k2hqueue::Push(LIFO nopass) - No Callback
	//
	it('k2hqueue - k2hqueue::Push(LIFO nopass) - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_000_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		const values: string[]	= [
			'data1',
			'data2',
			'data3'
		];
		let	qcount: number	= 0;
		for(var val in values){
			expect(k2hq.push(values[val])).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hq.count()).to.equal(qcount);
		}
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hq.pop()).to.equal('data3');
		expect(k2hq.pop('dummypass')).to.equal('data2');
		expect(k2hq.count()).to.equal(1);
		expect(k2hq.pop()).to.equal('data1');
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;
		done();
	});

	//
	// k2hqueue::Push(LIFO pass) - No Callback
	//
	it('k2hqueue - k2hqueue::Push(LIFO pass) - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_001_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		const values: string[]	= [
			'data1',
			'data2',
			'data3'
		];
		let	qcount: number	= 0;
		for(const val in values){
			expect(k2hq.push(values[val], 'mypass')).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hq.count()).to.equal(qcount);
		}
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hq.pop('mypass')).to.equal('data3');
		expect(k2hq.pop()).to.be.null;
		expect(k2hq.count()).to.equal(1);
		expect(k2hq.pop('mypass')).to.equal('data1');
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;
		done();
	});

	//
	// k2hqueue::Push(LIFO nopass) - on Callback
	//
	it('k2hqueue - k2hqueue::Push(LIFO nopass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_010_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(push) callback
		expect(k2hq.on('push', function(error?: any)
		{
			try{
				expect(error).to.be.null;

				// check queue
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;
				expect(k2hq.count()).to.equal(1);
				expect(k2hq.pop()).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('push');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Push(LIFO pass) - on Callback
	//
	it('k2hqueue - k2hqueue::Push(LIFO pass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_011_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(push) callback
		expect(k2hq.on('push', function(error?: any)
		{
			try{
				expect(error).to.be.null;

				// check queue
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;
				expect(k2hq.count()).to.equal(1);
				expect(k2hq.pop('mypass')).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('push');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Push(LIFO nopass) - onPush Callback
	//
	it('k2hqueue - k2hqueue::Push(LIFO nopass) - onPush Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_020_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onPush callback
		expect(k2hq.onPush(function(error?: any)
		{
			try{
				expect(error).to.be.null;

				// check queue
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;
				expect(k2hq.count()).to.equal(1);
				expect(k2hq.pop()).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offPush();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Push(LIFO pass) - onPush Callback
	//
	it('k2hqueue - k2hqueue::Push(LIFO pass) - onPush Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_021_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onPush callback
		expect(k2hq.onPush(function(error?: any)
		{
			try{
				expect(error).to.be.null;

				// check queue
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;
				expect(k2hq.count()).to.equal(1);
				expect(k2hq.pop('mypass')).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offPush();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Push(LIFO nopass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Push(LIFO nopass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_030_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1', function(error?: any)
		{
			try{
				expect(error).to.be.null;

				// check queue
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;
				expect(k2hq.count()).to.equal(1);
				expect(k2hq.pop()).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Push(LIFO pass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Push(LIFO pass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_031_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass', function(error?: any)
		{
			try{
				expect(error).to.be.null;

				// check queue
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;
				expect(k2hq.count()).to.equal(1);
				expect(k2hq.pop('mypass')).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offPush();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hqueue::Pop(LIFO)
	//-------------------------------------------------------------------
	//
	// k2hqueue::Pop(LIFO nopass) - No Callback : this case is already tested at push.
	// k2hqueue::Pop(LIFO pass) - No Callback   : this case is already tested at push.
	//

	//
	// k2hqueue::Pop(LIFO nopass) - on Callback
	//
	it('k2hqueue - k2hqueue::Pop(LIFO nopass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_110_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(pop) callback
		expect(k2hq.on('pop', function(error?: any, data?: string | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('pop');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// pop queue(no pass)
		expect(k2hq.pop()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Pop(LIFO pass) - on Callback
	//
	it('k2hqueue - k2hqueue::Pop(LIFO pass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_111_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(pop) callback
		expect(k2hq.on('pop', function(error?: any, data?: string | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('pop');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// pop queue(pass)
		expect(k2hq.pop('mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Pop(LIFO nopass) - onPop Callback
	//
	it('k2hqueue - k2hqueue::Pop(LIFO nopass) - onPop Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_120_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onPop callback
		expect(k2hq.onPop(function(error?: any, data?: string | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offPop();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// pop queue(no pass)
		expect(k2hq.pop()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Pop(LIFO pass) - onPop Callback
	//
	it('k2hqueue - k2hqueue::Pop(LIFO pass) - onPop Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_121_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onPop callback
		expect(k2hq.onPop(function(error?: any, data?: string | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offPop();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// pop queue(pass)
		expect(k2hq.pop('mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Pop(LIFO nopass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Pop(LIFO nopass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_130_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// pop queue(no pass)
		expect(k2hq.pop(function(error?: any, data?: string | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Pop(LIFO pass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Pop(LIFO pass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_131_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// pop queue(pass)
		expect(k2hq.pop('mypass', function(error?: any, data?: string | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hqueue::Read(LIFO)
	//-------------------------------------------------------------------
	//
	// k2hqueue::Read(LIFO nopass) - No Callback
	//
	it('k2hqueue - k2hqueue::Read(LIFO nopass) - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_200_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		const values: string[]	= [
			'data1',
			'data2'
		];
		let	qcount: number	= 0;
		for(const val in values){
			expect(k2hq.push(values[val])).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hq.count()).to.equal(qcount);
		}
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hq.read()).to.equal('data2');
		expect(k2hq.read('dummypass')).to.equal('data2');
		expect(k2hq.read(1)).to.equal('data1');
		expect(k2hq.count()).to.equal(2);
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		expect(k2h.close()).to.be.a('boolean').to.be.true;
		done();
	});

	//
	// k2hqueue::Read(LIFO pass) - No Callback
	//
	it('k2hqueue - k2hqueue::Read(LIFO pass) - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_201_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		const values: string[]	= [
			'data1',
			'data2'
		];
		let	qcount: number	= 0;
		for(const val in values){
			expect(k2hq.push(values[val], 'mypass')).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hq.count()).to.equal(qcount);
		}
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hq.read('mypass')).to.equal('data2');
		expect(k2hq.read()).to.be.null;
		expect(k2hq.read(1, 'mypass')).to.equal('data1');
		expect(k2hq.count()).to.equal(2);
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		expect(k2h.close()).to.be.a('boolean').to.be.true;
		done();
	});

	//
	// k2hqueue::Read(LIFO nopass) - on Callback
	//
	it('k2hqueue - k2hqueue::Read(LIFO nopass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_210_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(read) callback
		expect(k2hq.on('read', function(error?: any, data?: string | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('read');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// read queue(no pass)
		expect(k2hq.read()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Read(LIFO pass) - on Callback
	//
	it('k2hqueue - k2hqueue::Read(LIFO pass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_211_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(read) callback
		expect(k2hq.on('read', function(error?: any, data?: string | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('read');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// read queue(pass)
		expect(k2hq.read('mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Read(LIFO nopass) - onRead Callback
	//
	it('k2hqueue - k2hqueue::Read(LIFO nopass) - onRead Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_220_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onRead callback
		expect(k2hq.onRead(function(error?: any, data?: string | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offRead();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// read queue(no pass)
		expect(k2hq.read()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Read(LIFO pass) - onRead Callback
	//
	it('k2hqueue - k2hqueue::Read(LIFO pass) - onRead Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_221_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onRead callback
		expect(k2hq.onRead(function(error?: any, data?: string | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offRead();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// read queue(pass)
		expect(k2hq.read('mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Read(LIFO nopass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Read(LIFO nopass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_230_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// read queue(no pass)
		expect(k2hq.read(function(error?: any, data?: string | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Read(LIFO pass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Read(LIFO pass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_231_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// read queue(pass)
		expect(k2hq.read('mypass', function(error?: any, data?: string | null)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(data).to.equal('data1');
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hqueue::Remove(LIFO)
	//-------------------------------------------------------------------
	//
	// k2hqueue::Remove(LIFO nopass) - No Callback
	//
	it('k2hqueue - k2hqueue::Remove(LIFO nopass) - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_300_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		const values: string[]	= [
			'data1',
			'data2',
			'data3'
		];
		let	qcount: number	= 0;
		for(const val in values){
			expect(k2hq.push(values[val])).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hq.count()).to.equal(qcount);
		}
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hq.remove(1)).to.equal(1);
		expect(k2hq.count()).to.equal(2);
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;
		expect(k2hq.remove(2)).to.equal(2);
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;
		done();
	});

	//
	// k2hqueue::Remove(LIFO pass) - No Callback
	//
	it('k2hqueue - k2hqueue::Remove(LIFO pass) - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_301_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		const values: string[]	= [
			'data1',
			'data2',
			'data3'
		];
		let	qcount: number	= 0;
		for(const val in values){
			expect(k2hq.push(values[val], 'mypass')).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hq.count()).to.equal(qcount);
		}
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hq.remove(1, 'mypass')).to.equal(1);
		expect(k2hq.count()).to.equal(2);
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;
		expect(k2hq.remove(2, 'mypass')).to.equal(2);
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;
		done();
	});

	//
	// k2hqueue::Remove(LIFO nopass) - on Callback
	//
	it('k2hqueue - k2hqueue::Remove(LIFO nopass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_310_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2hq.on('remove', function(error?: any, remove_count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(remove_count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('remove');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// remove queue(no pass)
		expect(k2hq.remove(1)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Remove(LIFO pass) - on Callback
	//
	it('k2hqueue - k2hqueue::Remove(LIFO pass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_311_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2hq.on('remove', function(error?: any, remove_count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(remove_count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('remove');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// remove queue(pass)
		expect(k2hq.remove(1, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Remove(LIFO nopass) - onRemove Callback
	//
	it('k2hqueue - k2hqueue::Remove(LIFO nopass) - onRemove Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_320_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onRemove callback
		expect(k2hq.onRemove(function(error?: any, remove_count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(remove_count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offRemove();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// remove queue(no pass)
		expect(k2hq.remove(1)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Remove(LIFO pass) - onRemove Callback
	//
	it('k2hqueue - k2hqueue::Remove(LIFO pass) - onRemove Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_321_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onRemove callback
		expect(k2hq.onRemove(function(error?: any, remove_count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(remove_count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offRemove();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// remove queue(pass)
		expect(k2hq.remove(1, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Remove(LIFO nopass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Remove(LIFO nopass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_330_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// remove queue(no pass)
		expect(k2hq.remove(1, function(error?: any, remove_count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(remove_count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Remove(LIFO pass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Remove(LIFO pass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_331_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// remove queue(pass)
		expect(k2hq.remove(1, 'mypass', function(error?: any, remove_count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(remove_count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hqueue::Count(LIFO)
	//-------------------------------------------------------------------
	//
	// k2hqueue::Count(LIFO nopass) - No Callback
	//
	it('k2hqueue - k2hqueue::Count(LIFO nopass) - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_400_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		const values: string[]	= [
			'data1',
			'data2',
			'data3'
		];
		let	qcount: number	= 0;
		for(const val in values){
			expect(k2hq.push(values[val])).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hq.count()).to.equal(qcount);
		}
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hq.count()).to.equal(3);

		expect(k2h.close()).to.be.a('boolean').to.be.true;
		done();
	});

	//
	// k2hqueue::Count(LIFO pass) - No Callback
	//
	it('k2hqueue - k2hqueue::Count(LIFO pass) - No Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_401_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		const values: string[]	= [
			'data1',
			'data2',
			'data3'
		];
		let	qcount: number	= 0;
		for(const val in values){
			expect(k2hq.push(values[val], 'mypass')).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hq.count()).to.equal(qcount);
		}
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hq.count()).to.equal(3);

		expect(k2h.close()).to.be.a('boolean').to.be.true;
		done();
	});

	//
	// k2hqueue::Count(LIFO nopass) - on Callback
	//
	it('k2hqueue - k2hqueue::Count(LIFO nopass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_410_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(count) callback
		expect(k2hq.on('count', function(error?: any, count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('count');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// count queue(no pass)
		expect(k2hq.count()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Count(LIFO pass) - on Callback
	//
	it('k2hqueue - k2hqueue::Count(LIFO pass) - on Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_411_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(count) callback
		expect(k2hq.on('count', function(error?: any, count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.off('count');

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// count queue(pass)
		expect(k2hq.count()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Count(LIFO nopass) - onCount Callback
	//
	it('k2hqueue - k2hqueue::Count(LIFO nopass) - onCount Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_420_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onCount Callback
		expect(k2hq.onCount(function(error?: any, count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offCount();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// count queue(no pass)
		expect(k2hq.count()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Count(LIFO pass) - onCount Callback
	//
	it('k2hqueue - k2hqueue::Count(LIFO pass) - onCount Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_421_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onCount Callback
		expect(k2hq.onCount(function(error?: any, count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// unset
				k2hq.offCount();

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// count queue(pass)
		expect(k2hq.count()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Count(LIFO nopass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Count(LIFO nopass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_430_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;

		// count queue(no pass)
		expect(k2hq.count(function(error?: any, count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hqueue::Count(LIFO pass) - inline Callback
	//
	it('k2hqueue - k2hqueue::Count(LIFO pass) - inline Callback', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_431_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hq.push('data1', 'mypass')).to.be.a('boolean').to.be.true;

		// count queue(pass)
		expect(k2hq.count(function(error?: any, count?: number)
		{
			try{
				expect(error).to.be.null;

				// check data
				expect(count).to.equal(1);
				expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

				expect(k2h.close()).to.be.a('boolean').to.be.true;

				// end of this test case
				done();
			}catch(error){
				done(error);
			}
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hqueue::Dump(LIFO)
	//-------------------------------------------------------------------
	//
	// k2hqueue::Dump(LIFO)
	//
	it('k2hqueue - k2hqueue::Dump(LIFO)', function(done: Mocha.Done){
		const k2hfile: string	= '/tmp/test03_tmp.k2h';
		const k2h: any			= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		const k2hq: any			= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		// init queue & check
		expect(k2hq.init(k2h, false, '_queue_lifo_530_')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue
		expect(k2hq.push('data1')).to.be.a('boolean').to.be.true;
		expect(k2hq.isEmpty()).to.be.a('boolean').to.be.false;

		// dump
		const fd: number = fs.openSync('/tmp/result_queue_03_tmp.log', 'w');
		expect(k2hq.dump(fd)).to.be.a('boolean').to.be.true;
		fs.closeSync(fd);

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
