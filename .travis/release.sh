#!/bin/bash

set -ev

case "${TARGET_ARCH}" in
  i386)
    DIST_ARCH=x86
      ;;
  amd64)
    DIST_ARCH=x64
      ;;
  *)
    DIST_ARCH=$TARGET_ARCH
      ;;
esac

if [[ -z $TRAVIS_TAG && $TRAVIS_BRANCH == "beta" ]]; then
  TRAVIS_TAG=$(git describe --match "v[0-9]*.[0-9]*.[0-9]*")
  echo "TRAVIS_TAG is $TRAVIS_TAG";
fi

mkdir -p ${TRAVIS_TAG}

DIST_FILE=""

if [[ $TRAVIS_OS_NAME == 'linux' ]]; then # linux
  DIST_FILE=bin/Linux_${TARGET_ARCH}_release

  FIBJS_FILE=${DIST_FILE}/fxjs
  INSTALLER_FILE=${DIST_FILE}/installer.sh
  XZ_FILE=${DIST_FILE}/fxjs.xz
  GZ_FILE=${DIST_FILE}/fxjs.tar.gz

  cp ${FIBJS_FILE} ${TRAVIS_TAG}/fxjs-${TRAVIS_TAG}-linux-${DIST_ARCH}
  cp ${INSTALLER_FILE} ${TRAVIS_TAG}/installer-${TRAVIS_TAG}-linux-${DIST_ARCH}.sh
  cp ${XZ_FILE} ${TRAVIS_TAG}/fxjs-${TRAVIS_TAG}-linux-${DIST_ARCH}.xz
  cp ${GZ_FILE} ${TRAVIS_TAG}/fxjs-${TRAVIS_TAG}-linux-${DIST_ARCH}.tar.gz

else # darwin
  DIST_FILE=bin/Darwin_${TARGET_ARCH}_release

  FIBJS_FILE=${DIST_FILE}/fxjs
  INSTALLER_FILE=${DIST_FILE}/installer.sh
  XZ_FILE=${DIST_FILE}/fxjs.xz
  GZ_FILE=${DIST_FILE}/fxjs.tar.gz

  cp ${FIBJS_FILE} ${TRAVIS_TAG}/fxjs-${TRAVIS_TAG}-darwin-${DIST_ARCH}
  cp ${INSTALLER_FILE} ${TRAVIS_TAG}/installer-${TRAVIS_TAG}-darwin-${DIST_ARCH}.sh
  cp ${XZ_FILE} ${TRAVIS_TAG}/fxjs-${TRAVIS_TAG}-darwin-${DIST_ARCH}.xz
  cp ${GZ_FILE} ${TRAVIS_TAG}/fxjs-${TRAVIS_TAG}-darwin-${DIST_ARCH}.tar.gz

  if [[ $DIST_ARCH == 'x64' ]]; then
    echo "zip fullsrc..."
    sudo sh build clean
    zip -r ./${TRAVIS_TAG}/fullsrc.zip ./ -x *.git* ${TRAVIS_TAG} ${TRAVIS_TAG}/*
  fi
fi

exit 0;
