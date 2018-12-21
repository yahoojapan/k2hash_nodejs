#
# K2HASH
#
# Copyright 2015 Yahoo! JAPAN corporation.
#
# K2HASH is key-valuew store base libraries.
# K2HASH is made for the purpose of the construction of
# original KVS system and the offer of the library.
# The characteristic is this KVS library which Key can
# layer. And can support multi-processing and multi-thread,
# and is provided safely as available KVS.
#
# For the full copyright and license information, please view
# the LICENSE file that was distributed with this source code.
#
# AUTHOR:   taishiha@yahoo-corp.jp
# CREATE:   Tue 10 Mar 2015
# REVISION:
#

{
	"variables":	{
		"coverage":	"false"
	},
	"targets":		[
		{
			"target_name":	"k2hash",
			"sources":		[
				"k2hash.cc",
				"k2h_shm.cc",
				"k2h_queue.cc",
				"k2h_keyqueue.cc",
				"k2h_cbs.cc"
			],
			"cflags":		[
				#
				# We get too many deprecated message building with nodejs 0.12.x.
				# Those messages are depricated functions/methods, but we do not use those.
				# So that we ignore these deprecated warning here.
				#
				"-Wno-deprecated",
				#
				# For nodejs 9.x/10.x, it puts about MakeCallback / CreateDataProperty / DefineOwnProperty
				#
				"-Wno-deprecated-declarations"
			],
			"ldflags":		[
				"-Wl,-rpath,/home/y/lib64",
				"-Wl,-rpath,/home/y/lib"
			],
			"include_dirs":	[
				"/home/y/include",
				"/home/y/include64",
				"<!(node -e \"require('nan')\")",
			],
			"libraries":	[
				"-L/home/y/lib64",
				"-lk2hash"
			],
			"conditions":	[
				[
					"coverage == 'true'",
					{
						"cflags_cc":	[
							"--coverage"
						],
						"ldflags":		[
							"--coverage"
						]
					}
				]
			]
		},
		{
			"target_name":	"k2hqueue",
			"sources":		[
				"k2hqueue.cc",
				"k2h_queue.cc",
				"k2h_cbs.cc"
			],
			"cflags":		[
				#
				# We get too many deprecated message building with nodejs 0.12.x.
				# Those messages are depricated functions/methods, but we do not use those.
				# So that we ignore these deprecated warning here.
				#
				"-Wno-deprecated",
				#
				# For nodejs 9.x/10.x, it puts about MakeCallback / CreateDataProperty / DefineOwnProperty
				#
				"-Wno-deprecated-declarations"
			],
			"ldflags":		[
				"-Wl,-rpath,/home/y/lib64",
				"-Wl,-rpath,/home/y/lib"
			],
			"include_dirs":	[
				"/home/y/include",
				"/home/y/include64",
				"<!(node -e \"require('nan')\")",
			],
			"libraries":	[
				"-L/home/y/lib64",
				"-lk2hash"
			],
			"conditions":	[
				[
					"coverage == 'true'",
					{
						"cflags_cc":	[
							"--coverage"
						],
						"ldflags":		[
							"--coverage"
						]
					}
				]
			]
		},
		{
			"target_name":	"k2hkeyqueue",
			"sources":		[
				"k2hkeyqueue.cc",
				"k2h_keyqueue.cc",
				"k2h_cbs.cc"
			],
			"cflags":		[
				#
				# We get too many deprecated message building with nodejs 0.12.x.
				# Those messages are depricated functions/methods, but we do not use those.
				# So that we ignore these deprecated warning here.
				#
				"-Wno-deprecated",
				#
				# For nodejs 9.x/10.x, it puts about MakeCallback / CreateDataProperty / DefineOwnProperty
				#
				"-Wno-deprecated-declarations"
			],
			"ldflags":		[
				"-Wl,-rpath,/home/y/lib64",
				"-Wl,-rpath,/home/y/lib"
			],
			"include_dirs":	[
				"/home/y/include",
				"/home/y/include64",
				"<!(node -e \"require('nan')\")",
			],
			"libraries":	[
				"-L/home/y/lib64",
				"-lk2hash"
			],
			"conditions":	[
				[
					"coverage == 'true'",
					{
						"cflags_cc":	[
							"--coverage"
						],
						"ldflags":		[
							"--coverage"
						]
					}
				]
			]
		}
	]
}

#
# VIM modelines
#
# vim:set ts=4 fenc=utf-8:
#
