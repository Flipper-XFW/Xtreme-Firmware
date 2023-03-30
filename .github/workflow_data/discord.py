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
    fields = []

    match os.environ["GITHUB_EVENT_NAME"]:
        case "push":
            push = event
            count = len(push['commits'])
            branch = push["ref"].removeprefix("refs/heads/")
            change = "Force Push" if push["forced"] and not count else f"{count} New Commit{'' if count == 1 else 's'}"
            desc = f"[**{change}**]({push['compare']}) on [{branch}]({push['repository']['html_url']}/tree/{branch}) branch\n"
            for commit in push["commits"][:10]:
                msg = commit['message'].splitlines()[0]
                msg = msg[:50] + ("..." if len(msg) > 50 else "")
                desc += f"\n[`{commit['id'][:7]}`]({commit['url']}): {msg} - [__{commit['author']['username']}__](https://github.com/{commit['author']['username']})"
            if count > 10:
                desc += f"\n+ {count - 10} more commits"
            url = push["compare"]
            color = 16711680 if push["forced"] else 3669797

        case "pull_request":
            pr = event["pull_request"]
            url = pr["html_url"]
            branch = pr['base']['ref'] + ('' if pr['base']['full_name'] != pr['head']['full_name'] else f" <- {pr['head']['ref']}")
            name = pr['title'][:50] + ("..." if len(pr['title']) > 50 else "")
            title = f"Pull Request {event['action'].title()} ({branch}): {name}"
            if event['action'] == "closed":
                color = 16711680
            else:
                color = 3669797
                desc = pr["body"][:2045] + ("..." if len(pr["body"]) > 2045 else "")
                fields.append({
                    "name": "Files",
                    "value": str(pr["changed_files"]),
                    "inline": True
                })
                fields.append({
                    "name": "Added",
                    "value": "+" + str(pr["additions"]),
                    "inline": True
                })
                fields.append({
                    "name": "Removed",
                    "value": "-" + str(pr["deletions"]),
                    "inline": True
                })

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
                    "title": title[:256],
                    "description": desc[:2048],
                    "url": url,
                    "color": color,
                    "fields": fields[:25],
                    "author": {
                        "name": event["sender"]["login"][:256],
                        "url": event["sender"]["html_url"],
                        "icon_url": event["sender"]["avatar_url"]
                    }
                }
            ],
            "attachments": []  # TODO: attach artifacts
        }
    )
