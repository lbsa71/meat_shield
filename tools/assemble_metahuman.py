"""REQ-028: assemble the owned source into an optimized, cooked character."""
import unreal

character = unreal.load_asset("/Game/MeatShield/SourceCharacters/MC_Realism_A")
if character is None:
    raise RuntimeError("The owned MetaHuman source is missing")
subsystem = unreal.get_editor_subsystem(unreal.MetaHumanCharacterEditorSubsystem)
if not subsystem.is_object_added_for_editing(character):
    if not subsystem.try_add_object_to_edit(character):
        raise RuntimeError("Could not add the source character for editing")
if not subsystem.can_build_meta_human(character):
    raise RuntimeError("MetaHuman is not ready: finish rigging and both face/body texture downloads in Creator")
params = unreal.MetaHumanCharacterEditorBuildParameters()
params.pipeline_type = unreal.MetaHumanDefaultPipelineType.OPTIMIZED
params.pipeline_quality = unreal.MetaHumanQualityLevel.MEDIUM
params.absolute_build_path = "/Game/MeatShield/Humans"
params.common_folder_path = "/Game/MeatShield/Humans/Common"
params.enable_wardrobe_item_validation = False
subsystem.build_meta_human(character, params)
unreal.EditorAssetLibrary.save_loaded_asset(character)
unreal.log("MS_METAHUMAN_ASSEMBLY_FINISHED: inspect output assets and validation before runtime acceptance")
