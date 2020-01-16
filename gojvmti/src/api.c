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
char* get_method_name(jvmtiEnv* jvmti, jmethodID mid) {
    jclass method_class;
    char* class_sig = NULL;
    char* method_name = NULL;
	char* full_method_name = NULL;

    if ((*jvmti)->GetMethodDeclaringClass(jvmti, mid, &method_class) == 0 &&
        (*jvmti)->GetClassSignature(jvmti, method_class, &class_sig, NULL) == 0 &&
        (*jvmti)->GetMethodName(jvmti, mid, &method_name, NULL, NULL) == 0) {
		
		full_method_name = gStringAdd(decode_class_name(class_sig),method_name,".");
		//fprintf(stdout, "full_method_name:%s \n", full_method_name );
    } else {
        full_method_name = "*";
    }
    (*jvmti)->Deallocate(jvmti,(unsigned char*) method_name);
    (*jvmti)->Deallocate(jvmti,(unsigned char*) class_sig);
    return full_method_name;
}
void SampledObjectAlloc(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jobject object, jclass class, jlong size) {
	char* class_sig;
	(*jvmti)->GetClassSignature(jvmti, class, &class_sig, NULL);
	char* class_name = decode_class_name(class_sig);
	///////////////////////////////////////////////////////////
	jint count;
	int depth=1;
	jvmtiFrameInfo frames[depth];
	(*jvmti)->GetStackTrace(jvmti, thread, 0, depth, &frames, &count);
	char* method_name = get_method_name(jvmti, frames[0].method);
	//fprintf(stdout, "Sampling objects: size[%ld] class:%s in method: %s\n", size, class_name, method_name );
	gCacheObject(method_name,class_name,size);
    (*jvmti)->Deallocate(jvmti, (unsigned char*) class_sig);
	
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