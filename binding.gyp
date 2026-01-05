{
	"variables": {
		"coverage":	"false",
		"openssl_fips": ""
	},
	"targets": [
		{
			"target_name":	"k2hash",
			"sources": [
				"src/k2hash.cc",
				"src/k2h_shm.cc",
				"src/k2h_queue.cc",
				"src/k2h_keyqueue.cc",
				"src/k2h_cbs.cc"
			],
			"include_dirs": [
				"<!(node -p \"require('node-addon-api').include\")",
				"<(module_root_dir)/node_modules/node-addon-api"
			],
			"dependencies": [
				"<!(node -p \"require('node-addon-api').gyp\")"
			],
			"cflags!": [
				"-fno-exceptions"
			],
			"cflags_cc!": [
				"-fno-exceptions"
			],
			"cflags_cc": [
				"-std=c++17"
			],
			"defines": [
				"NAPI_CPP_EXCEPTIONS"
			],
			"link_settings": {
				"libraries": [
					"-lk2hash"
				]
			}
		},
		{
			"target_name":	"k2hqueue",
			"sources": [
				"src/k2hqueue.cc",
				"src/k2h_queue.cc",
				"src/k2h_cbs.cc"
			],
			"include_dirs": [
				"<!(node -p \"require('node-addon-api').include\")",
				"<(module_root_dir)/node_modules/node-addon-api"
			],
			"dependencies": [
				"<!(node -p \"require('node-addon-api').gyp\")"
			],
			"cflags!": [
				"-fno-exceptions"
			],
			"cflags_cc!": [
				"-fno-exceptions"
			],
			"cflags_cc": [
				"-std=c++17"
			],
			"defines": [
				"NAPI_CPP_EXCEPTIONS"
			],
			"link_settings": {
				"libraries": [
					"-lk2hash"
				]
			}
		},
		{
			"target_name":	"k2hkeyqueue",
			"sources": [
				"src/k2hkeyqueue.cc",
				"src/k2h_keyqueue.cc",
				"src/k2h_cbs.cc"
			],
			"include_dirs": [
				"<!(node -p \"require('node-addon-api').include\")",
				"<(module_root_dir)/node_modules/node-addon-api"
			],
			"dependencies": [
				"<!(node -p \"require('node-addon-api').gyp\")"
			],
			"cflags!": [
				"-fno-exceptions"
			],
			"cflags_cc!": [
				"-fno-exceptions"
			],
			"cflags_cc": [
				"-std=c++17"
			],
			"defines": [
				"NAPI_CPP_EXCEPTIONS"
			],
			"link_settings": {
				"libraries": [
					"-lk2hash"
				]
			}
		}
	]
}
