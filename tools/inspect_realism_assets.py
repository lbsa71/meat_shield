"""Sanitized, read-only editor asset evidence for REQ-028."""
import unreal

character = unreal.load_asset("/Game/MeatShield/SourceCharacters/MC_Realism_A")
subsystem = unreal.get_editor_subsystem(unreal.MetaHumanCharacterEditorSubsystem)
RESULT = {
    "character": character.get_path_name() if character else None,
    "can_build": subsystem.can_build_meta_human(character) if character else False,
    "assets": list(unreal.EditorAssetLibrary.list_assets("/Game/MeatShield", recursive=True)),
    "preset_class": unreal.load_asset("/MetaHumanCharacter/Optional/Presets/Ada").get_class().get_name(),
    "api": {name: str(getattr(unreal, name).__doc__) for name in (
        "GeometryScript_Primitives", "GeometryScript_AssetUtils", "GeometryScript_MeshEdits")
        if hasattr(unreal, name)},
}

blueprint = unreal.EditorAssetLibrary.load_blueprint_class("/Game/MeatShield/Humans/MC_Realism_A/BP_MC_Realism_A")
if blueprint:
    actors = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    actor = actors.spawn_actor_from_class(blueprint, unreal.Vector(0,0,0))
    RESULT["components"] = []
    for component in actor.get_components_by_class(unreal.SkeletalMeshComponent):
        mesh = component.get_skeletal_mesh_asset()
        if not mesh: continue
        physics = mesh.get_editor_property("physics_asset")
        RESULT["components"].append({"name":component.get_name(), "mesh":mesh.get_path_name(),
            "animation":str(component.get_editor_property("anim_class")),
            "parent":str(component.get_attach_parent()),
            "bones": [str(component.get_bone_name(i)) for i in range(component.get_num_bones())],
            "physics":physics.get_path_name() if physics else None,
            "physics_bones": [],
            "materials":[component.get_material(i).get_path_name() for i in range(component.get_num_materials())]})
    actors.destroy_actor(actor)
