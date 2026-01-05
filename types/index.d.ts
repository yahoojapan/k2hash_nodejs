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

//-------------------------------------------------------------
// This uses 'export' to be compatible with CommonJS consumers:
//	const k2hash = require('k2hash');
//
// And works for ESM consumers with default import:
//	import k2hash from 'k2hash';
//-------------------------------------------------------------

//
// callable / constructible main factory
//
declare function k2hash(): k2hash.K2hNode;

declare namespace k2hash
{
	//---------------------------------------------------------
	// Callback types for K2hNode
	//---------------------------------------------------------
	// [NOTE]
	// We allow Error in case it might be changed to an Error object
	// in the future, but currently we do not return Error.
	//
	export type K2hCreateCallback = (err?: Error | string | null, result?: boolean) => void;
	export type K2hOpenCallback = (err?: Error | string | null, result?: boolean) => void;
	export type K2hOpenRWCallback = (err?: Error | string | null, result?: boolean) => void;
	export type K2hOpenROCallback = (err?: Error | string | null, result?: boolean) => void;
	export type K2hOpenTempfileCallback = (err?: Error | string | null, result?: boolean) => void;
	export type K2hOpenMemCallback = (err?: Error | string | null, result?: boolean) => void;
	export type K2hCloseCallback = (err?: Error | string | null, result?: boolean) => void;
	export type K2hGetValueCallback = (err?: string | null, result?: string | null) => void;
	export type K2hGetSubkeysCallback = (err?: string | null, subkeys?: string[] | null) => void;
	export type K2hSetValueCallback = (err?: Error | string | null, result?: boolean) => void;
	export type K2hAddSubkeyCallback = (err?: Error | string | null, result?: boolean) => void;
	export type K2hAddSubkeysCallback = (err: Error | string | null, result?: boolean) => void;
	export type K2hRemoveCallback = (err: Error | string | null, result?: boolean) => void;
	export type K2hRemoveAllCallback = (err: Error | string | null, result?: boolean) => void;
	export type K2hLoadArchiveCallback = (err: Error | string | null, result?: boolean) => void;
	export type K2hPutArchiveCallback = (err: Error | string | null, result?: boolean) => void;
	export type K2hGetAttrsCallback = (err?: Error | string | null, attrs?: string[] | null) => void;
	export type K2hGetAttrValueCallback = (err?: Error | string | null, value?: string | null) => void;
	export type K2hAddAttrCallback = (err?: Error | string | null, result?: boolean) => void;

	//---------------------------------------------------------
	// Emitter callback types for K2hNode
	//---------------------------------------------------------
	export type OnK2hEmitterCallback = (err?: string | null, ...args: any[]) => void;
	export type OnK2hCreateEmitterCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hOpenEmitterCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hOpenRWEmitterCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hOpenROEmitterCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hOpenTempfileEmitterCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hOpenMemEmitterCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hCloseEmitterCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hGetEmitterCallback = (err?: string | null, result?: string | null) => void;
	export type OnK2hGetSubkeysEmitterCallback = (err?: string | null, result?: string[] | null) => void;
	export type OnK2hSetEmitterCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hAddSubkeyEmitterCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hAddSubkeysEmitterCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hRemoveEmitterCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hRemoveAllEmitterCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hLoadEmitterCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hLoadArchiveEmitterCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hPutEmitterCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hPutArchiveEmitterCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hGetAttrsEmitterCallback = (err?: string | null, attrs?: string[] | null) => void;
	export type OnK2hGetAttrValueEmitterCallback = (err?: string | null, value?: string | null) => void;
	export type OnK2hAddAttrEmitterCallback = (err?: string | null, result?: boolean) => void;

	//---------------------------------------------------------
	// K2hNode Class
	//---------------------------------------------------------
	export class K2hNode
	{
		// Constructor
		constructor();	// always no arguments

