#!/bin/bash

export VERSION_TAG="$(python -c 'import fbt_options; print(fbt_options.DIST_SUFFIX, end="")')"
echo "VERSION_TAG=${VERSION_TAG}" >> $GITHUB_ENV
