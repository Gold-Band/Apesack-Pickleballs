import bpy
import json
import os

output_path = bpy.path.abspath("//scene_export.json")

export_data = []

for obj in bpy.context.scene.objects:
    if obj.instance_type == 'COLLECTION' and obj.instance_collection:
        export_data.append({
            "name": obj.name,
            "collection": obj.instance_collection.name,
            "location": list(obj.location),
            "rotation": list(obj.rotation_euler),
            "scale": list(obj.scale)
        })

with open(output_path, 'w') as f:
    json.dump(export_data, f, indent=4)

print(f"Exported {len(export_data)} collection instances to {output_path}")

