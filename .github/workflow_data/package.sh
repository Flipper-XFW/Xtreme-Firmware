#!/bin/bash
export ARTIFACT_DIR="${VERSION_TAG}"

export ARTIFACT_TGZ="${VERSION_TAG}.tgz"
export ARTIFACT_ZIP="${VERSION_TAG}.zip"
export ARTIFACT_SDK="${VERSION_TAG}-sdk.zip"
cd dist/${DEFAULT_TARGET}-*
mv ${DEFAULT_TARGET}-update-* ${ARTIFACT_DIR}
tar --format=ustar -czvf ../../${ARTIFACT_TGZ} ${ARTIFACT_DIR}
cd ${ARTIFACT_DIR}
7z a ../../../${ARTIFACT_ZIP} .
cd ..
mv flipper-z-${DEFAULT_TARGET}-sdk-*.zip ../../${ARTIFACT_SDK}
cd ../..

echo "ARTIFACT_TGZ=${ARTIFACT_TGZ}" >> $GITHUB_ENV
echo "ARTIFACT_ZIP=${ARTIFACT_ZIP}" >> $GITHUB_ENV
echo "ARTIFACT_SDK=${ARTIFACT_SDK}" >> $GITHUB_ENV
