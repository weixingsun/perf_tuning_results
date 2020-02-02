#include <jvmti.h>
#include <jni.h>
//#include <JClass.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "hashmap.h"
#include "stack.h"
#include "_cgo_export.h"

static jrawMonitorID jvmti_lock;
static jvmtiEnv* jvmti = NULL;
static map_t CachedObjects = NULL;
unsigned long END_TIME = ULONG_MAX;
unsigned long COUNT_TIME = ULONG_MAX;
int COUNT_INTERVAL = 1;
char* LOG_FILE = NULL;
char* METHOD = NULL;
//char* CLASS = NULL;
static int flag_method_compiled = 0;
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
void jvmtiFree(char* ptr){
	(*jvmti)->Deallocate(jvmti, (unsigned char*) ptr);
	//free(copy);
}
void jvmtiFreeStack(struct Stack* stack){
	Stack_Iter(stack,jvmtiFree);
}
/*
char* deepCopyString(char* old_str){
	char* new_str = malloc(strlen(old_str) + 1);
	strcpy(new_str, old_str);
	return new_str;
}*/
char* AddString2(char* str1, char* str2){
	int ln = strlen(str1)+strlen(str2)+2;
	char* new_str = malloc(ln);
	snprintf(new_str, ln, "%s.%s", str1,str2);
	//free(str1);free(str2);
	//jvmtiFree(str1);jvmtiFree(str2);
	return new_str;
}
char* AddString2Int(char* str1, char* str2, int num){
	int length = snprintf( NULL, 0, "%d", num );
	int ln = strlen(str1)+strlen(str2)+length+4;
	char* new_str = malloc(ln);
	snprintf(new_str, ln, "%s.%s[%d]", str1,str2,num);
	//free(str1);free(str2);
	//jvmtiFree(str1);jvmtiFree(str2);jvmtiFree(str3);
	return new_str;
}
char* getClassName(jclass class,struct Stack* stack){
	char* class_sig;
	(*jvmti)->GetClassSignature(jvmti, class, &class_sig, NULL);
	Stack_Push(stack, class_sig);
	return decode_class_sign(class_sig);
}
char* getMethodName(jmethodID method,struct Stack* stack){
	char *name_ptr;
    //char *signature_ptr;
    //char *generic_ptr;
    (*jvmti)->GetMethodName(jvmti, method, &name_ptr, NULL,NULL); //&signature_ptr, &generic_ptr);
	Stack_Push(stack, name_ptr);
	return name_ptr;
}
char* getMethodClassName(jmethodID method,struct Stack* stack){
	jclass method_class;
	char* class_name = NULL;
	char* class_sig;
    if ((*jvmti)->GetMethodDeclaringClass(jvmti, method, &method_class) == 0 &&
        (*jvmti)->GetClassSignature(jvmti, method_class, &class_sig, NULL) == 0 ){
		//full_name = gStringAdd(cls_name,method_name,".");//cgo performance 4x slower than c
		class_name = decode_class_sign(class_sig);
    }
	Stack_Push(stack, class_sig);
	return class_name;
}
char* getCallerMethodName(jthread thread,struct Stack* stack){
	jint count;
	int depth=1;
	jvmtiFrameInfo frames[depth];
	(*jvmti)->GetStackTrace(jvmti, thread, 0, depth, frames, &count);
	//Stack_Push(stack, class_sig);
	jmethodID method = frames[0].method;
	return AddString2(getMethodClassName(method,stack),getMethodName(method,stack));
}
char* getThreadName(jthread thread,struct Stack* stack){
	jvmtiThreadInfo info;
	(*jvmti)->GetThreadInfo(jvmti, thread, &info);
	Stack_Push(stack, info.name);
	return info.name;
}
/*
//too much overhead
void JNICALL MethodEntry(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jmethodID method) {
	struct Stack* stack = Stack_Init();
	char* thread_name = getThreadName(thread,stack);
	char* method_name = getMethodName(method,stack);
	char* method_class_name = getMethodClassName(method,stack);
	char* full_method_name = AddString2(method_class_name, method_name);
	
	if (METHOD==NULL || strcmp(METHOD,method_name)==0){
		map_inc(CachedObjects, full_method_name);
	}
	jvmtiFreeStack(stack);
}
void JNICALL MethodExit(jvmtiEnv *jvmti, JNIEnv* jni, jthread thread, jmethodID method, jboolean was_popped_by_exception, jvalue return_value){
	if (COUNT_TIME < time(NULL) ) {
		hashmap_print(CachedObjects,LOG_FILE);
		hashmap_empty(CachedObjects);
		COUNT_TIME+=COUNT_INTERVAL;
	}
}
*/
//Actually this method won't do what it supposed to, should be a bug, may be race condition related
void cUnRegisterAll(){
	(*jvmti)->RawMonitorEnter(jvmti, jvmti_lock);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_DISABLE, JVMTI_EVENT_SAMPLED_OBJECT_ALLOC, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_DISABLE, JVMTI_EVENT_GARBAGE_COLLECTION_FINISH, NULL);
	jvmtiEventCallbacks calls = {0};
    (*jvmti)->SetEventCallbacks(jvmti, &calls, sizeof(calls));
	jvmtiCapabilities caps = {0};
    (*jvmti)->AddCapabilities(jvmti, &caps);
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

