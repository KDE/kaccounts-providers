#! /usr/bin/env bash

# XML extraction can't use -C,
# other attributes will be override by second invocation.
$XGETTEXT_PROGRAM `find . -name "*.provider.in"` \
    --force-po \
    -o "$podir/kaccounts-providers.pot"
$XGETTEXT_PROGRAM `find . -name "*.service.in"` \
    --force-po -j --its=services/accounts-service.its \
    -o "$podir/kaccounts-providers.pot"
$XGETTEXT `find . -name "*.h" -o -name "*.cpp" -o -name "*.qml"` \
    -j -c -kN_ -kC_:1c,2 \
    -o "$podir/kaccounts-providers.pot"
