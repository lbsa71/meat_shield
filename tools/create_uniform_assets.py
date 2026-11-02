"""REQ-029: editor-only fitted uniform shells with inherited MetaHuman skin weights.

Prototype garments are generated from the owned assembled body, not purchased art.
Run with Unreal's Python commandlet. Source geometry, skeleton and UVs stay intact;
only the derived wardrobe assets below are created/updated. No cloth solver required.
"""
import json
from pathlib import Path
import unreal as u

ROOT = "/Game/MeatShield/Wardrobe"
tools = u.AssetToolsHelpers.get_asset_tools()
body = u.load_asset("/Game/MeatShield/Humans/MC_Realism_A/Body/SKM_MC_Realism_A_BodyMesh")
if body is None:
    raise RuntimeError("Assemble the owned MetaHuman before generating uniforms")


def material(name, camouflage=False):
    path = ROOT + "/" + name
    mat = u.load_asset(path) if u.EditorAssetLibrary.does_asset_exist(path) else tools.create_asset(name, ROOT, u.Material, u.MaterialFactoryNew())
    e = u.MaterialEditingLibrary
    e.delete_all_material_expressions(mat)
    if camouflage:
        uv = e.create_material_expression(mat, u.MaterialExpressionTextureCoordinate)
        custom = e.create_material_expression(mat, u.MaterialExpressionCustom)
        inputs = []
        for key in ("UV", "C0", "C1", "C2", "Seed"):
            inp = u.CustomInput(); inp.set_editor_property("InputName", key); inputs.append(inp)
        custom.set_editor_property("Inputs", inputs)
        custom.set_editor_property("OutputType", u.CustomMaterialOutputType.CMOT_FLOAT3)
        custom.set_editor_property("Code", "float2 p=floor(UV*65+float2(Seed,Seed*.37)); float n=frac(sin(dot(p,float2(127.1,311.7)))*43758.5453); float2 b=floor(UV*19+Seed); float v=frac(sin(dot(b,float2(41.2,97.3)))*15231.17); float k=v*.8+n*.2; return k<.38?C0:(k<.70?C1:C2);")
        e.connect_material_expressions(uv, "", custom, "UV")
        for key, color in (("C0", (.1,.14,.06)),("C1",(.03,.045,.02)),("C2",(.24,.19,.10))):
            param = e.create_material_expression(mat,u.MaterialExpressionVectorParameter)
            param.set_editor_property("ParameterName",key); param.set_editor_property("DefaultValue",u.LinearColor(*color,1))
            e.connect_material_expressions(param,"",custom,key)
        seed=e.create_material_expression(mat,u.MaterialExpressionScalarParameter); seed.set_editor_property("ParameterName","PatternSeed")
        e.connect_material_expressions(seed,"",custom,"Seed"); e.connect_material_property(custom,"",u.MaterialProperty.MP_BASE_COLOR)
    else:
        color=e.create_material_expression(mat,u.MaterialExpressionConstant3Vector)
        color.constant=u.LinearColor(.028,.034,.018,1); e.connect_material_property(color,"",u.MaterialProperty.MP_BASE_COLOR)
    rough=e.create_material_expression(mat,u.MaterialExpressionConstant); rough.r=.92
    e.connect_material_property(rough,"",u.MaterialProperty.MP_ROUGHNESS)
    e.set_material_usage(mat,u.MaterialUsage.MATUSAGE_SKELETAL_MESH)
    e.recompile_material(mat); u.EditorAssetLibrary.save_loaded_asset(mat)
    return mat


