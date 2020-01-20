#include <jvmti.h>
#include <stdio.h>
#include <string.h>
#include "hashmap.h"
#include "_cgo_export.h"

static jrawMonitorID jvmti_lock;
static jvmtiEnv* jvmti = NULL;

//////////////////////////////////////////////////////////////////
void SampleThreadState(jvmtiEnv *jvmti){
	//err = (*jvmti)->GetThreadState(jvmti, thread, &state);
	//int s = state & JVMTI_JAVA_LANG_THREAD_STATE_MASK;
	//JVMTI_JAVA_LANG_THREAD_STATE_NEW
	//JVMTI_JAVA_LANG_THREAD_STATE_TERMINATED
	//JVMTI_JAVA_LANG_THREAD_STATE_RUNNABLE
	//JVMTI_JAVA_LANG_THREAD_STATE_BLOCKED
	//JVMTI_JAVA_LANG_THREAD_STATE_WAITING
	//JVMTI_JAVA_LANG_THREAD_STATE_TIMED_WAITING
	jvmtiStackInfo *stack_info;
	jint thread_count, ti;
	(*jvmti)->GetAllStackTraces(jvmti, 1, &stack_info, &thread_count);
	for (ti = 0; ti < thread_count; ++ti) {
	   jvmtiStackInfo *infop = &stack_info[ti];
	   jint state = infop->state;
	   //GetThreadCpuTime
	}
	/* this one Deallocate call frees all data allocated by GetAllStackTraces */
	//(*jvmti)->Deallocate(jvmti, stack_info);
}
void JNICALL MethodEntry(jvmtiEnv *jvmti, JNIEnv *jni, jthread thread, jmethodID method) {
    char *name_ptr;
    char *signature_ptr;
    char *generic_ptr;
    jvmtiError error;

    error = (*jvmti)->GetMethodName(jvmti, method, &name_ptr, &signature_ptr, &generic_ptr);
    printf("Entered method %s\n", name_ptr);
}
void GarbageCollectionStart(jvmtiEnv *jvmti) {
	//(*jvmti)->RawMonitorEnter(jvmti, jvmti_lock);
    //gLog("GCstart");
	//(*jvmti)->RawMonitorExit(jvmti, jvmti_lock);
}

void GarbageCollectionFinish(jvmtiEnv *jvmti) {
    gCacheMapCount();
	gCacheMapClean();
}
char* decode_class_name2(char* sig) {
	sig++;  //skip '['
    if (sig[1] == 0) {
		return gTranslateJVMType(sig[0]);
	}else{
		return gLastName(sig);
	}
}
char* decode_class_sign(char* sig) {
    sig++;	// rm 'L' or '['
    switch (sig[0]) {
        case 'B': return "byte";
        case 'C': return "char";
        case 'D': return "double";
        case 'F': return "float";
        case 'I': return "int";
        case 'J': return "long";
        case 'S': return "short";
        case 'Z': return "boolean";
    }
    sig[strlen(sig) - 1] = 0;	// rm ';'
	int i=0, idx = 0;
	for (char* c = sig; *c; c++){
		i++; if(*c=='/')idx=i;
	}
	sig+=idx;
    return sig;
}
void print_all_threads(jvmtiEnv* jvmti) {
	jint thread_count;
	jthread* threads;
	(*jvmti)->GetAllThreads(jvmti, &thread_count, &threads);
	/*
	for (int i = 0; i < thread_count; i++) {
		JavaThreadData* ptr = GetThread(jvmti, threads[i]);
		if (ptr != NULL) {
			ptr->CloseLog();
			filesystem::path threadpath = p / str(format("%d.trace") % ptr->GetID());
			ptr->SetLog(threadpath.string().c_str());
		}
	}
	Deallocate(jvmti, (void *)threads);
	*/
}
char* get_method_name(jvmtiEnv* jvmti, jmethodID mid) {
    jclass method_class;
    char* class_sig = NULL;
    char* method_name = NULL;
	char* full_method_name = NULL;

    if ((*jvmti)->GetMethodDeclaringClass(jvmti, mid, &method_class) == 0 &&
        (*jvmti)->GetClassSignature(jvmti, method_class, &class_sig, NULL) == 0 &&
        (*jvmti)->GetMethodName(jvmti, mid, &method_name, NULL, NULL) == 0) {
		full_method_name = gStringAdd(decode_class_sign(class_sig),method_name,".");
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
	char* class_name = decode_class_sign(class_sig);
	//fprintf(stdout, "Class Sign: %s \n", class_sig );
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
	//caps.can_generate_method_entry_events = 1;
    //caps.can_generate_all_class_hook_events = 1;
    //caps.can_generate_compiled_method_load_events = 1;
	//caps.can_access_local_variables = 1;
    caps.can_generate_garbage_collection_events = 1;
    (*jvmti)->AddCapabilities(jvmti, &caps);
	
    jvmtiEventCallbacks calls = {0};
	//calls.MethodEntry = &MethodEntry
	//calls.DataDumpRequest = DataDumpRequest;
    //calls.GarbageCollectionStart = GarbageCollectionStart;
    calls.SampledObjectAlloc = SampledObjectAlloc;
    calls.GarbageCollectionFinish = GarbageCollectionFinish;
    (*jvmti)->SetEventCallbacks(jvmti, &calls, sizeof(calls));
    
    //(*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_DATA_DUMP_REQUEST, NULL);
    //(*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_START, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_SAMPLED_OBJECT_ALLOC, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_FINISH, NULL);
}
JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *jvm, char *options, void *reserved) {
	(*jvm)->GetEnv(jvm, (void**) &jvmti, JVMTI_VERSION_1_0);
    //(*jvmti)->CreateRawMonitor(jvmti, "jvmti_lock", &jvmti_lock);
    cRegistry(jvmti, options);
	gJVMTypeInit();
    return JNI_OK;
}
void cSetHeapSamplingInterval(jvmtiEnv *jvmti, int interval) {
	fprintf(stdout, "HeapSamplingInterval=%d \n", interval );
	(*jvmti)->SetHeapSamplingInterval(jvmti, interval);
}