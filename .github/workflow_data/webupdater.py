import nextcloud_client
import requests
import json
import os

if __name__ == "__main__":
    client = nextcloud_client.Client(os.environ["NC_HOST"])
    _session = requests.session
    def session(*args, **kwargs):
        s = _session(*args, **kwargs)
        s.headers["User-Agent"] = os.environ["NC_USERAGENT"]
        return s
    requests.session = session
    client.login(os.environ["NC_USER"], os.environ["NC_PASS"])

    file = os.environ["ARTIFACT_TGZ"]
    path = f"XFW-Updater/{file}"
    try:
        client.delete(path)
    except Exception:
        pass
    client.put_file(path, file)

    file = file.removesuffix(".tgz") + ".md"
    path = path.removesuffix(".tgz") + ".md"
    try:
        client.delete(path)
    except Exception:
        pass
    client.put_file(path, file)

    version = os.environ['VERSION_TAG'].split("_")[0]
    files = (
        os.environ['ARTIFACT_TGZ'],
        os.environ['ARTIFACT_TGZ'].removesuffix(".tgz") + ".md"
    )
    for file in client.list("XFW-Updater"):
        if file.name.startswith(version) and file.name not in files:
            try:
                client.delete(file.path)
            except Exception:
                pass
