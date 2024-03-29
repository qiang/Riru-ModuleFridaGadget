#include <jni.h>
#include <sys/types.h>
#include <riru.h>
#include <malloc.h>
#include <vector>

#include <cstring>
#include <dlfcn.h>
#include "logging.h"
#include "main.h"

static char nice_process_name[256] = {0};
static char package_name[256] = {0};

static jint my_uid = 0;

static bool isApp(int uid) {
    if (uid < 0) {
        return false;
    }
    int appId = uid % 100000;

    // limit only regular app, or strange situation will happen, such as zygote process not start (dead for no reason and leave no clues?)
    // https://android.googlesource.com/platform/frameworks/base/+/android-9.0.0_r8/core/java/android/os/UserHandle.java#151
    return appId >= 10000 && appId <= 19999;
}

static void
my_forkAndSpecializePre(JNIEnv *env, jint *uid, jstring *niceName, jstring *appDataDir) {
    //LOGI("Q_M xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx %s", "forkAndSpecializePre");
    my_uid = *uid;
    if (!isApp(my_uid)) {
        return;
    }

    const char *tablePath = (env->GetStringUTFChars(*niceName, 0));
    sprintf(nice_process_name, "%s", tablePath);
    delete tablePath;

    if (!appDataDir) {
        LOGI("Q_M forkAndSpecializePre appDataDir null");
        return;
    }

    const char *app_data_dir = env->GetStringUTFChars(*appDataDir, NULL);
    if (app_data_dir == nullptr) {
        return;
    }
    //LOGI("Q_M xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx app_data_dir %s",app_data_dir);

    int user = 0;
    if (sscanf(app_data_dir, "/data/%*[^/]/%d/%s", &user, package_name) != 2) {
        if (sscanf(app_data_dir, "/data/%*[^/]/%s", package_name) != 1) {
            package_name[0] = '\0';
            LOGI("Q_M can't parse %s", app_data_dir);
        }
    }
    env->ReleaseStringUTFChars(*appDataDir, app_data_dir);

}

static void my_forkAndSpecializePost(JNIEnv *env) {
    if (!isApp(my_uid)) {
        return;
    }

//    if (!strstr(nice_process_name, "com.smile.gifmaker")
//        && !strstr(nice_process_name, "com.ss.android.ugc.aweme")
//        && !strstr(nice_process_name, "com.xingin.xhs")
//            ) {
//        return;
//    }

    //http://www.cplusplus.com/reference/cstdio/fread/ 读取整个文件
    char *white_list;
    //白名单的pkgName 最好以逗号或者分好分割开来
    const char *filepath = "/data/local/tmp/_white_list.config";
    FILE *fp = nullptr;
    fp = fopen(filepath, "r");
    if (fp != nullptr) {

        fseek(fp, 0, SEEK_END);
        int fileLen = ftell(fp);
        white_list = (char *) malloc(sizeof(char) * (fileLen + 1));
        fseek(fp, 0, SEEK_SET);
//        size_t count = fread(white_list, fileLen, sizeof(char), fp);
        size_t count = fread(white_list, 1, fileLen, fp);
//        LOGI("Q_M xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 白名单长度 %zu", count);
        white_list[count] = '\0';
        fclose(fp);

        LOGI("Q_M xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx 白名单：%s", white_list);
    } else {
        white_list = "";
    }

    if (!strstr(white_list, package_name)) {
        return;
    }

    LOGI("Q_M xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx nice_process_name=%s, pkg=%s,uid=%d, isApp= %d",
         nice_process_name, package_name, my_uid,
         isApp(my_uid));

    //添加这种机制，就可以提前设置进程名， 从而让frida 的gadget 能够识别到
    jclass java_Process = env->FindClass("android/os/Process");
    if (java_Process != nullptr && isApp(my_uid)) {
        jmethodID mtd_setArgV0 = env->GetStaticMethodID(java_Process, "setArgV0",
                                                        "(Ljava/lang/String;)V");
        jstring name = env->NewStringUTF(nice_process_name);
        env->CallStaticVoidMethod(java_Process, mtd_setArgV0, name);

        void *handle = dlopen(nextLoadSo, RTLD_LAZY);
        if (!handle) {
            //        LOGE("%s",dlerror());
            LOGE("Q_M  %s loaded in libgadget 出错 %s", nice_process_name, dlerror());
        } else {
            LOGI("Q_M xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx-> %s 加载 ' %s ' 成功 ", nice_process_name,
                 nextLoadSo);
        }
    }
}


static void forkAndSpecializePre(
        JNIEnv *env, jclass clazz, jint *uid, jint *gid, jintArray *gids, jint *runtimeFlags,
        jobjectArray *rlimits, jint *mountExternal, jstring *seInfo, jstring *niceName,
        jintArray *fdsToClose, jintArray *fdsToIgnore, jboolean *is_child_zygote,
        jstring *instructionSet, jstring *appDataDir, jboolean *isTopApp,
        jobjectArray *pkgDataInfoList,
        jobjectArray *whitelistedDataInfoList, jboolean *bindMountAppDataDirs,
        jboolean *bindMountAppStorageDirs) {
    my_forkAndSpecializePre(env, uid, niceName, appDataDir);
}