//void JNICALL DynamicCodeGenerated(jvmtiEnv *jvmti_env, const char* name, const void* address, jint length)
unsigned char* getMethodBytes(jmethodID method) {
	//jmethodID methodID = (*jvmti)->FromReflectedMethod(env, method);  //jobject method
    jint bytecode_size;
    unsigned char* bytecodes;
    (*jvmti)->GetBytecodes(jvmti, method, &bytecode_size, &bytecodes);
    //jbyteArray result = (*jvmti)->NewByteArray(jvmti, bytecode_size);
    //(*jvmti)->SetByteArrayRegion(jvmti, result, 0, bytecode_size, bytecodes);
    //(*jvmti)->Deallocate(jvmti, bytecodes);
	//Stack_Push(stack, bytecodes);
    return bytecodes;
}
void printHex(unsigned char* code,jint size){
	for(int i=0;i<size;i++){
		printf("%x",code[i]);
	}
	printf("]\n");
}
void printMethodBytecode(jmethodID method,jint code_size){
	unsigned char* code = getMethodBytes(method);
	printf("[%d] [",code_size);
	printHex(code,code_size);
}
void JNICALL ClassFileLoadHook(jvmtiEnv *jvmti, JNIEnv* jni, 
		jclass class_being_redefined, jobject loader, 
		const char* name, jobject protection_domain, 
		jint class_data_len, const unsigned char* class_data,
		jint* new_class_data_len, unsigned char** new_class_data){
	(*jvmti)->RawMonitorEnter(jvmti, jvmti_lock);
	int index = strcspn(METHOD,".");
	char CLASS[index+1];
	strncpy(CLASS, METHOD, index);
	//name = rtrim(name);
	if(strstr(name,CLASS) != NULL || strstr(CLASS,name) != NULL) {
		//fprintf(stdout, "[%d]name=%s [%d]class=%s  method=%s index=%d    \n", strlen(name),name, strlen(CLASS),CLASS, METHOD, index);
		fprintf(stdout, "-----------------------Class loaded [%d]%s \n", class_data_len, name );
		FILE *file = fopen(CLASS, "wb");
		if(file){
			fwrite(class_data,1,class_data_len, file);
			fflush(file);
			fclose(file);
		}
	}
	/*
	//add field to class_being_redefined
	static JNINativeMethod methods[1] = {
        {"_someMethod", "()V", (void*)&native_method},
    };
	jni->RegisterNatives(class_being_redefined, methods, 1);
	jfieldID eng = jni->GetStaticFieldID(class_being_redefined, "engaged", "I");
	jni->SetStaticIntField(class_being_redefined, eng, 1);
	
	//edit whole codes
	*new_class_data_len = str.length();
	jvmti->Allocate(str.length(), new_class_data);
	memcpy(*new_class_data, str.c_str(), str.length());
	*/
	(*jvmti)->RawMonitorExit(jvmti, jvmti_lock);
}
//~2% overhead
void SampledObjectAlloc(jvmtiEnv* jvmti, JNIEnv* env, jthread thread, jobject object, jclass class, jlong size) {
	struct Stack* stack = Stack_Init();
	char* class_name = getClassName(class,stack);
	char* method_name = getCallerMethodName(thread,stack);
	//fprintf(stdout, "Class Sign: %s \n", class_sig );
	char* full_method_name = AddString2Int(method_name,class_name,size);
	map_inc(CachedObjects, full_method_name);
	free(method_name);//free(class_name);
	jvmtiFreeStack(stack);
}
//////////////////////////////////////////////////////////////////