		//-----------------------------------------------------
		// Methods (Callback can be called)
		//-----------------------------------------------------
		// create
		create(filename: string, cb?: K2hCreateCallback): boolean;
		create(filename: string, isfullmapping: boolean, cb?: K2hCreateCallback): boolean;
		create(filename: string, isfullmapping: boolean, mask_bitcnt: number, cb?: K2hCreateCallback): boolean;
		create(filename: string, isfullmapping: boolean, mask_bitcnt: number, cmask_bitcnt: number, cb?: K2hCreateCallback): boolean;
		create(filename: string, isfullmapping: boolean, mask_bitcnt: number, cmask_bitcnt: number, max_element_cnt: number, cb?: K2hCreateCallback): boolean;
		create(filename: string, isfullmapping: boolean, mask_bitcnt: number, cmask_bitcnt: number, max_element_cnt: number, pagesize: number, cb?: K2hCreateCallback): boolean;

		// open
		open(filename: string, cb?: K2hOpenCallback): boolean;
		open(filename: string, isreadonly: boolean, cb?: K2hOpenCallback): boolean;
		open(filename: string, isreadonly: boolean, istempfile: boolean, cb?: K2hOpenCallback): boolean;
		open(filename: string, isreadonly: boolean, istempfile: boolean, isfullmapping: boolean, cb?: K2hOpenCallback): boolean;
		open(filename: string, isreadonly: boolean, istempfile: boolean, isfullmapping: boolean, mask_bitcnt: number, cb?: K2hOpenCallback): boolean;
		open(filename: string, isreadonly: boolean, istempfile: boolean, isfullmapping: boolean, mask_bitcnt: number, cmask_bitcnt: number, cb?: K2hOpenCallback): boolean;
		open(filename: string, isreadonly: boolean, istempfile: boolean, isfullmapping: boolean, mask_bitcnt: number, cmask_bitcnt: number, max_element_cnt: number, cb?: K2hOpenCallback): boolean;
		open(filename: string, isreadonly: boolean, istempfile: boolean, isfullmapping: boolean, mask_bitcnt: number, cmask_bitcnt: number, max_element_cnt: number, pagesize: number, cb?: K2hOpenCallback): boolean;

		openRW(filename: string, cb?: K2hOpenRWCallback): boolean;
		openRW(filename: string, isfullmapping: boolean, cb?: K2hOpenRWCallback): boolean;
		openRW(filename: string, isfullmapping: boolean, mask_bitcnt: number, cb?: K2hOpenRWCallback): boolean;
		openRW(filename: string, isfullmapping: boolean, mask_bitcnt: number, cmask_bitcnt: number, cb?: K2hOpenRWCallback): boolean;
		openRW(filename: string, isfullmapping: boolean, mask_bitcnt: number, cmask_bitcnt: number, max_element_cnt: number, cb?: K2hOpenRWCallback): boolean;
		openRW(filename: string, isfullmapping: boolean, mask_bitcnt: number, cmask_bitcnt: number, max_element_cnt: number, pagesize: number, cb?: K2hOpenRWCallback): boolean;

		openRO(filename: string, cb?: K2hOpenROCallback): boolean;
		openRO(filename: string, isfullmapping: boolean, cb?: K2hOpenROCallback): boolean;
		openRO(filename: string, isfullmapping: boolean, mask_bitcnt: number, cb?: K2hOpenROCallback): boolean;
		openRO(filename: string, isfullmapping: boolean, mask_bitcnt: number, cmask_bitcnt: number, cb?: K2hOpenROCallback): boolean;
		openRO(filename: string, isfullmapping: boolean, mask_bitcnt: number, cmask_bitcnt: number, max_element_cnt: number, cb?: K2hOpenROCallback): boolean;
		openRO(filename: string, isfullmapping: boolean, mask_bitcnt: number, cmask_bitcnt: number, max_element_cnt: number, pagesize: number, cb?: K2hOpenROCallback): boolean;

