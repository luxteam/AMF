//
// Notice Regarding Standards.  AMD does not provide a license or sublicense to
// any Intellectual Property Rights relating to any standards, including but not
// limited to any audio and/or video codec technologies such as MPEG-2, MPEG-4;
// AVC/H.264; HEVC/H.265; AAC decode/FFMPEG; AAC encode/FFMPEG; VC-1; and MP3
// (collectively, the "Media Technologies"). For clarity, you will pay any
// royalties due for such third party technologies, which may include the Media
// Technologies that are owed as a result of AMD providing the Software to you.
//
// MIT license
//
// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#include "../Thread.h"


#if defined (__linux) || defined(__APPLE__) || defined(__MACOSX)

#if defined(__GNUC__)
    //disable gcc warinings on STL code
    #pragma GCC diagnostic ignored "-Weffc++"
#endif

#define POSIX

#include <locale>
#include <algorithm>
#include <dirent.h>
#include <fnmatch.h>
#if !defined(__APPLE__) && !defined(__MACOSX)
#include <malloc.h>
#endif
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/mman.h>

#if defined(__ANDROID__)
#include <android/log.h>
#endif

#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>

#include "../AMFSTL.h"

using namespace amf;

extern "C" void AMF_STD_CALL amf_debug_trace(const wchar_t* text);

void perror(const char* errorModule)
{
    char buffer[256] = {};
#if defined(__ANDROID__)
    strerror_r(errno, buffer, 256);
    fprintf(stderr, "\n%s: %s\n", buffer, errorModule);
#else
    /*char* err = */strerror_r(errno, buffer, 256);
    fprintf(stderr, "\n%s: %s\n", buffer, errorModule);
#endif

    exit(1);
}

amf_uint32 AMF_STD_CALL get_current_thread_id()
{
#ifndef __APPLE__
    return static_cast<amf_uint64>(pthread_self());
#else
    throw "Error: not implemented!";
#endif
}

// int clock_gettime(clockid_t clk_id, struct timespec *tp);
//----------------------------------------------------------------------------------------
// threading
//----------------------------------------------------------------------------------------
amf_long AMF_STD_CALL amf_atomic_inc(amf_long* X)
{
    return __sync_add_and_fetch(X, 1);
}
//----------------------------------------------------------------------------------------
amf_long AMF_STD_CALL amf_atomic_dec(amf_long* X)
{
    return __sync_sub_and_fetch(X, 1);
}
//----------------------------------------------------------------------------------------
amf_handle AMF_STD_CALL amf_create_critical_section()
{
    pthread_mutex_t* mutex = new pthread_mutex_t;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(mutex, &attr);

    return (amf_handle)mutex;
}
//----------------------------------------------------------------------------------------
bool AMF_STD_CALL amf_delete_critical_section(amf_handle cs)
{
    pthread_mutex_t* mutex = (pthread_mutex_t*)cs;
    int err = pthread_mutex_destroy(mutex);
    delete mutex;
    return err == 0;
}
//----------------------------------------------------------------------------------------
bool AMF_STD_CALL amf_enter_critical_section(amf_handle cs)
{
    pthread_mutex_t* mutex = (pthread_mutex_t*)cs;
    return pthread_mutex_lock(mutex) == 0;
}
//----------------------------------------------------------------------------------------
bool AMF_STD_CALL amf_leave_critical_section(amf_handle cs)
{
    pthread_mutex_t* mutex = (pthread_mutex_t*)cs;
    return pthread_mutex_unlock(mutex) == 0;
}
//----------------------------------------------------------------------------------------
struct MyEvent
{
    bool m_manual_reset;
    pthread_cond_t m_cond;
    pthread_mutex_t m_mutex;
    bool m_triggered;
};
//----------------------------------------------------------------------------------------