void JNICALL CompiledMethodLoad(jvmtiEnv *jvmti_env, jmethodID method, jint code_size, 
		const void* code_addr, jint map_length, const jvmtiAddrLocationMap* map, const void* compile_info){
	struct Stack* stack = Stack_Init();
	char* method_name = AddString2(getMethodClassName(method,stack),getMethodName(method,stack));
	if(strcmp(METHOD,method_name)==0 && flag_method_compiled<1) {
		flag_method_compiled++;
		printf("---------Method %s()",method_name);
		printMethodBytecode(method,code_size);
		jclass klass;
		(*jvmti)->GetMethodDeclaringClass(jvmti, method, &klass);
		(*jvmti)->RetransformClasses(jvmti, 1, &klass);
	}
	jvmtiFreeStack(stack);
	
}
void JNICALL CompiledMethodUnload(jvmtiEnv *jvmti_env, jmethodID method, const void* code_addr){
	
}
void cRegisterBytecode(){
	jvmtiCapabilities caps = {0};
	//caps.can_access_local_variables = 1;
    caps.can_generate_compiled_method_load_events = 1;
	caps.can_generate_all_class_hook_events = 1;
	caps.can_retransform_classes = 1;
	caps.can_retransform_any_class = 1;
    caps.can_get_bytecodes = 1;
    caps.can_get_constant_pool = 1;
    (*jvmti)->AddCapabilities(jvmti, &caps);
	
    jvmtiEventCallbacks calls = {0};
	calls.ClassFileLoadHook = ClassFileLoadHook;
	calls.CompiledMethodLoad = CompiledMethodLoad;
	calls.CompiledMethodUnload = CompiledMethodUnload;
	//calls.FieldAccess = FieldAccess
	//calls.FieldModification = FieldModification;
	//calls.FramePop = FramePop;
	//calls.NativeMethodBind = NativeMethodBind;
    (*jvmti)->SetEventCallbacks(jvmti, &calls, sizeof(calls));
    //(*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_FIELD_ACCESS, NULL);
	//(*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE,  JVMTI_EVENT_FIELD_MODIFICATION, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_COMPILED_METHOD_LOAD, NULL);
    (*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_COMPILED_METHOD_UNLOAD, NULL);
}
void cRegisterFuncCount(){
	jvmtiCapabilities caps = {0};
	//caps.can_generate_method_entry_events = 1;
	//caps.can_generate_method_exit_events = 1;
    (*jvmti)->AddCapabilities(jvmti, &caps);
    jvmtiEventCallbacks calls = {0};
	//calls.MethodEntry = MethodEntry;
	//calls.MethodExit = MethodExit;
	(*jvmti)->SetEventCallbacks(jvmti, &calls, sizeof(calls));
	//(*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE,  JVMTI_EVENT_FRAME_POP, NULL);
	(*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE,  JVMTI_EVENT_METHOD_ENTRY, NULL);
	(*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE,  JVMTI_EVENT_METHOD_EXIT, NULL);
}
void cRegisterSampleAlloc(){
    jvmtiCapabilities caps = {0};
    caps.can_generate_sampled_object_alloc_events = 1;
    caps.can_generate_garbage_collection_events = 1;
    (*jvmti)->AddCapabilities(jvmti, &caps);
	
    jvmtiEventCallbacks calls = {0};
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
	init_log(file_path);   //, int size_threshold, int number_threshold
	LOG_FILE = file_path;
}
void cSetLogNumber(int number_threshold){
	fprintf(stdout, "| Agent Cache count > %d \n", number_threshold );
	init_log_number(number_threshold);
}
void cSetDuration(int duration){
	END_TIME  = time(NULL)+duration;
	fprintf(stdout, "| Agent Set END_TIME = %lu seconds \n", END_TIME );
}
void cSetFunc(char* func){
	fprintf(stdout, "| Agent Function: %s \n", func );
	METHOD = func;
}
void cSetClass(char* class){
	fprintf(stdout, "| Agent Class %s \n", class );
	//CLASS = class;
}
void cSetCountInterval(int count_interval){
	COUNT_INTERVAL = count_interval;
	COUNT_TIME = time(NULL)+count_interval;
	fprintf(stdout, "| Agent Func Count Interval = %d seconds \n", count_interval );
}
JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM *jvm, char *options, void *reserved) {
	(*jvm)->GetEnv(jvm, (void**) &jvmti, JVMTI_VERSION_1_0);
    (*jvmti)->CreateRawMonitor(jvmti, "jvmti_lock", &jvmti_lock);
	CachedObjects = hashmap_new();
	gOptions(options);
	//gJVMTypeInit();
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