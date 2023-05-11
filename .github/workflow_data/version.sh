#!/bin/bash

export VERSION_TAG="$(grep -o "DIST_SUFFIX = .*" fbt_options.py | cut -d'"' -f2)"
echo "VERSION_TAG=${VERSION_TAG}" >> $GITHUB_ENV
