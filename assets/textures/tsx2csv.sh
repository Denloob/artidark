#!/bin/bash

# csv structure: id,texturePath,classID,solid,callback,
xq -q tile -a id "$1" | paste -d ',' - <(xq -q img -a source "$1") | awk 'BEGIN {print "-1,,0,0,,"} {print $1",0,0,,"}'
