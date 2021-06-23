#!/usr/bin/bash
# Copyright © 2020-2021 Kontain Inc. All rights reserved.
#
# Kontain Inc CONFIDENTIAL
#
#  This file includes unpublished proprietary source code of Kontain Inc. The
#  copyright notice above does not evidence any actual or intended publication of
#  such source code. Disclosure of this source code or any related proprietary
#  information is strictly prohibited without the express written permission of
#  Kontain Inc.

missing_count=0
feature_str=$(lscpu | grep Flags | cut -d':' -f 2-)
read -ra feature_arr <<< "$feature_str"
for feature in pti pcid invpcid xsave
do
	feature_found=false
	for cpu_feature in "${feature_arr[@]}"
	do
		if [ "$cpu_feature" == "$feature" ]; then
			feature_found="true"
		fi
	done
	if [ "$feature_found" == "false" ]; then
		missing_count=$((missing_count + 1))
		echo "$feature missing from cpu"
	fi
done

if [ "$missing_count" -eq 0 ]; then
	echo "OK"
else
	echo "Not OK"
fi
exit $missing_count
