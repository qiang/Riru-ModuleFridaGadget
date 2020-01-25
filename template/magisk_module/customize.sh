SKIPUNZIP=1
RIRU_PATH="/data/misc/riru"
RIRU_MODULE_ID="template"
RIRU_MODULE_PATH="$RIRU_PATH/modules/$RIRU_MODULE_ID"

# check architecture
if [[ "$ARCH" != "arm" && "$ARCH" != "arm64" && "$ARCH" != "x86" && "$ARCH" != "x64" ]]; then
  abort "! Unsupported platform: $ARCH"
else
  ui_print "- Device platform: $ARCH"
fi

# check Riru version
if [[ ! -f "$RIRU_PATH/api_version" ]]; then
  ui_print "*********************************************************"
  ui_print "! 'Riru - Core' is not installed"
  ui_print "! You can download from 'Magisk Manager' or https://github.com/RikkaApps/Riru/releases"
  abort    "*********************************************************"
fi
RIRU_API_VERSION=$(cat "$RIRU_PATH/api_version")
ui_print "- Riru API version: $RIRU_API_VERSION"
if [[ "$RIRU_API_VERSION" -lt 4 ]]; then
  ui_print "*********************************************************"
  ui_print "! The latest version of 'Riru - Core' is required"
  ui_print "! You can download from 'Magisk Manager' or https://github.com/RikkaApps/Riru/releases"
  abort    "*********************************************************"
fi

# extract verify.sh
unzip -o "$ZIPFILE" 'verify.sh' -d "$TMPDIR" >&2
if [[ ! -f "$TMPDIR/verify.sh" ]]; then
  ui_print "*********************************************************"
  ui_print "! Unable to extract verify.sh!"
  ui_print "! This zip may be corrupted, please try downloading again"
  abort    "*********************************************************"
fi
. $TMPDIR/verify.sh

# extract libs
ui_print "- Extracting module files"

extract "$ZIPFILE" 'module.prop' "$MODPATH"
extract "$ZIPFILE" 'post-fs-data.sh' "$MODPATH"
extract "$ZIPFILE" 'uninstall.sh' "$MODPATH"
#extract "$ZIPFILE" 'sepolicy.rule' "$MODPATH"

if [[ "$ARCH" == "x86" || "$ARCH" == "x64" ]]; then
  ui_print "- Extracting x86 libraries"
  extract "$ZIPFILE" "system_x86/lib/libriru_$RIRU_MODULE_ID.so" "$MODPATH"
  mv "$MODPATH/system_x86/lib" "$MODPATH/system/lib"

  if [[ "$IS64BIT" == true ]]; then
    ui_print "- Extracting x64 libraries"
    extract "$ZIPFILE" "system_x86/lib64/libriru_$RIRU_MODULE_ID.so" "$MODPATH"
    mv "$MODPATH/system_x86/lib64" "$MODPATH/system/lib64"
  fi
else
  ui_print "- Extracting arm libraries"
  extract "$ZIPFILE" "system/lib/libriru_$RIRU_MODULE_ID.so" "$MODPATH"

  if [[ "$IS64BIT" == true ]]; then
    ui_print "- Extracting arm64 libraries"
    extract "$ZIPFILE" "system/lib64/libriru_$RIRU_MODULE_ID.so" "$MODPATH"
  fi
fi

# Riru files
ui_print "- Extracting extra files"
[[ -d "$RIRU_MODULE_PATH" ]] || mkdir -p "$RIRU_MODULE_PATH" || abort "! Can't create $RIRU_MODULE_PATH"

# set permission just in case
set_perm "$RIRU_PATH" 0 0 0700
set_perm "$RIRU_PATH/modules" 0 0 0700
set_perm "$RIRU_MODULE_PATH" 0 0 0700
set_perm "$RIRU_MODULE_PATH/bin" 0 0 0700

rm -f "$RIRU_MODULE_PATH/module.prop.new"
extract "$ZIPFILE" 'riru/module.prop.new' "$RIRU_MODULE_PATH" true
set_perm "$RIRU_MODULE_PATH/module.prop.new" 0 0 0600

# set permissions
ui_print "- Setting permissions"
set_perm_recursive "$MODPATH" 0 0 0755 0644
