#include <jvmti.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "hashmap.h"
#include "_cgo_export.h"

static jrawMonitorID jvmti_lock;
static jvmtiEnv* jvmti = NULL;
static map_t CachedObjects = NULL;
char* LOG_FILE = NULL;
unsigned long END_TIME = ULONG_MAX;

//////////////////////////////////////////////////////////////////
void map_set(map_t mymap, char* key, int v){
	data_struct_t* value = malloc(sizeof(data_struct_t));
	value->key_string = key;
    value->number = v;
	int error = hashmap_put(mymap, value->key_string, value);
    if(error!=MAP_OK) printf("set error: %d on key: %s value: %d\n",error,key,v);
}
int map_get(map_t mymap, char* key){
	data_struct_t* value = malloc(sizeof(data_struct_t));
	int error = hashmap_get(mymap, key, (void**)(&value));
	//printf("get key: %s  value: %d\n",key,value->number);
	if (error==MAP_MISSING){
		return -1;
	}else{
		return value->number;
	}
}
void map_inc(map_t mymap, char* key){
	int error = hashmap_inc(mymap, key);
	if(error!=MAP_OK) printf("inc error: %d on key: %s\n",error,key);
	//else printf("inc key: %s\n",key);
}
void map_rm(map_t mymap, char* key){
	int error = hashmap_remove(mymap, key);
	if(error!=MAP_OK) printf("rm error: %d on key: %s\n",error,key);
}
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

//Actually this method won't do what it supposed to, should be a bug, may be race condition related
void cUnRegisterAll(){
	(*jvmti)->RawMonitorEnter(jvmti, jvmti_lock);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_DISABLE, JVMTI_EVENT_SAMPLED_OBJECT_ALLOC, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_DISABLE, JVMTI_EVENT_GARBAGE_COLLECTION_FINISH, NULL);
	jvmtiEventCallbacks calls = {0};
    (*jvmti)->SetEventCallbacks(jvmti, &calls, sizeof(calls));
	jvmtiCapabilities caps = {0};
    (*jvmti)->AddCapabilities(jvmti, &caps);
	//fprintf(stdout, "| Agent cUnRegisterAll END_TIME = %d seconds \n", END_TIME );
	(*jvmti)->RawMonitorExit(jvmti, jvmti_lock);
}
void GarbageCollectionStart(jvmtiEnv *jvmti) {
	//(*jvmti)->RawMonitorEnter(jvmti, jvmti_lock);
    //gLog("GCstart");
	//(*jvmti)->RawMonitorExit(jvmti, jvmti_lock);
}

void GarbageCollectionFinish(jvmtiEnv *jvmti) {
    /////////////////////////////////////////////////////////////
	//gCacheMapCount();
	//gCacheMapClean();
    //printf("Sampled Alloc Objects---------------------------------------------\n");
	hashmap_print(CachedObjects,LOG_FILE);
	hashmap_empty(CachedObjects);
	//hashmap_free(CachedObjects);
	//if (END_TIME < time(NULL) ) cUnRegisterAll();
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
void print_all_threads() {
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
char* get_method_name(jmethodID mid) {
    jclass method_class;
    char* class_sig = NULL;
    char* method_name = NULL;
	char* full_method_name = NULL;

    if ((*jvmti)->GetMethodDeclaringClass(jvmti, mid, &method_class) == 0 &&
        (*jvmti)->GetClassSignature(jvmti, method_class, &class_sig, NULL) == 0 &&
        (*jvmti)->GetMethodName(jvmti, mid, &method_name, NULL, NULL) == 0) {
		char* cls_name = decode_class_sign(class_sig);
		//full_method_name = gStringAdd(cls_name,method_name,".");//cgo performance 4x slower than c
		int l = strlen(cls_name)+strlen(cls_name)+2;
		full_method_name = (char*) malloc(l);
		snprintf(full_method_name, l, "%s.%s", cls_name,method_name);
    } else {
        full_method_name = "*";
    }
    (*jvmti)->Deallocate(jvmti,(unsigned char*) method_name);
    (*jvmti)->Deallocate(jvmti,(unsigned char*) class_sig);
    return full_method_name;
}
void cCacheObject(char* method_name,char* class_name,int size){
	int l = strlen(method_name)+strlen(class_name)+10;
	char* k = (char*) malloc(l);
	snprintf(k, l, "%s()%s[%d]", method_name, class_name, size);
	//printf("M:%s C:%s size:%d , K=%s \n", method_name,class_name,size,k);
	map_inc(CachedObjects, k);
	//int i = get(mymap, k);
    //printf("hashmap_get not found: key=%s  value:%d\n", k,i);
}
//~2% overhead
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
	char* method_name = get_method_name(frames[0].method);
	//fprintf(stdout, "Sampling objects: size[%ld] class:%s in method: %s\n", size, class_name, method_name );
	//gCacheObject(method_name,class_name,size);
	cCacheObject(method_name,class_name,size);
    (*jvmti)->Deallocate(jvmti, (unsigned char*) class_sig);
}
//////////////////////////////////////////////////////////////////
void cRegisterSampleAlloc(char *options){
	CachedObjects = hashmap_new();
	gOptions(options);
	
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
void cSetHeapSamplingInterval(int interval) {
	fprintf(stdout, "| Agent HeapSamplingInterval=%d \n", interval );
	(*jvmti)->SetHeapSamplingInterval(jvmti, interval);
}
void cSetLogFile(char* file_path){
	fprintf(stdout, "| Agent Log writes to %s \n", file_path );
	fprintf(stdout, "-----------------------------------------------------------\n");
	init_log(file_path);
	LOG_FILE = file_path;
}
void cSetDuration(int duration){
	END_TIME  = time(NULL)+duration;
	fprintf(stdout, "| Agent Set END_TIME = %d seconds \n", END_TIME );
}
JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *jvm, char *options, void *reserved) {
	(*jvm)->GetEnv(jvm, (void**) &jvmti, JVMTI_VERSION_1_0);
    //(*jvmti)->CreateRawMonitor(jvmti, "jvmti_lock", &jvmti_lock);
    cRegisterSampleAlloc(options);
	gJVMTypeInit();
    return JNI_OK;
}
JNIEXPORT void JNICALL Agent_OnUnload(JavaVM *vm){
	fprintf(stdout, "| Agent Unload \n" );
	cUnRegisterAll();
	close_log();
}
JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM *vm, char *options, void *reserved){
    return Agent_OnLoad(vm,options,reserved);
}