amf_handle AMF_STD_CALL amf_create_event(bool initially_owned, bool manual_reset, const wchar_t* name)
{
    MyEvent* event = new MyEvent;

    // Linux does not natively support Named Condition variables
    // so raise an error.
    // Implement this using boost (NamedCondition), Qt, or some other framework.
    if(name != NULL)
    {
        perror("Named Events not supported under Linux yet");
        exit(1);
    }
    event->m_manual_reset = manual_reset;
    pthread_cond_t cond_tmp = PTHREAD_COND_INITIALIZER;
    event->m_cond = cond_tmp;
    pthread_mutex_t mutex_tmp = PTHREAD_MUTEX_INITIALIZER;
    event->m_mutex = mutex_tmp;

    event->m_triggered = false;
    if(initially_owned)
    {
        amf_set_event((amf_handle)event);
    }

    return (amf_handle)event;
}
//----------------------------------------------------------------------------------------
bool AMF_STD_CALL amf_delete_event(amf_handle hevent)
{
    MyEvent* event = (MyEvent*)hevent;
    int err1 = pthread_mutex_destroy(&event->m_mutex);
    int err2 = pthread_cond_destroy(&event->m_cond);
    delete event;
    return err1 == 0 && err2 == 0;
}
//----------------------------------------------------------------------------------------
bool AMF_STD_CALL amf_set_event(amf_handle hevent)
{
    MyEvent* event = (MyEvent*)hevent;
    pthread_mutex_lock(&event->m_mutex);
    event->m_triggered = true;
    int err1 = pthread_cond_broadcast(&event->m_cond);
    pthread_mutex_unlock(&event->m_mutex);

    return err1 == 0;
}
//----------------------------------------------------------------------------------------
bool AMF_STD_CALL amf_reset_event(amf_handle hevent)
{
    MyEvent* event = (MyEvent*)hevent;
    pthread_mutex_lock(&event->m_mutex);
    event->m_triggered = false;
    int err = pthread_mutex_unlock(&event->m_mutex);

    return err == 0;
}
//----------------------------------------------------------------------------------------
static bool AMF_STD_CALL amf_wait_for_event_int(amf_handle hevent, unsigned long timeout, bool bTimeoutErr)
{
    bool ret = true;
    int err = 0;
    MyEvent* event = (MyEvent*)hevent;
    pthread_mutex_lock(&event->m_mutex);

    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    amf_uint64 start_time = ((amf_uint64)ts.tv_sec) * 1000 + ((amf_uint64)ts.tv_nsec) / 1000000; //to msec

    if(event->m_manual_reset)
    {
        while(!event->m_triggered)
        {
            if(timeout == AMF_INFINITE)
            {
                err = pthread_cond_wait(&event->m_cond, &event->m_mutex);          //MM todo - timeout is not supported
                ret = err == 0;
            }
            else
            {
                clock_gettime(CLOCK_REALTIME, &ts);
                amf_uint64 current_time = ((amf_uint64)ts.tv_sec) * 1000 + ((amf_uint64)ts.tv_nsec) / 1000000; //to msec
                if(current_time - start_time > (amf_uint64)timeout)
                {
                    ret = bTimeoutErr ? false : true;
                    break;
                }
                amf_uint64 to_wait = start_time + timeout;

                timespec abstime;
                abstime.tv_sec = (time_t)(to_wait / 1000); // timeout is in millisec
                abstime.tv_nsec = (time_t)((to_wait - ((amf_uint64)abstime.tv_sec) * 1000) * 1000000); // the rest to nanosec

                err = pthread_cond_timedwait(&event->m_cond, &event->m_mutex, &abstime);
                ret = err == 0;
            }
        }
    }
    else
    {
        if(event->m_triggered)
        {
            ret = true;
        }
        else
        {
            if (timeout == AMF_INFINITE) {
                err = pthread_cond_wait(&event->m_cond, &event->m_mutex);
            } else {
                start_time += timeout;
                timespec abstime;
                abstime.tv_sec = (time_t) (start_time / 1000); // timeout is in millisec
                abstime.tv_nsec = (time_t) ((start_time - (amf_uint64) (abstime.tv_sec) * 1000) *
                                            1000000); // the rest to nanosec
                err = pthread_cond_timedwait(&event->m_cond, &event->m_mutex, &abstime);
            }

            if (bTimeoutErr) {
                ret = (err == 0);
            } else {
                ret = (err == 0 || err == ETIMEDOUT);
            }
        }
        if(ret == true)
        {
            event->m_triggered = false;
        }
    }
    pthread_mutex_unlock(&event->m_mutex);

    return ret;
}
//----------------------------------------------------------------------------------------
bool AMF_STD_CALL amf_wait_for_event(amf_handle hevent, unsigned long timeout)
{
    return amf_wait_for_event_int(hevent, timeout, true);
}
//----------------------------------------------------------------------------------------
bool AMF_STD_CALL amf_wait_for_event_timeout(amf_handle hevent, amf_ulong ulTimeout)
{
    return amf_wait_for_event_int(hevent, ulTimeout, false);
}

