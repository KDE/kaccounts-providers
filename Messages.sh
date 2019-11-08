#! /usr/bin/env bash

find . -name "*.provider.in" -o -name "*.service.in" -type f | xargs -n1 intltool-extract -l -type="gettext/xml"
$XGETTEXT `find . -name "*.h" -o -name "*.cpp"` -c -kN_ -kC_:1c,2 -o $podir/kaccounts-providers.pot
rm -rf ./tmp/
