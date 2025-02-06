import os
import shutil
import subprocess
import configparser
import json
import argparse


CONFIG_FILE = "ModSetup/mod.ini"
GAME_PROFILE_FILENAME = "GAME.ulp2"
MOD_PROFILE_FILENAME = "MOD.ulp2"

BASE_PATH = os.path.dirname(os.path.abspath(__file__))

# Load the configuration file
def load_mod_config():
    config = configparser.ConfigParser()
    config.optionxform = lambda option: option
    config.read(CONFIG_FILE)
    return config

# Updates mod.ini with the new config data
def update_mod_config(config):
    with open(CONFIG_FILE, "w") as configfile:
        config.write(configfile)
    print("Updated mod.ini with new config data")

# Rename the plugin using renom.exe
def rename_plugin(old_name, new_name):
    renom_exe = os.path.join(BASE_PATH, "ModSetup", "renom.exe")
    if not os.path.exists(renom_exe):
        print("Error: renom.exe not found.")
        return False

    project_path = BASE_PATH
    command = [
        renom_exe,
        "rename-plugin",
        "--project", project_path,
        "--plugin", old_name,
        "--new-name", new_name
    ]

    result = subprocess.run(command, capture_output=True, text=True)

    if result.returncode != 0:
        print(f"Error renaming plugin: {result.stderr}")
        return False

    print(f"Successfully renamed plugin from {old_name} to {new_name}.")
    return True

# Rename mod folder, .pak, and .umod files
def rename_mod_files(base_path, old_name, new_name):
    mods_folder = os.path.join(base_path, "Binaries")
    old_mod_folder = os.path.join(mods_folder, old_name)
    new_mod_folder = os.path.join(mods_folder, new_name)

    if not os.path.exists(old_mod_folder):
        print(f"Mod folder '{old_mod_folder}' does not exist. Skipping mod folder renaming.")
        return True

    os.rename(old_mod_folder, new_mod_folder)
    print(f"Renamed mod folder from {old_mod_folder} to {new_mod_folder}.")

    old_pak_file = os.path.join(new_mod_folder, f"{old_name}.pak")
    new_pak_file = os.path.join(new_mod_folder, f"{new_name}.pak")
    if os.path.exists(old_pak_file):
        os.rename(old_pak_file, new_pak_file)
        print(f"Renamed .pak file from {old_pak_file} to {new_pak_file}.")

    old_umod_file = os.path.join(new_mod_folder, f"{old_name}.umod")
    new_umod_file = os.path.join(new_mod_folder, f"{new_name}.umod")
    if os.path.exists(old_umod_file):
        os.rename(old_umod_file, new_umod_file)
        print(f"Renamed .umod file from {old_umod_file} to {new_umod_file}.")

    # Update the .umod file with the new name
    if os.path.exists(new_umod_file):
        with open(new_umod_file, "r") as f:
            umod_data = json.load(f)
        umod_data["name"] = new_name
        with open(new_umod_file, "w") as f:
            json.dump(umod_data, f, indent=4)
        print(f"Updated .umod file with new name: {new_name}.")

    return True

# Updates the given project launcher profile file with the correct paths
def update_profile(profile_file, unreal_path):
    with open(profile_file, 'r') as file:
        raw_profile_data = file.read()

    # Update the profile's content
    uproject_path = os.path.join(BASE_PATH, "MurkyDiversMod.uproject").replace("\\", "/")
    package_dir = os.path.join(BASE_PATH, "Intermediate", "Mod").replace("\\", "/") + "/"

    raw_profile_data = raw_profile_data.replace("%%UPROJECT%%", uproject_path)
    raw_profile_data = raw_profile_data.replace("%%PACKAGE_DIR%%", package_dir)
    raw_profile_data = raw_profile_data.replace("%%UNREAL_INSTALL_PATH%%", unreal_path.replace("\\", "\\\\"))
    raw_profile_data = raw_profile_data.replace("%%UNREAL_INSTALL_PATH%%", unreal_path.replace("/", "\\\\"))

    with open(profile_file, 'w') as file:
        file.write(raw_profile_data)

    print(f"Profile {profile_file} updated successfully")