struct SharedMemoryDescriptor
{
    int                 Handle      = 0;
    size_t              NameSize    = 0;
    char *              SharedName  = nullptr;

    SharedMemoryDescriptor(
        int             handle,
        size_t          nameSize,
        const char *    sharedName
        ):
        Handle          (handle),
        NameSize        (nameSize),
        SharedName      (new char[nameSize + 1])
    {
        strncpy(SharedName, sharedName, nameSize);
    }

    virtual ~SharedMemoryDescriptor()
    {
        delete [] SharedName;
    }
};

amf_handle AMF_STD_CALL amf_create_shared_memory(const char * name, bool * created)
{
    //name of the named shared memory
    //must be started by '/' does not have any more '/'
    //and have at least 1 character
    if((*name != '/') || (strlen(name) < 2) || strchr(name + 1, L'/'))
    {
        perror("Invalid shared object name");

        return nullptr;
    }

    //reset errno
    errno = 0;

    int sharedMemoryDescriptor = shm_open(name, O_RDWR, 0660);

    //try to open existing shared memory
    if((ENOENT == errno) || (-1 == sharedMemoryDescriptor))
    {
        //create new shared memory
        sharedMemoryDescriptor = shm_open(name, O_RDWR|O_CREAT, 0660);

        if(-1 == sharedMemoryDescriptor)
        {
            perror("Failed to open shared memory for named mutex");

            return nullptr;
        }

        if(created)
        {
            *created = true;
        }
    }
    else
    {
        if(created)
        {
            *created = false;
        }
    }

    SharedMemoryDescriptor *description = new SharedMemoryDescriptor(
        sharedMemoryDescriptor,
        strlen(name),
        name
        );

    if(!description)
    {
        shm_unlink(name);
    }

    return amf_handle(description);
}

bool AMF_CDECL_CALL amf_delete_shared_memory(amf_handle handle)
{
    SharedMemoryDescriptor *sharedMemoryDescriptor(
        reinterpret_cast<SharedMemoryDescriptor *>(handle)
        );

    if(!sharedMemoryDescriptor)
    {
        perror("Internal error");

        return false;
    }

    bool deleted = 0 == shm_unlink(sharedMemoryDescriptor->SharedName);

    if(deleted)
    {
        delete sharedMemoryDescriptor;
    }

    return deleted;
}

