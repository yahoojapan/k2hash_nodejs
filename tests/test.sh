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
	echo "Usage:   $1 [--debuglevel(-d)	INFO/WARN/ERR/SILENT(default)] [-logfile(-l) <path>] Command"
	echo ""
	echo "Command: all          - All test"
	echo "         k2hash       - k2hash test"
	echo "         k2hqueue     - k2hqueue test"
	echo "         k2hkeyqueue  - k2hkeyqueue test"
	echo ""
}

CMD_PREFIX="unit_"
CMD_SUFFIX="_spec.js"
DEBUG_MODE=""
DEBUG_LOG=""
COMMAND=""

while [ $# -ne 0 ]; do
	if [ -z "$1" ]; then
		break

	elif [ "$1" = "-h" ] || [ "$1" = "-H" ] || [ "$1" = "--help" ] || [ "$1" = "--HELP" ]; then
		PrintUsage "${PRGNAME}"
		exit 0

	elif [ "$1" = "-d" ] || [ "$1" = "-D" ] || [ "$1" = "--debuglevel" ] || [ "$1" = "--DEBUGLEVEL" ]; then
		#
		# DEBUG option
		#
		if [ -n "${DEBUG_MODE}" ]; then
			echo "[ERROR] Already specified --debuglevel(-d) option (${DEBUG_MODE}), thus could not set debug level $1."
			exit 1
		fi
		shift
		if [ $# -eq 0 ]; then
			echo "[ERROR] --debuglevel(-d) option needs parameter(info/warn/err/silent)"
			exit 1
		fi

		if [ "$1" = "info" ] || [ "$1" = "INFO" ]; then
			DEBUG_MODE="INFO"
		elif [ "$1" = "wan" ] || [ "$1" = "WAN" ] || [ "$1" = "warn" ] || [ "$1" = "WARN" ] || [ "$1" = "warning" ] || [ "$1" = "WARNING" ]; then
			DEBUG_MODE="WAN"
		elif [ "$1" = "err" ] || [ "$1" = "ERR" ] || [ "$1" = "error" ] || [ "$1" = "ERROR" ]; then
			DEBUG_MODE="ERR"
		elif [ "$1" = "silent" ] || [ "$1" = "SILENT" ] || [ "$1" = "slt" ] || [ "$1" = "SLT" ]; then
			DEBUG_MODE="SILENT"
		else
			echo "[ERROR] Unknown --debuglevel(-d) option parameter(info/warn/err/silent) : $1"
			exit 1
		fi

	elif [ "$1" = "--logfile" ] || [ "$1" = "--LOGFILE" ] || [ "$1" = "-l" ] || [ "$1" = "-L" ]; then
		#
		# LOG FILE option
		#
		if [ -n "${DEBUG_LOG}" ]; then
			echo "[ERROR] Already specified --logfile(-l) option (${DEBUG_LOG}), thus could not set log file path $1."
			exit 1
		fi
		shift
		if [ $# -eq 0 ]; then
			echo "[ERROR] --logfile(-l) option needs parameter"
			exit 1
		fi
		DEBUG_LOG="$1"

	else
		#
		# Run test command
		#
		if [ -n "${COMMAND}" ]; then
			echo "[ERROR] Already specified command name(${COMMAND}), could not specify multi command $1"
			exit 1
		fi
		if ! COMMAND=$(CheckCommands "$1"); then
			echo "[ERROR] $1 is not command name"
			exit 1
		fi
	fi
	shift
done

#----------------------------------------------------------
# Set default parameter if needs
#----------------------------------------------------------
if [ -z "${COMMAND}" ]; then
	COMMAND="all"
fi
if [ -z "${DEBUG_MODE}" ]; then
	DEBUG_MODE="SILENT"
fi
if [ -z "${DEBUG_LOG}" ]; then
	DEBUG_LOG="/dev/null"
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
if ! npm install mocha; then
	echo "[ERROR] Could not install mocha."
	exit 1
fi
if [ -f "${SRCTOP}"/node_modules/.bin/mocha ]; then
	MOCHA_BIN="${SRCTOP}/node_modules/.bin/mocha"
else
	echo "[ERROR] Not found mocha program"
	exit 1
fi

if [ "${DEBUG_MODE}" = "INFO" ]; then
	echo "***** RUN *****"
	echo "NODE_PATH=${K2HASH_NODE_PATH} K2HDBGMODE=${DEBUG_MODE} K2HDBGFILE=${DEBUG_LOG} ${MOCHA_BIN} ${SCRIPTDIR}/${CMD_PREFIX}${COMMAND}${CMD_SUFFIX}"
	echo "***************"
fi
if ! NODE_PATH="${K2HASH_NODE_PATH}" K2HDBGMODE="${DEBUG_MODE}" K2HDBGFILE="${DEBUG_LOG}" "${MOCHA_BIN}" "${SCRIPTDIR}/${CMD_PREFIX}${COMMAND}${CMD_SUFFIX}"; then
	echo "[ERROR] Failed to run test"
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
