#!/bin/bash

shopt -s dotglob

source version.cfg

version () {
  # set version
  sed -i 's/CoolOS-v.*"/CoolOS-v'"$version"'"/g' fbt_options.py
  sed -i 's/CoolOS-v.*"/CoolOS-v'"$version"'"/g' scripts/version.py
  sed -i 's/CoolOS version .*"/CoolOS version '"$version"'"/g' applications/settings/about/about.c
}

clean () {
  printf "\n### Cleaning Up ###"

  if [ "$(/bin/ls -Aq applications_user)" ]; then
    printf "\nCleaning ./applications_user"
    sleep 2
    rm -rv applications_user/*
  fi

  if [ "$(/bin/ls -Aq assets/resources/apps)" ]; then
    printf "\nCleaning ./assets/resources/apps"
    sleep 2
    rm -rv assets/resources/apps/*
  fi

  printf "\nRunning fbt -c\n"
  sleep 1
  ./fbt -c
}

apps () {
  printf "\n### Building Apps ###\n"
  sleep 1
  ./fbt fap_dist
  rsync -r dist/f7-C/apps/ applications_user
}

firmware () {
  printf "\n### Building Firmware Package ###\n"
  sleep 1
  ./fbt updater_package
}

help () {
  cat << EOF
Usage: ./compile [FLAG]

With no FLAG(s), will clean build directories and build full firmware update package with apps (can be found in dist/f7-C)

  -c, --clean       cleans build directories and removes built applications
  -a, --apps        builds all apps using fbt (.faps can be found in dist/f7-C/apps)
  -f, --firmware    builds firmware updater package using fbt (can be found in dist/f7-C)
  -h, --help        shows this help

Written and provided by 01001100: <https://github.com/o1001100>
EOF
}

case "$1" in
  -c | --clean) clean; exit;;
  -a | --apps) version; apps; exit;;
  -f | --firmware) version; firmware; exit;;
  -h | --help) help; exit;;
esac

clean; apps; firmware; exit
