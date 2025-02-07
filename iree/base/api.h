// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Dynamic Linkage
// -----------------------------------------------------------------------------
//
// Define IREE_API_NO_PROTOTYPES to disable function prototypes when linking and
// loading dynamically. This prevents accidental calls to functions without
// going through the resolved symbols.
//
// API Versioning
// -----------------------------------------------------------------------------
//
// The C API is designed to be versioned such that breaking changes either in
// ABI (data types, struct sizes, etc) or signatures (function arguments change)
// will result in a bump of the IREE_API_VERSION_LATEST value.
//
// When linked in statically the runtime should never have a version conflict,
// however dynamic linking where the runtime is a shared object loaded at
// runtime (via dlopen/etc) must always verify the version is as expected.
//
// In the current experimental state of the runtime the API may break frequently
// and the version is pinned at 0.
//
// Example:
//   void* library = dlopen("iree_rt.so", RTLD_LAZY | RTLD_LOCAL);
//   iree_api_version_t actual_version;
//   iree_status_t status = \
//       ((PFN_iree_api_version_check)dlsym(library, "iree_api_version_check"))(
//       IREE_API_VERSION_LATEST, &actual_version);
//   if (status != IREE_STATUS_OK) {
//     LOG(FATAL) << "Unsupported runtime API version " << actual_version;
//   }
//   dlclose(library);
//
// Object Ownership and Lifetime
// -----------------------------------------------------------------------------
//
// The API follows the CoreFoundation ownership policies:
// https://developer.apple.com/library/archive/documentation/CoreFoundation/Conceptual/CFMemoryMgmt/Concepts/Ownership.html
//
// These boil down to:
// * Objects returned from *_create or *_copy functions are owned by the caller
//   and must be released when the caller no longer needs them.
// * Objects returned from accessors are not owned by the caller and must be
//   retained by the caller if the object lifetime needs to be extended.
// * Objects passed to functions by argument may be retained by the callee if
//   required.
//
// Example:
//   iree_file_mapping_t* file_mapping;
//   s = iree_file_mapping_open_read(..., &file_mapping);
//   // file_mapping is now owned by this function.
//   s = iree_file_mapping_some_call(file_mapping, ...);
//   // Must release ownership when no longer required.
//   s = iree_file_mapping_release(file_mapping);

#ifndef IREE_BASE_API_H_
#define IREE_BASE_API_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

//===----------------------------------------------------------------------===//
// Types and Enums
//===----------------------------------------------------------------------===//

#ifdef __cplusplus
#define IREE_API_EXPORT extern "C"
#else
#define IREE_API_EXPORT
#endif  // __cplusplus

#if defined(_WIN32)
#define IREE_API_CALL __stdcall
#define IREE_API_PTR IREE_API_CALL
#else
#define IREE_API_CALL
#define IREE_API_PTR
#endif  // _WIN32

// Well-known status codes matching iree::StatusCode.
typedef enum {
  IREE_STATUS_OK = 0,
  IREE_STATUS_CANCELLED = 1,
  IREE_STATUS_UNKNOWN = 2,
  IREE_STATUS_INVALID_ARGUMENT = 3,
  IREE_STATUS_DEADLINE_EXCEEDED = 4,
  IREE_STATUS_NOT_FOUND = 5,
  IREE_STATUS_ALREADY_EXISTS = 6,
  IREE_STATUS_PERMISSION_DENIED = 7,
  IREE_STATUS_RESOURCE_EXHAUSTED = 8,
  IREE_STATUS_FAILED_PRECONDITION = 9,
  IREE_STATUS_ABORTED = 10,
  IREE_STATUS_OUT_OF_RANGE = 11,
  IREE_STATUS_UNIMPLEMENTED = 12,
  IREE_STATUS_INTERNAL = 13,
  IREE_STATUS_UNAVAILABLE = 14,
  IREE_STATUS_DATA_LOSS = 15,
  IREE_STATUS_UNAUTHENTICATED = 16,
} iree_status_t;

// TODO(benvanik): add ABSL_MUST_USE_RESULT to iree_status_t.

// Size, in bytes, of a buffer on the host.
typedef size_t iree_host_size_t;

// Size, in bytes, of a buffer on devices.
typedef uint64_t iree_device_size_t;
// Whole length of the underlying buffer.
#define IREE_WHOLE_BUFFER (iree_device_size_t(-1))

// An allocator for host-memory allocations.
// IREE will attempt to use this in place of the system malloc and free.
// Pass the IREE_ALLOCATOR_DEFAULT macro to use the system allocator.
typedef struct {
  // User-defined pointer passed to all functions.
  void* self;
  // Allocates |byte_length| of memory and stores the pointer in |out_ptr|.
  iree_status_t(IREE_API_PTR* alloc)(void* self, iree_host_size_t byte_length,
                                     void** out_ptr);
  // Frees |ptr| from a previous alloc call.
  iree_status_t(IREE_API_PTR* free)(void* self, void* ptr);
} iree_allocator_t;

