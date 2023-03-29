import nextcloud_client
import os

if __name__ == "__main__":
    client = nextcloud_client.Client(os.environ["NC_HOST"])
    client.login(os.environ["NC_USER"], os.environ["NC_PASS"])
    file = os.environ["NC_FILE"]
    path = os.environ["NC_PATH"] + file
    try:
        client.delete(path)
    except Exception:
        pass
    client.put_file(path, file)
    share_link = client.share_file_with_link(path).get_link()
    download_link = share_link.rstrip("/") + "/download/" + file
    print(download_link, end="")