		openTempfile(filename: string, cb?: K2hOpenTempfileCallback): boolean;
		openTempfile(filename: string, isfullmapping: boolean, cb?: K2hOpenTempfileCallback): boolean;
		openTempfile(filename: string, isfullmapping: boolean, mask_bitcnt: number, cb?: K2hOpenTempfileCallback): boolean;
		openTempfile(filename: string, isfullmapping: boolean, mask_bitcnt: number, cmask_bitcnt: number, cb?: K2hOpenTempfileCallback): boolean;
		openTempfile(filename: string, isfullmapping: boolean, mask_bitcnt: number, cmask_bitcnt: number, max_element_cnt: number, cb?: K2hOpenTempfileCallback): boolean;
		openTempfile(filename: string, isfullmapping: boolean, mask_bitcnt: number, cmask_bitcnt: number, max_element_cnt: number, pagesize: number, cb?: K2hOpenTempfileCallback): boolean;

		openMem(cb?: K2hOpenMemCallback): boolean;
		openMem(mask_bitcnt: number, cb?: K2hOpenMemCallback): boolean;
		openMem(mask_bitcnt: number, cmask_bitcnt: number, cb?: K2hOpenMemCallback): boolean;
		openMem(mask_bitcnt: number, cmask_bitcnt: number, max_element_cnt: number, cb?: K2hOpenMemCallback): boolean;
		openMem(mask_bitcnt: number, cmask_bitcnt: number, max_element_cnt: number, pagesize: number, cb?: K2hOpenMemCallback): boolean;

		// close
		close(cb?: K2hCloseCallback): boolean;

		// values
		getValue(key: string): string | null | undefined;
		getValue(key: string, cb: K2hGetValueCallback): boolean;
		getValue(key: string, subkey: string | null): string | null | undefined;
		getValue(key: string, subkey: string | null, cb: K2hGetValueCallback): boolean;
		getValue(key: string, subkey: string | null, attrchk: boolean): string | null | undefined;
		getValue(key: string, subkey: string | null, attrchk: boolean, cb: K2hGetValueCallback): boolean;
		getValue(key: string, subkey: string | null, attrchk: boolean, pass: string | null): string | null | undefined;
		getValue(key: string, subkey: string | null, attrchk: boolean, pass: string | null, cb: K2hGetValueCallback): boolean;

		getSubkeys(key: string): string[] | null;
		getSubkeys(key: string, cb: K2hGetSubkeysCallback): boolean;

		setValue(key: string, value: string | null, cb?: K2hSetValueCallback): boolean;
		setValue(key: string, value: string | null, subkey: string | null, cb?: K2hSetValueCallback): boolean;
		setValue(key: string, value: string | null, subkey: string | null, pass: string | null, cb?: K2hSetValueCallback): boolean;
		setValue(key: string, value: string | null, subkey: string | null, pass: string | null, expire: number, cb?: K2hSetValueCallback): boolean;

		addSubkey(key: string, subkey: string, cb?: K2hAddSubkeyCallback): boolean;
		addSubkey(key: string, subkey: string, value: string | null, cb?: K2hAddSubkeyCallback): boolean;

		addSubkeys(key: string, subkeys: string[], cb?: K2hAddSubkeysCallback): boolean;

		remove(key: string, cb?: K2hRemoveCallback): boolean;
		remove(key: string, subkey: string | null, cb?: K2hRemoveCallback): boolean;

		removeAll(key: string, cb?: K2hRemoveCallback): boolean;

		// archives
		loadArchive(filename: string, cb?: K2hLoadArchiveCallback): boolean;
		loadArchive(filename: string, errskip: boolean, cb?: K2hLoadArchiveCallback): boolean;

		putArchive(filename: string, cb?: K2hPutArchiveCallback): boolean;
		putArchive(filename: string, errskip: boolean, cb?: K2hPutArchiveCallback): boolean;

