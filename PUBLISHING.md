# Publishing a new release

## Preliminary steps

- Build and test the AppImage on a clean Ubuntu 24.04 system
- Make sure CI tests pass

## Making a major or minor release

- Create a new branch vX.Y and check it out
- Update the change log
- Update the version in meson.build to X.Y.0
- Commit the changes and push the branch
- Build the AppImage
  - Make sure the source code is clean and up to date
  - Use the script `build-aux/build_appimage.sh` to build the AppImage
- On Github, create a release from the branch. 
  - Tag it with vX.Y.0.
  - Title it vX.Y.0
  - Add a description that reflects the change log
  - Upload the AppImage binary and rename it to `Gorfector-vX_Y_0-x86_64-Ubuntu-24_04.AppImage`
  - Save as draft and review.
  - Publish the release

## Making a patch release

- Check out and pull the branch to be patched (vX.Y)
- Update the change log
- Update the version in meson.build to X.Y.Z
- Commit the changes and push the branch
- Build the AppImage
  - Make sure the source code is clean and up to date
  - Use the script `build-aux/build_appimage.sh` to build the AppImage
- On Github, create a release from the branch. 
  - Tag it with vX.Y.Z.
  - Title it vX.Y.Z
  - Add a description that reflects the change log
  - Upload the AppImage binary and rename it to `Gorfector-vX_Y_Z-x86_64-Ubuntu-24_04.AppImage`
  - Save as draft and review.
  - Publish the release
- Cherry-pick the changes to the main branch
