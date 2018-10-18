# Phase 1

### Constructs
* member-key - phase 1 key that starts with any string token
* member-value - list of copy or value-part
* element-value - list of copy or value-part
* copy-key - phase 1 key without -/append
* delete-key - syntactically the same as copy-key but requires special resolve function
* reference - recursive unrestricted key

### Functionality
* parse
* copy - (member/element)-value-part position
* delete - master-key position - initially any member-key position - let's see whether that's ok
* include - member-key position
* position
* pointer

### State Stack
* array
* object
* value_list
* reference (nested)

# Phase 2
