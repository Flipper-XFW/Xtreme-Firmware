import nextcloud_client
import json
import os

if __name__ == "__main__":
    client = nextcloud_client.Client(os.environ["NC_HOST"])
    client.login(os.environ["NC_USER"], os.environ["NC_PASS"])

    file = os.environ["NC_FILE"]
    path = os.environ["NC_PATH"] + "/" + file
    try:
        client.delete(path)
    except Exception:
        pass
    client.put_file(path, file)

    file = file.removesuffix(".tgz") + ".md"
    path = path.removesuffix(".tgz") + ".md"
    with open(os.environ['GITHUB_EVENT_PATH'], "r") as f:
        changelog = json.load(f)['pull_request']['body']
    with open(file, "w") as f:
        f.write(changelog)
    try:
        client.delete(path)
    except Exception:
        pass
    client.put_file(path, file)
