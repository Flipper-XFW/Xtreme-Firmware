#!/usr/bin/env python
import json
import os

if __name__ == "__main__":
    notes_path = '.github/workflow_data/release.md'
    with open(os.environ['GITHUB_EVENT_PATH'], "r") as f:
        changelog = json.load(f)['pull_request']['body']
    with open(notes_path, "r") as f:
        template = f.read()
    notes = template.format(
        ARTIFACT_TGZ=os.environ['ARTIFACT_TGZ'],
        ARTIFACT_ZIP=os.environ['ARTIFACT_ZIP'],
        VERSION_TAG=os.environ['VERSION_TAG'],
        CHANGELOG=changelog
    )
    with open(notes_path, "w") as f:
        f.write(notes)
    with open(os.environ["ARTIFACT_TGZ"].removesuffix(".tgz") + ".md", "w") as f:
        f.write(changelog.strip() + "\n\n")