fabric=material("M_FieldFabric",True)
gear=material("M_FieldGear")
records=[]
for name, vest in (("SK_FieldUniform",False),("SK_FieldVest",True)):
    path=ROOT+"/"+name
    mesh=u.DynamicMesh()
    lod=u.GeometryScriptMeshReadLOD(); lod.lod_index=1; lod.lod_type=u.GeometryScriptLODType.RENDER_DATA
    mesh,outcome=u.GeometryScript_AssetUtils.copy_mesh_from_skeletal_mesh(body,mesh,u.GeometryScriptCopyMeshFromAssetOptions(),lod)
    _,has_weights,max_bone=u.GeometryScript_BoneWeights.get_max_bone_weight_index(mesh)
    if not has_weights or max_bone<20: raise RuntimeError("Uniform source has no articulated skin weights; render-data extraction requires initialized rendering resources (do not use -nullrhi).")
    positions={}
    for i in range(u.GeometryScript_MeshQueries.get_vertex_count(mesh)):
        p,valid=u.GeometryScript_MeshQueries.get_vertex_position(mesh,i)
        if valid: positions[i]=p
    normals={i:u.Vector() for i in positions}
    remove=[]; kept=0
    for t in range(next(getattr(u.GeometryScript_MeshQueries,n) for n in dir(u.GeometryScript_MeshQueries) if n.startswith("get_num_triangle"))(mesh)):
        ids,valid=u.GeometryScript_MeshQueries.get_triangle_indices(mesh,t)
        if not valid: continue
        a,b,c=ids.x,ids.y,ids.z
        center=(positions[a]+positions[b]+positions[c])/3
        keep=(90<center.z<128 and abs(center.x)<22) if vest else (center.z<140 and abs(center.x)<61)
        if not keep: remove.append(t); continue
        kept+=1
        normal=(positions[c]-positions[a]).cross(positions[b]-positions[a]) # Unreal clockwise front faces
        for i in (a,b,c): normals[i]=normals[i]+normal
        material_id=0 if vest or center.z>18 and not 78<center.z<83 else 1
        u.GeometryScript_Materials.set_triangle_material_id(mesh,t,material_id,True)
    for i,p in positions.items():
        normal=normals[i]
        if normal.length()>0:
            distance=3.8 if vest else (2.0 if p.z>85 else 1.4)
            u.GeometryScript_MeshEdits.set_vertex_position(mesh,i,p+normal.normal()*distance,True)
    indices=u.GeometryScript_List.convert_array_to_index_list(remove)
    u.GeometryScript_MeshEdits.delete_triangles_from_mesh(mesh,indices,True)
    if kept<100: raise RuntimeError("Wardrobe selection produced too little geometry")
    # Preserve the assembled mesh's inverse bind matrices as well as its bone names.
    # New skeletal assets can match local references yet deform differently on MetaHumans.
    garment=u.load_asset(path) if u.EditorAssetLibrary.does_asset_exist(path) else u.EditorAssetLibrary.duplicate_asset(body.get_path_name().split(".")[0],path)
    if garment is None: raise RuntimeError("Could not copy the owned body binding")
    editor=u.get_editor_subsystem(u.SkeletalMeshEditorSubsystem)
    count=editor.get_lod_count(garment)
    if count>1: editor.remove_lods(garment,list(range(1,count)))
    garment.set_editor_property("lod_settings",None)
    garment.set_editor_property("physics_asset",None)
    garment.set_editor_property("post_process_anim_blueprint",None)
    options=u.GeometryScriptCopyMeshToAssetOptions(); options.enable_recompute_normals=True; options.enable_recompute_tangents=True
    options.replace_materials=True; options.new_materials=[gear if vest else fabric,gear]
    u.GeometryScript_AssetUtils.copy_mesh_to_skeletal_mesh(mesh,garment,options,u.GeometryScriptMeshWriteLOD())
    if garment is None: raise RuntimeError("Skeletal uniform bake failed: "+str(outcome))
    slots=garment.get_editor_property("materials")
    for index,slot in enumerate(slots): slot.set_editor_property("material_interface",gear if vest or index else fabric)
    garment.set_editor_property("materials",slots)
    u.EditorAssetLibrary.save_loaded_asset(garment)
    records.append({"asset":path,"triangles":kept,"source_lod":1,"inflation_cm":3.8 if vest else [1.4,2.0]})
result={"source":body.get_path_name(),"recipe":"create_uniform_assets.py","garments":records,"cloth_simulation":False}
Path(u.Paths.project_dir()).resolve().parent.joinpath(".local/uniform-bake.json").write_text(json.dumps(result,indent=2))