		// attributes
		getAttrs(key: string): string[] | null;
		getAttrs(key: string, cb: K2hGetAttrsCallback): boolean;

		getAttrValue(key: string, attr: string): string | null | undefined;
		getAttrValue(key: string, attr: string, cb: K2hGetAttrValueCallback): boolean;

		addAttr(key: string, attr: string, cb?: K2hAddAttrCallback): boolean;
		addAttr(key: string, attr: string, value: string | null, cb?: K2hAddAttrCallback): boolean;

		//-----------------------------------------------------
		// Methods (no callback)
		//-----------------------------------------------------
		// states
		printState(): string;
		printState(fd: number): boolean;

		// version
		printVersion(fd?: number): boolean;

		// dump
		dumpHead(fd?: number): boolean;
		dumpKeytable(fd?: number): boolean;
		dumpFullKeytable(fd?: number): boolean;
		dumpElementtable(fd?: number): boolean;
		dumpFull(fd?: number): boolean;

		// transaction
		transaction(enable: boolean): boolean;
		transaction(enable: boolean, transfile: string): boolean;
		transaction(enable: boolean, transfile: string, prefix: string): boolean;
		transaction(enable: boolean, transfile: string, prefix: string, param: string): boolean;
		transaction(enable: boolean, transfile: string, prefix: string, param: string, expire: number): boolean;

		enableTransaction(): boolean;
		enableTransaction(transfile: string): boolean;
		enableTransaction(transfile: string, prefix: string): boolean;
		enableTransaction(transfile: string, prefix: string, param: string): boolean;
		enableTransaction(transfile: string, prefix: string, param: string, expire: number): boolean;

		disableTransaction(): boolean;

		unsetTransactionThreadPool(): boolean;
		getTransactionThreadPool(): number;
		setTransactionThreadPool(count: number): boolean;

		// k2hqueue
		getQueue(isFifo?: boolean): K2hQueue;
		getQueue(isFifo: boolean, prefix: string): K2hQueue;

		// k2hkeyqueue
		getKeyQueue(isFifo?: boolean): K2hKeyQueue;
		getKeyQueue(isFifo: boolean, prefix: string): K2hKeyQueue;

		// common attributes
		setCommonAttribute(ismtime?: number): boolean;
		setCommonAttribute(ismtime: number, ishistory: number): boolean;
		setCommonAttribute(ismtime: number, ishistory: number, isencrypt: number): boolean;
		setCommonAttribute(ismtime: number, ishistory: number, isencrypt: number, pass: string | null): boolean;
		setCommonAttribute(ismtime: number, ishistory: number, isencrypt: number, pass: string | null, is_expire: number): boolean;
		setCommonAttribute(ismtime: number, ishistory: number, isencrypt: number, pass: string | null, is_expire: number, expire: number): boolean;

		cleanCommonAttribute(): boolean;

		// others
		addAttrPluginLib(libfile: string): boolean;
		addAttrCryptPass(pass: string, isDefaultEncrypt?: boolean): boolean;
		getAttrVersionInfos(fd?: number): boolean;
		getAttrInfos(fd?: number): boolean;

