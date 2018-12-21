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
 * the LICENSE file that was distributed with this source code.
 *
 * AUTHOR:   Takeshi Nakatani
 * CREATE:   Tue 6 Nov 2018
 * REVISION:
 */

'use strict';

var	common		= require('./unit_common');				// Common objects for Chai
var	chai		= common.chai;							// eslint-disable-line no-unused-vars
var	k2hash		= common.k2hash;						// eslint-disable-line no-unused-vars
var	assert		= common.assert;						// eslint-disable-line no-unused-vars
var	expect		= common.expect;						// eslint-disable-line no-unused-vars

//--------------------------------------------------------------
// Main discribe section
//--------------------------------------------------------------
describe('K2HKEYQUEUE', function(){						// eslint-disable-line no-undef
	//
	// Before in discribe section
	//
	before(function(done){								// eslint-disable-line no-undef
		// Nothing to do
		done();
	});

	//
	// After in discribe section
	//
	after(function(){									// eslint-disable-line no-undef
		// Nothing to do
	});

	//-------------------------------------------------------------------
	// Test create/close
	//-------------------------------------------------------------------
	//
	// k2hkeyqueue k2hash::create(), close()
	//
	it('k2hkeyqueue - k2hash::create(), close()', function(done){						// eslint-disable-line no-undef
		var	k2hfile1	= '/tmp/test04_tmp.k2h';
		var	k2hfile2	= '/tmp/test05_tmp.k2h';
		var	k2h			= k2hash();

		expect(k2h.create(k2hfile1, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;
		expect(k2h.create(k2hfile2, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//-------------------------------------------------------------------
	// Test getKeyQueue
	//-------------------------------------------------------------------
	//
	// k2hkeyqueue k2hash::getKeyQueue()
	//
	it('k2hkeyqueue - k2hash::getKeyQueue()', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';

		var	k2h		= k2hash();
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//-------------------------------------------------------------------
	// Test k2hkeyqueue::Push(FIFO)
	//-------------------------------------------------------------------
	//
	// k2hkeyqueue::Push(FIFO nopass) - No Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Push(FIFO nopass) - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_000_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		var	values	= {'key_000_1' : 'data_000_1', 'key_000_2' : 'data_000_2', 'key_000_3' : 'data_000_3' };
		var	qcount	= 0;
		for(var key in values){
			expect(k2hkq.push(key, values[key])).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hkq.count()).to.equal(qcount);
		}
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		var	queuedata = k2hkq.pop();
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_000_1');
		expect(queuedata[1]).to.equal('data_000_1');

		queuedata = k2hkq.pop('dummypass');																												// This case, pop returns data.
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_000_2');
		expect(queuedata[1]).to.equal('data_000_2');
		expect(k2hkq.count()).to.equal(1);

		queuedata = k2hkq.pop();
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_000_3');
		expect(queuedata[1]).to.equal('data_000_3');
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hkeyqueue::Push(FIFO pass) - No Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Push(FIFO pass) - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_001_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		var	values	= {'key_001_1' : 'data_001_1', 'key_001_2' : 'data_001_2', 'key_001_3' : 'data_001_3' };
		var	qcount	= 0;
		for(var key in values){
			expect(k2hkq.push(key, values[key], 'mypass')).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hkq.count()).to.equal(qcount);
		}
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		var	queuedata = k2hkq.pop('mypass');
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_001_1');
		expect(queuedata[1]).to.equal('data_001_1');

		queuedata = k2hkq.pop();
		expect(queuedata).to.be.null;
		expect(k2hkq.count()).to.equal(1);

		queuedata = k2hkq.pop('mypass');
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_001_3');
		expect(queuedata[1]).to.equal('data_001_3');
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hkeyqueue::Push(FIFO nopass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Push(FIFO nopass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_010_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(push) callback
		expect(k2hkq.on('push', function(error)
		{
			expect(error).to.be.null;

			// check queue
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(k2hkq.count()).to.equal(1);
			var	queuedata = k2hkq.pop();
			expect(queuedata.length).to.equal(2);
			expect(queuedata[0]).to.equal('key_010_1');
			expect(queuedata[1]).to.equal('data_010_1');
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('push');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_010_1', 'data_010_1')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Push(FIFO pass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Push(FIFO pass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_011_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(push) callback
		expect(k2hkq.on('push', function(error)
		{
			expect(error).to.be.null;

			// check queue
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(k2hkq.count()).to.equal(1);
			var	queuedata = k2hkq.pop('mypass');
			expect(queuedata.length).to.equal(2);
			expect(queuedata[0]).to.equal('key_011_1');
			expect(queuedata[1]).to.equal('data_011_1');
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('push');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_011_1', 'data_011_1', 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Push(FIFO nopass) - onPush Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Push(FIFO nopass) - onPush Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_020_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onPush callback
		expect(k2hkq.onPush(function(error)
		{
			expect(error).to.be.null;

			// check queue
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(k2hkq.count()).to.equal(1);
			var	queuedata = k2hkq.pop();
			expect(queuedata.length).to.equal(2);
			expect(queuedata[0]).to.equal('key_020_1');
			expect(queuedata[1]).to.equal('data_020_1');
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offPush();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_020_1', 'data_020_1')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Push(FIFO pass) - onPush Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Push(FIFO pass) - onPush Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_021_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onPush callback
		expect(k2hkq.onPush(function(error)
		{
			expect(error).to.be.null;

			// check queue
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(k2hkq.count()).to.equal(1);
			var	queuedata = k2hkq.pop('mypass');
			expect(queuedata.length).to.equal(2);
			expect(queuedata[0]).to.equal('key_021_1');
			expect(queuedata[1]).to.equal('data_021_1');
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offPush();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_021_1', 'data_021_1', 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Push(FIFO nopass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Push(FIFO nopass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_030_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_030_1', 'data_030_1', function(error)
		{
			expect(error).to.be.null;

			// check queue
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(k2hkq.count()).to.equal(1);
			var	queuedata = k2hkq.pop();
			expect(queuedata.length).to.equal(2);
			expect(queuedata[0]).to.equal('key_030_1');
			expect(queuedata[1]).to.equal('data_030_1');
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Push(FIFO pass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Push(FIFO pass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_031_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_031_1', 'data_031_1', 'mypass', function(error)
		{
			expect(error).to.be.null;

			// check queue
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(k2hkq.count()).to.equal(1);
			var	queuedata = k2hkq.pop('mypass');
			expect(queuedata.length).to.equal(2);
			expect(queuedata[0]).to.equal('key_031_1');
			expect(queuedata[1]).to.equal('data_031_1');
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offPush();

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hkeyqueue::Pop(FIFO)
	//-------------------------------------------------------------------
	//
	// k2hkeyqueue::Pop(FIFO nopass) - No Callback : this case is already tested at push.
	// k2hkeyqueue::Pop(FIFO pass) - No Callback   : this case is already tested at push.
	//

	//
	// k2hkeyqueue::Pop(FIFO nopass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Pop(FIFO nopass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_110_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(pop) callback
		expect(k2hkq.on('pop', function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;
			expect(key).to.equal('key_110_1');
			expect(value).to.equal('data_110_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('pop');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_110_1', 'data_110_1')).to.be.a('boolean').to.be.true;

		// pop queue(no pass)
		expect(k2hkq.pop()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Pop(FIFO pass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Pop(FIFO pass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_111_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(pop) callback
		expect(k2hkq.on('pop', function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;
			expect(key).to.equal('key_111_1');
			expect(value).to.equal('data_111_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('pop');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_111_1', 'data_111_1', 'mypass')).to.be.a('boolean').to.be.true;

		// pop queue(pass)
		expect(k2hkq.pop('mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Pop(FIFO nopass) - onPop Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Pop(FIFO nopass) - onPop Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_120_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onPop callback
		expect(k2hkq.onPop(function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;
			expect(key).to.equal('key_120_1');
			expect(value).to.equal('data_120_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offPop();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_120_1', 'data_120_1')).to.be.a('boolean').to.be.true;

		// pop queue(no pass)
		expect(k2hkq.pop()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Pop(FIFO pass) - onPop Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Pop(FIFO pass) - onPop Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_121_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onPop callback
		expect(k2hkq.onPop(function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;
			expect(key).to.equal('key_121_1');
			expect(value).to.equal('data_121_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offPop();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_121_1', 'data_121_1', 'mypass')).to.be.a('boolean').to.be.true;

		// pop queue(pass)
		expect(k2hkq.pop('mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Pop(FIFO nopass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Pop(FIFO nopass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_130_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_130_1', 'data_130_1')).to.be.a('boolean').to.be.true;

		// pop queue(no pass)
		expect(k2hkq.pop(function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;
			expect(key).to.equal('key_130_1');
			expect(value).to.equal('data_130_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Pop(FIFO pass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Pop(FIFO pass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_131_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_131_1', 'data_131_1', 'mypass')).to.be.a('boolean').to.be.true;

		// pop queue(pass)
		expect(k2hkq.pop('mypass', function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;
			expect(key).to.equal('key_131_1');
			expect(value).to.equal('data_131_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hkeyqueue::Read(FIFO)
	//-------------------------------------------------------------------
	//
	// k2hkeyqueue::Read(FIFO nopass) - No Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Read(FIFO nopass) - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_200_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		var	values	= {'key_200_1' : 'data_200_1', 'key_200_2' : 'data_200_2' };
		var	qcount	= 0;
		for(var key in values){
			expect(k2hkq.push(key, values[key])).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hkq.count()).to.equal(qcount);
		}
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		var	queuedata = k2hkq.read();
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_200_1');
		expect(queuedata[1]).to.equal('data_200_1');

		queuedata = k2hkq.read('dummypass');																												// This case, pop returns data.
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_200_1');
		expect(queuedata[1]).to.equal('data_200_1');

		queuedata = k2hkq.read(1);
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_200_2');
		expect(queuedata[1]).to.equal('data_200_2');

		expect(k2hkq.count()).to.equal(2);
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hkeyqueue::Read(FIFO pass) - No Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Read(FIFO pass) - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_201_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		var	values	= {'key_201_1' : 'data_201_1', 'key_201_2' : 'data_201_2' };
		var	qcount	= 0;
		for(var key in values){
			expect(k2hkq.push(key, values[key], 'mypass')).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hkq.count()).to.equal(qcount);
		}
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		var	queuedata = k2hkq.read('mypass');
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_201_1');
		expect(queuedata[1]).to.equal('data_201_1');

		queuedata = k2hkq.read();
		expect(queuedata).to.be.null;

		queuedata = k2hkq.read(1, 'mypass');
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_201_2');
		expect(queuedata[1]).to.equal('data_201_2');

		expect(k2hkq.count()).to.equal(2);
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hkeyqueue::Read(FIFO nopass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Read(FIFO nopass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_210_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(read) callback
		expect(k2hkq.on('read', function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(key).to.equal('key_210_1');
			expect(value).to.equal('data_210_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('read');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_210_1', 'data_210_1')).to.be.a('boolean').to.be.true;

		// read queue(no pass)
		expect(k2hkq.read()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Read(FIFO pass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Read(FIFO pass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_211_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(read) callback
		expect(k2hkq.on('read', function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(key).to.equal('key_211_1');
			expect(value).to.equal('data_211_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('read');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_211_1', 'data_211_1', 'mypass')).to.be.a('boolean').to.be.true;

		// read queue(pass)
		expect(k2hkq.read('mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Read(FIFO nopass) - onRead Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Read(FIFO nopass) - onRead Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_220_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onRead callback
		expect(k2hkq.onRead(function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(key).to.equal('key_220_1');
			expect(value).to.equal('data_220_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offRead();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_220_1', 'data_220_1')).to.be.a('boolean').to.be.true;

		// read queue(no pass)
		expect(k2hkq.read()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Read(FIFO pass) - onRead Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Read(FIFO pass) - onRead Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_221_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onRead callback
		expect(k2hkq.onRead(function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(key).to.equal('key_221_1');
			expect(value).to.equal('data_221_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offRead();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_221_1', 'data_221_1', 'mypass')).to.be.a('boolean').to.be.true;

		// read queue(pass)
		expect(k2hkq.read('mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Read(FIFO nopass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Read(FIFO nopass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_230_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_230_1', 'data_230_1')).to.be.a('boolean').to.be.true;

		// read queue(no pass)
		expect(k2hkq.read(function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(key).to.equal('key_230_1');
			expect(value).to.equal('data_230_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Read(FIFO pass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Read(FIFO pass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_231_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_231_1', 'data_231_1', 'mypass')).to.be.a('boolean').to.be.true;

		// read queue(pass)
		expect(k2hkq.read('mypass', function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(key).to.equal('key_231_1');
			expect(value).to.equal('data_231_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hkeyqueue::Remove(FIFO)
	//-------------------------------------------------------------------
	//
	// k2hkeyqueue::Remove(FIFO nopass) - No Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Remove(FIFO nopass) - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_300_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		var	values	= {'key_300_1' : 'data_300_1', 'key_300_2' : 'data_300_2', 'key_300_3' : 'data_300_3' };
		var	qcount	= 0;
		for(var key in values){
			expect(k2hkq.push(key, values[key])).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hkq.count()).to.equal(qcount);
		}
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hkq.remove(1)).to.equal(1);
		expect(k2hkq.count()).to.equal(2);
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
		expect(k2hkq.remove(2)).to.equal(2);
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hkeyqueue::Remove(FIFO pass) - No Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Remove(FIFO pass) - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_301_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		var	values	= {'key_301_1' : 'data_301_1', 'key_301_2' : 'data_301_2', 'key_301_3' : 'data_301_3' };
		var	qcount	= 0;
		for(var key in values){
			expect(k2hkq.push(key, values[key], 'mypass')).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hkq.count()).to.equal(qcount);
		}
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hkq.remove(1, 'mypass')).to.equal(1);
		expect(k2hkq.count()).to.equal(2);
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
		expect(k2hkq.remove(2, 'mypass')).to.equal(2);
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hkeyqueue::Remove(FIFO nopass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Remove(FIFO nopass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_310_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2hkq.on('remove', function(error, remove_count)
		{
			expect(error).to.be.null;

			// check data
			expect(remove_count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('remove');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_310_1', 'data_310_1')).to.be.a('boolean').to.be.true;

		// remove queue(no pass)
		expect(k2hkq.remove(1)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Remove(FIFO pass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Remove(FIFO pass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_311_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2hkq.on('remove', function(error, remove_count)
		{
			expect(error).to.be.null;

			// check data
			expect(remove_count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('remove');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_311_1', 'data_311_1', 'mypass')).to.be.a('boolean').to.be.true;

		// remove queue(pass)
		expect(k2hkq.remove(1, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Remove(FIFO nopass) - onRemove Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Remove(FIFO nopass) - onRemove Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_320_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onRemove callback
		expect(k2hkq.onRemove(function(error, remove_count)
		{
			expect(error).to.be.null;

			// check data
			expect(remove_count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offRemove();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_320_1', 'data_320_1')).to.be.a('boolean').to.be.true;

		// remove queue(no pass)
		expect(k2hkq.remove(1)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Remove(FIFO pass) - onRemove Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Remove(FIFO pass) - onRemove Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_321_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onRemove callback
		expect(k2hkq.onRemove(function(error, remove_count)
		{
			expect(error).to.be.null;

			// check data
			expect(remove_count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offRemove();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_321_1', 'data_321_1', 'mypass')).to.be.a('boolean').to.be.true;

		// remove queue(pass)
		expect(k2hkq.remove(1, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Remove(FIFO nopass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Remove(FIFO nopass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_330_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_330_1', 'data_330_1')).to.be.a('boolean').to.be.true;

		// remove queue(no pass)
		expect(k2hkq.remove(1, function(error, remove_count)
		{
			expect(error).to.be.null;

			// check data
			expect(remove_count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Remove(FIFO pass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Remove(FIFO pass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_331_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_331_1', 'data_331_1', 'mypass')).to.be.a('boolean').to.be.true;

		// remove queue(pass)
		expect(k2hkq.remove(1, 'mypass', function(error, remove_count)
		{
			expect(error).to.be.null;

			// check data
			expect(remove_count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hkeyqueue::Count(FIFO)
	//-------------------------------------------------------------------
	//
	// k2hkeyqueue::Count(FIFO nopass) - No Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Count(FIFO nopass) - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_400_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		var	values	= {'key_400_1' : 'data_400_1', 'key_400_2' : 'data_400_2', 'key_400_3' : 'data_400_3' };
		var	qcount	= 0;
		for(var key in values){
			expect(k2hkq.push(key, values[key])).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hkq.count()).to.equal(qcount);
		}
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hkq.count()).to.equal(3);

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hkeyqueue::Count(FIFO pass) - No Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Count(FIFO pass) - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_401_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		var	values	= {'key_401_1' : 'data_401_1', 'key_401_2' : 'data_401_2', 'key_401_3' : 'data_401_3' };
		var	qcount	= 0;
		for(var key in values){
			expect(k2hkq.push(key, values[key], 'mypass')).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hkq.count()).to.equal(qcount);
		}
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hkq.count()).to.equal(3);

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hkeyqueue::Count(FIFO nopass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Count(FIFO nopass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_410_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(count) callback
		expect(k2hkq.on('count', function(error, count)
		{
			expect(error).to.be.null;

			// check data
			expect(count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('count');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_410_1', 'data_410_1')).to.be.a('boolean').to.be.true;

		// count queue(no pass)
		expect(k2hkq.count()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Count(FIFO pass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Count(FIFO pass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_411_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(count) callback
		expect(k2hkq.on('count', function(error, count)
		{
			expect(error).to.be.null;

			// check data
			expect(count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('count');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_411_1', 'data_411_1', 'mypass')).to.be.a('boolean').to.be.true;

		// count queue(pass)
		expect(k2hkq.count()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Count(FIFO nopass) - onCount Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Count(FIFO nopass) - onCount Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_420_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onCount Callback
		expect(k2hkq.onCount(function(error, count)
		{
			expect(error).to.be.null;

			// check data
			expect(count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offCount();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_420_1', 'data_420_1')).to.be.a('boolean').to.be.true;

		// count queue(no pass)
		expect(k2hkq.count()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Count(FIFO pass) - onCount Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Count(FIFO pass) - onCount Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_421_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onCount Callback
		expect(k2hkq.onCount(function(error, count)
		{
			expect(error).to.be.null;

			// check data
			expect(count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offCount();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_421_1', 'data_421_1', 'mypass')).to.be.a('boolean').to.be.true;

		// count queue(pass)
		expect(k2hkq.count()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Count(FIFO nopass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Count(FIFO nopass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_430_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_430_1', 'data_430_1')).to.be.a('boolean').to.be.true;

		// count queue(no pass)
		expect(k2hkq.count(function(error, count)
		{
			expect(error).to.be.null;

			// check data
			expect(count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Count(FIFO pass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Count(FIFO pass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_431_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_431_1', 'data_431_1', 'mypass')).to.be.a('boolean').to.be.true;

		// count queue(pass)
		expect(k2hkq.count(function(error, count)
		{
			expect(error).to.be.null;

			// check data
			expect(count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hkeyqueue::Dump(FIFO)
	//-------------------------------------------------------------------
	//
	// k2hkeyqueue::Dump(FIFO)
	//
	it('k2hkeyqueue - k2hkeyqueue::Dump(FIFO)', function(done){						// eslint-disable-line no-undef
		var	fs		= require('fs');
		var	k2hfile	= '/tmp/test04_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, true, '_queue_fifo_530_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue
		expect(k2hkq.push('key_530_1', 'data_530_1')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// dump
		var	fd = fs.openSync('/tmp/result_queue_04_tmp.log', 'w');
		expect(k2hkq.dump(fd)).to.be.a('boolean').to.be.true;
		fs.closeSync(fd);

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//-------------------------------------------------------------------
	// Test k2hkeyqueue::Push(LIFO)
	//-------------------------------------------------------------------
	//
	// k2hkeyqueue::Push(LIFO nopass) - No Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Push(LIFO nopass) - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_000_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		var	values	= {'key_000_1' : 'data_000_1', 'key_000_2' : 'data_000_2', 'key_000_3' : 'data_000_3' };
		var	qcount	= 0;
		for(var key in values){
			expect(k2hkq.push(key, values[key])).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hkq.count()).to.equal(qcount);
		}
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		var	queuedata = k2hkq.pop();
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_000_3');
		expect(queuedata[1]).to.equal('data_000_3');

		queuedata = k2hkq.pop('dummypass');																	// This case, pop returns data.
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_000_2');
		expect(queuedata[1]).to.equal('data_000_2');
		expect(k2hkq.count()).to.equal(1);

		queuedata = k2hkq.pop();
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_000_1');
		expect(queuedata[1]).to.equal('data_000_1');
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hkeyqueue::Push(LIFO pass) - No Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Push(LIFO pass) - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_001_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		var	values	= {'key_001_1' : 'data_001_1', 'key_001_2' : 'data_001_2', 'key_001_3' : 'data_001_3' };
		var	qcount	= 0;
		for(var key in values){
			expect(k2hkq.push(key, values[key], 'mypass')).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hkq.count()).to.equal(qcount);
		}
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		var	queuedata = k2hkq.pop('mypass');
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_001_3');
		expect(queuedata[1]).to.equal('data_001_3');

		queuedata = k2hkq.pop();
		expect(queuedata).to.be.null;
		expect(k2hkq.count()).to.equal(1);

		queuedata = k2hkq.pop('mypass');
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_001_1');
		expect(queuedata[1]).to.equal('data_001_1');
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hkeyqueue::Push(LIFO nopass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Push(LIFO nopass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_010_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(push) callback
		expect(k2hkq.on('push', function(error)
		{
			expect(error).to.be.null;

			// check queue
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(k2hkq.count()).to.equal(1);
			var	queuedata = k2hkq.pop();
			expect(queuedata.length).to.equal(2);
			expect(queuedata[0]).to.equal('key_010_1');
			expect(queuedata[1]).to.equal('data_010_1');
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('push');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_010_1', 'data_010_1')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Push(LIFO pass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Push(LIFO pass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_011_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(push) callback
		expect(k2hkq.on('push', function(error)
		{
			expect(error).to.be.null;

			// check queue
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(k2hkq.count()).to.equal(1);
			var	queuedata = k2hkq.pop('mypass');
			expect(queuedata.length).to.equal(2);
			expect(queuedata[0]).to.equal('key_011_1');
			expect(queuedata[1]).to.equal('data_011_1');
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('push');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_011_1', 'data_011_1', 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Push(LIFO nopass) - onPush Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Push(LIFO nopass) - onPush Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_020_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onPush callback
		expect(k2hkq.onPush(function(error)
		{
			expect(error).to.be.null;

			// check queue
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(k2hkq.count()).to.equal(1);
			var	queuedata = k2hkq.pop();
			expect(queuedata.length).to.equal(2);
			expect(queuedata[0]).to.equal('key_020_1');
			expect(queuedata[1]).to.equal('data_020_1');
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offPush();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_020_1', 'data_020_1')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Push(LIFO pass) - onPush Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Push(LIFO pass) - onPush Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_021_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onPush callback
		expect(k2hkq.onPush(function(error)
		{
			expect(error).to.be.null;

			// check queue
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(k2hkq.count()).to.equal(1);
			var	queuedata = k2hkq.pop('mypass');
			expect(queuedata.length).to.equal(2);
			expect(queuedata[0]).to.equal('key_021_1');
			expect(queuedata[1]).to.equal('data_021_1');
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offPush();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_021_1', 'data_021_1', 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Push(LIFO nopass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Push(LIFO nopass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_030_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_030_1', 'data_030_1', function(error)
		{
			expect(error).to.be.null;

			// check queue
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(k2hkq.count()).to.equal(1);
			var	queuedata = k2hkq.pop();
			expect(queuedata.length).to.equal(2);
			expect(queuedata[0]).to.equal('key_030_1');
			expect(queuedata[1]).to.equal('data_030_1');
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Push(LIFO pass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Push(LIFO pass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_031_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_031_1', 'data_031_1', 'mypass', function(error)
		{
			expect(error).to.be.null;

			// check queue
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(k2hkq.count()).to.equal(1);
			var	queuedata = k2hkq.pop('mypass');
			expect(queuedata.length).to.equal(2);
			expect(queuedata[0]).to.equal('key_031_1');
			expect(queuedata[1]).to.equal('data_031_1');
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offPush();

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hkeyqueue::Pop(LIFO)
	//-------------------------------------------------------------------
	//
	// k2hkeyqueue::Pop(LIFO nopass) - No Callback : this case is already tested at push.
	// k2hkeyqueue::Pop(LIFO pass) - No Callback   : this case is already tested at push.
	//

	//
	// k2hkeyqueue::Pop(LIFO nopass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Pop(LIFO nopass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_110_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(pop) callback
		expect(k2hkq.on('pop', function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;
			expect(key).to.equal('key_110_1');
			expect(value).to.equal('data_110_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('pop');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_110_1', 'data_110_1')).to.be.a('boolean').to.be.true;

		// pop queue(no pass)
		expect(k2hkq.pop()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Pop(LIFO pass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Pop(LIFO pass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_111_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(pop) callback
		expect(k2hkq.on('pop', function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;
			expect(key).to.equal('key_111_1');
			expect(value).to.equal('data_111_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('pop');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_111_1', 'data_111_1', 'mypass')).to.be.a('boolean').to.be.true;

		// pop queue(pass)
		expect(k2hkq.pop('mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Pop(LIFO nopass) - onPop Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Pop(LIFO nopass) - onPop Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_120_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onPop callback
		expect(k2hkq.onPop(function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;
			expect(key).to.equal('key_120_1');
			expect(value).to.equal('data_120_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offPop();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_120_1', 'data_120_1')).to.be.a('boolean').to.be.true;

		// pop queue(no pass)
		expect(k2hkq.pop()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Pop(LIFO pass) - onPop Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Pop(LIFO pass) - onPop Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_121_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onPop callback
		expect(k2hkq.onPop(function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;
			expect(key).to.equal('key_121_1');
			expect(value).to.equal('data_121_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offPop();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_121_1', 'data_121_1', 'mypass')).to.be.a('boolean').to.be.true;

		// pop queue(pass)
		expect(k2hkq.pop('mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Pop(LIFO nopass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Pop(LIFO nopass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_130_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_130_1', 'data_130_1')).to.be.a('boolean').to.be.true;

		// pop queue(no pass)
		expect(k2hkq.pop(function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;
			expect(key).to.equal('key_130_1');
			expect(value).to.equal('data_130_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Pop(LIFO pass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Pop(LIFO pass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_131_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_131_1', 'data_131_1', 'mypass')).to.be.a('boolean').to.be.true;

		// pop queue(pass)
		expect(k2hkq.pop('mypass', function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;
			expect(key).to.equal('key_131_1');
			expect(value).to.equal('data_131_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hkeyqueue::Read(LIFO)
	//-------------------------------------------------------------------
	//
	// k2hkeyqueue::Read(LIFO nopass) - No Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Read(LIFO nopass) - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_200_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		var	values	= {'key_200_1' : 'data_200_1', 'key_200_2' : 'data_200_2' };
		var	qcount	= 0;
		for(var key in values){
			expect(k2hkq.push(key, values[key])).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hkq.count()).to.equal(qcount);
		}
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		var	queuedata = k2hkq.read();
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_200_2');
		expect(queuedata[1]).to.equal('data_200_2');

		queuedata = k2hkq.read('dummypass');																												// This case, pop returns data.
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_200_2');
		expect(queuedata[1]).to.equal('data_200_2');

		queuedata = k2hkq.read(1);
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_200_1');
		expect(queuedata[1]).to.equal('data_200_1');

		expect(k2hkq.count()).to.equal(2);
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hkeyqueue::Read(LIFO pass) - No Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Read(LIFO pass) - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_201_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		var	values	= {'key_201_1' : 'data_201_1', 'key_201_2' : 'data_201_2' };
		var	qcount	= 0;
		for(var key in values){
			expect(k2hkq.push(key, values[key], 'mypass')).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hkq.count()).to.equal(qcount);
		}
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		var	queuedata = k2hkq.read('mypass');
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_201_2');
		expect(queuedata[1]).to.equal('data_201_2');

		queuedata = k2hkq.read();
		expect(queuedata).to.be.null;

		queuedata = k2hkq.read(1, 'mypass');
		expect(queuedata.length).to.equal(2);
		expect(queuedata[0]).to.equal('key_201_1');
		expect(queuedata[1]).to.equal('data_201_1');

		expect(k2hkq.count()).to.equal(2);
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hkeyqueue::Read(LIFO nopass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Read(LIFO nopass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_210_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(read) callback
		expect(k2hkq.on('read', function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(key).to.equal('key_210_1');
			expect(value).to.equal('data_210_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('read');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_210_1', 'data_210_1')).to.be.a('boolean').to.be.true;

		// read queue(no pass)
		expect(k2hkq.read()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Read(LIFO pass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Read(LIFO pass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_211_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(read) callback
		expect(k2hkq.on('read', function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(key).to.equal('key_211_1');
			expect(value).to.equal('data_211_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('read');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_211_1', 'data_211_1', 'mypass')).to.be.a('boolean').to.be.true;

		// read queue(pass)
		expect(k2hkq.read('mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Read(LIFO nopass) - onRead Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Read(LIFO nopass) - onRead Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_220_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onRead callback
		expect(k2hkq.onRead(function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(key).to.equal('key_220_1');
			expect(value).to.equal('data_220_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offRead();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_220_1', 'data_220_1')).to.be.a('boolean').to.be.true;

		// read queue(no pass)
		expect(k2hkq.read()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Read(LIFO pass) - onRead Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Read(LIFO pass) - onRead Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_221_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onRead callback
		expect(k2hkq.onRead(function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(key).to.equal('key_221_1');
			expect(value).to.equal('data_221_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offRead();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_221_1', 'data_221_1', 'mypass')).to.be.a('boolean').to.be.true;

		// read queue(pass)
		expect(k2hkq.read('mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Read(LIFO nopass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Read(LIFO nopass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_230_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_230_1', 'data_230_1')).to.be.a('boolean').to.be.true;

		// read queue(no pass)
		expect(k2hkq.read(function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(key).to.equal('key_230_1');
			expect(value).to.equal('data_230_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Read(LIFO pass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Read(LIFO pass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_231_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_231_1', 'data_231_1', 'mypass')).to.be.a('boolean').to.be.true;

		// read queue(pass)
		expect(k2hkq.read('mypass', function(error, key, value)
		{
			expect(error).to.be.null;

			// check data
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
			expect(key).to.equal('key_231_1');
			expect(value).to.equal('data_231_1');

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hkeyqueue::Remove(LIFO)
	//-------------------------------------------------------------------
	//
	// k2hkeyqueue::Remove(LIFO nopass) - No Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Remove(LIFO nopass) - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_300_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		var	values	= {'key_300_1' : 'data_300_1', 'key_300_2' : 'data_300_2', 'key_300_3' : 'data_300_3' };
		var	qcount	= 0;
		for(var key in values){
			expect(k2hkq.push(key, values[key])).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hkq.count()).to.equal(qcount);
		}
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hkq.remove(1)).to.equal(1);
		expect(k2hkq.count()).to.equal(2);
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
		expect(k2hkq.remove(2)).to.equal(2);
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hkeyqueue::Remove(LIFO pass) - No Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Remove(LIFO pass) - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_301_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		var	values	= {'key_301_1' : 'data_301_1', 'key_301_2' : 'data_301_2', 'key_301_3' : 'data_301_3' };
		var	qcount	= 0;
		for(var key in values){
			expect(k2hkq.push(key, values[key], 'mypass')).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hkq.count()).to.equal(qcount);
		}
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hkq.remove(1, 'mypass')).to.equal(1);
		expect(k2hkq.count()).to.equal(2);
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;
		expect(k2hkq.remove(2, 'mypass')).to.equal(2);
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hkeyqueue::Remove(LIFO nopass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Remove(LIFO nopass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_310_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2hkq.on('remove', function(error, remove_count)
		{
			expect(error).to.be.null;

			// check data
			expect(remove_count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('remove');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_310_1', 'data_310_1')).to.be.a('boolean').to.be.true;

		// remove queue(no pass)
		expect(k2hkq.remove(1)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Remove(LIFO pass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Remove(LIFO pass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_311_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2hkq.on('remove', function(error, remove_count)
		{
			expect(error).to.be.null;

			// check data
			expect(remove_count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('remove');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_311_1', 'data_311_1', 'mypass')).to.be.a('boolean').to.be.true;

		// remove queue(pass)
		expect(k2hkq.remove(1, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Remove(LIFO nopass) - onRemove Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Remove(LIFO nopass) - onRemove Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_320_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onRemove callback
		expect(k2hkq.onRemove(function(error, remove_count)
		{
			expect(error).to.be.null;

			// check data
			expect(remove_count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offRemove();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_320_1', 'data_320_1')).to.be.a('boolean').to.be.true;

		// remove queue(no pass)
		expect(k2hkq.remove(1)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Remove(LIFO pass) - onRemove Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Remove(LIFO pass) - onRemove Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_321_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onRemove callback
		expect(k2hkq.onRemove(function(error, remove_count)
		{
			expect(error).to.be.null;

			// check data
			expect(remove_count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offRemove();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_321_1', 'data_321_1', 'mypass')).to.be.a('boolean').to.be.true;

		// remove queue(pass)
		expect(k2hkq.remove(1, 'mypass')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Remove(LIFO nopass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Remove(LIFO nopass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_330_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_330_1', 'data_330_1')).to.be.a('boolean').to.be.true;

		// remove queue(no pass)
		expect(k2hkq.remove(1, function(error, remove_count)
		{
			expect(error).to.be.null;

			// check data
			expect(remove_count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Remove(LIFO pass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Remove(LIFO pass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_331_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_331_1', 'data_331_1', 'mypass')).to.be.a('boolean').to.be.true;

		// remove queue(pass)
		expect(k2hkq.remove(1, 'mypass', function(error, remove_count)
		{
			expect(error).to.be.null;

			// check data
			expect(remove_count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hkeyqueue::Count(LIFO)
	//-------------------------------------------------------------------
	//
	// k2hkeyqueue::Count(LIFO nopass) - No Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Count(LIFO nopass) - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_400_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		var	values	= {'key_400_1' : 'data_400_1', 'key_400_2' : 'data_400_2', 'key_400_3' : 'data_400_3' };
		var	qcount	= 0;
		for(var key in values){
			expect(k2hkq.push(key, values[key])).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hkq.count()).to.equal(qcount);
		}
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hkq.count()).to.equal(3);

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hkeyqueue::Count(LIFO pass) - No Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Count(LIFO pass) - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_401_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		var	values	= {'key_401_1' : 'data_401_1', 'key_401_2' : 'data_401_2', 'key_401_3' : 'data_401_3' };
		var	qcount	= 0;
		for(var key in values){
			expect(k2hkq.push(key, values[key], 'mypass')).to.be.a('boolean').to.be.true;
			++qcount;
			expect(k2hkq.count()).to.equal(qcount);
		}
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// check queue
		expect(k2hkq.count()).to.equal(3);

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hkeyqueue::Count(LIFO nopass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Count(LIFO nopass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_410_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(count) callback
		expect(k2hkq.on('count', function(error, count)
		{
			expect(error).to.be.null;

			// check data
			expect(count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('count');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_410_1', 'data_410_1')).to.be.a('boolean').to.be.true;

		// count queue(no pass)
		expect(k2hkq.count()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Count(LIFO pass) - on Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Count(LIFO pass) - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_411_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set on(count) callback
		expect(k2hkq.on('count', function(error, count)
		{
			expect(error).to.be.null;

			// check data
			expect(count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.off('count');

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_411_1', 'data_411_1', 'mypass')).to.be.a('boolean').to.be.true;

		// count queue(pass)
		expect(k2hkq.count()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Count(LIFO nopass) - onCount Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Count(LIFO nopass) - onCount Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_420_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onCount Callback
		expect(k2hkq.onCount(function(error, count)
		{
			expect(error).to.be.null;

			// check data
			expect(count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offCount();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_420_1', 'data_420_1')).to.be.a('boolean').to.be.true;

		// count queue(no pass)
		expect(k2hkq.count()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Count(LIFO pass) - onCount Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Count(LIFO pass) - onCount Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_421_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// set onCount Callback
		expect(k2hkq.onCount(function(error, count)
		{
			expect(error).to.be.null;

			// check data
			expect(count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2hkq.offCount();

			done();
		})).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_421_1', 'data_421_1', 'mypass')).to.be.a('boolean').to.be.true;

		// count queue(pass)
		expect(k2hkq.count()).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Count(LIFO nopass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Count(LIFO nopass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_430_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(no pass)
		expect(k2hkq.push('key_430_1', 'data_430_1')).to.be.a('boolean').to.be.true;

		// count queue(no pass)
		expect(k2hkq.count(function(error, count)
		{
			expect(error).to.be.null;

			// check data
			expect(count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hkeyqueue::Count(LIFO pass) - inline Callback
	//
	it('k2hkeyqueue - k2hkeyqueue::Count(LIFO pass) - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_431_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue(pass)
		expect(k2hkq.push('key_431_1', 'data_431_1', 'mypass')).to.be.a('boolean').to.be.true;

		// count queue(pass)
		expect(k2hkq.count(function(error, count)
		{
			expect(error).to.be.null;

			// check data
			expect(count).to.equal(1);
			expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test k2hkeyqueue::Dump(LIFO)
	//-------------------------------------------------------------------
	//
	// k2hkeyqueue::Dump(LIFO)
	//
	it('k2hkeyqueue - k2hkeyqueue::Dump(LIFO)', function(done){						// eslint-disable-line no-undef
		var	fs		= require('fs');
		var	k2hfile	= '/tmp/test05_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get queue
		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		// init queue & check
		expect(k2hkq.init(k2h, false, '_queue_lifo_530_')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.true;

		// push queue
		expect(k2hkq.push('key_530_1', 'data_530_1')).to.be.a('boolean').to.be.true;
		expect(k2hkq.isEmpty()).to.be.a('boolean').to.be.false;

		// dump
		var	fd = fs.openSync('/tmp/result_queue_05_tmp.log', 'w');
		expect(k2hkq.dump(fd)).to.be.a('boolean').to.be.true;
		fs.closeSync(fd);

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});
});

/*
 * VIM modelines
 *
 * vim:set ts=4 fenc=utf-8:
 */
