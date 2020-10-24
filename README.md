# Riru - Template

[Riru](https://github.com/RikkaApps/Riru) module template.

## Build

1. Rename `module.example.gradle` to `module.gradle`
2. Replace module info in `module.gradle` (all lines end with "replace with yours")
3. Write your codes
4. Run gradle task `:module:assembleRelease` task from Android Studio or command line, zip will be saved in `out`.

### About prefab

The template use the prefab feature for native dependencies. The prefab support is added from AGP 4.0 but only works correctly on higher versions.
If you can't or don't willing to use APG 4.1, you can comment out prefab-related parts in `build.gradle` and copy `riru.h` from [RikkaApps/Riru](https://github.com/RikkaApps/Riru/blob/master/module/src/main/cpp/include/riru.h).

## API changes

### v9, Riru v22.0

#### API

Functions like `nativeForkAnd...` do not need to be exported directly. The only function to export is `void *init(void *)`. See the comment of `init` and template's implementation for more.

This has these advantages:

* Module can support different Riru versions
* Riru itself will not relay on ".prop" file (unreliable) to get module information

#### Riru

Starting v22.0, Riru has switched to "native bridge" (`ro.dalvik.vm.native.bridge`) to inject zygote, this will lead that Riru and modules later to be loaded ([`LoadNativeBridge`](https://cs.android.com/android/platform/superproject/+/android-11.0.0_r1:art/libnativebridge/native_bridge.cc;l=227) vs `__attribute__((constructor))`).

For most modules, this should have not problem, but modules like Xposed framework which know to rely on the old behavior may have to make changes. 

> Magisk may provider Riru-like features in the far future, and of course, it will have more strict restrictions that module codes will not be run in zygote. Maybe Xposed framework modules should prepare for this?

Riru v22 also provides hide function to make the memory of the module to anonymous memory ([see the implementation](https://github.com/RikkaApps/Riru/blob/master/core/src/main/cpp/hide.cpp)). This is an opt-in behavior (`module->supportHide`) and Riru itself also has a global toggle (`/data/adb/riru/enable_hide`).