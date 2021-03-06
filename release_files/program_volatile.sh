#!/bin/bash
# Script to program software to DE2 over JTAG

SOF_FILE=de2_video_processor_volatile.sof
ELF_FILE=pong_volatile.elf

QUARTUS_SEARCH_DIRS="\
/opt/altera
/OPT/altera"

find_bin_path () {
	for dir in $QUARTUS_SEARCH_DIRS
	do
		result=$(find $dir -name $1 -type f | grep bin)
		if [ -n "$result" ]
		then
			echo $(dirname $result)
			return 0
		fi
	done
	return 1
}

find_lib_path () {
	for dir in $QUARTUS_SEARCH_DIRS
	do
		result=$(find $dir -name $1 -type f | grep -v "linux64")
		if [ -n "$result" ]
		then
			echo $(dirname $result)
			return 0
		fi
	done
	return 1
}

# Environment Setup
export LM_LICENSE_FILE=12000@lic.ece.ualberta.ca

QUARTUS_PATH=$(find_bin_path quartus)
EDS_PATH=$(find_bin_path eclipse-nios2)
EDS_TOOLS_PATH=$(find_bin_path nios2-elf-objcopy)
JTAG_LIB=$(find_lib_path libjtag_client.so)

export PATH=$PATH:$QUARTUS_PATH:$EDS_PATH:$EDS_TOOLS_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$JTAG_LIB

# Program board over JTAG
quartus_pgm -c "USB-Blaster" -m JTAG -o "P;$SOF_FILE"

# Launch code
nios2-download -g $ELF_FILE

echo "Done!"