		//-----------------------------------------------------
		// Emitter registration/unregistration
		//-----------------------------------------------------
		on(emitter: string, cb: OnK2hEmitterCallback): boolean;
		onCreate(cb: OnK2hCreateEmitterCallback): boolean;
		onOpen(cb: OnK2hOpenEmitterCallback): boolean;
		onOpenRW(cb: OnK2hOpenRWEmitterCallback): boolean;
		onOpenRO(cb: OnK2hOpenROEmitterCallback): boolean;
		onOpenTempfile(cb: OnK2hOpenTempfileEmitterCallback): boolean;
		onOpenMem(cb: OnK2hOpenMemEmitterCallback): boolean;
		onClose(cb: OnK2hCloseEmitterCallback): boolean;
		onGet(cb: OnK2hGetEmitterCallback): boolean;
		onGetSubkeys(cb: OnK2hGetSubkeysEmitterCallback): boolean;
		onSet(cb: OnK2hSetEmitterCallback): boolean;
		onAddSubkey(cb: OnK2hAddSubkeyEmitterCallback): boolean;
		onAddSubkeys(cb: OnK2hAddSubkeysEmitterCallback): boolean;
		onRemove(cb: OnK2hRemoveEmitterCallback): boolean;
		onRemoveAll(cb: OnK2hRemoveAllEmitterCallback): boolean;
		onLoad(cb: OnK2hLoadEmitterCallback): boolean;
		onLoadArchive(cb: OnK2hLoadArchiveEmitterCallback): boolean;
		onPut(cb: OnK2hPutEmitterCallback): boolean;
		onPutArchive(cb: OnK2hPutArchiveEmitterCallback): boolean;
		onGetAttrs(cb: OnK2hGetAttrsEmitterCallback): boolean;
		onGetAttrValue(cb: OnK2hGetAttrValueEmitterCallback): boolean;
		onAddAttr(cb: OnK2hAddAttrEmitterCallback): boolean;

		off(emitter: string): boolean;
		offCreate(): boolean;
		offOpen(): boolean;
		offOpenRW(): boolean;
		offOpenRO(): boolean;
		offOpenTempfile(): boolean;
		offOpenMem(): boolean;
		offClose(): boolean;
		offGet(): boolean;
		offGetSubkeys(): boolean;
		offGetAttrs(): boolean;
		offGetAttrValue(): boolean;
		offSet(): boolean;
		offAddSubkey(): boolean;
		offAddSubkeys(): boolean;
		offAddAttr(): boolean;
		offRemove(): boolean;
		offRemoveAll(): boolean;
		offLoad(): boolean;
		offLoadArchive(): boolean;
		offPut(): boolean;
		offPutArchive(): boolean;

		//-----------------------------------------------------
		// Promise APIs(Currently no imprelemnts)
		//-----------------------------------------------------
		// [NOTE]
		// Although it is not currently implemented, here is an example definition:
		//
		// ex.	addSubkeysAsync?(key: string, subkeys: string[]): Promise<boolean>;
		//
	}

	//---------------------------------------------------------
	// Callback type for K2hQueue class
	//---------------------------------------------------------
	// [NOTE]
	// We allow Error in case it might be changed to an Error object
	// in the future, but currently we do not return Error.
	//
	export type K2hqCountCallback = (err?: Error | string | null, result?: number) => void;
	export type K2hqPushCallback = (err?: Error | string | null, result?: boolean) => void;
	export type K2hqPopCallback = (err?: Error | string | null, data?: string | null) => void;
	export type K2hqReadCallback = (err?: Error | string | null, data?: string | null) => void;
	export type K2hqRemoveCallback = (err?: Error | string | null, result?: number) => void;

	//---------------------------------------------------------
	// Emitter Callbask functions for K2hQueue class
	//---------------------------------------------------------
	export type OnK2hqEmitterCallback = (err?: string | null, ...args: any[]) => void;
	export type OnK2hqCountCallback = (err?: string | null, result?: number) => void;
	export type OnK2hqPushCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hqPopCallback = (err?: string | null, data?: string | null) => void;
	export type OnK2hqReadCallback = (err?: string | null, data?: string | null) => void;
	export type OnK2hqRemoveCallback = (err?: string | null, result?: number) => void;

	//---------------------------------------------------------
	// K2hQueue Class
	//---------------------------------------------------------
	export class K2hQueue
	{
		// Constructor
		constructor();	// always no arguments

		//-----------------------------------------------------
		// Methods (Callback can be called)
		//-----------------------------------------------------
		// count
		count(): number;
		count(cb: K2hqCountCallback): boolean;

