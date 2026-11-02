"""REQ-028: request rig/textures for the owned synthetic source in editor Python.

Run after prepare_metahuman.py. Authentication remains in Epic's editor UI.
Requests are asynchronous so an authentication dialog cannot block the editor.
"""
import unreal

character = unreal.load_asset("/Game/MeatShield/SourceCharacters/MC_Realism_A")
if character is None:
    raise RuntimeError("Run tools/prepare_metahuman.py before requesting the source rig")
subsystem = unreal.get_editor_subsystem(unreal.MetaHumanCharacterEditorSubsystem)
if not subsystem.is_object_added_for_editing(character):
    if not subsystem.try_add_object_to_edit(character):
        raise RuntimeError("Creator could not open the source character for editing")
rig = unreal.MetaHumanCharacterAutoRiggingRequestParams()
rig.blocking = False
rig.report_progress = True
rig.rig_type = unreal.MetaHumanRigType.JOINTS_ONLY
subsystem.request_auto_rigging(character, rig)
textures = unreal.MetaHumanCharacterTextureRequestParams()
textures.blocking = False
textures.report_progress = True
subsystem.request_texture_sources(character, textures)
unreal.log("MS_METAHUMAN_CLOUD_REQUESTS_SUBMITTED; inspect Creator/log for results before assembly")
