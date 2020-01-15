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
    gCacheMapCount();
	gCacheMapClean();
}
char* decode_class_name(char* sig) {
    switch (sig[0]) {
        case 'B': return "byte";
        case 'C': return "char";
        case 'S': return "short";
        case 'I': return "int";
        case 'J': return "long";
        case 'F': return "float";
        case 'D': return "double";
        case 'Z': return "boolean";
        case '[': return decode_class_name(sig+1);
    }
    sig++;	// rm 'L'
    sig[strlen(sig) - 1] = 0;	// rm ';'
	int i=0, idx = 0;
	for (char* c = sig; *c; c++){
		i++; if(*c=='/')idx=i;
	}
	sig+=idx;
    return sig;
}
char* getFuncName(jvmtiEnv* jvmti, jthread thread){
	int depth=1;
	jvmtiFrameInfo frames[depth];
	jint count;
	char* class_sig;
	char* method_name;
	char* full_method_name;
	if (  (*jvmti)->GetStackTrace(jvmti, thread, 0, depth, &frames, &count) == JVMTI_ERROR_NONE  &&
		  (*jvmti)->GetMethodName(jvmti, frames[0].method, &method_name, &class_sig, NULL) == JVMTI_ERROR_NONE ) {
	    char* class_name = decode_class_name(class_sig);
		full_method_name = gStringAdd(class_name,method_name,".");
	}else{
		full_method_name="";
	}
    (*jvmti)->Deallocate(jvmti, (unsigned char*) class_sig);
    (*jvmti)->Deallocate(jvmti, (unsigned char*) method_name);
	return full_method_name;
}
void SampledObjectAlloc(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jobject object, jclass class, jlong size) {
	char* sig;
	(*jvmti)->GetClassSignature(jvmti, class, &sig, NULL);
	char* class_name = decode_class_name(sig);
	//////////////////////////////////////////////////////////////
	int depth=1;
	jvmtiFrameInfo frames[depth];
	jint count;
	char* method_class;
	char* method_name;
	char* full_method_name="";
	if (  (*jvmti)->GetStackTrace(jvmti, thread, 0, depth, &frames, &count) == JVMTI_ERROR_NONE  &&
		  (*jvmti)->GetMethodName(jvmti, frames[0].method, &method_name, &method_class, NULL) == JVMTI_ERROR_NONE ) {
	    char* method_class_name = decode_class_name(method_class);
		full_method_name = gStringAdd(method_class_name,method_name,".");
	}else{
		fprintf(stdout, "Error getting method:%s \n", method_name );
	}
	//fprintf(stdout, "Sampling objects: size[%ld] class:%s \n", size, class_name );
	gCacheObject(method_name,class_name,size);
    (*jvmti)->Deallocate(jvmti, (unsigned char*) method_class);
    (*jvmti)->Deallocate(jvmti, (unsigned char*) method_name);
    (*jvmti)->Deallocate(jvmti, (unsigned char*) sig);
	
}
//////////////////////////////////////////////////////////////////
void cRegistry(jvmtiEnv *jvmti, char *options){
	
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
    //calls.GarbageCollectionStart = GarbageCollectionStart;
    calls.GarbageCollectionFinish = GarbageCollectionFinish;
    (*jvmti)->SetEventCallbacks(jvmti, &calls, sizeof(calls));
    
    //(*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_DATA_DUMP_REQUEST, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_SAMPLED_OBJECT_ALLOC, NULL);
    //(*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_START, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_FINISH, NULL);
}
JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *jvm, char *options, void *reserved) {
	(*jvm)->GetEnv(jvm, (void**) &jvmti, JVMTI_VERSION_1_0);
    //(*jvmti)->CreateRawMonitor(jvmti, "jvmti_lock", &jvmti_lock);
    cRegistry(jvmti, options);
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