#!/bin/bash
ls -l /usr/bin | awk '$1~"^-"&&$9~"^b" {printf "%s %s %s\n",$9,$3,$1}' | sort >output
chmod o=r-- output

