"""REQ-028 / TC-032: run in Unreal's editor Python console, not system Python.

Creates the owned source character idempotently and opens Creator for inspection.
Assembly and cloud operations are deliberately separate from this local step.
"""
import unreal

PATH = "/Game/MeatShield/SourceCharacters/MC_Realism_A"
character = unreal.load_asset(PATH) if unreal.EditorAssetLibrary.does_asset_exist(PATH) else None
if character is None:
    character = unreal.AssetToolsHelpers.get_asset_tools().create_asset(
        asset_name="MC_Realism_A", package_path="/Game/MeatShield/SourceCharacters",
        asset_class=unreal.MetaHumanCharacter,
        factory=unreal.new_object(unreal.MetaHumanCharacterFactoryNew))
if character is None:
    raise RuntimeError("MetaHuman source creation failed; check Creator and its core data installation")
subsystem = unreal.get_editor_subsystem(unreal.MetaHumanCharacterEditorSubsystem)
unreal.log("MS_METAHUMAN_SOURCE=" + character.get_path_name())
unreal.log("MS_METAHUMAN_CAN_BUILD=" + str(subsystem.can_build_meta_human(character)))
unreal.log("MS_METAHUMAN_API=" + str([name for name in dir(subsystem) if any(
    word in name for word in ("rig", "preset", "texture", "auth", "login"))]))
unreal.EditorAssetLibrary.save_loaded_asset(character)
unreal.get_editor_subsystem(unreal.AssetEditorSubsystem).open_editor_for_assets([character])
