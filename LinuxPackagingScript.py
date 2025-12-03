import os
import sys
from pathlib import Path
import requests
import stat
import subprocess
from datetime import date

ARGS={
"SCRIPT_PATH": 0,
"OFFICIAL_NAME": 1,
"PROJECT_NAME": 2,
"MAIN_TARGET_NAME": 3,
"APPDIR_PATH": 4,
"INSTALL_PREFIX": 5,
"VERSION": 6,
"DEV_NAME": 7,
"PROJECT_SUMMARY": 8
}

DESKTOP_FILE_TEMPLATE="""\
[Desktop Entry]
Name={}
Exec={}
Icon=icon
Type=Application
Categories=Utility;Development;
Keywords=TB;
Comment=Application to organize balanced tournament teams
StartupWMClass={}
"""

LINUX_PAYLOAD_URL="https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"

def main(argv: list) -> None:
    official_name: str= argv[ARGS["OFFICIAL_NAME"]]
    project_name: str= argv[ARGS["PROJECT_NAME"]]
    main_target_name: str= argv[ARGS["MAIN_TARGET_NAME"]]
    appdir_path: Path= Path(argv[ARGS["APPDIR_PATH"]])
    install_prefix: Path= Path(argv[ARGS["INSTALL_PREFIX"]]) 
    main_dir_path: Path= Path(argv[ARGS["SCRIPT_PATH"]]).parent
    version: str= argv[ARGS["VERSION"]]
    dev_name: str= argv[ARGS["DEV_NAME"]]
    project_summary: str= argv[ARGS["PROJECT_SUMMARY"]]
    
    global DESKTOP_FILE_TEMPLATE
    global LINUX_PAYLOAD_URL
    dev_prefix=dev_name+"."
    dst_desktop_path=appdir_path/(dev_prefix+project_name+".desktop")
    if not dst_desktop_path.exists():
        print("Creating desktop file...")
        with open(dst_desktop_path, "w") as desktop:
            desktop.write(DESKTOP_FILE_TEMPLATE.format(
                official_name,
                main_target_name,
                project_name
            ))
    
    linuxdeploy_payload= requests.get(LINUX_PAYLOAD_URL)
    linuxdeploy_payload.raise_for_status()
    dst_ld_path=install_prefix/"linuxdeploy.AppImage"
    if not dst_ld_path.exists():
        print("Downloading linuxdeploy...")
        with open(dst_ld_path, "wb") as dst:
            dst.write(linuxdeploy_payload.content)
            os.chmod(dst_ld_path, 
                stat.S_IRUSR | stat.S_IWUSR | stat.S_IXUSR| 
                stat.S_IRGRP | stat.S_IXGRP | 
                stat.S_IROTH | stat.S_IXOTH
            )
    
    metainfo_filename=dev_prefix+project_name+".appdata.xml"
    dst_metadatainfo_path=appdir_path/("usr/share/metainfo/"+metainfo_filename)
    dst_metadatainfo_path.parent.mkdir(parents=True, exist_ok=True)
    
    print("Installing metadata info file...")
    with open(dst_metadatainfo_path, "w") as dst:
        with open(main_dir_path/(metainfo_filename+".template"), "r") as template:
            metainfo_template_payload=template.read()
            metainfo_template_payload=metainfo_template_payload.format(
                dev_prefix+project_name,
                project_summary,
                dev_prefix+project_name,
                dev_prefix+project_name,
                dev_name,
                version,
                date.today().strftime("%Y-%m-%d"),
                version
            )
            dst.write(metainfo_template_payload)

    command=[
        dst_ld_path, 
         "--appdir", appdir_path,
         "--icon-file", appdir_path/"usr/share/icons/hicolor/256x256/apps/icon.png",
         "--desktop-file", dst_desktop_path,
         "--executable", appdir_path/f"usr/bin/{main_target_name}",
         "--output", "appimage"
    ]    
    print("Running linuxdeploy...", flush=True)
    print(f"Command: {" ".join([str(arg) for arg in command])}", flush=True)
    subprocess.run(command, cwd=install_prefix, text=True)
    
if __name__ == "__main__":
    argc: int=len(sys.argv)
    expected_argc=len(ARGS)
    ill_args_msg=f"Missing args, found {argc}, expected {expected_argc}..."
    assert argc==expected_argc, ill_args_msg #script path not counted

    main(sys.argv)