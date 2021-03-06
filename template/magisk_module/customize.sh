SKIPUNZIP=1

# check_architecture
if [ "$ARCH" != "arm" ] && [ "$ARCH" != "arm64" ] && [ "$ARCH" != "x86" ] && [ "$ARCH" != "x64" ]; then
  abort "! Unsupported platform: $ARCH"
else
  ui_print "- Device platform: $ARCH"
fi

# extract verify.sh
ui_print "- Extracting verify.sh"
unzip -o "$ZIPFILE" 'verify.sh' -d "$TMPDIR" >&2
if [ ! -f "$TMPDIR/verify.sh" ]; then
  ui_print    "*********************************************************"
  ui_print    "! Unable to extract verify.sh!"
  ui_print    "! This zip may be corrupted, please try downloading again"
  abort "*********************************************************"
fi
. $TMPDIR/verify.sh

# extract riru.sh
extract "$ZIPFILE" 'riru.sh' "$MODPATH"
. $MODPATH/riru.sh

check_riru_version

# extract libs
ui_print "- Extracting module files"

extract "$ZIPFILE" 'module.prop' "$MODPATH"
extract "$ZIPFILE" 'post-fs-data.sh' "$MODPATH"
extract "$ZIPFILE" 'uninstall.sh' "$MODPATH"


if [ "$ARCH" = "x86" ] || [ "$ARCH" = "x64" ]; then
  ui_print "- Extracting x86 libraries"
  extract "$ZIPFILE" "system_x86/lib/libriru_$RIRU_MODULE_ID.so" "$MODPATH"
  mv "$MODPATH/system_x86" "$MODPATH/system"

  if [ "$IS64BIT" = true ]; then
    ui_print "- Extracting x64 libraries"
    extract "$ZIPFILE" "system_x86/lib64/libriru_$RIRU_MODULE_ID.so" "$MODPATH"
    mv "$MODPATH/system_x86/lib64" "$MODPATH/system/lib64"
  fi
else
  ui_print "- Extracting arm libraries"

  REPLACEMENT_DIR="system/*"
  # 一劳永逸。完全提取。在某些一加手机上这个方法不能提取 system/lib/ 里面的数据，为啥？是unzip这个命令工具的问题。
  # 注意加上引号
  unzip -o "$ZIPFILE" "$REPLACEMENT_DIR" -d "$MODPATH" >&2 || abort "! Can't extract system/: $?"

#  extract "$ZIPFILE" "system/lib/libgadget.so" "$MODPATH"
#  extract "$ZIPFILE" "system/lib/libgadget.config.so" "$MODPATH"
#  extract "$ZIPFILE" "system/lib/libriru_FridaGadgetRiruMoudle.so" "$MODPATH"
#
#  extract "$ZIPFILE" "system/lib64/libgadget.so" "$MODPATH"
#  extract "$ZIPFILE" "system/lib64/libgadget.config.so" "$MODPATH"
#  extract "$ZIPFILE" "system/lib64/libriru_FridaGadgetRiruMoudle.so" "$MODPATH"

fi

set_perm_recursive "$MODPATH" 0 0 0755 0644

# extract Riru files
ui_print "- Extracting extra files"
[ -d "$RIRU_MODULE_PATH" ] || mkdir -p "$RIRU_MODULE_PATH" || abort "! Can't create $RIRU_MODULE_PATH"

rm -f "$RIRU_MODULE_PATH/module.prop.new"
extract "$ZIPFILE" 'riru/module.prop.new' "$RIRU_MODULE_PATH" true
set_perm "$RIRU_MODULE_PATH/module.prop.new" 0 0 0600 $RIRU_SECONTEXT