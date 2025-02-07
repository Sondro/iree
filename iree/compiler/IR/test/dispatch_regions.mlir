// RUN: iree-opt %s -split-input-file | FileCheck %s --dump-input=fail

// CHECK-LABEL: @singleArg
func @singleArg(%arg0 : tensor<?xf32>) {
  // CHECK-NEXT: %0 = "some.shape"
  // CHECK-NEXT: iree.dispatch_region[%0 : tensor<1xi32>](%arg1 = %arg0 : tensor<?xf32>) {
  // CHECK-NEXT:   iree.return
  // CHECK-NEXT: }
  %workload = "some.shape"(%arg0) : (tensor<?xf32>) -> tensor<1xi32>
  iree.dispatch_region[%workload : tensor<1xi32>](%i0 = %arg0 : tensor<?xf32>) {
    iree.return
  }
  // CHECK-NEXT: return
  return
}

// -----

// CHECK-LABEL: @multipleArgs
func @multipleArgs(%arg0 : tensor<?xf32>, %arg1 : tensor<?xf32>) {
  // CHECK-NEXT: %0 = "some.shape"
  // CHECK-NEXT: iree.dispatch_region[%0 : tensor<1xi32>](%arg2 = %arg0 : tensor<?xf32>, %arg3 = %arg1 : tensor<?xf32>) {
  // CHECK-NEXT:   iree.return
  // CHECK-NEXT: }
  %workload = "some.shape"(%arg0) : (tensor<?xf32>) -> tensor<1xi32>
  iree.dispatch_region[%workload : tensor<1xi32>](%i0 = %arg0 : tensor<?xf32>, %i1 = %arg1 : tensor<?xf32>) {
    iree.return
  }
  // CHECK-NEXT: return
  return
}

// -----

// CHECK-LABEL: @singleResult
func @singleResult(%arg0 : tensor<?xf32>) -> tensor<?xf32> {
  // CHECK-NEXT: %0 = "some.shape"
  // CHECK-NEXT: %1 = iree.dispatch_region[%0 : tensor<1xi32>](%arg1 = %arg0 : tensor<?xf32>) : tensor<?xf32> {
  // CHECK-NEXT:   iree.return %arg1 : tensor<?xf32>
  // CHECK-NEXT: }
  %workload = "some.shape"(%arg0) : (tensor<?xf32>) -> tensor<1xi32>
  %ret0 = iree.dispatch_region[%workload : tensor<1xi32>](%i0 = %arg0 : tensor<?xf32>) : tensor<?xf32> {
    iree.return %i0 : tensor<?xf32>
  }
  // CHECK-NEXT: return %1 : tensor<?xf32>
  return %ret0 : tensor<?xf32>
}

// -----

// CHECK-LABEL: @multipleResults
func @multipleResults(%arg0 : tensor<?xf32>) -> (tensor<?xf32>, tensor<?xf32>) {
  // CHECK-NEXT: %0 = "some.shape"
  // CHECK-NEXT: %1:2 = iree.dispatch_region[%0 : tensor<1xi32>](%arg1 = %arg0 : tensor<?xf32>) : tensor<?xf32>, tensor<?xf32> {
  // CHECK-NEXT:   iree.return %arg1, %arg1 : tensor<?xf32>, tensor<?xf32>
  // CHECK-NEXT: }
  %workload = "some.shape"(%arg0) : (tensor<?xf32>) -> tensor<1xi32>
  %ret0, %ret1 = iree.dispatch_region[%workload : tensor<1xi32>](%i0 = %arg0 : tensor<?xf32>) : tensor<?xf32>, tensor<?xf32> {
    iree.return %i0, %i0 : tensor<?xf32>, tensor<?xf32>
  }
  // CHECK-NEXT: return %1#0, %1#1 : tensor<?xf32>, tensor<?xf32>
  return %ret0, %ret1 : tensor<?xf32>, tensor<?xf32>
}
