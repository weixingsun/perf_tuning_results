#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <iostream>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <bcc/BPF.h>
#include <jvmti.h>

using namespace std;

#define MAX_STACK_DEPTH 128
static FILE* out;
static jlong start_time;
static jrawMonitorID vmtrace_lock;
static jvmtiEnv* jvmti = NULL;
static jrawMonitorID tree_lock;

struct Frame {
    jlong samples;
    jlong bytes;
    map<jmethodID, Frame> children;
};
static map<string, Frame> root;

// Define the same struct to use in user space.
struct stack_key_t {
  int pid;
  uint64_t kernel_ip;
  uint64_t kernel_ret_ip;
  int user_stack_id;
  int kernel_stack_id;
  char name[16];
};

string BPF_TXT = R"(
#include <linux/sched.h>
#include <uapi/linux/ptrace.h>
#include <uapi/linux/bpf_perf_event.h>
struct stack_key_t {
    u32 pid;
    u64 kernel_ip;
    u64 kernel_ret_ip;
    int user_stack_id;
    int kernel_stack_id;
    char name[TASK_COMM_LEN];
};
BPF_HASH(counts, struct stack_key_t);
BPF_STACK_TRACE(stack_traces, 16384); //STACK_SIZE

int do_perf_event(struct bpf_perf_event_data *ctx) {
    u64 id = bpf_get_current_pid_tgid();
    u32 tgid = id >> 32;
    u32 pid = id;
    if (pid == 0) return 0;
    if (!PID) return 0;
    struct stack_key_t key = {.pid = tgid};
    bpf_get_current_comm(&key.name, sizeof(key.name));
    // get stacks
    key.user_stack_id = stack_traces.get_stackid(&ctx->regs, BPF_F_USER_STACK);
    key.kernel_stack_id = stack_traces.get_stackid(&ctx->regs, 0);
    if (key.kernel_stack_id >= 0) {
        // populate extras to fix the kernel stack
        u64 ip = PT_REGS_IP(&ctx->regs);
        u64 page_offset;
        // if ip isn't sane, leave key ips as zero for later checking
#if defined(CONFIG_X86_64) && defined(__PAGE_OFFSET_BASE)
        // x64, 4.16, ..., 4.11, etc., but some earlier kernel didn't have it
        page_offset = __PAGE_OFFSET_BASE;
#elif defined(CONFIG_X86_64) && defined(__PAGE_OFFSET_BASE_L4)
        // x64, 4.17, and later
#if defined(CONFIG_DYNAMIC_MEMORY_LAYOUT) && defined(CONFIG_X86_5LEVEL)
        page_offset = __PAGE_OFFSET_BASE_L5;
#else
        page_offset = __PAGE_OFFSET_BASE_L4;
#endif
#else
        // earlier x86_64 kernels, e.g., 4.6, comes here
        // arm64, s390, powerpc, x86_32
        page_offset = PAGE_OFFSET;
#endif
        if (ip > page_offset) {
            key.kernel_ip = ip;
        }
    }
    counts.increment(key);
    return 0;
}
)";

static void trace(jvmtiEnv* jvmti, const char* fmt, ...) {
    jlong current_time;
    jvmti->GetTime(&current_time);

    char buf[MAX_STACK_DEPTH];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    jvmti->RawMonitorEnter(vmtrace_lock);

    fprintf(out, "[%.5f] %s\n", (current_time - start_time) / 1000000000.0, buf);
    
    jvmti->RawMonitorExit(vmtrace_lock);
}

// Converts JVM internal class signature to human readable name
static string decode_class_signature(char* class_sig) {
    switch (class_sig[0]) {
        case 'B': return "byte";
        case 'C': return "char";
        case 'S': return "short";
        case 'I': return "int";
        case 'J': return "long";
        case 'F': return "float";
        case 'D': return "double";
        case 'Z': return "boolean";
        case '[': return decode_class_signature(class_sig + 1) + "[]";
    }

    // Strip 'L' and ';'
    class_sig++;
    class_sig[strlen(class_sig) - 1] = 0;

    // Replace '/' with '.'
    for (char* c = class_sig; *c; c++) {
        if (*c == '/') *c = '.';
    }

    return class_sig;
}

