#!/usr/bin/env python
import requests
import json
import os


if __name__ == "__main__":
    with open(os.environ["GITHUB_EVENT_PATH"], "r") as f:
        push = json.load(f)

    count = len(push['commits'])
    branch = push["ref"].removeprefix("refs/heads/")
    change = "Force Push" if push["forced"] and not count else f"{count} New Commit{'' if count == 1 else 's'}"
    desc = f"[**{change}**]({push['compare']}) on [{branch}]({push['repository']['html_url']}/tree/{branch}) branch\n"

    for commit in push["commits"]:
        desc += f"\n[`{commit['id'][:7]}`]({commit['url']}): {commit['message'].splitlines()[0]} - [__{commit['author']['username']}__](https://github.com/{commit['author']['username']})"

    requests.post(
        os.environ["DEV_DISCORD_WEBHOOK"],
        headers={
            "Accept": "application/json",
            "Content-Type": "application/json"
        },
        json={
            "content": None,
            "embeds": [
                {
                    "description": desc,
                    "url": push["compare"],
                    "color": 16711680 if push["forced"] else 3669797,
                    "author": {
                        "name": push["sender"]["login"],
                        "url": push["sender"]["html_url"],
                        "icon_url": push["sender"]["avatar_url"]
                    }
                }
            ],
            "attachments": []
        }
    )