		// push
		push(data: string, cb?: K2hqPushCallback): boolean;
		push(data: string, pass: string, cb?: K2hqPushCallback): boolean;
		push(data: string, pass: string, expire: number, cb?: K2hqPushCallback): boolean;

		// pop
		pop(): string | null;
		pop(cb: OnK2hqPopCallback): boolean;
		pop(pass: string): string | null;
		pop(pass: string, cb: OnK2hqPopCallback): boolean;

		// read
		read(): string | null;
		read(cb: K2hqReadCallback): boolean;
		read(pos: number): string | null;
		read(pos: number, cb: K2hqReadCallback): boolean;
		read(pos: number, pass: string): string | null;
		read(pos: number, pass: string, cb: K2hqReadCallback): boolean;

		// remove
		remove(count: number): number;
		remove(count: number, cb: OnK2hqRemoveCallback): boolean;
		remove(count: number, pass: string): number;
		remove(count: number, pass: string, cb: OnK2hqRemoveCallback): boolean;

		//-----------------------------------------------------
		// Methods (no callback)
		//-----------------------------------------------------
		// init(initialize)
		init(k2hnode: K2hNode, isFifo?: boolean): boolean;
		init(k2hnode: K2hNode, isFifo: boolean, prefix?: string): boolean;

		isEmpty(): boolean;
		dump(fd?: number): boolean;

		//-----------------------------------------------------
		// Emitter registration/unregistration
		//-----------------------------------------------------
		on(emitter: string, cb: OnK2hqEmitterCallback): boolean;
		onCount(cb: OnK2hqCountCallback): boolean;
		onPush(cb: OnK2hqPushCallback): boolean;
		onPop(cb: OnK2hqPopCallback): boolean;
		onRead(cb: OnK2hqReadCallback): boolean;
		onRemove(cb: OnK2hqRemoveCallback): boolean;

		off(emitter: string): boolean;
		offCount(): boolean;
		offPush(): boolean;
		offPop(): boolean;
		offRead(): boolean;
		offRemove(): boolean;

		//-----------------------------------------------------
		// Promise APIs(Currently no imprelemnts)
		//-----------------------------------------------------
		// [NOTE]
		// Although it is not currently implemented, here is an example definition:
		//
		// ex.	addSubkeysAsync?(key: string, subkeys: string[]): Promise<boolean>;
		//
	}

	//---------------------------------------------------------
	// Callback type for K2hKeyQueue class
	//---------------------------------------------------------
	// [NOTE]
	// We allow Error in case it might be changed to an Error object
	// in the future, but currently we do not return Error.
	//
	export type K2hkqCountCallback = (err?: Error | string | null, result?: number) => void;
	export type K2hkqPushCallback = (err?: Error | string | null, result?: boolean) => void;
	export type K2hkqPopCallback = (err?: Error | string | null, data?: [string, string] | null) => void;
	export type K2hkqReadCallback = (err?: Error | string | null, data?: [string, string] | null) => void;
	export type K2hkqRemoveCallback = (err?: Error | string | null, result?: number) => void;

	//---------------------------------------------------------
	// Emitter Callbask functions for K2hKeyQueue class
	//---------------------------------------------------------
	export type OnK2hkqEmitterCallback = (err?: string | null, ...args: any[]) => void;
	export type OnK2hkqCountCallback = (err?: string | null, result?: number) => void;
	export type OnK2hkqPushCallback = (err?: string | null, result?: boolean) => void;
	export type OnK2hkqPopCallback = (err?: string | null, data?: [string, string] | null) => void;
	export type OnK2hkqReadCallback = (err?: string | null, data?: [string, string] | null) => void;
	export type OnK2hkqRemoveCallback = (err?: string | null, result?: number) => void;

	//---------------------------------------------------------
	// K2hKeyQueue Class
	//---------------------------------------------------------
	export class K2hKeyQueue
	{
		// Constructor
		constructor();	// always no arguments

