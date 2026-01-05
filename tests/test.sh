#!/bin/sh
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
# AUTHOR:   Takeshi Nakatani
# CREATE:   Fri 21 Oct 2016
# REVISION:
#

#==========================================================
# Common Variables
#==========================================================
PRGNAME=$(basename "$0")
SCRIPTDIR=$(dirname "$0")
SCRIPTDIR=$(cd "${SCRIPTDIR}" || exit 1; pwd)
SRCTOP=$(cd "${SCRIPTDIR}"/.. || exit 1; pwd)
if [ -f "${SRCTOP}"/src/binding.gyp ]; then
	BUILDDIR="${SRCTOP}/src/build/Release"
else
	BUILDDIR="${SRCTOP}/build/Release"
fi

CMD_PREFIX="unit_"
CMD_SUFFIX=".ts"
CMD_CJS_SUFFIX=".js"
SCRIPT_DEBUG_MODE=""
SCRIPT_DEBUG_LOG=""
SCRIPT_CJS_MODE=0
LIB_DEBUG_MODE=""
LIB_DEBUG_LOG=""
COMMAND=""

#==========================================================
# Utility functions
#==========================================================
#
# Escape sequence
#
SetColor()
{
	CBLD=$(printf '\033[1m')
	CREV=$(printf '\033[7m')
	CRED=$(printf '\033[31m')
	CYEL=$(printf '\033[33m')
	CGRN=$(printf '\033[32m')
	CDEF=$(printf '\033[0m')
}

UnSetColor()
{
	CBLD=""
	CREV=""
	CRED=""
	CYEL=""
	CGRN=""
	CDEF=""
}

if [ -t 1 ]; then
	SetColor
else
	UnSetColor
fi

#--------------------------------------------------------------
# Message functions
#--------------------------------------------------------------
# [NOTE]
# Do not use "${ENV:-<default>}", because when redirect file is not specify,
# ee need to use "/dev/stdout". The "/dev/stdout" is not always defined all
# OSes.
#
PRNTITLE()
{
	if [ -n "${SCRIPT_DEBUG_LOG}" ]; then
		{
			echo "${CGRN}---------------------------------------------------------------------${CDEF}"
			echo "${CGRN}${CREV}[TITLE]${CDEF} ${CGRN}$*${CDEF}"
			echo "${CGRN}---------------------------------------------------------------------${CDEF}"
		} >> "${SCRIPT_DEBUG_LOG}"
	else
		echo "${CGRN}---------------------------------------------------------------------${CDEF}"
		echo "${CGRN}${CREV}[TITLE]${CDEF} ${CGRN}$*${CDEF}"
		echo "${CGRN}---------------------------------------------------------------------${CDEF}"
	fi
}

PRNERR()
{
	if [ -n "${SCRIPT_DEBUG_LOG}" ]; then
		echo "${CBLD}${CRED}[ERROR]${CDEF} ${CRED}$*${CDEF}" >> "${SCRIPT_DEBUG_LOG}"
	else
		echo "${CBLD}${CRED}[ERROR]${CDEF} ${CRED}$*${CDEF}"
	fi
}

PRNWARN()
{
	if [ -n "${SCRIPT_DEBUG_LOG}" ]; then
		echo "${CBLD}${CYEL}[WARNING]${CDEF} ${CYEL}$*${CDEF}" >> "${SCRIPT_DEBUG_LOG}"
	else
		echo "${CBLD}${CYEL}[WARNING]${CDEF} ${CYEL}$*${CDEF}"
	fi
}

PRNINFO()
{
	if [ "${SCRIPT_DEBUG_MODE}" = "INFO" ]; then
		if [ -n "${SCRIPT_DEBUG_LOG}" ]; then
			echo "${CREV}[INFO]${CDEF} $*" >> "${SCRIPT_DEBUG_LOG}"
		else
			echo "${CREV}[INFO]${CDEF} $*"
		fi
	fi
}

PRNSUCCESS()
{
	if [ -n "${SCRIPT_DEBUG_LOG}" ]; then
		{
			echo ""
			echo "${CBLD}${CGRN}${CREV}[SUCCEED]${CDEF} ${CGRN}$*${CDEF}"
			echo ""
		} >> "${SCRIPT_DEBUG_LOG}"
	else
		echo ""
		echo "${CBLD}${CGRN}${CREV}[SUCCEED]${CDEF} ${CGRN}$*${CDEF}"
		echo ""
	fi
}

