"""Validate and persist only the recipe-owned assembled character packages."""
import unreal

root = "/Game/MeatShield/Humans/MC_Realism_A"
required = [root + "/BP_MC_Realism_A", root + "/Body/SKM_MC_Realism_A_BodyMesh",
            root + "/Face/SKM_MC_Realism_A_FaceMesh", root + "/Body/PHYS_MC_Realism_A"]
for path in required:
    if unreal.load_asset(path) is None:
        raise RuntimeError("Assembly output missing: " + path)
saved = []
unreal.EditorAssetLibrary.save_asset("/Game/MeatShield/SourceCharacters/MC_Realism_A", only_if_is_dirty=False)
for path in unreal.EditorAssetLibrary.list_assets("/Game/MeatShield", recursive=True):
    if str(path).endswith("AS_MetaHuman_ARKit_Mapping.AS_MetaHuman_ARKit_Mapping"):
        asset = unreal.load_asset(path)
        unreal.EditorLoadingAndSavingUtils.reload_packages([asset.get_outermost()], unreal.ReloadPackagesInteractionMode.ASSUME_POSITIVE)
    if not unreal.EditorAssetLibrary.save_asset(path, only_if_is_dirty=False):
        raise RuntimeError("Could not save owned assembly asset: " + path)
    saved.append(str(path))
RESULT = {"validated_outputs": required, "saved_packages": len(saved)}
