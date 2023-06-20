#!/bin/sh

# You can't call OverStrike directly, rather, call it via runit.sh.
# > runit.sh 'cmd-line-parameters'
#
# Shell notes:
# - This script is provided for "sh" shell. If you need to use
# a different shell feel free to edit the script as required.
# Remember to change the first line to invoke that shell, for
# example, replace /bin/sh with /bin/bash
#
# - In most environments $0 returns the path and name of this
# script, but that is not guaranteed to be true. If using the
# bash shell, it is more reliable to define RUN_DIR like this:
# RUN_DIR=$(dirname $(readlink -f BASH_SOURCE[0]))
#
# - Enclosing whole parameter list in quotes is recommended, like this:
#
#    > runit.sh 'cmd-line-parameters'
#

RUN_DIR=$(dirname $(readlink -f $0))
export LD_LIBRARY_PATH=$RUN_DIR/links
$LD_LIBRARY_PATH/ld-linux-x86-64.so.2 --library-path $LD_LIBRARY_PATH $RUN_DIR/OverStrike $@