//很遗憾，执行这行代码的时候，还没有设置进程名字，导致，这个方法里面加载 frida gadget 的动态库获取不到进程名
// 只有执行完 Zygote.forkAndSpecialize 会在 handleChildProc 里面设置进程名，Process.setArgV0(parsedArgs.niceName);
// libandroid_runtime.so
//https://cs.android.com/android/platform/superproject/+/master:frameworks/base/core/jni/android_util_Process.cpp;l=593?q=setSwappiness&ss=android%2Fplatform%2Fsuperproject
static void forkAndSpecializePost(JNIEnv *env, jclass clazz, jint res) {
    if (res == 0) {
        my_forkAndSpecializePost(env);
    } else {
        // in zygote process, res is child pid
        // don't print log here, see https://github.com/RikkaApps/Riru/blob/77adfd6a4a6a81bfd20569c910bc4854f2f84f5e/riru-core/jni/main/jni_native_method.cpp#L55-L66
    }
}

static void specializeAppProcessPre(
        JNIEnv *env, jclass clazz, jint *uid, jint *gid, jintArray *gids, jint *runtimeFlags,
        jobjectArray *rlimits, jint *mountExternal, jstring *seInfo, jstring *niceName,
        jboolean *startChildZygote, jstring *instructionSet, jstring *appDataDir,
        jboolean *isTopApp, jobjectArray *pkgDataInfoList, jobjectArray *whitelistedDataInfoList,
        jboolean *bindMountAppDataDirs, jboolean *bindMountAppStorageDirs) {
    // added from Android 10, but disabled at least in Google Pixel devices

    my_forkAndSpecializePre(env, uid, niceName, appDataDir);
}

static void specializeAppProcessPost(
        JNIEnv *env, jclass clazz) {
    // added from Android 10, but disabled at least in Google Pixel devices
    my_forkAndSpecializePost(env);
}

static void forkSystemServerPre(
        JNIEnv *env, jclass clazz, uid_t *uid, gid_t *gid, jintArray *gids, jint *runtimeFlags,
        jobjectArray *rlimits, jlong *permittedCapabilities, jlong *effectiveCapabilities) {

}

static void forkSystemServerPost(JNIEnv *env, jclass clazz, jint res) {
    if (res == 0) {
        // in system server process
    } else {
        // in zygote process, res is child pid
        // don't print log here, see https://github.com/RikkaApps/Riru/blob/77adfd6a4a6a81bfd20569c910bc4854f2f84f5e/riru-core/jni/main/jni_native_method.cpp#L55-L66
    }
}

static int shouldSkipUid(int uid) {
    // By default (the module does not provide this function in init), Riru will only call
    // module functions in "normal app processes" (10000 <= uid % 100000 <= 19999)

    // Provide this function so that the module can control if a specific uid should be skipped
    return false;
}

static void onModuleLoaded() {
    // called when the shared library of Riru core is loaded
}

extern "C" {

int riru_api_version;
RiruApiV9 *riru_api_v9;

/*
 * Init will be called three times.
 *
 * The first time:
 *   Returns the highest version number supported by both Riru and the module.
 *
 *   arg: (int *) Riru's API version
 *   returns: (int *) the highest possible API version
 *
 * The second time:
 *   Returns the RiruModuleX struct created by the module.
 *   (X is the return of the first call)
 *
 *   arg: (RiruApiVX *) RiruApi strcut, this pointer can be saved for further use
 *   returns: (RiruModuleX *) RiruModule strcut
 *
 * The third time:
 *   Let the module to cleanup (such as RiruModuleX struct created before).
 *
 *   arg: null
 *   returns: (ignored)
 *
 */
void *init(void *arg) {
    static int step = 0;
    step += 1;
    LOGI("Q_M Riru-ModuleFridaGadget xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx %s", "init");

    static void *_module;

    switch (step) {
        case 1: {
            auto core_max_api_version = *(int *) arg;
            riru_api_version =
                    core_max_api_version <= RIRU_MODULE_API_VERSION ? core_max_api_version
                                                                    : RIRU_MODULE_API_VERSION;
            return &riru_api_version;
        }
        case 2: {
            switch (riru_api_version) {
                // RiruApiV10 and RiruModuleInfoV10 are equal to V9
                case 10:
                case 9: {
                    riru_api_v9 = (RiruApiV9 *) arg;

                    auto module = (RiruModuleInfoV9 *) malloc(sizeof(RiruModuleInfoV9));
                    memset(module, 0, sizeof(RiruModuleInfoV9));
                    _module = module;

                    module->supportHide = true;

                    module->version = RIRU_MODULE_VERSION;
                    module->versionName = RIRU_MODULE_VERSION_NAME;
                    module->onModuleLoaded = onModuleLoaded;
                    module->shouldSkipUid = shouldSkipUid;
                    module->forkAndSpecializePre = forkAndSpecializePre;
                    module->forkAndSpecializePost = forkAndSpecializePost;
                    module->specializeAppProcessPre = specializeAppProcessPre;
                    module->specializeAppProcessPost = specializeAppProcessPost;
                    module->forkSystemServerPre = forkSystemServerPre;
                    module->forkSystemServerPost = forkSystemServerPost;
                    return module;
                }
                default: {
                    return nullptr;
                }
            }
        }
        case 3: {
            free(_module);
            return nullptr;
        }
        default: {
            return nullptr;
        }
    }
}
}