static string get_method_name(jmethodID method) {
    jclass method_class;
    char* class_sig = NULL;
    char* method_name = NULL;
    string result;

    if (jvmti->GetMethodDeclaringClass(method, &method_class) == 0 &&
        jvmti->GetClassSignature(method_class, &class_sig, NULL) == 0 &&
        jvmti->GetMethodName(method, &method_name, NULL, NULL) == 0) {
        result.assign(decode_class_signature(class_sig) + "." + method_name);
    } else {
        result.assign("[unknown]");
    }

    jvmti->Deallocate((unsigned char*) method_name);
    jvmti->Deallocate((unsigned char*) class_sig);
    return result;
}

static void dump_tree(const string stack_line, const string& class_name, const Frame* f) {
    if (f->samples > 0) {
        // Output sample in 'collapsed stack traces' format understood by flamegraph.pl
        cout << stack_line << class_name << "_[i] " << f->samples << endl;
    }
    for (auto it = f->children.begin(); it != f->children.end(); ++it) {
        dump_tree(stack_line + get_method_name(it->first) + ";", class_name, &it->second);
    }
}

static void dump_profile() {
    for (auto it = root.begin(); it != root.end(); ++it) {
        dump_tree("", it->first, &it->second);
    }
}

static void record_stack_trace(char* class_sig, jvmtiFrameInfo* frames, jint count, jlong size) {
    Frame* f = &root[decode_class_signature(class_sig)];
    while (--count >= 0) {
        f = &f->children[frames[count].method];
    }
    f->samples++;
    f->bytes += size;
}

void JNICALL SampledObjectAlloc(jvmtiEnv* jvmti, JNIEnv* env, jthread thread,
                                jobject object, jclass object_klass, jlong size) {

    jvmtiFrameInfo frames[MAX_STACK_DEPTH];
    jint count;
    if (jvmti->GetStackTrace(thread, 0, MAX_STACK_DEPTH, frames, &count) != 0) {
        return;
    }

    char* class_sig;
    if (jvmti->GetClassSignature(object_klass, &class_sig, NULL) != 0) {
        return;
    }

    jvmti->RawMonitorEnter(tree_lock);
    record_stack_trace(class_sig, frames, count, size);
    jvmti->RawMonitorExit(tree_lock);

    jvmti->Deallocate((unsigned char*) class_sig);
}

void JNICALL DataDumpRequest(jvmtiEnv* jvmti) {
    jvmti->RawMonitorEnter(tree_lock);
    dump_profile();
    jvmti->RawMonitorExit(tree_lock);
}

void JNICALL VMDeath(jvmtiEnv* jvmti, JNIEnv* env) {
    DataDumpRequest(jvmti);
}

void JNICALL GarbageCollectionStart(jvmtiEnv *jvmti) {
    //log class instances: class_histo_before
    //trace(jvmti, "GC started.......................");
    //DataDumpRequest(jvmti);
}

