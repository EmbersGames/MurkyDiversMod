import os

# Chemin relatif du dossier Content d'Unreal Engine
CONTENT_DIR = os.path.join(os.path.dirname(os.getcwd()), "Content")
INI_FILE_PATH = os.path.join(os.path.dirname(os.getcwd()), "Config", "DefaultGame.ini")

# Fonction pour récupérer les chemins des fichiers .uasset sous la forme /Game/... 
def get_uasset_paths(content_dir):
    uasset_paths = []
    for root, _, files in os.walk(content_dir):
        for file in files:
            if file.endswith(".uasset"):
                relative_path = os.path.relpath(os.path.join(root, file), content_dir)
                asset_path = "/Game/" + relative_path.replace("\\", "/").replace(".uasset", "")
                uasset_paths.append(asset_path)
    return uasset_paths

# Fonction pour mettre à jour la section SpecificAssets du fichier DefaultGame.ini
def update_ini_file(ini_file_path, uasset_paths):
    with open(ini_file_path, "r", encoding="utf-8") as file:
        lines = file.readlines()
    
    new_specific_assets = "SpecificAssets=(" + ",".join(f'"{path}"' for path in uasset_paths) + ")"
    
    updated_lines = []
    for line in lines:
        if line.strip().startswith("+AdditionalDomains=(DomainName=\"Proxy\"") and "SpecificAssets=" in line:
            line = line.split("SpecificAssets=")[0] + new_specific_assets + "," + line.split(",SpecificAssets=")[1].split(",", 1)[-1] if ",SpecificAssets=" in line else new_specific_assets + ")"
        updated_lines.append(line)
    
    with open(ini_file_path, "w", encoding="utf-8") as file:
        file.writelines(updated_lines)

# Exécution du script
if __name__ == "__main__":
    uasset_paths = get_uasset_paths(CONTENT_DIR)
    update_ini_file(INI_FILE_PATH, uasset_paths)
    print(f"Mise à jour de {INI_FILE_PATH} avec {len(uasset_paths)} assets.")