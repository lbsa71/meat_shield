"""REQ-028: reproducible material/Geometry Script/PCG environment assets.

Only /Game/MeatShield/Environment is owned by this recipe. Run in editor Python.
"""
import math
import unreal as u

ROOT = "/Game/MeatShield/Environment"
tools = u.AssetToolsHelpers.get_asset_tools()


def material(name, rock=False):
    path = ROOT + "/" + name
    mat = u.load_asset(path) if u.EditorAssetLibrary.does_asset_exist(path) else tools.create_asset(name, ROOT, u.Material, u.MaterialFactoryNew())
    u.MaterialEditingLibrary.delete_all_material_expressions(mat)
    edit = u.MaterialEditingLibrary
    pos = edit.create_material_expression(mat, u.MaterialExpressionWorldPosition, -700, 0)
    color = edit.create_material_expression(mat, u.MaterialExpressionCustom, -350, 0)
    inp = u.CustomInput(); inp.set_editor_property("InputName", "P")
    color.set_editor_property("Inputs", [inp])
    color.set_editor_property("OutputType", u.CustomMaterialOutputType.CMOT_FLOAT3)
    code = ("float2 q=P.xy*.035; float2 i=floor(q), f=frac(q); f=f*f*(3-2*f);"
            "float4 n=frac(sin(float4(dot(i,float2(127.1,311.7)),dot(i+float2(1,0),float2(127.1,311.7)),"
            "dot(i+float2(0,1),float2(127.1,311.7)),dot(i+1,float2(127.1,311.7))))*43758.5453);"
            "float v=lerp(lerp(n.x,n.y,f.x),lerp(n.z,n.w,f.x),f.y);"
            "return lerp(float3(.085,.066,.044),float3(.19,.15,.10),v);")
    if rock:
        code = "float n=.5+.5*sin(P.x*.17+sin(P.y*.23)); return lerp(float3(.07,.065,.055),float3(.24,.22,.18),n);"
    color.set_editor_property("Code", code)
    edit.connect_material_expressions(pos, "", color, "P")
    edit.connect_material_property(color, "", u.MaterialProperty.MP_BASE_COLOR)
    normal = edit.create_material_expression(mat, u.MaterialExpressionCustom, -350, 260)
    inp = u.CustomInput(); inp.set_editor_property("InputName", "P"); normal.set_editor_property("Inputs", [inp])
    normal.set_editor_property("OutputType", u.CustomMaterialOutputType.CMOT_FLOAT3)
    normal.set_editor_property("Code", "return normalize(float3(.045*sin(P.x*.19+P.y*.13),.045*cos(P.y*.17-P.x*.11),1));")
    edit.connect_material_expressions(pos, "", normal, "P")
    edit.connect_material_property(normal, "", u.MaterialProperty.MP_NORMAL)
    rough = edit.create_material_expression(mat, u.MaterialExpressionConstant, -300, 430)
    rough.r = .87; edit.connect_material_property(rough, "", u.MaterialProperty.MP_ROUGHNESS)
    if rock: edit.set_material_usage(mat, u.MaterialUsage.MATUSAGE_INSTANCED_STATIC_MESHES)
    edit.recompile_material(mat)
    u.EditorAssetLibrary.save_loaded_asset(mat)
    return mat


soil = material("M_RangeSoil")
rock = material("M_RangeRock", True)
terrain_path = ROOT + "/SM_RangeSurface"
if not u.EditorAssetLibrary.does_asset_exist(terrain_path):
    mesh = u.DynamicMesh()
    u.GeometryScript_Primitives.append_rectangle_xy(mesh, u.GeometryScriptPrimitiveOptions(), u.Transform(), 8400, 2800, 84, 28)
    count = u.GeometryScript_MeshQueries.get_vertex_count(mesh)
    for index in range(count):
        position, valid = u.GeometryScript_MeshQueries.get_vertex_position(mesh, index)
        if not valid: continue
        edge = max(0., min(1., (abs(position.y)-950)/300))
        position.z = .1 + edge * (7 + 5*math.sin(position.x*.009)*math.cos(position.y*.013))
        u.GeometryScript_MeshEdits.set_vertex_position(mesh, index, position, True)
    options = u.GeometryScriptCreateNewStaticMeshAssetOptions()
    options.enable_recompute_normals = True
    options.enable_recompute_tangents = True
    options.enable_collision = False
    terrain, outcome = u.GeometryScript_NewAssetUtils.create_new_static_mesh_asset_from_mesh(mesh, terrain_path, options)
    if terrain is None: raise RuntimeError("Geometry Script terrain bake failed: " + str(outcome))
    terrain.set_material(0, soil)
    u.EditorAssetLibrary.save_loaded_asset(terrain)

graph_path = ROOT + "/PCG_EdgeGravel"
if True:
    graph = u.load_asset(graph_path) if u.EditorAssetLibrary.does_asset_exist(graph_path) else tools.create_asset("PCG_EdgeGravel", ROOT, u.PCGGraph, u.PCGGraphFactory())
    for node in list(graph.get_editor_property("nodes")): graph.remove_node(node)
    grid, settings = graph.add_node_of_type(u.PCGCreatePointsGridSettings)
    settings.grid_extents = u.Vector(3750, 65, 0)
    settings.cell_size = u.Vector(80, 65, 100)
    settings.coordinate_space = u.PCGCoordinateSpace.LOCAL_COMPONENT
    transform, ts = graph.add_node_of_type(u.PCGTransformPointsSettings)
    ts.offset_min = u.Vector(-25,-20,2); ts.offset_max = u.Vector(25,20,5)
    ts.rotation_min = u.Rotator(-25,-180,-25); ts.rotation_max = u.Rotator(25,180,25)
    ts.scale_min = u.Vector(.04,.03,.02); ts.scale_max = u.Vector(.18,.13,.08); ts.uniform_scale = False
    spawn, ss = graph.add_node_of_type(u.PCGStaticMeshSpawnerSettings)
    entry = u.PCGMeshSelectorWeightedEntry()
    descriptor = entry.get_editor_property("descriptor")
    descriptor.set_editor_property("static_mesh", u.load_asset("/Engine/BasicShapes/Sphere"))
    descriptor.set_editor_property("override_materials", [rock])
    body_instance = descriptor.get_editor_property("body_instance")
    body_instance.set_editor_property("collision_profile_name", "NoCollision")
    descriptor.set_editor_property("body_instance", body_instance)
    entry.set_editor_property("descriptor", descriptor)
    ss.mesh_selector_parameters.set_editor_property("mesh_entries", [entry])
    graph.add_edge(grid, "Out", transform, "In")
    graph.add_edge(transform, "Out", spawn, "In")
    u.EditorAssetLibrary.save_loaded_asset(graph)

RESULT = {"terrain": terrain_path, "graph": graph_path, "material": soil.get_path_name()}
