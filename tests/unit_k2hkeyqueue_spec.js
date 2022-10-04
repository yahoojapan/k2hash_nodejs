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

//--------------------------------------------------------------
// Before in global section
//--------------------------------------------------------------
before(function(){										// eslint-disable-line no-undef
});

//--------------------------------------------------------------
// After in global section
//--------------------------------------------------------------
after(function(){										// eslint-disable-line no-undef
});

//--------------------------------------------------------------
// BeforeEach in global section
//--------------------------------------------------------------
beforeEach(function(){									// eslint-disable-line no-undef
	// Nothing to do
});

//--------------------------------------------------------------
// AfterEach in global section
//--------------------------------------------------------------
afterEach(function(){									// eslint-disable-line no-undef
	// Nothing to do
});

//--------------------------------------------------------------
// Sub describe section
//--------------------------------------------------------------
describe('SUB TEST: K2HKEYQUEUE', function(){			// eslint-disable-line no-undef
	require('./unit_k2hkeyqueue');
});

/*
 * VIM modelines
 *
 * vim:set ts=4 fenc=utf-8:
 */