//----------------------------------------------------------------------------------------
amf_handle AMF_STD_CALL amf_create_mutex(bool initially_owned, const wchar_t* name)
{
    amf_handle mutex = nullptr;

    if(!name)
    {
        pthread_mutex_t* mutex = new pthread_mutex_t;
        pthread_mutex_t mutex_tmp = PTHREAD_MUTEX_INITIALIZER;
        *mutex = mutex_tmp;
    }
    else
    {
        //name converted to mbs
        auto nameMultibyte = amf_from_unicode_to_multibyte(name);

        bool created = false;

        amf_handle handle(amf_create_shared_memory(nameMultibyte.c_str(), &created));

        if(!handle)
        {
            return nullptr;
        }

        SharedMemoryDescriptor *sharedMemoryDescriptor(
            reinterpret_cast<SharedMemoryDescriptor *>(handle)
            );

        size_t demandedInfoSize = sizeof(amf_handle) + nameMultibyte.length();

        int result = ftruncate(sharedMemoryDescriptor->Handle, demandedInfoSize);

        if(created && (0 != result))
        {
            perror("Failed to resize shared memory for named mutex");

            return nullptr;
        }

        // Map pthread mutex into the shared memory.
        void * sharedAddress = mmap(
            nullptr,
            demandedInfoSize,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            sharedMemoryDescriptor->Handle,
            0
            );

        if(sharedAddress == MAP_FAILED)
        {
            perror("Failed to store mutex in shared memory");

            return nullptr;
        }

        if(created)
        {
            pthread_mutexattr_t attributes = {0};
            if(pthread_mutexattr_init(&attributes))
            {
                perror("Failed to init mutex");

                return nullptr;
            }

            if(pthread_mutexattr_setpshared(&attributes, PTHREAD_PROCESS_SHARED))
            {
                perror("Failed to share mutex");

                return nullptr;
            }

#if !defined(__APPLE__) && !defined(__MACOSX)
            if(pthread_mutexattr_setrobust(&attributes, PTHREAD_MUTEX_ROBUST))
            {
                perror("Failed to set mutex robust");

                return nullptr;
            }
#endif

            mutex = (pthread_mutex_t *)sharedAddress;

            if(pthread_mutex_init((pthread_mutex_t *)mutex, &attributes))
            {
                perror("Failed to init mutex");

                return mutex;
            }
        }
        else
        {
            mutex = (pthread_mutex_t *)sharedAddress;
        }
    }

    if(initially_owned)
    {
        pthread_mutex_lock((pthread_mutex_t *)mutex);
    }

    return (amf_handle)mutex;
}
//----------------------------------------------------------------------------------------
amf_handle AMF_STD_CALL amf_open_mutex(const wchar_t* pName)
{
    assert(false);
    return 0;
}
//----------------------------------------------------------------------------------------
bool AMF_STD_CALL amf_delete_mutex(amf_handle hmutex)
{
    pthread_mutex_t* mutex = (pthread_mutex_t*)hmutex;
    int err = pthread_mutex_destroy(mutex);
    delete mutex;
    return err == 0;
}
//----------------------------------------------------------------------------------------

#if defined(__ANDROID__)
int pthread_mutex_timedlock1(pthread_mutex_t* mutex, const struct timespec* timeout)
{
    struct timeval timenow;
    struct timespec sleepytime;
    int retcode;

    /// This is just to avoid a completely busy wait
    sleepytime.tv_sec = 0;
    sleepytime.tv_nsec = 10000000; // 10ms

    while((retcode = pthread_mutex_trylock (mutex)) == EBUSY)
    {
        gettimeofday (&timenow, NULL);

        if((timenow.tv_sec >= timeout->tv_sec) && ((timenow.tv_usec * 1000) >= timeout->tv_nsec))
        {
            return ETIMEDOUT;
        }
        nanosleep (&sleepytime, NULL);
    }
    return retcode;
}
#endif

//----------------------------------------------------------------------------------------
bool AMF_STD_CALL amf_wait_for_mutex(amf_handle hmutex, unsigned long timeout)
{
    pthread_mutex_t* mutex = (pthread_mutex_t*)hmutex;

    if(timeout == AMF_INFINITE)
    {
        auto lockResult = pthread_mutex_lock(mutex);

        if(EOWNERDEAD == lockResult)
        {
#ifndef __APPLE__
            lockResult = pthread_mutex_consistent(mutex);

            if(0 != lockResult)
            {
                perror("Could not lock mutex");

                return false;
            }
#else
            return false;
#endif
        }

        return true;
    }
    else
    {
        int result = 0;

#ifndef __APPLE__

        // ulTimeout is in milliseconds
        timespec wait_time; //absolute time
        clock_gettime(CLOCK_REALTIME, &wait_time);

        wait_time.tv_sec += timeout / 1000;      /* Seconds */
        wait_time.tv_nsec += (timeout - (timeout / 1000) * 1000) * 1000;     /* Nanoseconds [0 .. 999999999] */

#if defined(__ANDROID__)
        result = pthread_mutex_timedlock1(mutex, &wait_time) == 0;
#else
        result = pthread_mutex_timedlock(mutex, &wait_time) == 0;
#endif

#else
        uint64_t total = 0;

        do
        {
            result = pthread_mutex_trylock(mutex);

            if(result == EBUSY)
            {
                timespec ts = {0};
                ts.tv_sec = 0;
                ts.tv_sec = 10000000;

                /* Sleep for 10,000,000 nanoseconds before trying again. */
                int status = -1;
                while(status == -1)
                {
                    status = nanosleep(&ts, &ts);

                    total += ts.tv_sec;
                }
            }
            else
            {
                break;
            }
        }
        while(result != 0 && (total < timeout));
#endif

	    return result ? true : false;
    }
}
//----------------------------------------------------------------------------------------
bool AMF_STD_CALL amf_release_mutex(amf_handle hmutex)
{
    pthread_mutex_t* mutex = (pthread_mutex_t*)hmutex;
    return pthread_mutex_unlock(mutex) != 0;
}

