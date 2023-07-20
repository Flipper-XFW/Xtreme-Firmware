#!/usr/bin/env python
import datetime as dt
import requests
import pathlib
import json
import sys
import os
import re

if __name__ == "__main__":
    with open(os.environ["GITHUB_EVENT_PATH"], "r") as f:
        event = json.load(f)

    release = requests.get(
        event["repository"]["releases_url"].rsplit("{/")[0] + "/latest",
        headers={
            "Accept": "application/vnd.github.v3+json",
            "Authorization": f"token {os.environ['GITHUB_TOKEN']}"
        }
    ).json()

    artifacts = {
        os.environ['ARTIFACT_TGZ']: "application/gzip",
        os.environ['ARTIFACT_ZIP']: "application/zip",
        os.environ['ARTIFACT_SDK']: "application/zip",
    }

    for asset in release["assets"]:
        req = requests.delete(
            asset["url"],
            headers={
                "Accept": "application/vnd.github.v3+json",
                "Authorization": f"token {os.environ['GITHUB_TOKEN']}"
            }
        )
        if not req.ok:
            print(f"{req.url = }\n{req.status_code = }\n{req.content = }")
            sys.exit(1)

    for artifact, mediatype in artifacts.items():
        req = requests.post(
            release["upload_url"].rsplit("{?", 1)[0],
            headers={
                "Accept": "application/vnd.github.v3+json",
                "Authorization": f"token {os.environ['GITHUB_TOKEN']}",
                "Content-Type": mediatype
            },
            params={
                "name": artifact
            },
            data=pathlib.Path(artifact).read_bytes()
        )
        if not req.ok:
            print(f"{req.url = }\n{req.status_code = }\n{req.content = }")
            sys.exit(1)

    hotfix_time = dt.datetime.now().strftime(r"%d-%m-%Y %H:%M")
    hotfix_desc = event['pull_request']['body']
    hotfix = f"- `{hotfix_time}`: {hotfix_desc}\n"

    body = release["body"]
    body = re.sub(
        r"(https://github\.com/Flipper-XFW/Xtreme-Firmware/releases/download/[A-Za-z0-9_-]+?/)[A-Za-z0-9_-]+",
        r"\1" + os.environ['VERSION_TAG'],
        body
    )
    body = body.replace("<!--- <HOTFIXES>\n", "")
    body = body.replace("\n<HOTFIXES> -->", "")
    insert = body.find("\n  [//]: <NEXT_HOTFIX>\n")
    body = body[:insert] + hotfix + body[insert:]

    req = requests.patch(
        release["url"],
        headers={
            "Accept": "application/vnd.github.v3+json",
            "Authorization": f"token {os.environ['GITHUB_TOKEN']}"
        },
        json={
            "body": body
        }
    )
    if not req.ok:
        print(f"{req.url = }\n{req.status_code = }\n{req.content = }")
        sys.exit(1)

    changelog = body.split("## 🚀 Changelog", 1)[1]
    with open(os.environ["ARTIFACT_TGZ"].removesuffix(".tgz") + ".md", "w") as f:
        f.write(changelog.strip() + "\n\n")
