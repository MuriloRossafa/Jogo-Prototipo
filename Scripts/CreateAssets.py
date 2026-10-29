"""Execute only through UnrealEditor-Cmd: creates the map and imports the warning cue."""
from pathlib import Path
import unreal

root = Path(unreal.Paths.project_dir())
lib = unreal.EditorAssetLibrary
lib.make_directory('/Game/Audio')
lib.make_directory('/Game/Maps')
if not lib.does_asset_exist('/Game/Audio/Warning'):
    source = root / 'SourceAudio' / 'Warning.wav'
    if not source.is_file():
        raise RuntimeError(f'Missing source: {source}')
    task = unreal.AssetImportTask()
    task.set_editor_property('filename', str(source))
    task.set_editor_property('destination_path', '/Game/Audio')
    task.set_editor_property('destination_name', 'Warning')
    task.set_editor_property('automated', True)
    task.set_editor_property('save', True)
    unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks([task])
    if not lib.does_asset_exist('/Game/Audio/Warning'):
        raise RuntimeError('Warning audio import failed')
if not lib.does_asset_exist('/Game/Maps/Case001'):
    world = unreal.EditorLoadingAndSavingUtils.new_blank_map(False)
    if not world or not unreal.EditorLoadingAndSavingUtils.save_map(world, '/Game/Maps/Case001'):
        raise RuntimeError('Map creation failed')
unreal.log('OTHER_SIDE_ASSETS_READY')