void JNICALL GarbageCollectionFinish(jvmtiEnv *jvmti_env) {
    //log class instances: class_histo_before - class_histo_after
    //DataDumpRequest(jvmti);
    //trace(jvmti_env, "GC finished");
}
bool str_replace(string& str, const string& from, const string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void InitBPF(int duration) {
    ebpf::BPF bpf;
    int pid = getpid();
    //const string PID = "(tgid=="+to_string(pid)+")";
    const string PID = "1";
    str_replace(BPF_TXT, "PID", PID);
    auto init_r = bpf.init(BPF_TXT);
    if (init_r.code() != 0) {
        cerr << init_r.msg() << endl;
    }
    int PERF_TYPE_SOFTWARE = 1;
    int PERF_COUNT_HW_CPU_CYCLES = 0;
    int pid2=-1;
    auto att_r = bpf.attach_perf_event(PERF_TYPE_SOFTWARE, PERF_COUNT_HW_CPU_CYCLES, "do_perf_event", 99, 0, pid2);
    if (att_r.code() != 0) {
        cerr << att_r.msg() << endl;
    }else{
        cout << "attached to pid:" << pid << " perf event "<< duration <<" seconds"<< endl;
    }
    sleep(duration);
    bpf.detach_perf_event(PERF_TYPE_SOFTWARE, PERF_COUNT_HW_CPU_CYCLES);
    auto table = bpf.get_hash_table<stack_key_t, uint64_t>("counts").get_table_offline();
    sort( table.begin(), table.end(),
      [](pair<stack_key_t, uint64_t> a, pair<stack_key_t, uint64_t> b) {
        return a.second < b.second;
      }
    );
    auto stacks = bpf.get_stack_table("stack_traces");
    for (auto it : table) {
        cout << "PID:" << it.first.pid << it.first.name << endl;
        if (it.first.kernel_stack_id >= 0) {
            auto syms = stacks.get_stack_symbol(it.first.kernel_stack_id, -1);
            for (auto sym : syms) cout << "    " << sym << endl;
        }
	if (it.first.user_stack_id >= 0) {
            auto syms = stacks.get_stack_symbol(it.first.user_stack_id, it.first.pid);
            for (auto sym : syms) cout << "    " << sym << endl;
	}
    }
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM* vm, char* options, void* reserved) {
    if (options == NULL || !options[0]) {
        out = stderr;
    } else if ((out = fopen(options, "w")) == NULL) {
        fprintf(stderr, "Cannot open output file: %s\n", options);
        return 1;
    }
    int duration = 10;
    if (options != NULL ) {
        string sep = "=";
        string opt = string(options);
        int i = opt.find(sep);
        //jvmti->SetHeapSamplingInterval(atoi(options));
        string k = opt.substr(0,i);
        string vs = opt.substr(i+1);
        int v = stoi(vs);
        //cout << "k=" << k << " v=" << v << endl;
        if (k.compare("flame") == 0){
            InitBPF(v);
        }
    }
    vm->GetEnv((void**) &jvmti, JVMTI_VERSION_1_0);
    jvmti->CreateRawMonitor("tree_lock", &tree_lock);

    jvmtiCapabilities capabilities = {0};
    capabilities.can_generate_sampled_object_alloc_events = 1;
    capabilities.can_generate_all_class_hook_events = 1;
    capabilities.can_generate_compiled_method_load_events = 1;
    capabilities.can_generate_garbage_collection_events = 1;
    jvmti->AddCapabilities(&capabilities);

    if (options != NULL && options[0] >= '0' && options[0] <= '9') {
        jvmti->SetHeapSamplingInterval(atoi(options));
    }

    jvmtiEventCallbacks callbacks = {0};
    callbacks.SampledObjectAlloc = SampledObjectAlloc;
    callbacks.DataDumpRequest = DataDumpRequest;
    callbacks.VMDeath = VMDeath;
    callbacks.GarbageCollectionStart = GarbageCollectionStart;
    callbacks.GarbageCollectionFinish = GarbageCollectionFinish;
    jvmti->SetEventCallbacks(&callbacks, sizeof(callbacks));
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_SAMPLED_OBJECT_ALLOC, NULL);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_DATA_DUMP_REQUEST, NULL);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_DEATH, NULL);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_START, NULL);
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_GARBAGE_COLLECTION_FINISH, NULL);

    return 0;
}

JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM* vm, char* options, void* reserved) {
    // Protect against repeated load
    if (jvmti != NULL) {
        return 0;
    }
    return Agent_OnLoad(vm, options, reserved);
}