//----------------------------------------------------------------------------------------
amf_handle AMF_STD_CALL amf_create_semaphore(amf_long iInitCount, amf_long iMaxCount, const wchar_t* /*pName*/)
{
    if(iMaxCount == 0 || iInitCount > iMaxCount)
    {
        return NULL;
    }

    sem_t* semaphore = new sem_t;
    if(sem_init(semaphore, 0, iInitCount) != 0)
    {
        delete semaphore;
        return NULL;
    }
    return (amf_handle)semaphore;
}
//----------------------------------------------------------------------------------------
bool AMF_STD_CALL amf_delete_semaphore(amf_handle hsemaphore)
{
    bool ret = true;
    if(hsemaphore == NULL)
    {
        return true;
    }
    sem_t* semaphore = (sem_t*)hsemaphore;
    ret = (0==sem_destroy(semaphore)) ? 1:0;
    delete semaphore;
    return ret;
}
//----------------------------------------------------------------------------------------
bool AMF_STD_CALL amf_wait_for_semaphore(amf_handle hsemaphore, amf_ulong timeout)
{
#ifndef __APPLE__
    if(hsemaphore == NULL)
    {
        return true;
    }
    // ulTimeout is in milliseconds
    timespec wait_time; //absolute time
    clock_gettime(CLOCK_REALTIME, &wait_time);

    wait_time.tv_sec += timeout / 1000;      /* Seconds */
    wait_time.tv_nsec += (timeout - (timeout / 1000) * 1000) * 1000;     /* Nanoseconds [0 .. 999999999] */

    sem_t* semaphore = (sem_t*)hsemaphore;
    if(timeout != AMF_INFINITE)
    {
        return sem_timedwait (semaphore, &wait_time) == 0; // errno=ETIMEDOUT
    }
    else
    {
        return sem_wait(semaphore) == 0;
    }
#else
    throw "Error: not implemented!";
#endif
}
//----------------------------------------------------------------------------------------
bool AMF_STD_CALL amf_release_semaphore(amf_handle hsemaphore, amf_long iCount, amf_long* iOldCount)
{
    if(hsemaphore == NULL)
    {
        return true;
    }
    sem_t* semaphore = (sem_t*)hsemaphore;

    if(iOldCount != NULL)
    {
        int iTmp = 0;
        sem_getvalue(semaphore, &iTmp);
        *iOldCount = iTmp;
    }

    for(int i = 0; i < iCount; i++)
    {
        sem_post(semaphore);
    }
    return true;
}
//------------------------------------------------------------------------------
/*
 * Delay is specified in milliseconds.
 * Function will return prematurely if msDelay value is invalid.
 *
 * */
void AMF_STD_CALL amf_sleep(amf_ulong msDelay)
{
#if defined(NANOSLEEP_DONTUSE)
    struct timespec sts, sts_remaining;
    int iErrorCode;

    ts.tv_sec = msDelay / 1000;
    ts.tv_nsec = (msDelay - sts.tv_sec * 1000) * 1000000; // nanosec
    // put in code to measure sleep clock jitter
    do
    {
        iErrorCode = nanosleep(&sts, &sts_remaining);
        if(iErrorCode)
        {
            switch(errno)
            {
            case EINTR:
                sts = sts_remaining;
                break;

            case EFAULT:
            case EINVAL:
            case default:
                perror("amf_sleep");
                return;
                /* TODO: how to log errors? */
            }
        }
    } while(iErrorCode);
#else
    usleep(msDelay * 1000);
#endif
}