		//-----------------------------------------------------
		// Methods (Callback can be called)
		//-----------------------------------------------------
		// count
		count(): number;
		count(cb: K2hkqCountCallback): boolean;

		// push
		push(data: string, val: string, cb?: K2hkqPushCallback): boolean;
		push(data: string, val: string, pass: string, cb?: K2hkqPushCallback): boolean;
		push(data: string, val: string, pass: string, expire: number, cb?: K2hkqPushCallback): boolean;

		// pop
		pop(): [string, string] | null;
		pop(cb: OnK2hkqReadCallback): boolean;
		pop(pass: string): [string, string] | null;
		pop(pass: string, cb: OnK2hkqReadCallback): boolean;

		// read
		read(): [string, string] | null;
		read(cb: OnK2hkqReadCallback): boolean;
		read(pos: number | string): [string, string] | null;
		read(pos: number | string, cb: OnK2hkqReadCallback): boolean;
		read(pos: number | string, pass: string): [string, string] | null;
		read(pos: number | string, pass: string, cb: OnK2hkqReadCallback): boolean;

		// remove
		remove(count: number): number;
		remove(count: number, cb: OnK2hkqRemoveCallback): boolean;
		remove(count: number, pass: string): number;
		remove(count: number, pass: string, cb: OnK2hkqRemoveCallback): boolean;

		//-----------------------------------------------------
		// Methods (no callback)
		//-----------------------------------------------------
		// init(initialize)
		init(k2hnode: K2hNode, isFifo?: boolean): boolean;
		init(k2hnode: K2hNode, isFifo: boolean, prefix?: string): boolean;

		isEmpty(): boolean;
		dump(fd?: number): boolean;

		//-----------------------------------------------------
		// Emitter registration/unregistration
		//-----------------------------------------------------
		on(emitter: string, cb: OnK2hkqEmitterCallback): boolean;
		onCount(cb: OnK2hkqCountCallback): boolean;
		onPush(cb: OnK2hkqPushCallback): boolean;
		onPop(cb: OnK2hkqPopCallback): boolean;
		onRead(cb: OnK2hkqReadCallback): boolean;
		onRemove(cb: OnK2hkqRemoveCallback): boolean;

		off(emitter: string): boolean;
		offCount(): boolean;
		offPush(): boolean;
		offPop(): boolean;
		offRead(): boolean;
		offRemove(): boolean;

		//-----------------------------------------------------
		// Promise APIs(Currently no imprelemnts)
		//-----------------------------------------------------
		// [NOTE]
		// Although it is not currently implemented, here is an example definition:
		//
		// ex.	addSubkeysAsync?(key: string, subkeys: string[]): Promise<boolean>;
		//
	}

	//---------------------------------------------------------
	// K2hashFactoryType
	//---------------------------------------------------------
	export type K2hashFactoryType = {
		():				K2hNode;
		new():			K2hNode;
		K2hNode:		typeof K2hNode;
		K2hQueue:		typeof K2hQueue;
		K2hKeyQueue:	typeof K2hKeyQueue;
	};
} // end namespace k2hash

//-------------------------------------------------------------
// Exports
//-------------------------------------------------------------
//
// UMD global name
//
export as namespace k2hash;

//
// CommonJS default export
//
export = k2hash;

//
// Ambient module
//
// For "import", "import type" users(type-only export).
// This provides named type exports(type-only).
//
declare module 'k2hash'
{
	//
	// Default(value-level default import with esModuleInterop)
	//
	const _default: typeof k2hash;
	export default _default;

	//
	// Type named exports
	//
	// [NOTE]
	// ex. "import type { K2hNode } from 'k2hash'"
	//
	export type K2hNode				= k2hash.K2hNode;
	export type K2hQueue			= k2hash.K2hQueue;
	export type K2hKeyQueue			= k2hash.K2hKeyQueue;
	export type K2hashFactoryType	= k2hash.K2hashFactoryType;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
