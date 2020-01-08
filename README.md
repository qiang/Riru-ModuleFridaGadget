# Riru - Template

[Riru](https://github.com/RikkaApps/Riru) module template.

## Build

1. Replace all "template" to your name
   
   * `module/build.gradle`: "template" in variable `moduleId`, `moduleProp`
   * `module/src/main/cpp/CMakeLists.txt`: "riru_template" in `set(MODULE_NAME )`
   * `template/magisk_module/post-fs-data.sh`: `MODULE_ID="template"`
   * `template/magisk_module/uninstall.sh`: `MODULE_ID="template"`

2. Write you codes
3. Run gradle task `:module:assembleRelease` task from Android Studio or command line, zip will be saved in `out`.

## Notes

* DO NOT overwrite `android.os.SystemProperties#native_set` in core, or your data may be wiped
  ([Detail info](https://github.com/RikkaApps/Riru/blob/v7/riru-core/jni/main/jni_native_method.cpp#L162-L176))
  (If you really need to hook this, remember to clear exception)
* DO NO print log (`__android_log_print`) in `nativeForkAndSpecialize(Pre/Post)` `nativeForkSystemServer(Pre/Post)` when in zygote process, or it may cause zygote not work
  (magic not confirmed, [Detail info](https://github.com/RikkaApps/Riru/blob/77adfd6a4a6a81bfd20569c910bc4854f2f84f5e/riru-core/jni/main/jni_native_method.cpp#L55-L66))
* Add `-ffixed-x18` to both compiler and linker parameter, or it will cause problems on Android Q (see template)