PRNFAILURE()
{
	if [ -n "${SCRIPT_DEBUG_LOG}" ]; then
		{
			echo ""
			echo "${CBLD}${CRED}${CREV}[FAILURE]${CDEF} ${CRED}$*${CDEF}"
			echo ""
		} >> "${SCRIPT_DEBUG_LOG}"
	else
		echo ""
		echo "${CBLD}${CGRN}${CREV}[SUCCEED]${CDEF} ${CGRN}$*${CDEF}"
		echo ""
	fi
}

#==========================================================
# Commands
#==========================================================
COMMANDS="
	all
	k2hash
	k2hqueue
	k2hkeyqueue
"

CheckCommands()
{
	for command in ${COMMANDS}; do
		if [ -n "$1" ] && [ "$1" = "${command}" ]; then
			echo "${command}"
			return 0
		fi
	done
	echo ""
	return 1
}

#==========================================================
# Parse arguments
#==========================================================
PrintUsage()
{
	echo "Usage:   $1 [--debuglevel(-d) <mode>] [--logfile(-l) <path>] [--libdebuglevel(-ld) <mode> [--liblogfile(-ll) <path>]] [--commonjs(-cjs)] Command"
	echo ""
	echo "Command:"
	echo "  all          Run all test"
	echo "  k2hash       Run only k2hash test"
	echo "  k2hqueue     Run only k2hqueue test"
	echo "  k2hkeyqueue  Run only k2hkeyqueue test"
	echo ""
	echo "Option:"
	echo "  --debuglevel(-d) <mode>     Specifies the debug level(INFO / ERR) for this script.(default: ERR)"
	echo "  --logfile(-l) <path>        Specifies the file path for writing this script output."
	echo "  --libdebuglevel(-ld) <mode> Specifies the debug level(INFO / WARN / ERR / SILENT) for K2HASH library.(default: SILENT)"
	echo "  --liblogfile(-ll) <path>    Specifies the file path for writing K2HASH library debug messages."
	echo "  --commonjs(-cjs)            Specifies that the test will be performed using CommonJS."
	echo ""
	echo "Environment:"
	echo "  TEST_SCRIPTTYPE             Specify \"commonjs(cjs)\" or \"typescript(ts)\".(default: typescript)"
	echo ""
	echo "Note:"
	echo "  The file path is required to output K2HASH library debug messages."
	echo "  If that file path is not specified, K2HASH library debug level will not be set."
	echo "  If \"--libdebuglevel(-ld)\" is specified but \"--liblogfile(-ll)\" is not,"
	echo "  the value of \"--logfile(-l)\" will be used instead."
	echo "  You can also specify \"K2HDBGMODE\" and \"K2HDBGFILE\" environment variables"
	echo "  (if \"--libdebuglevel(-ld)\" and \"--liblogfile(-ll)\" are not set)."
	echo ""
}

