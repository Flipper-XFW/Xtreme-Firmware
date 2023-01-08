#!/usr/bin/env python3

import os
import struct

from flipper.app import App
from flipper.assets.icon import file2image


import requests
import zipfile

     
class Main(App):
    MAGIC = 0x72676468
    VERSION = 1

    url = "TBA"
    if not os.isdir("assets/dolphin/external/lvl_1"):
    check = input("We noticed that you do not have the original XFW animations downloaded. Would you like to do that? (y/n)")
    if check.lower() == "y":
        zipr = requests.get(url, allow_redirects=True)
        open('assets.zip', 'wb').write(zipr.content)
        with zipfile.ZipFile("assets.zip", 'r') as zip_ref:
            zip_ref.extractall("assets/dolphin/external")
            print("Finished downloading assets.")

    
    def init(self):
        self.parser.add_argument("-i", "--input", help="input folder", required=True)
        self.parser.add_argument("-o", "--output", help="output file", required=True)

        self.parser.set_defaults(func=self.pack)

    def pack(self):
        if not os.path.exists(self.args.input):
            self.logger.error(f'"{self.args.input}" does not exist')
            return 1

        file_idx = 0
        images = []
        while True:
            frame_filename = os.path.join(self.args.input, f"frame_{file_idx:02}.png")
            if not os.path.exists(frame_filename):
                break

            self.logger.debug(f"working on {frame_filename}")
            try:
                images.append(file2image(frame_filename))
                self.logger.debug(f"Processed frame #{file_idx}")
                file_idx += 1
            except Exception as e:
                self.logger.error(e)
                return 3

        widths = set(img.width for img in images)
        heights = set(img.height for img in images)
        if len(widths) != 1 or len(heights) != 1:
            self.logger.error("All images must have same dimensions!")
            return 2

        data = struct.pack(
            "<IBBBB", self.MAGIC, self.VERSION, widths.pop(), heights.pop(), len(images)
        )
        for image in images:
            data += struct.pack("<H", len(image.data))
            data += image.data

        with open(self.args.output, mode="wb") as file:
            file.write(data)

        return 0


if __name__ == "__main__":
    Main()()