//----------------------------------------------------------------------------------------
// memory
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void AMF_STD_CALL amf_debug_trace(const wchar_t* text)
{
#if defined(__ANDROID__)
    const char* str = amf_from_unicode_to_multibyte(text).c_str();
    __android_log_write(ANDROID_LOG_DEBUG, "AMF_TRACE", str);
#else
    fprintf(stderr, "%ls", text);
#endif
}

void* AMF_STD_CALL amf_virtual_alloc(size_t size)
{
    void* mem = NULL;
#if defined(__ANDROID__)
    mem = memalign(sysconf(_SC_PAGESIZE), size);
    if(mem == NULL)
    {
        amf_debug_trace(L"Failed to alloc memory using memalign() function.");
    }
#else
    int exitCode = posix_memalign(&mem, sysconf(_SC_PAGESIZE), size);
    if(exitCode != 0)
    {
        amf_debug_trace(L"Failed to alloc memory using posix_memaling() function.");
    }
#endif

    return mem;
}
//-------------------------------------------------------------------------------------------------------
void AMF_STD_CALL amf_virtual_free(void* ptr)
{
    free(ptr); // according to linux help memory allocated by memalign() must be freed by free()
}
//----------------------------------------------------------------------------------------
void* AMF_STD_CALL amf_aligned_alloc(size_t count, size_t alignment)
{
#ifndef __APPLE__
    return memalign(alignment, count);
#else
    //test power of two and more then zero
    if(alignment && (alignment & (alignment - 1)))
    {
        throw std::runtime_error("Error: alignment is not a power of 2!");
    }

    void *address(nullptr);

    auto result = posix_memalign(&address, alignment, count);

    if(!result)
    {
        return address;
    }

    return nullptr;
#endif
}
//----------------------------------------------------------------------------------------
void AMF_STD_CALL amf_aligned_free(void* ptr)
{
    return free(ptr);
}
//----------------------------------------------------------------------------------------
// clock and time
//----------------------------------------------------------------------------------------
double AMF_STD_CALL amf_clock()
{
    //MM: clock() Win32 - returns time from beginning of the program
    //MM: clock() works different in Linux - returns consumed processor time
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    double cur_time = ((double)ts.tv_sec) + ((double)ts.tv_nsec) / 1000000000.; //to sec
    return cur_time;
}
//----------------------------------------------------------------------------------------
amf_int64 AMF_STD_CALL get_time_in_seconds_with_fraction()
{
   struct timeval tv;

   gettimeofday(&tv, NULL);

   amf_int64 ntp_time = ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
    return ntp_time;
}
//----------------------------------------------------------------------------------------
amf_pts AMF_STD_CALL amf_high_precision_clock()
{
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 10000000LL + ts.tv_nsec / 100.; //to nanosec
}
//-------------------------------------------------------------------------------------------------
amf_handle AMF_STD_CALL amf_load_library(const wchar_t* filename)
{
    void *ret = dlopen(amf_from_unicode_to_multibyte(filename).c_str(), RTLD_NOW | RTLD_GLOBAL);

    if(!ret)
    {
        const char *err = dlerror();
        fprintf(stderr, "\nError: %s\n", err);
    }

    return ret;
}

void* AMF_STD_CALL amf_get_proc_address(amf_handle module, const char* procName)
{
    return dlsym(module, procName);
}
//-------------------------------------------------------------------------------------------------
int AMF_STD_CALL amf_free_library(amf_handle module)
{
    return dlclose(module) == 0;
}
void AMF_STD_CALL amf_increase_timer_precision()
{
}
void AMF_STD_CALL amf_restore_timer_precision()
{
}
//--------------------------------------------------------------------------------
// the end
//--------------------------------------------------------------------------------

#endif