# Build the project with the selected profile
def build_with_profile(config, profile_filename):

    # Ensure profile exists
    profile_path = os.path.join(config["Env"]["UnrealInstallPath"], "Engine", "Programs", "UnrealFrontend", "Profiles")
    if not os.path.exists(profile_path):
        print(f"Profile is not found. Please set up your dev environment using set-env first.")
        exit(1)

    profile_name = os.path.splitext(profile_filename)[0]

    # Define the paths and profile
    uat_path = os.path.join(config["Env"]["UnrealInstallPath"], "Engine", "Binaries", "DotNET", "AutomationTool", "AutomationTool.exe")
    
    unreal_cmd_exe_path = os.path.join(config['Env']['UnrealInstallPath'], "Engine", "Binaries", "Win64", "UnrealEditor-Cmd.exe").replace("/", "\\")
    
    uproject_path = os.path.join(BASE_PATH, "MurkyDiversMod.uproject")

    # Construct the command
    command = [
        uat_path,
        f"-ScriptsForProject={uproject_path}",
        "BuildCookRun",
        f"-project={uproject_path}",
        "-noP4",
        "-clientconfig=Shipping",
        "-serverconfig=Shipping",
        "-nocompile",
        "-nocompileeditor",
        "-installed",
        # TODO fix this path
        f"-unrealexe={unreal_cmd_exe_path}",
        "-utf8output",
        "-platform=Win64",
        "-build",
        "-cook",
        "-map=MAP_Main+MAP_Main",
        "-CookCultures=fr",
        "-unversionedcookedcontent",
        "-pak",
        "-compressed",
        "-createreleaseversion=1.0",
        "-stage",
        "-package",
        f"-stagingdirectory={BASE_PATH}/MD_Mod/Intermediate/Mod/",
        "-cmdline=\"MAP_Main -Messaging\"",
        "-addcmdline=\"-SessionId=94D6C25D46B780191F1E74BB10C1A993 -SessionOwner='damie' -SessionName='Game'\""
    ]

    # Execute the command
    try:
        print(f"Trying to build the profile {profile_name} through Unreal ...")
        with subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1) as process:
            for line in process.stdout:
                print(line, end="")  # Print each line as it comes
        process.wait()  # Wait for the process to complete
        if process.returncode == 0:
            print("\nBuild successful:")
        else:
            print(f"\nCommand failed with return code {process.returncode}.")
    except FileNotFoundError:
        print("Error: UnrealEditor-Cmd.exe not found. Please set the path to UE 5.3 using set-env --unreal-path.")
        exit(1)
    except Exception as e:
        print(f"An error occurred: {e}")
        exit(1)

# Locate the .pak file
def find_pak_file(base_path):
    staged_build_path = os.path.join(base_path, "Plugins", "MD_Mod_Test", "Saved", "StagedBuilds", "Windows", "MurkyDiversMod", "Plugins", "MD_Mod_Test", "Content", "Paks", "Windows")
    print(staged_build_path)
    if os.path.exists(staged_build_path):
        for file in os.listdir(staged_build_path):
            if file.endswith(".pak"):
                return os.path.join(staged_build_path, file)
    return None

# Returns the folder where the mod was built (in Intermediate)
def get_mod_build_folder(config):
    mods_folder = os.path.join(BASE_PATH, "Intermediate", "Mod")
    mod_folder = os.path.join(mods_folder, config["Mod"]["Name"])
    os.makedirs(mod_folder, exist_ok=True)
    return mod_folder

# Returns the folder where the mod has been packaged (in Binaries)
def get_mod_package_folder(config):
    mods_folder = os.path.join(BASE_PATH, "Binaries")
    mod_folder = os.path.join(mods_folder, config["Mod"]["Name"])
    os.makedirs(mod_folder, exist_ok=True)
    return mod_folder


