#include <string>
#include <regex>

#include <dlfcn.h>

#include "interfaces.h"

struct ClassImpl {
  void* constructor_ptr = nullptr;
};

struct ClassLoaderImpl {
  ClassLoaderError error;
};

AbstractClass::AbstractClass() {
  pImpl = new
  ClassImpl();
}

AbstractClass::~AbstractClass() {
  delete pImpl;
}

std::string Name(const std::string
& className) {
std::string name = "_ZN";
for (
size_t i = 0;
i < className.
size();
++i) {
char c = className[i];
size_t len = 0;
if (c != ':') {
while (i < className.
size()
&& className[i] != ':') {
++len, ++
i;
}
name +=
std::to_string(len);
name += className.
substr(i
- len, len);
}
}
name += "C1Ev";
return
name;
}

void* AbstractClass::newInstanceWithSize(size_t sizeofClass) {
  void* ptr = malloc(sizeofClass);
  typedef void (* constructor_t)(void*);
  auto constructor = (constructor_t) pImpl->constructor_ptr;
  constructor(ptr);
  return ptr;
}

ClassLoader::ClassLoader() {
  pImpl = new
  ClassLoaderImpl();
  pImpl->error = ClassLoaderError::NoError;
}

ClassLoader::~ClassLoader() {
  delete pImpl;
}

ClassLoaderError ClassLoader::lastError()
const {
return pImpl->
error;
}

AbstractClass* ClassLoader::loadClass(const std::string
& fullyQualifiedName) {
std::string
    filename = std::regex_replace(fullyQualifiedName, std::regex("::"), "/");
std::string path = std::getenv("CLASSPATH");
std::string filepath = path + "/" + filename + ".so";
auto*
classs = new
AbstractClass();
void* lib = dlopen(filepath.c_str(), RTLD_NOW | RTLD_GLOBAL);
if (!lib) {
if (errno == ENOENT) {
pImpl->
error = ClassLoaderError::FileNotFound;
} else {
pImpl->
error = ClassLoaderError::LibraryLoadError;
}
return
nullptr;
}
std::string name = Name(fullyQualifiedName);
void* sym = dlsym(lib, name.c_str());
if (!sym) {
pImpl->
error = ClassLoaderError::NoClassInLibrary;
return
nullptr;
}
classs->pImpl->
constructor_ptr = sym;
return
classs;
}


