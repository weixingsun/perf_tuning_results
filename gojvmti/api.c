#include <jvmti.h>
#include <stdio.h>
#include <string.h>
#include "_cgo_export.h"

static jrawMonitorID jvmti_lock;
static jvmtiEnv* jvmti = NULL;

//////////////////////////////////////////////////////////////////

void GarbageCollectionStart(jvmtiEnv *jvmti) {
	//(*jvmti)->RawMonitorEnter(jvmti, jvmti_lock);
    //gLog("GCstart");
	//(*jvmti)->RawMonitorExit(jvmti, jvmti_lock);
}

void GarbageCollectionFinish(jvmtiEnv *jvmti) {
    //gLog("GCfinish: print GC & clear cache");
}

char* find_class_name(char* sig) {
    switch (sig[0]) {
        case 'B': return "byte";
        case 'C': return "char";
        case 'S': return "short";
        case 'I': return "int";
        case 'J': return "long";
        case 'F': return "float";
        case 'D': return "double";
        case 'Z': return "boolean";
        case '[': return find_class_name(sig+1);
		
    }
    sig++;	// rm 'L'
    sig[strlen(sig) - 1] = 0;	// rm ';'
    for (char* c = sig; *c; c++) if (*c == '/') *c = '.';	// '/' -> '.'
	
    return sig;
}

void SampledObjectAlloc(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jobject object, jclass class, jlong size) {
	char* class_sig;
    (*jvmti)->GetClassSignature(jvmti, class, &class_sig, NULL);
	//jvmtiFrameInfo frames[64];
	//jint count;
	//(*jvmti)->GetStackTrace(jvmti, thread, 0, 64, frames, &count);
	char* class_name = find_class_name(class_sig);
	fprintf(stdout, "Sampling objects: size[%ld] class:%s\n", size, class_name);
	
	//(*jvmti)->RawMonitorEnter(jvmti, jvmti_lock);
    //(*jvmti)->RawMonitorExit(jvmti, jvmti_lock);
    (*jvmti)->Deallocate(jvmti, (unsigned char*) class_sig);
}
//////////////////////////////////////////////////////////////////
void cRegister(jvmtiEnv *jvmti, char *options){
	
	gOptions(jvmti, options);
	
    jvmtiCapabilities caps = {0};
    caps.can_generate_sampled_object_alloc_events = 1;
    caps.can_generate_all_class_hook_events = 1;
    caps.can_generate_compiled_method_load_events = 1;
    caps.can_generate_garbage_collection_events = 1;
    (*jvmti)->AddCapabilities(jvmti, &caps);
	
    jvmtiEventCallbacks calls = {0};
	//calls.DataDumpRequest = DataDumpRequest;
    calls.SampledObjectAlloc = SampledObjectAlloc;
    calls.GarbageCollectionStart = GarbageCollectionStart;
    calls.GarbageCollectionFinish = GarbageCollectionFinish;
    (*jvmti)->SetEventCallbacks(jvmti, &calls, sizeof(calls));
    
    //(*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_DATA_DUMP_REQUEST, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_SAMPLED_OBJECT_ALLOC, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_START, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_FINISH, NULL);
}
JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *jvm, char *options, void *reserved) {
	(*jvm)->GetEnv(jvm, (void**) &jvmti, JVMTI_VERSION_1_0);
    (*jvmti)->CreateRawMonitor(jvmti, "jvmti_lock", &jvmti_lock);
    cRegister(jvmti, options);
    return JNI_OK;
}
void cSetHeapSamplingInterval(jvmtiEnv *jvmti, int interval) {
	(*jvmti)->SetHeapSamplingInterval(jvmti, interval);
}
//const jint cagent_DestroyJvm(JavaVM *jvm) {
//    return (*jvm)->DestroyJavaVM(jvm);
//}

/*
// Use jvmti
void GarbageCollectionStart(jvmtiEnv *jvmti) {
	//(*jvmti)->RawMonitorEnter(jvmti, jvmti_lock);
    //gLog(jvmti,"GCstart");
	//(*jvmti)->RawMonitorExit(jvmti, jvmti_lock);
}
// Use mutex
void NotifyGCWaitingThreadInternal() {
  std::unique_lock<std::mutex> lock(gc_waiting_mutex_);
  gc_notified_ = true;
  gc_waiting_cv_.notify_all();
}
void WaitForGC() {
  std::unique_lock<std::mutex> lock(gc_waiting_mutex_);
  gc_notified_ = false;
  // If we are woken up without having been notified, just go back to sleep.
  gc_waiting_cv_.wait(lock, [this] { return gc_notified_; } );
}
*/