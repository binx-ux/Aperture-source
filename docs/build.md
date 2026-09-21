# Build

## requirements

- Windows
- Visual Studio with Desktop development with C++
- this repo (imgui + minhook are under `third_party\`)

## steps

1. open `FortniteExternalCheat.sln`
2. set configuration to **Release**, platform **x64**
3. build

or from a VS developer prompt:

```
MSBuild FortniteExternalCheat.sln /p:Configuration=Release /p:Platform=x64
```

## output

`bin\Release\Aperture.dll`

## notes

- Debug can work but Release is what gets checked
- you still need your own way to load the dll into the target process
- if the link fails, make sure `third_party\imgui` and `third_party\minhook` are actually present