def update_default_game_ini(old_mod_name, new_mod_name):
    ini_path = f"{BASE_PATH}/Config/DefaultGame.ini"
    
    try:
        with open(ini_path, "r", encoding="utf-8") as file:
            lines = file.readlines()
        
        in_target_section = False
        updated_lines = []
        
        for line in lines:
            if line.strip() == "[/Script/AssetReferenceRestrictions.AssetReferencingPolicySettings]":
                in_target_section = True
            
            if in_target_section and line.strip().startswith("ProjectPlugins="):
                line = line.replace(old_mod_name, new_mod_name)
                in_target_section = False  # Stop modifying after ProjectPlugins
            
            updated_lines.append(line)
        
        with open(ini_path, "w", encoding="utf-8") as file:
            file.writelines(updated_lines)
        
    except FileNotFoundError:
        pass
    except Exception as e:
        pass


# Sets the mod information in mod.ini, renames the mod if new name is given
def update_uplugin_friendly_name(new_name, new_friendly_name):
    uplugin_file_path = f"{BASE_PATH}/Plugins/{new_name}/{new_name}.uplugin"

    try:
        # Read the .uplugin file as plain text
        with open(uplugin_file_path, 'r') as file:
            lines = file.readlines()

        # Find and update the FriendlyName line
        updated = False
        for i, line in enumerate(lines):
            if '"FriendlyName":' in line:
                lines[i] = f"\t\"FriendlyName\": \"{new_friendly_name}\",\n"
                updated = True
                break

        if not updated:
            print("No 'FriendlyName' field not found in the .uplugin file.")
            return

        # Write the updated content back to the file
        with open(uplugin_file_path, 'w') as file:
            file.writelines(lines)

        print(f"FriendlyName updated to '{new_friendly_name}' in {uplugin_file_path}")

    except FileNotFoundError:
        print(f"Error: File not found at {uplugin_file_path}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")


################################################################################
# Commands

def setup_mod(mod_name, creator_name, mod_version):
    config = load_mod_config()

    if mod_name is not None and mod_name != config["Mod"]["Name"]:
        old_name = config["Mod"]["Name"]
        new_name = mod_name.replace(" ", "_")
        print(f"Renaming Mod from {old_name} to {new_name} ...")
        config["Mod"]["Name"] = new_name
        rename_plugin(old_name, new_name)
        rename_mod_files(BASE_PATH, old_name, new_name)
        update_default_game_ini(old_name, new_name)
        update_uplugin_friendly_name(new_name, mod_name)
    
    if creator_name is not None: config["Mod"]["Creator"] = creator_name
    if mod_version is not None: config["Mod"]["Version"] = mod_version
    
    update_mod_config(config)
    print("Mod setup complete")


# Sets the environment for developing the mod: stores paths in config file, updates and install project launcher profiles
def set_env(murky_path, unreal_path):
    config = load_mod_config()

    # Update config
    if murky_path is not None: config["Env"]["MurkyDiversInstallPath"] = murky_path.replace("\\", "/")
    if unreal_path is not None: config["Env"]["UnrealInstallPath"] = unreal_path

    # Copy project launcher profiles in Unreal
    profiles_path = os.path.join(config["Env"]["UnrealInstallPath"], "Engine", "Programs", "UnrealFrontend", "Profiles")
    game_profile = os.path.join(BASE_PATH, "ModSetup", GAME_PROFILE_FILENAME)
    mod_profile = os.path.join(BASE_PATH, "ModSetup", MOD_PROFILE_FILENAME)

    print("Installing project launcher profiles ...")
    os.makedirs(profiles_path, exist_ok=True)
    game_profile = shutil.copy2(game_profile, profiles_path)
    mod_profile = shutil.copy2(mod_profile, profiles_path)

    update_profile(game_profile, config["Env"]["UnrealInstallPath"])
    update_profile(mod_profile, config["Env"]["UnrealInstallPath"])

    update_mod_config(config)

    # TODO Build the Game proxies
    #build_with_profile(config, GAME_PROFILE_FILENAME)

    print("Environment setup complete")

