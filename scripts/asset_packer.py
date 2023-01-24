#!/usr/bin/env python
from PIL import Image, ImageOps
import heatshrink2
import pathlib
import shutil
import struct
import typing
import time
import io


def convert_bm(img: "Image.Image | pathlib.Path") -> bytes:
    if not isinstance(img, Image.Image):
        img = Image.open(img)

    with io.BytesIO() as output:
        img = img.convert("1")
        img = ImageOps.invert(img)
        img.save(output, format="XBM")
        xbm = output.getvalue()

    f = io.StringIO(xbm.decode().strip())
    data = f.read().strip().replace("\n", "").replace(" ", "").split("=")[1][:-1]
    data_str = data[1:-1].replace(",", " ").replace("0x", "")
    data_bin = bytearray.fromhex(data_str)

    data_encoded_str = heatshrink2.compress(data_bin, window_sz2=8, lookahead_sz2=4)
    data_enc = bytearray(data_encoded_str)
    data_enc = bytearray([len(data_enc) & 0xFF, len(data_enc) >> 8]) + data_enc

    if len(data_enc) < len(data_bin) + 1:
        return b"\x01\x00" + data_enc
    else:
        return b"\x00" + data_bin


def convert_bmx(img: "Image.Image | pathlib.Path") -> bytes:
    if not isinstance(img, Image.Image):
        img = Image.open(img)

    data = struct.pack("<II", *img.size)
    data += convert_bm(img)
    return data


def pack(input: "str | pathlib.Path", output: "str | pathlib.Path", logger: typing.Callable):
    input = pathlib.Path(input)
    output = pathlib.Path(output)
    output.mkdir(parents=True, exist_ok=True)
    for source in input.iterdir():
        if source == output:
            continue
        if not source.is_dir():
            continue

        logger(f"Packing {source.name}... ")
        packed = output / source.name
        if packed.exists():
            try:
                if packed.is_dir():
                    shutil.rmtree(packed, ignore_errors=True)
                else:
                    packed.unlink()
            except Exception:
                pass

        packed.mkdir(parents=True, exist_ok=True)

        if (source / "Anims/manifest.txt").exists():
            (packed / "Anims").mkdir(parents=True, exist_ok=True)
            shutil.copyfile(source / "Anims/manifest.txt", packed / "Anims/manifest.txt")
            for anim in (source / "Anims").iterdir():
                if not anim.is_dir():
                    continue
                (packed / "Anims" / anim.name).mkdir(parents=True, exist_ok=True)
                for frame in anim.iterdir():
                    if not frame.is_file():
                        continue
                    if frame.name == "meta.txt":
                        shutil.copyfile(frame, packed / "Anims" / anim.name / "meta.txt")
                    elif frame.name.startswith("frame_"):
                        (packed / "Anims" / anim.name / frame.with_suffix(".bm").name).write_bytes(convert_bm(frame))

        if (source / "Icons").is_dir():
            for icons in (source / "Icons").iterdir():
                if not icons.is_dir():
                    continue
                (packed / "Icons" / icons.name).mkdir(parents=True, exist_ok=True)
                for icon in icons.iterdir():
                    if not icon.is_file():
                        continue
                    (packed / "Icons" / icons.name / icon.with_suffix(".bmx").name).write_bytes(convert_bmx(icon))


if __name__ == "__main__":
    input(
        "This will look through all the subfolders next to this file and try to pack them\n"
        "The resulting asset packs will be saved to 'dolphin_custom' in this folder\n"
        "Press [Enter] if you wish to continue"
    )
    print()
    here = pathlib.Path(__file__).absolute().parent
    start = time.perf_counter()

    pack(here, here / "dolphin_custom", logger=print)

    end = time.perf_counter()
    input(
        f"\nFinished in {round(end - start, 2)}s\n"
        "Press [Enter] to exit"
    )
