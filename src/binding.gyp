#
# K2HASH
#
# Copyright 2015 Yahoo Japan Corporation.
#
# K2HASH is key-valuew store base libraries.
# K2HASH is made for the purpose of the construction of
# original KVS system and the offer of the library.
# The characteristic is this KVS library which Key can
# layer. And can support multi-processing and multi-thread,
# and is provided safely as available KVS.
#
# For the full copyright and license information, please view
# the license file that was distributed with this source code.
#
# AUTHOR:   Taku Ishihara
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
				"-Wno-deprecated-declarations",
				#
				# For nodejs 12.x/..., it suppress warnings: "'deprecated' attribute directive ignored"
				#
				"-Wno-attributes",
				#
				# nodejs/nan#807(https://github.com/nodejs/nan/issues/807#issuecomment-455750192)
				# recommends using the "-Wno-cast-function-type" to silence deprecations warnings
				# that appear with GCC 8.
				#
				"-Wno-cast-function-type"
			],
			"include_dirs":	[
				"<!(node -e \"require('nan')\")"
			],
			"libraries":	[
				"-lk2hash"
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
				"-Wno-deprecated-declarations",
				#
				# For nodejs 12.x/..., it suppress warnings: "'deprecated' attribute directive ignored"
				#
				"-Wno-attributes",
				#
				# nodejs/nan#807(https://github.com/nodejs/nan/issues/807#issuecomment-455750192)
				# recommends using the "-Wno-cast-function-type" to silence deprecations warnings
				# that appear with GCC 8.
				#
				"-Wno-cast-function-type"
			],
			"include_dirs":	[
				"<!(node -e \"require('nan')\")"
			],
			"libraries":	[
				"-lk2hash"
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
				"-Wno-deprecated-declarations",
				#
				# For nodejs 12.x/..., it suppress warnings: "'deprecated' attribute directive ignored"
				#
				"-Wno-attributes",
				#
				# nodejs/nan#807(https://github.com/nodejs/nan/issues/807#issuecomment-455750192)
				# recommends using the "-Wno-cast-function-type" to silence deprecations warnings
				# that appear with GCC 8.
				#
				"-Wno-cast-function-type"
			],
			"include_dirs":	[
				"<!(node -e \"require('nan')\")"
			],
			"libraries":	[
				"-lk2hash"
			]
		}
	]
}

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim600: noexpandtab sw=4 ts=4 fdm=marker
# vim<600: noexpandtab sw=4 ts=4
#
