import os
import re
import shutil

folder = r'E:\Storage\Coding\Flipper_Zero\Firmware\build-src'
Build_path = folder + "\\dist\\f7-C\\"
Firmware_base = rf"E:\\Storage\\Coding\\Flipper_Zero\\Firmware\\"
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
        newnum = int(line.split("-")[1].replace('"', "", 2)) + 1
        newline = 'DIST_SUFFIX = "CC_CL-' + "0" * (4 -len(str(newnum))) + str(newnum) + '"'
        print(f"New Version: {newline}")
        src.write(file.replace(line, newline))
        src.truncate()
        src.close()
        return newline.split('"')[1].split('"')[0], newnum

def main():
    numbers = push_number(find_number())
    ren = numbers[0]
    release = numbers[1]
    print(ren)
    os.system(f' cd "{folder}" && powershell -command "./fbt updater_package"')
    os.system(f'move {os.path.join(Build_path + f"f7-update-{ren}")} {Firmware_base}')
    old_build = f"f7-update-{ren}"
    os.system(f"cd {Firmware_base} && ren {old_build} {ren}")
    os.system(f'git add * && git commit -m "{commit}"')
    zipfile = shutil.make_archive(os.path.join(Firmware_base, ren), 'zip', os.path.join(Firmware_base, ren))
    os.system(f'''gh release create {release} -R "ClaraCrazy/flipper-firmware" --generate-notes {zipfile}''')
    os.remove(zipfile)
main()