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

//-------------------------------------------------------------------
// Common function
//
// [NOTE]
// This sleep is high load average, but this script is only test.
//
function sleep(milliseconds)
{
	var start = new Date().getTime();
	while(true){
		if ((new Date().getTime() - start) > milliseconds){
			break;
		}
	}
}

//--------------------------------------------------------------
// Main describe section
//--------------------------------------------------------------
describe('K2HKEYQUEUE', function(){						// eslint-disable-line no-undef
	//
	// Before in describe section
	//
	before(function(done){								// eslint-disable-line no-undef
		// Nothing to do
		done();
	});

	//
	// After in describe section
	//
	after(function(){									// eslint-disable-line no-undef
		// Nothing to do
	});

	//-------------------------------------------------------------------
	// Test create/close
	//-------------------------------------------------------------------
	//
	// k2hash::create(), close() - No Callback
	//
	it('k2hash - k2hash::create(), k2hash::close() - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile0	= '/tmp/test01_tmp.k2h';			// For all test
		var	k2hfile1	= '/tmp/test01_tmp_open.k2h';		// For open
		var	k2hfile2	= '/tmp/test01_tmp_orw.k2h';		// For openRW
		var	k2hfile3	= '/tmp/test01_tmp_oro.k2h';		// For openRO
		var	k2hfile4	= '/tmp/test01_tmp_otmp1.k2h';		// For openTempfile
		var	k2hfile5	= '/tmp/test01_tmp_otmp2.k2h';		// For openTempfile
		var	k2hfile6	= '/tmp/test01_tmp_otmp3.k2h';		// For openTempfile
		var	k2hfile7	= '/tmp/test01_tmp_otmp4.k2h';		// For openTempfile
		var	k2hfile8	= '/tmp/test01_tmp_ar1.k2h';		// For Archive
		var	k2hfile9	= '/tmp/test01_tmp_queue1.k2h';		// For getQueue
		var	k2hfile10	= '/tmp/test01_tmp_queue2.k2h';		// For getKeyQueue
		var	k2h			= k2hash();

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
	it('k2hash - k2hash::create(), k2hash::close() - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_c01.k2h';
		var	k2h		= k2hash();

		// set on(close) callback
		expect(k2h.on('close', function(error)
		{
			expect(error).to.be.null;

			// unset
			k2h.off('close');

			done();
		})).to.be.a('boolean').to.be.true;

		// set on(create) callback
		expect(k2h.on('create', function(error)
		{
			expect(error).to.be.null;

			// do close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2h.off('create');
		})).to.be.a('boolean').to.be.true;

		// do create
		expect(k2h.create(k2hfile, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hash::create(), close() - onCreate/onClose Callback
	//
	it('k2hash - k2hash::create(), k2hash::close() - onCreate/onClose Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_c02.k2h';
		var	k2h		= k2hash();

		// set on(close) callback
		expect(k2h.onClose(function(error)
		{
			expect(error).to.be.null;

			// unset
			k2h.offClose();

			done();
		})).to.be.a('boolean').to.be.true;

		// set on(create) callback
		expect(k2h.onCreate(function(error)
		{
			expect(error).to.be.null;

			// do close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2h.offCreate();
		})).to.be.a('boolean').to.be.true;

		// do create
		expect(k2h.create(k2hfile, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hash::create(), close() - inline Callback
	//
	it('k2hash - k2hash::create(), k2hash::close() - inline Callback', function(done){			// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_c03.k2h';
		var	k2h		= k2hash();

		// create and callback
		expect(k2h.create(k2hfile, 2, 4, 4, 128, function(error)
		{
			expect(error).to.be.null;

			// close and callback
			expect(k2h.close(function(error)
			{
				expect(error).to.be.null;

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
	it('k2hash - k2hnode::open(), close() - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_open.k2h';
		var	k2h		= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::open(), close() - on Callback
	//
	it('k2hash - k2hnode::open(), close() - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_open.k2h';
		var	k2h		= k2hash();

		// set on(close) callback
		expect(k2h.on('close', function(error)
		{
			expect(error).to.be.null;

			// unset
			k2h.off('close');

			done();
		})).to.be.a('boolean').to.be.true;

		// set on(open) callback
		expect(k2h.on('open', function(error)
		{
			expect(error).to.be.null;

			// do close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2h.off('open');
		})).to.be.a('boolean').to.be.true;

		// do open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::open(), close() - onOpen/onClose Callback
	//
	it('k2hash - k2hnode::open(), close() - onOpen/onClose Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_open.k2h';
		var	k2h		= k2hash();

		// set onClose() callback
		expect(k2h.onClose(function(error)
		{
			expect(error).to.be.null;

			// unset
			k2h.offClose();

			done();
		})).to.be.a('boolean').to.be.true;

		// set onOpen() callback
		expect(k2h.onOpen(function(error)
		{
			expect(error).to.be.null;

			// do close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2h.offOpen();
		})).to.be.a('boolean').to.be.true;

		// do open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::open(), close() - inline Callback
	//
	it('k2hash - k2hnode::open(), close() - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_open.k2h';
		var	k2h		= k2hash();

		// do open() callback
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128, function(error)
		{
			expect(error).to.be.null;

			// do close() callback
			expect(k2h.close(function(error)
			{
				expect(error).to.be.null;

				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openRW(), close() - No Callback
	//
	it('k2hash - k2hnode::openRW(), close() - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_orw.k2h';
		var	k2h		= k2hash();

		expect(k2h.openRW(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::openRW(), close() - on Callback
	//
	it('k2hash - k2hnode::openRW(), close() - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_orw.k2h';
		var	k2h		= k2hash();

		// set on(close) callback
		expect(k2h.on('close', function(error)
		{
			expect(error).to.be.null;

			// unset
			k2h.off('close');

			done();
		})).to.be.a('boolean').to.be.true;

		// set on(openRW) callback
		expect(k2h.on('openRW', function(error)
		{
			expect(error).to.be.null;

			// do close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2h.off('openRW');
		})).to.be.a('boolean').to.be.true;

		// do openRW
		expect(k2h.openRW(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openRW(), close() - onOpenRW/onClose Callback
	//
	it('k2hash - k2hnode::openRW(), close() - onOpenRW/onClose Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_orw.k2h';
		var	k2h		= k2hash();

		// set onClose() callback
		expect(k2h.onClose(function(error)
		{
			expect(error).to.be.null;

			// unset
			k2h.offClose();

			done();
		})).to.be.a('boolean').to.be.true;

		// set onOpenRW() callback
		expect(k2h.onOpenRW(function(error)
		{
			expect(error).to.be.null;

			// do close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2h.offOpenRW();
		})).to.be.a('boolean').to.be.true;

		// do openRW
		expect(k2h.openRW(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openRW(), close() - inline Callback
	//
	it('k2hash - k2hnode::openRW(), close() - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_orw.k2h';
		var	k2h		= k2hash();

		// do openRW() callback
		expect(k2h.openRW(k2hfile, true, 2, 4, 4, 128, function(error)
		{
			expect(error).to.be.null;

			// do close() callback
			expect(k2h.close(function(error)
			{
				expect(error).to.be.null;

				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openRO(), close() - No Callback
	//
	it('k2hash - k2hnode::openRO(), close() - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_oro.k2h';
		var	k2h		= k2hash();

		expect(k2h.openRO(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::openRO(), close() - on Callback
	//
	it('k2hash - k2hnode::openRO(), close() - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_oro.k2h';
		var	k2h		= k2hash();

		// set on(close) callback
		expect(k2h.on('close', function(error)
		{
			expect(error).to.be.null;

			// unset
			k2h.off('close');

			done();
		})).to.be.a('boolean').to.be.true;

		// set on(openRO) callback
		expect(k2h.on('openRO', function(error)
		{
			expect(error).to.be.null;

			// do close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2h.off('openRO');
		})).to.be.a('boolean').to.be.true;

		// do openRO
		expect(k2h.openRO(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openRO(), close() - onOpenRO/onClose Callback
	//
	it('k2hash - k2hnode::openRO(), close() - onOpenRO/onClose Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_oro.k2h';
		var	k2h		= k2hash();

		// set onClose() callback
		expect(k2h.onClose(function(error)
		{
			expect(error).to.be.null;

			// unset
			k2h.offClose();

			done();
		})).to.be.a('boolean').to.be.true;

		// set onOpenRO() callback
		expect(k2h.onOpenRO(function(error)
		{
			expect(error).to.be.null;

			// do close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2h.offOpenRO();
		})).to.be.a('boolean').to.be.true;

		// do openRO
		expect(k2h.openRO(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openRO(), close() - inline Callback
	//
	it('k2hash - k2hnode::openRO(), close() - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_oro.k2h';
		var	k2h		= k2hash();

		// do openRO() callback
		expect(k2h.openRO(k2hfile, true, 2, 4, 4, 128, function(error)
		{
			expect(error).to.be.null;

			// do close() callback
			expect(k2h.close(function(error)
			{
				expect(error).to.be.null;

				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openTempfile(), close() - No Callback
	//
	it('k2hash - k2hnode::openTempfile(), close() - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_otmp1.k2h';
		var	k2h		= k2hash();

		expect(k2h.openTempfile(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::openTempfile(), close() - on Callback
	//
	it('k2hash - k2hnode::openTempfile(), close() - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_otmp2.k2h';
		var	k2h		= k2hash();

		// set on(close) callback
		expect(k2h.on('close', function(error)
		{
			expect(error).to.be.null;

			// unset
			k2h.off('close');

			done();
		})).to.be.a('boolean').to.be.true;

		// set on(openTempfile) callback
		expect(k2h.on('openTempfile', function(error)
		{
			expect(error).to.be.null;

			// do close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2h.off('openTempfile');
		})).to.be.a('boolean').to.be.true;

		// do openTempfile
		expect(k2h.openTempfile(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openTempfile(), close() - onOpenTempfile/onClose Callback
	//
	it('k2hash - k2hnode::openTempfile(), close() - onOpenTempfile/onClose Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_otmp3.k2h';
		var	k2h		= k2hash();

		// set onClose() callback
		expect(k2h.onClose(function(error)
		{
			expect(error).to.be.null;

			// unset
			k2h.offClose();

			done();
		})).to.be.a('boolean').to.be.true;

		// set onOpenTempfile() callback
		expect(k2h.onOpenTempfile(function(error)
		{
			expect(error).to.be.null;

			// do close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2h.offOpenTempfile();
		})).to.be.a('boolean').to.be.true;

		// do openTempfile
		expect(k2h.openTempfile(k2hfile, true, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openTempfile(), close() - inline Callback
	//
	it('k2hash - k2hnode::openTempfile(), close() - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_otmp4.k2h';
		var	k2h		= k2hash();

		// do openTempfile() callback
		expect(k2h.openTempfile(k2hfile, true, 2, 4, 4, 128, function(error)
		{
			expect(error).to.be.null;

			// do close() callback
			expect(k2h.close(function(error)
			{
				expect(error).to.be.null;

				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openMem(), close() - No Callback
	//
	it('k2hash - k2hnode::openMem(), close() - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2h = k2hash();

		expect(k2h.openMem(2, 4, 4, 128)).to.be.a('boolean').to.be.true;
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::openMem(), close() - on Callback
	//
	it('k2hash - k2hnode::openMem(), close() - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2h = k2hash();

		// set on(close) callback
		expect(k2h.on('close', function(error)
		{
			expect(error).to.be.null;

			// unset
			k2h.off('close');

			done();
		})).to.be.a('boolean').to.be.true;

		// set on(openMem) callback
		expect(k2h.on('openMem', function(error)
		{
			expect(error).to.be.null;

			// do close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2h.off('openMem');
		})).to.be.a('boolean').to.be.true;

		// do openMem
		expect(k2h.openMem(2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openMem(), close() - onOpenMem/onClose Callback
	//
	it('k2hash - k2hnode::openMem(), close() - onOpenMem/onClose Callback', function(done){						// eslint-disable-line no-undef
		var	k2h = k2hash();

		// set onClose callback
		expect(k2h.onClose(function(error)
		{
			expect(error).to.be.null;

			// unset
			k2h.offClose();

			done();
		})).to.be.a('boolean').to.be.true;

		// set onOpenMem() callback
		expect(k2h.onOpenMem(function(error)
		{
			expect(error).to.be.null;

			// do close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			// unset
			k2h.offOpenMem();
		})).to.be.a('boolean').to.be.true;

		// do openMem
		expect(k2h.openMem(2, 4, 4, 128)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::openMem(), close() - inline Callback
	//
	it('k2hash - k2hnode::openMem(), close() - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2h = k2hash();

		// do onOpenMem() inline callback
		expect(k2h.openMem(2, 4, 4, 128, function(error)
		{
			expect(error).to.be.null;

			// do close() inline callback
			expect(k2h.close(function(error)
			{
				expect(error).to.be.null;

				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test getValue/setValue
	//-------------------------------------------------------------------
	//
	// k2hnode::open(), setValue(), getValue(), close() - No Callback
	//
	it('k2hash - k2hnode::open(), setValue(), getValue(), close() - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();
		var	testval;

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
	it('k2hash - k2hnode::open(), setValue(), getValue(), close() - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set on(set) callback
		expect(k2h.on('set', function(error)
		{
			expect(error).to.be.null;

			// get
			expect(k2h.getValue('setget_test_key_21')).to.be.a('boolean').to.be.true;

			// unset
			k2h.off('set');
		})).to.be.a('boolean').to.be.true;

		// set on(get) callback
		expect(k2h.on('get', function(error, value)
		{
			expect(error).to.be.null;
			expect(value).to.equal('setget_test_key_val_21');

			// unset
			k2h.off('get');

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// set/get pattern(only key with no parameter)
		expect(k2h.setValue('setget_test_key_21', 'setget_test_key_val_21')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::open(), setValue(), getValue(), close() - onGet/onSet Callback
	//
	it('k2hash - k2hnode::open(), setValue(), getValue(), close() - onGet/onSet Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set onSet() callback
		expect(k2h.onSet(function(error)
		{
			expect(error).to.be.null;

			// get
			expect(k2h.getValue('setget_test_key_31')).to.be.a('boolean').to.be.true;

			// unset
			k2h.offSet();
		})).to.be.a('boolean').to.be.true;

		// set onGet() callback
		expect(k2h.onGet(function(error, value)
		{
			expect(error).to.be.null;
			expect(value).to.equal('setget_test_key_val_31');

			// unset
			k2h.offGet();

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// set/get pattern(only key with no parameter)
		expect(k2h.setValue('setget_test_key_31', 'setget_test_key_val_31')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::open(), setValue(), getValue(), close() - inline Callback
	//
	it('k2hash - k2hnode::open(), setValue(), getValue(), close() - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set pattern(only key with no parameter)
		expect(k2h.setValue('setget_test_key_41', 'setget_test_key_val_41', function(error)
		{
			expect(error).to.be.null;

			// get
			expect(k2h.getValue('setget_test_key_41', function(error, value)
			{
				expect(error).to.be.null;
				expect(value).to.equal('setget_test_key_val_41');

				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;

				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::open(), setValue(), getValue(), close() - special words
	//
	it('k2hash - k2hnode::open(), setValue(), getValue(), close() - special words', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var keyvals	= {	setget_test_key_51	: 'fooval',
						setget_test_key_52	: 'barval',
						setget_test_key_53	: '',
						setget_test_key_54	: null,
						'日本語キー'		: '日本語バリュー',
						'aaa諦本語キー'	: 'aaa諦本語バリュー'	};
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		for(var key in keyvals){
			expect(k2h.setValue(key, keyvals[key])).to.be.a('boolean').to.be.true;
		}
		for(var key in keyvals){
			var	testval = k2h.getValue(key);
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
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

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
		var	subkeylist	= ['addsubkeys_test_key_020/addsubkeys_test_subkey_020', 'addsubkeys_test_key_020/addsubkeys_test_subkey_021'];
		expect(k2h.addSubkeys('addsubkeys_test_key_020',	subkeylist)).to.be.a('boolean').to.be.true;

		// getSubkey(error)
		expect(k2h.getSubkeys('addsubkeys_test_key_000')).to.be.null;

		// getSubkey(1 subkey)
		var	skeylist = k2h.getSubkeys('addsubkeys_test_key_010');
		expect(skeylist instanceof Array).to.be.a('boolean').to.be.true;
		expect(skeylist.length).to.equal(1);
		expect(skeylist[0]).to.equal('addsubkeys_test_key_010/addsubkeys_test_subkey_010');

		// getSubkey(2 subkey)
		var	skeylist = k2h.getSubkeys('addsubkeys_test_key_020');
		expect(skeylist instanceof Array).to.be.a('boolean').to.be.true;
		expect(skeylist.length).to.equal(2);
		expect(skeylist[0]).to.equal('addsubkeys_test_key_020/addsubkeys_test_subkey_020');
		expect(skeylist[1]).to.equal('addsubkeys_test_key_020/addsubkeys_test_subkey_021');

		// close
		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - on Callback(error)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - on Callback(error)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_100', 'addsubkeys_test_key_value_100')).to.be.a('boolean').to.be.true;

		// set on(getSubkeys) callback
		expect(k2h.on('getSubkeys', function(error, skeylist)
		{
			expect(error).to.not.be.null;
			// unset
			k2h.off('getSubkeys');

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// getSubkeys
		expect(k2h.getSubkeys('addsubkeys_test_key_100')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - on Callback(1 subkey)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - on Callback(1 subkey)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_110', 'addsubkeys_test_key_value_110')).to.be.a('boolean').to.be.true;

		// set on(addSubkey) callback
		expect(k2h.on('addSubkey', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// getSubkeys
			expect(k2h.getSubkeys('addsubkeys_test_key_110')).to.be.a('boolean').to.be.true;

			// unset
			k2h.off('addSubkey');
		})).to.be.a('boolean').to.be.true;

		// set on(getSubkeys) callback
		expect(k2h.on('getSubkeys', function(error, skeylist)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// check
			expect(skeylist instanceof Array).to.be.a('boolean').to.be.true;
			expect(skeylist.length).to.equal(1);
			expect(skeylist[0]).to.equal('addsubkeys_test_key_110/addsubkeys_test_subkey_110');

			// unset
			k2h.off('getSubkeys');

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// addSubkey
		expect(k2h.addSubkey('addsubkeys_test_key_110', 'addsubkeys_test_key_110/addsubkeys_test_subkey_110', 'addsubkeys_test_subkey_value_110')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - on Callback(2 subkey)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - on Callback(2 subkey)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_120', 'addsubkeys_test_key_value_120')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('addsubkeys_test_key_120/addsubkeys_test_subkey_120', 'addsubkeys_test_subkey_value_120')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('addsubkeys_test_key_120/addsubkeys_test_subkey_121', 'addsubkeys_test_subkey_value_121')).to.be.a('boolean').to.be.true;

		// set on(addSubkeys) callback
		expect(k2h.on('addSubkeys', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// getSubkeys
			expect(k2h.getSubkeys('addsubkeys_test_key_120')).to.be.a('boolean').to.be.true;

			// unset
			k2h.off('addSubkeys');
		})).to.be.a('boolean').to.be.true;

		// set on(getSubkeys) callback
		expect(k2h.on('getSubkeys', function(error, skeylist)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// check
			expect(skeylist instanceof Array).to.be.a('boolean').to.be.true;
			expect(skeylist.length).to.equal(2);
			expect(skeylist[0]).to.equal('addsubkeys_test_key_120/addsubkeys_test_subkey_120');
			expect(skeylist[1]).to.equal('addsubkeys_test_key_120/addsubkeys_test_subkey_121');

			// unset
			k2h.off('getSubkeys');

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// addSubkeys
		var	subkeylist	= ['addsubkeys_test_key_120/addsubkeys_test_subkey_120', 'addsubkeys_test_key_120/addsubkeys_test_subkey_121'];
		expect(k2h.addSubkeys('addsubkeys_test_key_120',	subkeylist)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - onGetSubkeys Callback(error)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - onGetSubkeys Callback(error)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_200', 'addsubkeys_test_key_value_200')).to.be.a('boolean').to.be.true;

		// set onGetSubkeys() callback
		expect(k2h.onGetSubkeys(function(error, skeylist)
		{
			expect(error).to.not.be.null;
			// unset
			k2h.offGetSubkeys();
			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// getSubkeys
		expect(k2h.getSubkeys('addsubkeys_test_key_200')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - onGet(Add)Subkey(s) Callback(1 subkey)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - onGet(Add)Subkey(s) Callback(1 subkey)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_210', 'addsubkeys_test_key_value_210')).to.be.a('boolean').to.be.true;

		// set onAddSubkey callback
		expect(k2h.onAddSubkey(function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// getSubkeys
			expect(k2h.getSubkeys('addsubkeys_test_key_210')).to.be.a('boolean').to.be.true;

			// unset
			k2h.offAddSubkey();
		})).to.be.a('boolean').to.be.true;

		// set onGetSubkeys() callback
		expect(k2h.onGetSubkeys(function(error, skeylist)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// check
			expect(skeylist instanceof Array).to.be.a('boolean').to.be.true;
			expect(skeylist.length).to.equal(1);
			expect(skeylist[0]).to.equal('addsubkeys_test_key_210/addsubkeys_test_subkey_210');

			// unset
			k2h.offGetSubkeys();

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// addSubkey
		expect(k2h.addSubkey('addsubkeys_test_key_210',	'addsubkeys_test_key_210/addsubkeys_test_subkey_210', 'addsubkeys_test_subkey_value_210')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - onGet(Add)Subkey(s) Callback(2 subkey)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - onGet(Add)Subkey(s) Callback(2 subkey)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_220', 'addsubkeys_test_key_value_220')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('addsubkeys_test_key_220/addsubkeys_test_subkey_220', 'addsubkeys_test_subkey_value_220')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('addsubkeys_test_key_220/addsubkeys_test_subkey_221', 'addsubkeys_test_subkey_value_221')).to.be.a('boolean').to.be.true;

		// set onAddSubkeys() callback
		expect(k2h.onAddSubkeys(function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// getSubkeys
			expect(k2h.getSubkeys('addsubkeys_test_key_220')).to.be.a('boolean').to.be.true;

			// unset
			k2h.offAddSubkeys();
		})).to.be.a('boolean').to.be.true;

		// set onGetSubkeys() callback
		expect(k2h.onGetSubkeys(function(error, skeylist)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// check
			expect(skeylist instanceof Array).to.be.a('boolean').to.be.true;
			expect(skeylist.length).to.equal(2);
			expect(skeylist[0]).to.equal('addsubkeys_test_key_220/addsubkeys_test_subkey_220');
			expect(skeylist[1]).to.equal('addsubkeys_test_key_220/addsubkeys_test_subkey_221');

			// unset
			k2h.offGetSubkeys();

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// addSubkeys
		var	subkeylist	= ['addsubkeys_test_key_220/addsubkeys_test_subkey_220', 'addsubkeys_test_key_220/addsubkeys_test_subkey_221'];
		expect(k2h.addSubkeys('addsubkeys_test_key_220',	subkeylist)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - inline Callback(error)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - inline Callback(error)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_300', 'addsubkeys_test_key_value_300')).to.be.a('boolean').to.be.true;

		// getSubkeys
		expect(k2h.getSubkeys('addsubkeys_test_key_300', function(error, skeylist)
		{
			expect(error).to.not.be.null;

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - inline Callback(1 subkey)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - inline Callback(1 subkey)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_310', 'addsubkeys_test_key_value_310')).to.be.a('boolean').to.be.true;

		// addSubkey
		expect(k2h.addSubkey('addsubkeys_test_key_310', 'addsubkeys_test_key_310/addsubkeys_test_subkey_310', 'addsubkeys_test_subkey_value_310', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// getSubkeys
			expect(k2h.getSubkeys('addsubkeys_test_key_310', function(error, skeylist)
			{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// check
				expect(skeylist instanceof Array).to.be.a('boolean').to.be.true;
				expect(skeylist.length).to.equal(1);
				expect(skeylist[0]).to.equal('addsubkeys_test_key_310/addsubkeys_test_subkey_310');

				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;

				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::addSubkey(), addSubkeys(), getSubkeys() - inline Callback(2 subkey)
	//
	it('k2hash - k2hnode::addSubkey(), addSubkeys(), getSubkeys() - inline Callback(2 subkey)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make key
		expect(k2h.setValue('addsubkeys_test_key_320', 'addsubkeys_test_key_value_320')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('addsubkeys_test_key_320/addsubkeys_test_subkey_320', 'addsubkeys_test_subkey_value_320')).to.be.a('boolean').to.be.true;
		expect(k2h.setValue('addsubkeys_test_key_320/addsubkeys_test_subkey_321', 'addsubkeys_test_subkey_value_321')).to.be.a('boolean').to.be.true;

		// addSubkeys
		var	subkeylist	= ['addsubkeys_test_key_320/addsubkeys_test_subkey_320', 'addsubkeys_test_key_320/addsubkeys_test_subkey_321'];
		expect(k2h.addSubkeys('addsubkeys_test_key_320', subkeylist, function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// getSubkeys
			expect(k2h.getSubkeys('addsubkeys_test_key_320', function(error, skeylist)
			{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// check
				expect(skeylist instanceof Array).to.be.a('boolean').to.be.true;
				expect(skeylist.length).to.equal(2);
				expect(skeylist[0]).to.equal('addsubkeys_test_key_320/addsubkeys_test_subkey_320');
				expect(skeylist[1]).to.equal('addsubkeys_test_key_320/addsubkeys_test_subkey_321');

				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;

				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test remove/removeAll
	//-------------------------------------------------------------------
	//
	// k2hnode::remove(), removeAll() - No Callback
	//
	it('k2hash - k2hnode::remove(), removeAll() - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

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
	it('k2hash - k2hnode::remove(), removeAll() - on Callback(error 1)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.on('remove', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// unset
			k2h.off('remove');

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.remove('remove_test_key_XXX')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - on Callback(error 2)
	//
	it('k2hash - k2hnode::remove(), removeAll() - on Callback(error 2)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set on(removeAll) callback
		expect(k2h.on('removeAll', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// unset
			k2h.off('removeAll');

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.removeAll('remove_test_key_XXX')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - on Callback(error 3)
	//
	it('k2hash - k2hnode::remove(), removeAll() - on Callback(error 3)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key
		expect(k2h.setValue('remove_test_key_100', 'remove_test_key_value_100')).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.on('remove', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// unset
			k2h.off('remove');

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.remove('remove_test_key_100', 'remove_test_key_100/remove_test_subkey_100')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - on Callback(key)
	//
	it('k2hash - k2hnode::remove(), removeAll() - on Callback(key)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key
		expect(k2h.setValue('remove_test_key_100', 'remove_test_key_value_100')).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.on('remove', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;
			expect(k2h.getValue('remove_test_key_100')).to.be.null;

			// unset
			k2h.off('remove');

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// remove(key)
		expect(k2h.remove('remove_test_key_100')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - on Callback(subkey)
	//
	it('k2hash - k2hnode::remove(), removeAll() - on Callback(subkey)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key and subkey
		expect(k2h.setValue('remove_test_key_110', 'remove_test_key_value_110')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_110', 'remove_test_key_110/remove_test_subkey_110', 'remove_test_subkey_value_110')).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.on('remove', function(error)
		{
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

			done();
		})).to.be.a('boolean').to.be.true;

		// remove(subkey)
		expect(k2h.remove('remove_test_key_110', 'remove_test_key_110/remove_test_subkey_110')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - on Callback(removeall)
	//
	it('k2hash - k2hnode::remove(), removeAll() - on Callback(removeall)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key and subkey
		expect(k2h.setValue('remove_test_key_120', 'remove_test_key_value_120')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_120', 'remove_test_key_120/remove_test_subkey_120', 'remove_test_subkey_value_120')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_120', 'remove_test_key_120/remove_test_subkey_121', 'remove_test_subkey_value_121')).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.on('removeAll', function(error)
		{
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

			done();
		})).to.be.a('boolean').to.be.true;

		// remove(removeall)
		expect(k2h.removeAll('remove_test_key_120')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - onRemove(All) Callback(error 1)
	//
	it('k2hash - k2hnode::remove(), removeAll() - onRemove(All) Callback(error 1)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.onRemove(function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// unset
			k2h.offRemove();

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.remove('remove_test_key_XXX')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - onRemove(All) Callback(error 2)
	//
	it('k2hash - k2hnode::remove(), removeAll() - onRemove(All) Callback(error 2)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set on(removeAll) callback
		expect(k2h.onRemoveAll(function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// unset
			k2h.offRemoveAll();

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.removeAll('remove_test_key_XXX')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - onRemove(All) Callback(error 3)
	//
	it('k2hash - k2hnode::remove(), removeAll() - onRemove(All) Callback(error 3)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key
		expect(k2h.setValue('remove_test_key_200', 'remove_test_key_value_200')).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.onRemove(function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// unset
			k2h.offRemove();

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.remove('remove_test_key_200', 'remove_test_key_200/remove_test_subkey_200')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - onRemove(All) Callback(key)
	//
	it('k2hash - k2hnode::remove(), removeAll() - onRemove(All) Callback(key)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key
		expect(k2h.setValue('remove_test_key_200', 'remove_test_key_value_200')).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.onRemove(function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;
			expect(k2h.getValue('remove_test_key_200')).to.be.null;

			// unset
			k2h.offRemove();

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// remove(key)
		expect(k2h.remove('remove_test_key_200')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - onRemove(All) Callback(subkey)
	//
	it('k2hash - k2hnode::remove(), removeAll() - onRemove(All) Callback(subkey)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key and subkey
		expect(k2h.setValue('remove_test_key_210', 'remove_test_key_value_210')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_210', 'remove_test_key_210/remove_test_subkey_210', 'remove_test_subkey_value_210')).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.onRemove(function(error)
		{
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

			done();
		})).to.be.a('boolean').to.be.true;

		// remove(subkey)
		expect(k2h.remove('remove_test_key_210', 'remove_test_key_210/remove_test_subkey_210')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - onRemove(All) Callback(removeall)
	//
	it('k2hash - k2hnode::remove(), removeAll() - onRemove(All) Callback(removeall)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key and subkey
		expect(k2h.setValue('remove_test_key_220', 'remove_test_key_value_220')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_220', 'remove_test_key_220/remove_test_subkey_220', 'remove_test_subkey_value_220')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_220', 'remove_test_key_220/remove_test_subkey_221', 'remove_test_subkey_value_221')).to.be.a('boolean').to.be.true;

		// set on(remove) callback
		expect(k2h.onRemoveAll(function(error)
		{
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

			done();
		})).to.be.a('boolean').to.be.true;

		// remove(removeall)
		expect(k2h.removeAll('remove_test_key_220')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - inline Callback(error 1)
	//
	it('k2hash - k2hnode::remove(), removeAll() - inline Callback(error 1)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.remove('remove_test_key_XXX', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - inline Callback(error 2)
	//
	it('k2hash - k2hnode::remove(), removeAll() - inline Callback(error 2)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.removeAll('remove_test_key_XXX', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - inline Callback(error 3)
	//
	it('k2hash - k2hnode::remove(), removeAll() - inline Callback(error 3)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key
		expect(k2h.setValue('remove_test_key_300', 'remove_test_key_value_300')).to.be.a('boolean').to.be.true;

		// remove(error) - [NOTE] on this case always returns true
		expect(k2h.remove('remove_test_key_300', 'remove_test_key_300/remove_test_subkey_300', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - inline Callback(key)
	//
	it('k2hash - k2hnode::remove(), removeAll() - inline Callback(key)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key
		expect(k2h.setValue('remove_test_key_300', 'remove_test_key_value_300')).to.be.a('boolean').to.be.true;

		// remove(key)
		expect(k2h.remove('remove_test_key_300', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;
			expect(k2h.getValue('remove_test_key_300')).to.be.null;

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - inline Callback(subkey)
	//
	it('k2hash - k2hnode::remove(), removeAll() - inline Callback(subkey)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key and subkey
		expect(k2h.setValue('remove_test_key_310', 'remove_test_key_value_310')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_310', 'remove_test_key_310/remove_test_subkey_310', 'remove_test_subkey_value_310')).to.be.a('boolean').to.be.true;

		// remove(subkey)
		expect(k2h.remove('remove_test_key_310', 'remove_test_key_310/remove_test_subkey_310', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;
			expect(k2h.getValue('remove_test_key_310/remove_test_subkey_310')).to.be.null;
			expect(k2h.getSubkeys('remove_test_key_310')).to.be.null;

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::remove(), removeAll() - inline Callback(removeall)
	//
	it('k2hash - k2hnode::remove(), removeAll() - inline Callback(removeall)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// make a key and subkey
		expect(k2h.setValue('remove_test_key_320', 'remove_test_key_value_320')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_320', 'remove_test_key_320/remove_test_subkey_320', 'remove_test_subkey_value_320')).to.be.a('boolean').to.be.true;
		expect(k2h.addSubkey('remove_test_key_320', 'remove_test_key_320/remove_test_subkey_321', 'remove_test_subkey_value_321')).to.be.a('boolean').to.be.true;

		// remove(removeall)
		expect(k2h.removeAll('remove_test_key_320', function(error)
		{
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

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test printState/printVersion/getAttrVersionInfos/getAttrInfos
	//-------------------------------------------------------------------
	//
	// k2hnode::printState(), printVersion(), getAttrVersionInfos(), getAttrInfos()
	//
	it('k2hash - k2hnode::printState(), printVersion(), getAttrVersionInfos(), getAttrInfos()', function(done){						// eslint-disable-line no-undef
		var	fs		= require('fs');
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		expect(k2h.printState()).to.be.a('boolean').to.be.true;
		expect(k2h.printVersion()).to.be.a('boolean').to.be.true;
		expect(k2h.getAttrVersionInfos()).to.be.a('boolean').to.be.true;
		expect(k2h.getAttrInfos()).to.be.a('boolean').to.be.true;

		var	fd = fs.openSync('/tmp/result01_tmp.log', 'w');

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
	it('k2hash - k2hnode::dumpHead(), dumpKeytable(), dumpFullKeytable(), dumpElementtable(), dumpFull()', function(done){						// eslint-disable-line no-undef
		var	fs		= require('fs');
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		var	fd = fs.openSync('/tmp/result01_tmp.log', 'w');

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
	it('k2hash - k2hnode::transaction(), enableTransaction(), disableTransaction()', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

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
	it('k2hash - k2hnode::unsetTransactionThreadPool(), setTransactionThreadPool(), getTransactionThreadPool()', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

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
	it('k2hash - k2hnode::putArchive(), loadArchive() - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		expect(k2h.putArchive('/tmp/result_archive_tmp00.ar', false)).to.be.a('boolean').to.be.true;
		expect(k2h.loadArchive('/tmp/result_archive_tmp00.ar', false)).to.be.a('boolean').to.be.true;

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::putArchive(), loadArchive() - on Callback
	//
	it('k2hash - k2hnode::putArchive(), loadArchive() - on Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set on(put) callback
		expect(k2h.on('put', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// load
			expect(k2h.loadArchive('/tmp/result_archive_tmp01.ar', false)).to.be.a('boolean').to.be.true;

			// unset
			k2h.off('put');
		})).to.be.a('boolean').to.be.true;

		// set on(load) callback
		expect(k2h.on('load', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// unset
			k2h.off('load');

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// put
		expect(k2h.putArchive('/tmp/result_archive_tmp01.ar', false)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::putArchive(), loadArchive() - onLoad/onPut Callback
	//
	it('k2hash - k2hnode::putArchive(), loadArchive() - onLoad/onPut Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set onPut callback
		expect(k2h.onPut(function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// load
			expect(k2h.loadArchive('/tmp/result_archive_tmp01.ar', false)).to.be.a('boolean').to.be.true;

			// unset
			k2h.offPut();
		})).to.be.a('boolean').to.be.true;

		// set onLoad callback
		expect(k2h.onLoad(function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// unset
			k2h.offLoad();

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// put
		expect(k2h.putArchive('/tmp/result_archive_tmp01.ar', false)).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::putArchive(), loadArchive() - inline Callback
	//
	it('k2hash - k2hnode::putArchive(), loadArchive() - inline Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_ar1.k2h';
		var	k2h		= k2hash();

		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// put
		expect(k2h.putArchive('/tmp/result_archive_tmp02.ar', false, function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// load
			expect(k2h.loadArchive('/tmp/result_archive_tmp02.ar', false, function(error)
			{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// close
				expect(k2h.close()).to.be.a('boolean').to.be.true;

				done();
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test Common attributes
	//-------------------------------------------------------------------
	//
	// k2hnode::setCommonAttribute(), getAttrVersionInfos(), cleanCommonAttribute()
	//
	it('k2hash - k2hnode::setCommonAttribute(), getAttrVersionInfos(), cleanCommonAttribute()', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

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
	it('k2hash - k2hnode::addAttrPluginLib(), addAttrCryptPass()', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

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
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - No Callback', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

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
		var	attrslist1 = k2h.getAttrs('attr_test_key_001');
		expect(attrslist1 instanceof Array).to.be.a('boolean').to.be.true;
		expect(attrslist1.length).to.equal(1);
		expect(attrslist1[0]).to.equal('attr_test_attr_001');

		var	attrslist2 = k2h.getAttrs('attr_test_key_002');
		expect(attrslist2 instanceof Array).to.be.a('boolean').to.be.true;
		expect(attrslist2.length).to.equal(1);
		expect(attrslist2[0]).to.equal('aes256_cbc_pad.pbkdf2.sha256');

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
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - on Callback(error 1)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set on(getAttrValue) callback
		expect(k2h.on('getAttrValue', function(error, value)
		{
			expect(error).to.not.be.null;

			// unset
			k2h.off('getAttrValue');

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// get attribute value(error)
		expect(k2h.getAttrValue('attr_test_key_XXX', 'attr_test_attr_XXX')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - on Callback(error 2)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - on Callback(error 2)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set value
		expect(k2h.setValue('attr_test_key_100', 'attr_test_key_val_100')).to.be.a('boolean').to.be.true;

		// set on(getAttrValue) callback
		expect(k2h.on('getAttrValue', function(error, value)
		{
			expect(error).to.not.be.null;

			// unset
			k2h.off('getAttrValue');

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// set on(getAttrs) callback
		expect(k2h.on('getAttrs', function(error, attrs)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// check attributes
			expect(attrs instanceof Array).to.be.a('boolean').to.be.true;
			expect(attrs.length).to.equal(1);
			expect(attrs[0]).to.equal('attr_test_attr_100');

			// get attribute value(error)
			expect(k2h.getAttrValue('attr_test_key_100', 'attr_test_attr_XXX')).to.be.a('boolean').to.be.true;

			// unset
			k2h.off('getAttrs');
		})).to.be.a('boolean').to.be.true;


		// set on(addAttr) callback
		expect(k2h.on('addAttr', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// get attributes
			expect(k2h.getAttrs('attr_test_key_100')).to.be.a('boolean').to.be.true;

			// unset
			k2h.off('addAttr');
		})).to.be.a('boolean').to.be.true;

		// set attribute
		expect(k2h.addAttr('attr_test_key_100', 'attr_test_attr_100', 'attr_test_attr_val_100')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - on Callback(ok)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - on Callback(ok)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set value
		expect(k2h.setValue('attr_test_key_101', 'attr_test_key_val_101')).to.be.a('boolean').to.be.true;

		// set on(getAttrValue) callback
		expect(k2h.on('getAttrValue', function(error, value)
		{
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

			done();
		})).to.be.a('boolean').to.be.true;

		// set on(getAttrs) callback
		expect(k2h.on('getAttrs', function(error, attrs)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// check attributes
			expect(attrs instanceof Array).to.be.a('boolean').to.be.true;
			expect(attrs.length).to.equal(1);
			expect(attrs[0]).to.equal('attr_test_attr_101');

			// get attribute value
			expect(k2h.getAttrValue('attr_test_key_101', 'attr_test_attr_101')).to.be.a('boolean').to.be.true;

			// unset
			k2h.off('getAttrs');
		})).to.be.a('boolean').to.be.true;

		// set on(addAttr) callback
		expect(k2h.on('addAttr', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// get attributes
			expect(k2h.getAttrs('attr_test_key_101')).to.be.a('boolean').to.be.true;

			// unset
			k2h.off('addAttr');
		})).to.be.a('boolean').to.be.true;

		// set attribute
		expect(k2h.addAttr('attr_test_key_101', 'attr_test_attr_101', 'attr_test_attr_val_101')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - onXXX Callback(error 1)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - onXXX Callback(error 1)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set onGetAttrValue callback
		expect(k2h.onGetAttrValue(function(error, value)
		{
			expect(error).to.not.be.null;

			// unset
			k2h.offGetAttrValue();

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// get attribute value(error)
		expect(k2h.getAttrValue('attr_test_key_XXX', 'attr_test_attr_XXX')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - onXXX Callback(error 2)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - onXXX Callback(error 2)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set value
		expect(k2h.setValue('attr_test_key_200', 'attr_test_key_val_200')).to.be.a('boolean').to.be.true;

		// set onGetAttrValue callback
		expect(k2h.onGetAttrValue(function(error, value)
		{
			expect(error).to.not.be.null;

			// unset
			k2h.offGetAttrValue();

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;

		// set onGetAttrs callback
		expect(k2h.onGetAttrs(function(error, attrs)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// check attributes
			expect(attrs instanceof Array).to.be.a('boolean').to.be.true;
			expect(attrs.length).to.equal(1);
			expect(attrs[0]).to.equal('attr_test_attr_200');

			// get attribute value(error)
			expect(k2h.getAttrValue('attr_test_key_200', 'attr_test_attr_XXX')).to.be.a('boolean').to.be.true;

			// unset
			k2h.offGetAttrs();
		})).to.be.a('boolean').to.be.true;


		// set onAddAttr callback
		expect(k2h.onAddAttr(function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// get attributes
			expect(k2h.getAttrs('attr_test_key_200')).to.be.a('boolean').to.be.true;

			// unset
			k2h.off('addAttr');
		})).to.be.a('boolean').to.be.true;

		// set attribute
		expect(k2h.addAttr('attr_test_key_200', 'attr_test_attr_200', 'attr_test_attr_val_200')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - onXXX Callback(ok)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - onXXX Callback(ok)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set value
		expect(k2h.setValue('attr_test_key_201', 'attr_test_key_val_201')).to.be.a('boolean').to.be.true;

		// set onGetAttrValue callback
		expect(k2h.on('getAttrValue', function(error, value)
		{
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

			done();
		})).to.be.a('boolean').to.be.true;

		// set onGetAttrs callback
		expect(k2h.onGetAttrs(function(error, attrs)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// check attributes
			expect(attrs instanceof Array).to.be.a('boolean').to.be.true;
			expect(attrs.length).to.equal(1);
			expect(attrs[0]).to.equal('attr_test_attr_201');

			// get attribute value
			expect(k2h.getAttrValue('attr_test_key_201', 'attr_test_attr_201')).to.be.a('boolean').to.be.true;

			// unset
			k2h.offGetAttrs();
		})).to.be.a('boolean').to.be.true;

		// set onAddAttr callback
		expect(k2h.onAddAttr(function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// get attributes
			expect(k2h.getAttrs('attr_test_key_201')).to.be.a('boolean').to.be.true;

			// unset
			k2h.offAddAttr();
		})).to.be.a('boolean').to.be.true;

		// set attribute
		expect(k2h.addAttr('attr_test_key_201', 'attr_test_attr_201', 'attr_test_attr_val_201')).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - inline Callback(error 1)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - inline Callback(error 1)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// get attribute value(error)
		expect(k2h.getAttrValue('attr_test_key_XXX', 'attr_test_attr_XXX', function(error, value)
		{
			expect(error).to.not.be.null;

			// close
			expect(k2h.close()).to.be.a('boolean').to.be.true;

			done();
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - inline Callback(error 2)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - inline Callback(error 2)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set value
		expect(k2h.setValue('attr_test_key_300', 'attr_test_key_val_300')).to.be.a('boolean').to.be.true;

		// set attribute
		expect(k2h.addAttr('attr_test_key_300', 'attr_test_attr_300', 'attr_test_attr_val_300', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// get attributes
			expect(k2h.getAttrs('attr_test_key_300', function(error, attrs)
			{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// check attributes
				expect(attrs instanceof Array).to.be.a('boolean').to.be.true;
				expect(attrs.length).to.equal(1);
				expect(attrs[0]).to.equal('attr_test_attr_300');

				// get attribute value(error)
				expect(k2h.getAttrValue('attr_test_key_300', 'attr_test_attr_XXX', function(error, value)
				{
					expect(error).to.not.be.null;

					// close
					expect(k2h.close()).to.be.a('boolean').to.be.true;

					done();
				})).to.be.a('boolean').to.be.true;
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//
	// k2hnode::getAttrs(), getAttrValue(), addAttr() - inline Callback(ok)
	//
	it('k2hash - k2hnode::getAttrs(), getAttrValue(), addAttr() - inline Callback(ok)', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp.k2h';
		var	k2h		= k2hash();

		// open
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		// set value
		expect(k2h.setValue('attr_test_key_301', 'attr_test_key_val_301')).to.be.a('boolean').to.be.true;

		// set attribute
		expect(k2h.addAttr('attr_test_key_301', 'attr_test_attr_301', 'attr_test_attr_val_301', function(error)
		{
			if(null != error){
				k2h.close();
			}
			expect(error).to.be.null;

			// get attributes
			expect(k2h.getAttrs('attr_test_key_301', function(error, attrs)
			{
				if(null != error){
					k2h.close();
				}
				expect(error).to.be.null;

				// check attributes
				expect(attrs instanceof Array).to.be.a('boolean').to.be.true;
				expect(attrs.length).to.equal(1);
				expect(attrs[0]).to.equal('attr_test_attr_301');

				// get attribute value
				expect(k2h.getAttrValue('attr_test_key_301', 'attr_test_attr_301', function(error, value)
				{
					if(null != error){
						k2h.close();
					}
					expect(error).to.be.null;

					// check attribute value
					expect(value).to.equal('attr_test_attr_val_301');

					// close
					expect(k2h.close()).to.be.a('boolean').to.be.true;

					done();
				})).to.be.a('boolean').to.be.true;
			})).to.be.a('boolean').to.be.true;
		})).to.be.a('boolean').to.be.true;
	});

	//-------------------------------------------------------------------
	// Test get object
	//-------------------------------------------------------------------
	//
	// k2hnode::getQueue()
	//
	it('k2hash - k2hnode::getQueue()', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_queue1.k2h';

		var	k2h		= k2hash();
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		var k2hq	= k2h.getQueue();
		expect(typeof k2hq).to.equal('object');

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});

	//
	// k2hnode::getKeyQueue()
	//
	it('k2hash - k2hnode::getKeyQueue()', function(done){						// eslint-disable-line no-undef
		var	k2hfile	= '/tmp/test01_tmp_queue2.k2h';

		var	k2h		= k2hash();
		expect(k2h.open(k2hfile, false, false, false, 2, 4, 4, 128)).to.be.a('boolean').to.be.true;

		var k2hkq	= k2h.getKeyQueue();
		expect(typeof k2hkq).to.equal('object');

		expect(k2h.close()).to.be.a('boolean').to.be.true;

		done();
	});
});

/*
 * VIM modelines
 *
 * vim:set ts=4 fenc=utf-8:
 */
