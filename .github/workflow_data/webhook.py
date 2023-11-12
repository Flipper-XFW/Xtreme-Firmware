#!/usr/bin/env python
import requests
import json
import sys
import os


if __name__ == "__main__":
    with open(os.environ["GITHUB_EVENT_PATH"], "r") as f:
        event = json.load(f)

    webhook = "DEV_WEBHOOK"
    title = desc = url = ""
    color = 0
    fields = []

    match os.environ["GITHUB_EVENT_NAME"]:
        case "push":
            webhook = "BUILD_WEBHOOK"
            count = len(event["commits"])
            if count == 20:
                count = int(requests.get(
                    event["compare"].replace("github.com", "api.github.com/repos"),
                    headers={
                        "Accept": "application/vnd.github.v3+json",
                        "Authorization": f"token {os.environ['GITHUB_TOKEN']}"
                    }
                ).json()["total_commits"])
            branch = event["ref"].removeprefix("refs/heads/")
            change = (
                "Force Push"
                if event["forced"] and not count
                else f"{count} New Commit{'' if count == 1 else 's'}"
            )
            desc = f"[**{change}**]({event['compare']}) | [{branch}]({event['repository']['html_url']}/tree/{branch})\n"
            for i, commit in enumerate(event["commits"]):
                msg = commit['message'].splitlines()[0].replace("`", "")
                msg = msg[:50] + ("..." if len(msg) > 50 else "")
                desc += f"\n[`{commit['id'][:7]}`]({commit['url']}): {msg} - [__{commit['author'].get('username')}__](https://github.com/{commit['author'].get('username')})"
                if len(desc) > 2020:
                    desc = desc.rsplit("\n", 1)[0] + f"\n+ {count - i} more commits"
                    break
            url = event["compare"]
            color = 16723712 if event["forced"] else 3669797

        # case "pull_request":
        #     pr = event["pull_request"]
        #     url = pr["html_url"]
        #     branch = pr["base"]["ref"] + (
        #         ""
        #         if pr["base"]["repo"]["full_name"] != pr["head"]["repo"]["full_name"]
        #         else f" <- {pr['head']['ref']}"
        #     )
        #     name = pr["title"][:50] + ("..." if len(pr["title"]) > 50 else "")
        #     title = f"Pull Request {event['action'].title()} ({branch}): {name}"
        #     match event["action"]:
        #         case "opened":
        #             desc = (pr["body"][:2045] + "...") if len(pr["body"]) > 2048 else pr["body"]
        #             color = 3669797

        #             fields.append(
        #                 {
        #                     "name": "Changed Files:",
        #                     "value": str(pr["changed_files"]),
        #                     "inline": True,
        #                 }
        #             )
        #             fields.append(
        #                 {
        #                     "name": "Added:",
        #                     "value": "+" + str(pr["additions"]),
        #                     "inline": True,
        #                 }
        #             )
        #             fields.append(
        #                 {
        #                     "name": "Removed:",
        #                     "value": "-" + str(pr["deletions"]),
        #                     "inline": True,
        #                 }
        #             )

        #         case "closed":
        #             color = 16723712
        #         case "reopened":
        #             color = 16751872
        #         case _:
        #             sys.exit(1)

        case "release":
            match event["action"]:
                case "published":
                    webhook = "RELEASE_WEBHOOK"
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

        case "workflow_run":
            run = event["workflow_run"]
            url = run["html_url"]
            title = "Workflow "
            match run["conclusion"]:
                case "action_required":
                    title += "Requires Attention"
                    color = 16751872
                case "failure":
                    title += "Failed"
                    color = 16723712
                case _:
                    sys.exit(0)
            title += f": {run['name']}"

        case "issues":
            issue = event["issue"]
            url = issue["html_url"]
            name = issue["title"][:50] + ("..." if len(issue["title"]) > 50 else "")
            title = f"Issue {event['action'].title()}: {name}"
            match event["action"]:
                case "opened":
                    desc = (issue["body"][:2045] + "...") if len(issue["body"]) > 2048 else issue["body"]
                    color = 3669797
                case "closed":
                    color = 16723712
                case "reopened":
                    color = 16751872
                case _:
                    sys.exit(1)

        case "issue_comment":
            comment = event["comment"]
            issue = event["issue"]
            url = comment["html_url"]
            title = f"New Comment on Issue: {issue['title']}"
            color = 3669797
            desc = (comment["body"][:2045] + "...") if len(comment["body"]) > 2048 else comment["body"]

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
