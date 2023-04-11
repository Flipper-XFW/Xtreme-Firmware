#!/usr/bin/env python
import requests
import json
import sys
import os


if __name__ == "__main__":
    with open(os.environ["GITHUB_EVENT_PATH"], "r") as f:
        event = json.load(f)

    webhook = "DEV_DISCORD_WEBHOOK"
    title = desc = url = ""
    color = 0
    fields = []

    match os.environ["GITHUB_EVENT_NAME"]:
        case "push":
            count = len(event["commits"])
            branch = event["ref"].removeprefix("refs/heads/")
            change = (
                "Force Push"
                if event["forced"] and not count
                else f"{count} New Commit{'' if count == 1 else 's'}"
            )
            desc = f"[**{change}**]({event['compare']}) | [{branch}]({event['repository']['html_url']}/tree/{branch})\n"
            for commit in event["commits"][:10]:
                msg = commit['message'].splitlines()[0]
                msg = msg[:50] + ("..." if len(msg) > 50 else "")
                desc += f"\n[`{commit['id'][:7]}`]({commit['url']}): {msg} - [__{commit['author']['username']}__](https://github.com/{commit['author']['username']})"
            if count > 10:
                desc += f"\n+ {count - 10} more commits"
            url = event["compare"]
            color = 16723712 if event["forced"] else 3669797

        case "pull_request":
            pr = event["pull_request"]
            url = pr["html_url"]
            branch = pr["base"]["ref"] + (
                ""
                if pr["base"]["full_name"] != pr["head"]["full_name"]
                else f" <- {pr['head']['ref']}"
            )
            name = pr["title"][:50] + ("..." if len(pr["title"]) > 50 else "")
            title = f"Pull Request {event['action'].title()} ({branch}): {name}"
            match event["action"]:
                case "opened":
                    desc = (event["body"][:2045] + "...") if len(event["body"]) > 2048 else event["body"]
                    color = 3669797

                    fields.append(
                        {
                            "name": "Changed Files:",
                            "value": str(pr["changed_files"]),
                            "inline": True,
                        }
                    )
                    fields.append(
                        {
                            "name": "Added:",
                            "value": "+" + str(pr["additions"]),
                            "inline": True,
                        }
                    )
                    fields.append(
                        {
                            "name": "Removed:",
                            "value": "-" + str(pr["deletions"]),
                            "inline": True,
                        }
                    )

                case "closed":
                    color = 16723712
                case "reopened":
                    color = 16751872
                case _:
                    sys.exit(1)

        case "release":
            match event["action"]:
                case "published":
                    webhook = "DEV_DISCORD_WEBHOOK"
                    color = 13845998
                    title = f"New Release published: {event['name']}"
                    desc += f"Changelog:"

                    changelog = "".join(
                        event["body"]
                        .split("Changelog")[1]
                        .split("<!---")[0]
                        .split("###")
                    )
                    downloads = [
                        option
                        for option in [
                            Type.replace("\n\n>", "")
                            for Type in event["body"]
                            .split("Download\n>")[1]
                            .split("### ")[:3]
                        ]
                        if option
                    ]

                    for category in changelog:
                        group = category.split(":")[0].replace(" ", "")
                        data = category.split(":")[1:].join(":")
                        fields.append(
                            {
                                "name": {group},
                                "value": {
                                    (data[:2045] + "...") if len(data) > 2048 else data
                                },
                            }
                        )
                        fields.append(
                            {
                                "name": "Downloads:",
                                "value": "\n".join(downloads),
                                "inline": True,
                            }
                        )
                case _:
                    sys.exit(1)

        case _:
            sys.exit(1)

    requests.post(
        os.environ[webhook],
        headers={"Accept": "application/json", "Content-Type": "application/json"},
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
                        "icon_url": event["sender"]["avatar_url"],
                    },
                }
            ],
            "attachments": [],
        },
    )
