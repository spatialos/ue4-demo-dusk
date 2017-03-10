#!/bin/sh

echo Removing old snapshots first...
rm -rf "snapshots/default.snapshot"

echo Generating new snapshot...
"$UNREAL_HOME"/Engine/Binaries/Win64/UE4Editor-Cmd.exe "$(pwd)"/workers/unreal/Engineers.uproject -run=ExportSnapshotCommandlet 0 default.snapshot
