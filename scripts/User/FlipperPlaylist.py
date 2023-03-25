# Flipper Zero SUB-GHZ Playlist Generator
import os
import pip

try:
    from easygui import diropenbox
except ImportError:
    pip.main(["Install"], "easygui")
    from easygui import diropenbox


def main():
    folder_path = diropenbox("Select the folder with Subghz files", "Subghz selector")
    output_path = diropenbox("Select your output location", "Output location")
    playlist_name = str(input("Enter a name for the playlist: "))
    playlist_file = open((output_path + playlist_name.lower() + ".txt"), "w")
    playlist_file.write("# " + playlist_name + "\n")
    for root, dirs, files in os.walk(folder_path, topdown=True):
        for file in files:
            if file.endswith(".sub"):
                file_path = os.path.join(root, file)
                file_path = file_path.replace("\\", "/")
                file_path = file_path.replace(f"{folder_path.split(':')[0]}:", "ext")
                playlist_file.write(f"sub: {file_path}\n")
    playlist_file.close()
    print("Done!")


if __name__ == "__main__":
    main()
