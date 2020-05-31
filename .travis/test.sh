#!/bin/bash

set -ev

if [[ $TRAVIS_OS_NAME == 'linux' ]]; then
    if [[ $ARCH == "amd64" ]]; then
        ./bin/Linux_amd64_release/fxjs test
    fi
else # darwin
    if [[ $ARCH == "amd64" ]]; then
        ./bin/Darwin_amd64_release/fxjs test
    else
        ./bin/Darwin_i386_release/fxjs test
    fi
fi

exit 0;
