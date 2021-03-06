//
//  simulate_crash
//  utilities
//
//  Copyright (c) 2014 Apple Inc. All Rights Reserved.
//

#include "debugging.h"

#include <dispatch/dispatch.h>
#include <dlfcn.h>
#include <mach/mach.h>

/// Type to represent a boolean value.
#if TARGET_OS_IPHONE  &&  __LP64__
typedef bool BOOL;
#else
typedef signed char BOOL;
// BOOL is explicitly signed so @encode(BOOL) == "c" rather than "C"
// even if -funsigned-char is used.
#endif

static void __security_simulatecrash_link(CFStringRef reason, uint32_t code)
{
#if !TARGET_IPHONE_SIMULATOR
    // Prototype defined in <CrashReporterSupport/CrashReporterSupport.h>, but objC only.
    // Soft linking here so we don't link unless we hit this.
    static BOOL (*__SimulateCrash)(pid_t pid, mach_exception_data_type_t exceptionCode, CFStringRef description);

    static dispatch_once_t once = 0;
    dispatch_once(&once, ^{
        void *image = dlopen("/System/Library/PrivateFrameworks/CrashReporterSupport.framework/CrashReporterSupport", RTLD_NOW);
        if (image)
            __SimulateCrash = dlsym(image, "SimulateCrash");
        else
            __SimulateCrash = NULL;
    });

    if (__SimulateCrash)
        __SimulateCrash(getpid(), code, reason);
    else
        secerror("SimulateCrash not available");
#else
    secerror("SimulateCrash not available in iOS simulator");
#endif
}

static int __simulate_crash_counter = -1;

void __security_simulatecrash(CFStringRef reason, uint32_t code)
{
    secerror("Simulating crash, reason: %@, code=%08x", reason, code);
    if (__simulate_crash_counter < 0)
        __security_simulatecrash_link(reason, code);
    else
        __simulate_crash_counter++;
}

void __security_stackshotreport(CFStringRef reason, uint32_t code)
{
    secerror("stackshot report, reason: %@, code=%08x", reason, code);
#if !TARGET_IPHONE_SIMULATOR
    // Prototype defined in <CrashReporterSupport/CrashReporterSupport.h>, but objC only.
    // Soft linking here so we don't link unless we hit this.
    static BOOL (*__WriteStackshotReport)(void *, mach_exception_data_type_t) = NULL;

    static dispatch_once_t once = 0;
    dispatch_once(&once, ^{
        void *image = dlopen("/System/Library/PrivateFrameworks/CrashReporterSupport.framework/CrashReporterSupport", RTLD_NOW);
        if (image)
            __WriteStackshotReport = dlsym(image, "WriteStackshotReport");
    });

    if (__WriteStackshotReport)
        __WriteStackshotReport((void *)reason, code);
    else
        secerror("WriteStackshotReport not available");
#else
    secerror("WriteStackshotReport not available in iOS simulator");
#endif
}


int __security_simulatecrash_enable(bool enable)
{
    int count = __simulate_crash_counter;
    __simulate_crash_counter = enable ? -1 : 0;
    return count;
}
