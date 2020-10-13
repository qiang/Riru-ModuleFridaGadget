# Riru - Template

[Riru](https://github.com/RikkaApps/Riru) module template.

## Build

1. Rename `module.example.gradle` to `module.gradle`
2. Replace module info in `module.gradle` (all lines end with `// FIXME replace with yours`)
3. Write you codes
4. Run gradle task `:module:assembleRelease` task from Android Studio or command line, zip will be saved in `out`.

## About Android Gradle plugin 4.1+ requirement

The template use prefab feature for native dependencies. The prefab support is added from AGP 4.0 but only works correctly on higher versions.
If you can't or don't willing to use APG 4.1, you can comment out prefab related parts in `build.gradle` and copy `riru.h` from [RikkaApps/Riru](https://github.com/RikkaApps/Riru/blob/master/module/src/main/cpp/include/riru.h).

## Notes

* DO NOT overwrite `android.os.SystemProperties#native_set` in core, or your data may be wiped
  ([Detail info](https://github.com/RikkaApps/Riru/blob/v7/riru-core/jni/main/jni_native_method.cpp#L162-L176))
  (If you really need to hook this, remember to clear exception)
* DO NO print log (`__android_log_print`) in `nativeForkAndSpecialize(Pre/Post)` `nativeForkSystemServer(Pre/Post)` when in zygote process, or it may cause zygote not work
  (magic not confirmed, [Detail info](https://github.com/RikkaApps/Riru/blob/77adfd6a4a6a81bfd20569c910bc4854f2f84f5e/riru-core/jni/main/jni_native_method.cpp#L55-L66))
* Add `-ffixed-x18` to both compiler and linker parameter, or it will cause problems on Android Q (see template)