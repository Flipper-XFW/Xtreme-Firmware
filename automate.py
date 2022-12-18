import os
import re
import shutil

folder = r'E:\Storage\Coding\Flipper Zero\Firmware\build-src'
settings = folder + r"\fbt_options.py"
commit = input("Whats your commit message?\nCommit Message: ")

def find_number():
    regex = re.compile(r'(DIST_SUFFIX = \"CC_CL-)+[0]*([0-9]*)+(\")')
    with open(settings) as f:
        for line in f:
            try:
                obj = regex.search(line)
                if not obj == None:
                    result = obj.group(0)
            except:
                pass
    return result

def push_number(line):
    with open(os.path.join(settings), "r+") as src:
        file = src.read()
        src.seek(0)
        newnum = int(line.split("-")[1].replace('"', "").replace("0", "", 3)) + 1
        newline = 'DIST_SUFFIX = "CC_CL-' + "0" * (4 -len(str(newnum))) + str(newnum) + '"'
        print(f"New Version: {newline}")
        src.write(file.replace(line, newline))
        src.truncate()
        src.close()
        return newline.split('"')[1].split('"')[0]

def main():
    ren = push_number(find_number())
    os.system(f' cd "{folder}" && powershell -command "./fbt updater_package"')
    newfolder = rf"E:\\Storage\\Coding\\Flipper-Zero\\Firmware\\{ren}"
    shutil.move(os.path.join(folder + f"\\dist\\f7-C\\f7-update-{ren}"), newfolder)
    os.system(f'git add * && git commit -m "{commit}"')
    shutil.make_archive(ren, 'zip', newfolder)
    os.system(f'''gh release create {ren.split('"')[1].split('"')[0]} -R "ClaraCrazy/flipper-firmware" --generate-notes {ren}.zip''')
main()