while [ $# -ne 0 ]; do
	if [ -z "$1" ]; then
		break

	elif echo "$1" | grep -q -i -e "^-h$" -e "^--help$"; then
		PrintUsage "${PRGNAME}"
		exit 0

	elif echo "$1" | grep -q -i -e "^-d$" -e "^--debuglevel$"; then
		#
		# DEBUG option
		#
		if [ -n "${SCRIPT_DEBUG_MODE}" ]; then
			PRNERR "Already specified --debuglevel(-d) option (${SCRIPT_DEBUG_MODE}), thus could not set debug level $1."
			exit 1
		fi
		shift
		if [ $# -eq 0 ]; then
			PRNERR "--debuglevel(-d) option needs parameter(info/warn/err/silent)"
			exit 1
		fi
		if echo "$1" | grep -q -i -e "^inf$" -e "^info$"; then
			SCRIPT_DEBUG_MODE="INFO"
		elif echo "$1" | grep -q -i -e "^err$" -e "^error$"; then
			SCRIPT_DEBUG_MODE="ERR"
		else
			PRNERR "Unknown --debuglevel(-d) option parameter(info/err) : $1"
			exit 1
		fi

	elif echo "$1" | grep -q -i -e "^-l$" -e "^--logfile$"; then
		#
		# LOG FILE option
		#
		if [ -n "${SCRIPT_DEBUG_LOG}" ]; then
			PRNERR "Already specified --logfile(-l) option (${SCRIPT_DEBUG_LOG}), thus could not set log file path $1."
			exit 1
		fi
		shift
		if [ $# -eq 0 ]; then
			PRNERR "--logfile(-l) option needs parameter"
			exit 1
		fi
		SCRIPT_DEBUG_LOG="$1"

	elif echo "$1" | grep -q -i -e "^-ld$" -e "^--libdebuglevel$"; then
		#
		# DEBUG option
		#
		if [ -n "${LIB_DEBUG_MODE}" ]; then
			PRNERR "Already specified --libdebuglevel(-ld) option (${LIB_DEBUG_MODE}), thus could not set debug level $1."
			exit 1
		fi
		shift
		if [ $# -eq 0 ]; then
			PRNERR "--libdebuglevel(-ld) option needs parameter(info/warn/err/silent)"
			exit 1
		fi
		if echo "$1" | grep -q -i "^info$"; then
			LIB_DEBUG_MODE="INFO"
		elif echo "$1" | grep -q -i -e "^wan$" -e "^warn$" -e "^warning$"; then
			LIB_DEBUG_MODE="WAN"
		elif echo "$1" | grep -q -i -e "^err$" -e "^error$"; then
			LIB_DEBUG_MODE="ERR"
		elif echo "$1" | grep -q -i -e "^silent$" -e "^slt$"; then
			LIB_DEBUG_MODE="SILENT"
		else
			PRNERR "Unknown --libdebuglevel(-ld) option parameter(info/warn/err/silent) : $1"
			exit 1
		fi

	elif echo "$1" | grep -q -i -e "^-ll$" -e "^--liblogfile$"; then
		#
		# LOG FILE option
		#
		if [ -n "${LIB_DEBUG_LOG}" ]; then
			PRNERR "Already specified --liblogfile(-ll) option (${LIB_DEBUG_LOG}), thus could not set log file path $1."
			exit 1
		fi
		shift
		if [ $# -eq 0 ]; then
			PRNERR "--liblogfile(-ll) option needs parameter"
			exit 1
		fi
		LIB_DEBUG_LOG="$1"

	elif echo "$1" | grep -q -i -e "^-cjs$" -e "^--commonjs$"; then
		#
		# CommonJS test
		#
		if [ "${SCRIPT_CJS_MODE}" -ne 0 ]; then
			PRNERR "Already specified --commonjs(-cjs) option."
			exit 1
		fi
		SCRIPT_CJS_MODE=1

	else
		#
		# Run test command
		#
		if [ -n "${COMMAND}" ]; then
			PRNERR "Already specified command name(${COMMAND}), could not specify multi command $1"
			exit 1
		fi
		if ! COMMAND=$(CheckCommands "$1"); then
			PRNERR "$1 is not command name"
			exit 1
		fi
	fi
	shift
done

#----------------------------------------------------------
# Set default parameter if needs
#----------------------------------------------------------
#
# Set Command
#
if [ -z "${COMMAND}" ]; then
	COMMAND="all"
fi
if [ "${COMMAND}" = "all" ]; then
	COMMAND="k2hash k2hqueue k2hkeyqueue"
fi

#
# Set from environments if not set option.
#
if [ -z "${LIB_DEBUG_MODE}" ] && [ -n "${K2HDBGMODE}" ]; then
	# other than "SILENT"
	if echo "${K2HDBGMODE}" | grep -q -i "^info$"; then
		LIB_DEBUG_MODE="INFO"
	elif echo "${K2HDBGMODE}" | grep -q -i -e "^wan$" -e "^warn$" -e "^warning$"; then
		LIB_DEBUG_MODE="WAN"
	elif echo "${K2HDBGMODE}" | grep -q -i -e "^err$" -e "^error$"; then
		LIB_DEBUG_MODE="ERR"
	fi
fi
if [ -z "${LIB_DEBUG_LOG}" ] && [ -n "${K2HDBGFILE}" ]; then
	LIB_DEBUG_LOG="${K2HDBGFILE}"
fi

#
# Check debug mode/file for K2HASH library
#
if [ -z "${LIB_DEBUG_MODE}" ]; then
	if [ -n "${LIB_DEBUG_LOG}" ]; then
		PRNERR "\"--libdebuglevel(-ld)\" or \"K2HDBGMODE\" is not set or \"SILENT\" is specified, but \"--liblogfile(-ll)\" or \"K2HDBGFILE\" is set."
		exit 1
	fi
	# Set default(=no logging)
	LIB_DEBUG_MODE="SILENT"
	LIB_DEBUG_LOG="/dev/null"
else
	if [ -z "${LIB_DEBUG_LOG}" ]; then
		# check script log file path
		if [ -z "${SCRIPT_DEBUG_LOG}" ]; then
			PRNERR "\"--libdebuglevel(-ld)\" or \"K2HDBGMODE\" is specified(other than \"SILENT\"), but \"--liblogfile(-ll)\" or \"K2HDBGFILE\" or \"SCRIPT_DEBUG_LOG\" is not set."
			exit 1
		fi
		LIB_DEBUG_LOG="${SCRIPT_DEBUG_LOG}"
	fi
fi

#
# Set debug mode
#
if [ -z "${SCRIPT_DEBUG_MODE}" ]; then
	SCRIPT_DEBUG_MODE="ERR"
fi
if [ -n "${SCRIPT_DEBUG_LOG}" ]; then
	UnSetColor
fi

#
# Check node version for CommonJS test
#
# [NOTE]
# When running tests in NodeJS 20 with TypeScript, Mocha will throw the
# error ERR_REQUIRE_CYCLE_MODULE.
# Currently, there is no workaround, so you will need to convert the test
# scripts to CommonJS beforehand and run them as CommonJS.
# Note that TypeScript tests are run in other NodeJS versions, so there
# is no problem.
#
if [ "${SCRIPT_CJS_MODE}" -eq 0 ]; then
	#
	# Check TEST_SCRIPTTYPE environment
	#
	if [ -n "${TEST_SCRIPTTYPE}" ]; then
		if echo "${TEST_SCRIPTTYPE}" | grep -q -i -e "commonjs" -e "cjs"; then
			PRNINFO "Environment TEST_SCRIPTTYPE value is CommonJS."
			SCRIPT_CJS_MODE=1
		elif echo "${TEST_SCRIPTTYPE}" | grep -q -i -e "typescript" -e "ts"; then
			PRNINFO "Environment TEST_SCRIPTTYPE value is Typescript."
		else
			PRNWARN "Environment TEST_SCRIPTTYPE value is unknown(${TEST_SCRIPTTYPE})."
		fi
	fi
fi
if [ "${SCRIPT_CJS_MODE}" -eq 0 ]; then
	#
	# Check NodeJS version
	#
	if ! NODE_MAJOR_VER=$(node -v 2>/dev/null | sed -e 's#^v##g' -e 's#\..*$##g') || [ -z "${NODE_MAJOR_VER}" ]; then
		PRNERR "Unknown node version."
		exit 1
	fi
	if [ "${NODE_MAJOR_VER}" = "20" ]; then
		PRNINFO "NodeJS is v20, so the test uses CommonJS."
		SCRIPT_CJS_MODE=1
	fi

	#
	# Check OS type
	#
	if [ -f /etc/os-release ] && grep '^ID[[:space:]]*=[[:space:]]*' /etc/os-release | grep -q -i 'alpine'; then
		PRNINFO "OS is alpine, so the test uses CommonJS."
		SCRIPT_CJS_MODE=1
	fi
fi

#----------------------------------------------------------
# node path(relative path from SRCTOP) for k2hash
#----------------------------------------------------------
if [ -n "${NODE_PATH}" ]; then
	K2HASH_NODE_PATH="${NODE_PATH}:"
fi
K2HASH_NODE_PATH="${K2HASH_NODE_PATH}${BUILDDIR}"

#==========================================================
# Executing(current at SRCTOP)
#==========================================================
cd "${SRCTOP}" || exit 1

#
# Check mocha path
#
if ! npm list mocha --depth=0 >/dev/null 2>&1; then
	PRNINFO "Not found mocha package, so try to install it."
	if ! npm install mocha >/dev/null 2>&1; then
		PRNERR "Could not install mocha."
		exit 1
	fi
fi
if [ -f "${SRCTOP}/node_modules/.bin/mocha" ]; then
	MOCHA_BIN="${SRCTOP}/node_modules/.bin/mocha"
else
	PRNERR "Not found mocha program"
	exit 1
fi

#
# Check ts-node
#
# [NOTE]
# This is not necessary in CommonJS mode, but the check is still performed.
#
if ! npm list ts-node --depth=0 >/dev/null 2>&1; then
	PRNINFO "Not found ts-node package, so try to install it."
	if ! npm install ts-node >/dev/null 2>&1; then
		PRNERR "Could not install ts-node."
		exit 1
	fi
fi

#
# Make mocha parameters
#
if [ "${SCRIPT_CJS_MODE}" -eq 0 ]; then
	#
	# TS test
	#
	if [ ! -f "${SRCTOP}/tests/esm_wrapper.mjs" ]; then
		PRNERR "Not found esm_wrapper.mjs file"
		exit 1
	fi
	PRINT_CJS_MODE=""
	ESM_LOADER_FILE="${SRCTOP}/tests/esm_wrapper.mjs"
	ESM_IMPORT_OPT="--import ${ESM_LOADER_FILE}"
	ESM_EXPR_OPT="--experimental-specifier-resolution=node"

	TEST_FILE_DIR_PATH="${SCRIPTDIR}"
	TEST_FILE_SUFFIX="${CMD_SUFFIX}"
else
	#
	# CJS test
	#
	PRINT_CJS_MODE=" (CommonJS)"
	ESM_IMPORT_OPT=""
	ESM_EXPR_OPT=""

	#
	# See. dirname in tsconfig.tests.json file
	#
	TESTS_DIRNAME="tests_cjs"
	TEST_FILE_DIR_PATH="${SRCTOP}/${TESTS_DIRNAME}"
	if [ ! -d "${TEST_FILE_DIR_PATH}" ]; then
		PRNINFO "Not found ${TEST_FILE_DIR_PATH} directory, thus try to run \"npm run build:ts:tests:cjs\""
		if [ -n "${SCRIPT_DEBUG_LOG}" ]; then
			if ! npm run build:ts:tests:cjs >>"${SCRIPT_DEBUG_LOG}" 2>&1; then
				PRNERR "Failed to run \"npm run build:ts:tests:cjs\" for creating ${TEST_FILE_DIR_PATH} directory and files."
				exit 1
			fi
		else
			if ! npm run build:ts:tests:cjs; then
				PRNERR "Failed to run \"npm run build:ts:tests:cjs\" for creating ${TEST_FILE_DIR_PATH} directory and files."
				exit 1
			fi
		fi
	fi
	TEST_FILE_SUFFIX="${CMD_CJS_SUFFIX}"
fi

#
# Run command
#
TEST_RESULT=0
for ONE_COMMAND in ${COMMAND}; do
	PRNTITLE "Test : ${ONE_COMMAND}${PRINT_CJS_MODE}"

	PRNINFO "Run : NODE_PATH=${K2HASH_NODE_PATH} K2HDBGMODE=${LIB_DEBUG_MODE} K2HDBGFILE=${LIB_DEBUG_LOG} node ${ESM_IMPORT_OPT} ${ESM_EXPR_OPT} ${MOCHA_BIN} --extensions ts ${TEST_FILE_DIR_PATH}/${CMD_PREFIX}${ONE_COMMAND}${TEST_FILE_SUFFIX}"

	_ONE_RESULT=0
	if [ -n "${SCRIPT_DEBUG_LOG}" ]; then
		if ! /bin/sh -c "NODE_PATH=${K2HASH_NODE_PATH} K2HDBGMODE=${LIB_DEBUG_MODE} K2HDBGFILE=${LIB_DEBUG_LOG} node ${ESM_IMPORT_OPT} ${ESM_EXPR_OPT} ${MOCHA_BIN} --extensions ts ${TEST_FILE_DIR_PATH}/${CMD_PREFIX}${ONE_COMMAND}${TEST_FILE_SUFFIX} >>${SCRIPT_DEBUG_LOG} 2>&1"; then
			_ONE_RESULT=1
		fi
	else
		if ! /bin/sh -c "NODE_PATH=${K2HASH_NODE_PATH} K2HDBGMODE=${LIB_DEBUG_MODE} K2HDBGFILE=${LIB_DEBUG_LOG} node ${ESM_IMPORT_OPT} ${ESM_EXPR_OPT} ${MOCHA_BIN} --extensions ts ${TEST_FILE_DIR_PATH}/${CMD_PREFIX}${ONE_COMMAND}${TEST_FILE_SUFFIX}"; then
			_ONE_RESULT=1
		fi
	fi
	if [ "${_ONE_RESULT}" -ne 0 ]; then
		PRNFAILURE "Test: ${ONE_COMMAND}"
		TEST_RESULT=1
	else
		PRNSUCCESS "Test: ${ONE_COMMAND}"
	fi
done
if [ "${TEST_RESULT}" -ne 0 ]; then
	PRNERR "All tests are complete but there are errors."
	exit 1
fi

exit 0

#
# Local variables:
# tab-width: 4
# c-basic-offset: 4
# End:
# vim600: noexpandtab sw=4 ts=4 fdm=marker
# vim<600: noexpandtab sw=4 ts=4
#
