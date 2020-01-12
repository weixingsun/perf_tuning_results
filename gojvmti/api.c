#include <jvmti.h>
#include <stdio.h>
#include "_cgo_export.h"

static jrawMonitorID raw_lock;
static jvmtiEnv* jvmti = NULL;

//////////////////////////////////////////////////////////////////
void GarbageCollectionStart(jvmtiEnv *jvmti) {
	//(*jvmti)->RawMonitorEnter(jvmti, raw_lock);
    //gLog(jvmti,"GCstart");
	//(*jvmti)->RawMonitorExit(jvmti, raw_lock);
}

void GarbageCollectionFinish(jvmtiEnv *jvmti) {
    gLog(jvmti,"GCfinish: print GC & clear cache");
}
void SampledObjectAlloc(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jobject object, jclass object_klass, jlong size) {
    //gLog(jvmti,"SampledObjectAlloc");
}
//////////////////////////////////////////////////////////////////
void cRegister(jvmtiEnv *jvmti, char *options){
	
    jvmtiCapabilities caps = {0};
    caps.can_generate_sampled_object_alloc_events = 1;
    caps.can_generate_all_class_hook_events = 1;
    caps.can_generate_compiled_method_load_events = 1;
    caps.can_generate_garbage_collection_events = 1;
    (*jvmti)->AddCapabilities(jvmti, &caps);
	
    if (options != NULL && options[0] >= '0' && options[0] <= '9') {
		//sampling_interval in bytes
		int i = goAtoi(options); //goAtoi/atoi
		fprintf(stdout, "heap_agent_option: %d\n",i);
        (*jvmti)->SetHeapSamplingInterval(jvmti, i);
    }
	
    jvmtiEventCallbacks calls = {0};
	//calls.DataDumpRequest = DataDumpRequest;
    calls.SampledObjectAlloc = SampledObjectAlloc;
    calls.GarbageCollectionStart = GarbageCollectionStart;
    calls.GarbageCollectionFinish = GarbageCollectionFinish;
    (*jvmti)->SetEventCallbacks(jvmti, &calls, sizeof(calls));
    
	(*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_VM_DEATH, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_DATA_DUMP_REQUEST, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_SAMPLED_OBJECT_ALLOC, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_START, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_FINISH, NULL);
}
JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *jvm, char *options, void *reserved) {
	(*jvm)->GetEnv(jvm, (void**) &jvmti, JVMTI_VERSION_1_0);
    (*jvmti)->CreateRawMonitor(jvmti, "raw_lock", &raw_lock);
    gLoad(jvmti, options);
	cRegister(jvmti, options);
    return JNI_OK;
}

const jint cagent_DestroyJvm(JavaVM *jvm) {
    return (*jvm)->DestroyJavaVM(jvm);
}

/*
// Use jvmti
void GarbageCollectionStart(jvmtiEnv *jvmti) {
	//(*jvmti)->RawMonitorEnter(jvmti, raw_lock);
    //gLog(jvmti,"GCstart");
	//(*jvmti)->RawMonitorExit(jvmti, raw_lock);
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