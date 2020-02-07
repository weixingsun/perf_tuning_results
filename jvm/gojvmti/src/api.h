#ifndef API_H
#define API_H

/////////////////////////////////////////////////////////////////////////////////
void cSetHeapSamplingInterval(int heap_interval);
void cRegisterSampleAlloc();
void cRegisterFuncCount();
void cRegisterBytecode();
void cSetLogFile(char* file_path);
void cSetLogNumber(int log_number);
void cSetDuration(int duration);
void cSetFunc(char* func);
void cSetClass(char* class);
void cSetCountInterval(int count_interval);

#endif // API_H