# Packaging the mod in Binaries/{ModName}, creating and naming everything accordingly
def package_mod():
    config = load_mod_config()

    print(f"Packaging the mod in Binaries/{config['Mod']['Name']} ...")

    pak_file = find_pak_file(BASE_PATH)
    if not pak_file:
        print("No valid .pak file found.")
        return

    print(f"Found .pak file: {pak_file}")

    # Package .pak and .umod in the correct folder
    mod_folder = get_mod_package_folder(config)

    mod_info = {
        "name": config["Mod"]["Name"],
        "creator": config["Mod"]["Creator"],
        "mod_version": config["Mod"]["Version"],
    }
    umod_file = os.path.join(mod_folder, f"{config['Mod']['Name']}.umod")
    with open(umod_file, "w") as f:
        json.dump(mod_info, f, indent=4)

    new_pak_path = os.path.join(mod_folder, f"{config['Mod']['Name']}.pak")
    shutil.copy(pak_file, new_pak_path)
    print(f"Mod successfully packaged in {new_pak_path}")

# Copies the mod into Murky Divers' mod folder
def install_mod():
    config = load_mod_config()

    if not (os.path.exists(config["Env"]["MurkyDiversInstallPath"]) and os.path.isdir(config["Env"]["MurkyDiversInstallPath"])):
        print(f"Murky Divers' install path does not exist: {config['Env']['MurkyDiversInstallPath']}. Use set-env -m to update it.")
        exit(2)

    mod_name = config["Mod"]["Name"]

    print(f"Installing the mod {mod_name} in Murky Divers' folder ...")


    dest_mod_folder = os.path.join(config["Env"]["MurkyDiversInstallPath"], "MurkyDivers","Mods",mod_name)
    os.makedirs(dest_mod_folder, exist_ok=True)

    mod_folder = get_mod_package_folder(config)

    for item in os.listdir(mod_folder):
        src_item = os.path.join(mod_folder, item)
        dest_item = os.path.join(dest_mod_folder, item)
        if os.path.isdir(src_item):
            if os.path.exists(dest_item):
                shutil.rmtree(dest_item)
            shutil.copytree(src_item, dest_item)
        else:
            shutil.copy2(src_item, dest_item)

    print(f"Copied mod '{mod_name}' to {dest_mod_folder}")


################################################################################

# Main function
def main():
    
    # Parse arguments
    parser = argparse.ArgumentParser(
        description=(
            "Script for managing Murky Divers mod environment setup, development and packaging.\n\n"
            "You need to setup at least your mod name and environment before starting developing."
        )
    )

    # Subparser for each operation
    subparsers = parser.add_subparsers(dest="command", required=True, help="Available operations")

    # Command: --setup-mod / -s
    setup_mod_parser = subparsers.add_parser(
        "setup-mod", aliases=["s"],
        help="Sets the mod information and setups all the folders and naming accordingly."
    )
    setup_mod_parser.add_argument("--mod-name", "-n", required=False, help="Name of the mod. Spaces will be replaced by \"_\"")
    setup_mod_parser.add_argument("--creator-name", "-c", required=False, help="Name of the creator")
    setup_mod_parser.add_argument("--version", "-v", required=False, help="Version of the mod")

    # Command: --set-env / -e
    set_env_parser = subparsers.add_parser(
        "set-env", aliases=["e"],
        help="Sets the required environment for developing your mod."
    )
    set_env_parser.add_argument("--murky-path", "-m", required=False, help="Path to Murky Divers' installation")
    set_env_parser.add_argument("--unreal-path", "-u", required=False, help="Path to the Unreal Engine installation")

    # Command: --package / -p
    package_parser = subparsers.add_parser(
        "package", aliases=["p"],
        help="Packages the mod in Binaries/{ModName}. No additional arguments required."
    )

    # Command: --install / -i
    install_parser = subparsers.add_parser(
        "install", aliases=["i"],
        help="Installs the mod in Murky Divers. No additional arguments required."
    )

    # Parse arguments
    args = parser.parse_args()


    # Commands execution
    if args.command in ["setup-mod", "s"]:
        if not (args.mod_name or args.creator_name or args.version):
            print("Error: At least one of the required arguments must be provided for 'setup-mod'. See python .\\mod_setup.py setup-mod -h for more details.")
            exit(1)
        setup_mod(args.mod_name, args.creator_name, args.version)

    elif args.command in ["set-env", "e"]:
        set_env(args.murky_path, args.unreal_path)

    elif args.command in ["package", "p"]:
        package_mod()

    elif args.command in ["install", "i"]:
        install_mod()

if __name__ == "__main__":
    main()