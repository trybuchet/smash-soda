# Development and Contributing

This page focuses on contributor workflow.  
If you need setup/build instructions first, use [Manual Build (Windows)](manual-build.md).

## Work on Smash Soda in VS Code

This repository includes shared VS Code task/launch profiles in:

- `.vscode/tasks.json`
- `.vscode/launch.json`

### Build in VS Code

- Press `Ctrl+Shift+B` to run the default build task (`cmake-build-release`).
- Or run specific tasks from `Terminal -> Run Task`:
  - `cmake-build-release`
  - `cmake-build-debug` (builds `RelWithDebInfo`)

### Run or Debug in VS Code

Open `Run and Debug` and select:

- `SmashSoda Debug (CMake)` for step debugging (`F5`)
- `SmashSoda Release (CMake)` for release run (`Ctrl+F5`)

Both profiles launch from the executable output folder so runtime assets like `fonts/`, `icons/`, and `sfx/` load correctly.

## Contributing with Pull Requests

Before coding:

1. Check existing issues, or open one describing the change you want to make.
2. Fork the repo to your GitHub account.
3. Create a focused feature branch.

```powershell
git checkout -b feat/short-description
```

While working:

1. Keep changes scoped to one feature/fix.
2. Build at least:
   - `Release`
   - `RelWithDebInfo`
3. Update docs if behavior/setup changed.

Submit your PR:

1. Commit your branch.
2. Push to your fork.
3. Open a pull request to `soda-arcade/smash-soda`.
4. Include:
   - What changed
   - Why it changed
   - Any issue link(s)
   - Screenshots/video for UI changes when useful

After opening:

1. Respond to review feedback.
2. Push follow-up commits to the same branch until approved.
