#include <v8.h>
#include <node.h>
#include <node_buffer.h>

#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <nan.h>

namespace {

using namespace v8;
using namespace node;
using v8::String;
using Nan::New;
using v8::Isolate;
using Nan::Set;
using Nan::GetFunction;

void Unmap(char* data, void* hint) {
	munmap(data, (size_t) hint);
}

//Handle<Value> Map(const v8::FunctionCallbackInfo<v8::Value>& args) {
NAN_METHOD(Map) {
	if (info.Length() <= 3) {
		return Nan::ThrowError("function takes 4 arguments: size, protection, flags, fd and offset.");
	}

	const size_t size = info[0]->ToInteger()->Value();
	const int protection = info[1]->ToInteger()->Value();
	const int flags = info[2]->ToInteger()->Value();
	const int fd = info[3]->ToInteger()->Value();
	const off_t offset = info[4]->ToInteger()->Value();

	char* data = (char *) mmap(0, size, protection, flags, fd, offset);

	if (data == MAP_FAILED) {
		return Nan::ThrowError(Nan::ErrnoException(errno, "mmap"));
	}

	v8::Local<v8::Object> buffer = Nan::NewBuffer(data, size, Unmap, (void *) size).ToLocalChecked();
	info.GetReturnValue().Set(buffer);
}

NAN_MODULE_INIT(InitAll) {
	Set(target, New<String>("PROT_READ").ToLocalChecked(), New<Integer>(static_cast<int8_t>(PROT_READ)));
	Set(target, New<String>("PROT_WRITE").ToLocalChecked(), New<Integer>(static_cast<int8_t>(PROT_WRITE)));
	Set(target, New<String>("PROT_EXEC").ToLocalChecked(), New<Integer>(static_cast<int8_t>(PROT_EXEC)));
	Set(target, New<String>("PROT_NONE").ToLocalChecked(), New<Integer>(static_cast<int8_t>(PROT_NONE)));
	Set(target, New<String>("MAP_SHARED").ToLocalChecked(), New<Integer>(static_cast<int8_t>(MAP_SHARED)));
	Set(target, New<String>("MAP_PRIVATE").ToLocalChecked(), New<Integer>(static_cast<int8_t>(MAP_PRIVATE)));
	Set(target, New<String>("PAGESIZE").ToLocalChecked(), New<Integer>(static_cast<int8_t>(sysconf(_SC_PAGESIZE))));
	Set(target, New<String>("map").ToLocalChecked(), GetFunction(New<FunctionTemplate>(Map)).ToLocalChecked());
}

NODE_MODULE(init, InitAll)

}
