#! /usr/bin/env bash

find . -name "*.provider" -o -name "*.service" -type f | xargs -n1 intltool-extract -l -type="gettext/xml"
$XGETTEXT `find . -name "*.h"` -c -a -o $podir/kaccounts-providers.pot
rm -rf ./tmp/
