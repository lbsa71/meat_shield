"""Retry texture sources after the first Creator account authorization."""
import unreal

character = unreal.load_asset("/Game/MeatShield/SourceCharacters/MC_Realism_A")
subsystem = unreal.get_editor_subsystem(unreal.MetaHumanCharacterEditorSubsystem)
params = unreal.MetaHumanCharacterTextureRequestParams()
params.blocking = False
params.report_progress = True
subsystem.request_texture_sources(character, params)
unreal.EditorAssetLibrary.save_loaded_asset(character)
unreal.log("MS_METAHUMAN_TEXTURE_RETRY_SUBMITTED")
