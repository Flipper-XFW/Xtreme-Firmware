#!/usr/bin/env python
from PIL import Image, ImageOps
import heatshrink2
import pathlib
import shutil
import struct
import typing
import time
import re
import io
import os


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

    if len(data_enc) + 2 < len(data_bin) + 1:
        return b"\x01\x00" + data_enc
    else:
        return b"\x00" + data_bin


def convert_bmx(img: "Image.Image | pathlib.Path") -> bytes:
    if not isinstance(img, Image.Image):
        img = Image.open(img)

    data = struct.pack("<II", *img.size)
    data += convert_bm(img)
    return data


def pack_anim(src: pathlib.Path, dst: pathlib.Path):
    if not (src / "meta.txt").is_file():
        return
    dst.mkdir(parents=True, exist_ok=True)
    for frame in src.iterdir():
        if not frame.is_file():
            continue
        if frame.name == "meta.txt":
            shutil.copyfile(src / "meta.txt", dst / "meta.txt")
            continue
        elif frame.name.startswith("frame_"):
            (dst / frame.with_suffix(".bm").name).write_bytes(convert_bm(frame))


def pack_icon_animated(src: pathlib.Path, dst: pathlib.Path):
    if not (src / "frame_rate").is_file():
        return
    dst.mkdir(parents=True, exist_ok=True)
    frame_count = 0
    frame_rate = None
    size = None
    for frame in src.iterdir():
        if not frame.is_file():
            continue
        if frame.name == "frame_rate":
            frame_rate = int((src / "frame_rate").read_text())
            continue
        elif frame.name.startswith("frame_"):
            frame_count += 1
            if not size:
                size = Image.open(frame).size
            (dst / frame.with_suffix(".bm").name).write_bytes(convert_bm(frame))
    (dst / "meta").write_bytes(struct.pack("<IIII", *size, frame_rate, frame_count))


def pack_icon_static(src: pathlib.Path, dst: pathlib.Path):
    dst.parent.mkdir(parents=True, exist_ok=True)
    dst.with_suffix(".bmx").write_bytes(convert_bmx(src))


def pack_font(src: pathlib.Path, dst: pathlib.Path):
    code = src.read_bytes().split(b' U8G2_FONT_SECTION("')[1].split(b'") =')[1].strip()
    font = b""
    for line in code.splitlines():
        if line.count(b'"') == 2:
            font += (
                line[line.find(b'"') + 1 : line.rfind(b'"')]
                .decode("unicode_escape")
                .encode("latin_1")
            )
    dst.parent.mkdir(parents=True, exist_ok=True)
    dst.with_suffix(".u8f").write_bytes(font)


def pack(
    input: "str | pathlib.Path", output: "str | pathlib.Path", logger: typing.Callable
):
    input = pathlib.Path(input)
    output = pathlib.Path(output)
    for source in input.iterdir():
        if source == output:
            continue
        if not source.is_dir():
            continue

        logger(f"Pack: custom user pack '{source.name}'")
        packed = output / source.name
        if packed.exists():
            try:
                if packed.is_dir():
                    shutil.rmtree(packed, ignore_errors=True)
                else:
                    packed.unlink()
            except Exception:
                pass

        if (source / "Anims/manifest.txt").exists():
            (packed / "Anims").mkdir(parents=True, exist_ok=True)
            shutil.copyfile(
                source / "Anims/manifest.txt", packed / "Anims/manifest.txt"
            )
            manifest = (source / "Anims/manifest.txt").read_bytes()
            for anim in re.finditer(rb"Name: (.*)", manifest):
                anim = (
                    anim.group(1)
                    .decode()
                    .replace("\\", "/")
                    .replace("/", os.sep)
                    .replace("\r", "\n")
                    .strip()
                )
                logger(f"Compile: anim for pack '{source.name}': {anim}")
                pack_anim(source / "Anims" / anim, packed / "Anims" / anim)

        if (source / "Icons").is_dir():
            for icons in (source / "Icons").iterdir():
                if not icons.is_dir():
                    continue
                for icon in icons.iterdir():
                    if icon.is_dir():
                        logger(
                            f"Compile: icon for pack '{source.name}': {icons.name}/{icon.name}"
                        )
                        pack_icon_animated(
                            icon, packed / "Icons" / icons.name / icon.name
                        )
                    elif icon.is_file():
                        logger(
                            f"Compile: icon for pack '{source.name}': {icons.name}/{icon.name}"
                        )
                        pack_icon_static(
                            icon, packed / "Icons" / icons.name / icon.name
                        )

        if (source / "Fonts").is_dir():
            for font in (source / "Fonts").iterdir():
                if not font.is_file():
                    continue
                logger(f"Compile: font for pack '{source.name}': {font.name}")
                pack_font(font, packed / "Fonts" / font.name)


if __name__ == "__main__":
    input(
        "This will look through all the subfolders next to this file and try to pack them\n"
        "The resulting asset packs will be saved to 'asset_packs' in this folder\n"
        "Press [Enter] if you wish to continue"
    )
    print()
    here = pathlib.Path(__file__).absolute().parent
    start = time.perf_counter()

    pack(here, here / "asset_packs", logger=print)

    end = time.perf_counter()
    input(f"\nFinished in {round(end - start, 2)}s\n" "Press [Enter] to exit")