// TODO(benvanik): ensure this works correctly.
#define IREE_ALLOCATOR_DEFAULT \
  { 0, iree_allocator_alloc, iree_allocator_free }

// Like absl::Time, represented as nanoseconds since unix epoch.
// TODO(benvanik): pick something easy to get into/outof time_t/etc.
typedef int64_t iree_time_t;
// Like absl::InfinitePast.
#define IREE_TIME_INFINITE_PAST INT64_MIN
// Like absl::InfiniteFuture.
#define IREE_TIME_INFINITE_FUTURE INT64_MAX

// A span of bytes (ala std::span of uint8_t).
typedef struct {
  uint8_t* data;
  iree_host_size_t data_length;
} iree_byte_span_t;

// A string view (ala std::string_view) into a non-NUL-terminated string.
typedef struct {
  const char* data;
  size_t size;
} iree_string_view_t;

#define IREE_SHAPE_MAX_RANK 5
typedef struct {
  int32_t rank;
  int32_t dims[IREE_SHAPE_MAX_RANK];
} iree_shape_t;

// Known versions of the API that can be referenced in code.
// Out-of-bounds values are possible in forward-versioned changes.
typedef enum {
  IREE_API_VERSION_0 = 0,
  // Always set to the latest version of the library from source.
  IREE_API_VERSION_LATEST = IREE_API_VERSION_0,
} iree_api_version_t;

typedef struct iree_file_mapping iree_file_mapping_t;

//===----------------------------------------------------------------------===//
// iree Core API
//===----------------------------------------------------------------------===//

#ifndef IREE_API_NO_PROTOTYPES

// Checks whether the |expected_version| of the caller matches the implemented
// version of |out_actual_version|. Forward compatibility of the API is
// supported but backward compatibility is not: newer binaries using older
// shared libraries of the runtime will fail.
//
// Returns IREE_STATUS_OUT_OF_RANGE if the actual version is not compatible with
// the expected version.
IREE_API_EXPORT iree_status_t IREE_API_CALL
iree_api_version_check(iree_api_version_t expected_version,
                       iree_api_version_t* out_actual_version);

// Allocates a block of |byte_length| bytes from the default system allocator.
IREE_API_EXPORT iree_status_t IREE_API_CALL
iree_allocator_alloc(void* self, iree_host_size_t byte_length, void** out_ptr);

// Frees a previously-allocated block of memory to the default system allocator.
IREE_API_EXPORT iree_status_t IREE_API_CALL iree_allocator_free(void* self,
                                                                void* ptr);

#endif  // IREE_API_NO_PROTOTYPES

typedef iree_status_t(IREE_API_PTR* PFN_iree_api_version_check)(
    iree_api_version_t expected_version,
    iree_api_version_t* out_actual_version);
typedef iree_status_t(IREE_API_PTR* PFN_iree_allocator_alloc)(
    void* self, iree_host_size_t byte_length, void** out_ptr);
typedef iree_status_t(IREE_API_PTR* PFN_iree_allocator_free)(void* self,
                                                             void* ptr);

//===----------------------------------------------------------------------===//
// iree::Shape
//===----------------------------------------------------------------------===//

// TODO(benvanik): shape functions.

//===----------------------------------------------------------------------===//
// iree::FileMapping
//===----------------------------------------------------------------------===//

#ifndef IREE_API_NO_PROTOTYPES

// Opens a file at |path| for read-only access via a file mapping.
// |out_file_mapping| must be released by the caller.
IREE_API_EXPORT iree_status_t IREE_API_CALL
iree_file_mapping_open_read(iree_string_view_t path, iree_allocator_t allocator,
                            iree_file_mapping_t** out_file_mapping);

// Retains the given |file_mapping| for the caller.
IREE_API_EXPORT iree_status_t IREE_API_CALL
iree_file_mapping_retain(iree_file_mapping_t* file_mapping);

// Releases the given |file_mapping| from the caller.
IREE_API_EXPORT iree_status_t IREE_API_CALL
iree_file_mapping_release(iree_file_mapping_t* file_mapping);

// Returns a reference to the byte buffer the |file_mapping| backs.
// Though the returned buffer is non-const behavior is undefined if read-only
// mappings are written to (exceptions, segfaults, etc).
IREE_API_EXPORT iree_byte_span_t IREE_API_CALL
iree_file_mapping_data(iree_file_mapping_t* file_mapping);

#endif  // IREE_API_NO_PROTOTYPES

typedef iree_status_t(IREE_API_PTR* PFN_iree_file_mapping_open_read)(
    iree_string_view_t path, iree_allocator_t allocator,
    iree_file_mapping_t** out_file_mapping);
typedef iree_status_t(IREE_API_PTR* PFN_iree_file_mapping_retain)(
    iree_file_mapping_t* file_mapping);
typedef iree_status_t(IREE_API_PTR* PFN_iree_file_mapping_release)(
    iree_file_mapping_t* file_mapping);
typedef iree_byte_span_t(IREE_API_PTR* PFN_iree_file_mapping_data)(
    iree_file_mapping_t* file_mapping);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#endif  // IREE_BASE_API_H_
