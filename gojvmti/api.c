#include <jvmti.h>
#include <stdio.h>
#include "_cgo_export.h"

static jrawMonitorID raw_lock;
static jvmtiEnv* jvmti = NULL;
//////////////////////////////////////////////////////////////////
int isDigit(char c) {
    return (c >= '0') && (c <= '9');
}

int atoi(char *str) {
    int result = 0;
    int neg_multiplier = 1;

    // Scrub leading whitespace
    while (*str && (
            (*str == ' ') ||
            (*str == '\t'))) 
        str++;

    // Check for negative
    if (*str && *str == '-') {
        neg_multiplier = -1;
        str++;
    }

    // Do number
    for (; *str && isDigit(*str); str++) {
        result = (result * 10) + (*str - '0');
    }
	int i = result * neg_multiplier;
    return i;
}
//////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL GarbageCollectionStart(jvmtiEnv *jvmti) {
	//(*jvmti)->RawMonitorEnter(jvmti, raw_lock);
    gCount(jvmti,"start");
	//(*jvmti)->RawMonitorExit(jvmti, raw_lock);
}

JNIEXPORT void JNICALL GarbageCollectionFinish(jvmtiEnv *jvmti) {
    //gCount(jvmti,"finish");
}
JNIEXPORT void JNICALL SampledObjectAlloc(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jobject object, jclass object_klass, jlong size) {
    //gCount(jvmti,"finish");
}
//////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL cRegister(jvmtiEnv *jvmti, char *options){
	
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
