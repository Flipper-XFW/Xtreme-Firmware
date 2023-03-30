#!/usr/bin/env python
import requests
import json
import sys
import os


if __name__ == "__main__":
    with open(os.environ["GITHUB_EVENT_PATH"], "r") as f:
        event = json.load(f)

    webhook = "DEV_DISCORD_WEBHOOK"
    title = ""
    desc = ""
    url = ""
    color = 0

    match os.environ["GITHUB_EVENT_NAME"]:
        case "push":
            push = event
            count = len(push['commits'])
            branch = push["ref"].removeprefix("refs/heads/")
            change = "Force Push" if push["forced"] and not count else f"{count} New Commit{'' if count == 1 else 's'}"
            desc = f"[**{change}**]({push['compare']}) on [{branch}]({push['repository']['html_url']}/tree/{branch}) branch\n"
            for commit in push["commits"]:
                desc += f"\n[`{commit['id'][:7]}`]({commit['url']}): {commit['message'].splitlines()[0]} - [__{commit['author']['username']}__](https://github.com/{commit['author']['username']})"
            url = push["compare"]
            color = 16711680 if push["forced"] else 3669797

        case "pull_request":
            pr = event["pull_request"]
            url = pr["html_url"]
            branch = pr['base']['ref'] + ('' if pr['base']['full_name'] != pr['head']['full_name'] else f" <- {pr['head']['ref']}")
            title = f"Pull Request {event['action'].title()} ({branch}): {pr['title']}"
            if event['action'] != "closed":
                max_len = 2045
                desc = pr["body"][:max_len] + ("..." if len(pr["body"]) > max_len else "")
                color = 3669797
            else:
                color = 16711680

        # case "release":
        #     webhook = "DEV_DISCORD_WEBHOOK"
            # count = len(event['commits'])
            # branch = event["ref"].removeprefix("refs/heads/")
            # change = "Force Push" if event["forced"] and not count else f"{count} New Commit{'' if count == 1 else 's'}"
            # desc += f"[**{change}**]({event['compare']}) on [{branch}]({event['repository']['html_url']}/tree/{branch}) branch\n"
            # for commit in event["commits"]:
            #     desc += f"\n[`{commit['id'][:7]}`]({commit['url']}): {commit['message'].splitlines()[0]} - [__{commit['author']['username']}__](https://github.com/{commit['author']['username']})"
            # url = event["compare"]
            # color = 16711680 if event["forced"] else 3669797

        case _:
            sys.exit(1)

    requests.post(
        os.environ[webhook],
        headers={
            "Accept": "application/json",
            "Content-Type": "application/json"
        },
        json={
            "content": None,
            "embeds": [
                {
                    "title": title,
                    "description": desc,
                    "url": url,
                    "color": color,
                    "author": {
                        "name": event["sender"]["login"],
                        "url": event["sender"]["html_url"],
                        "icon_url": event["sender"]["avatar_url"]
                    }
                }
            ],
            "attachments": []  # TODO: attach artifacts
        }
    )
