# Foo

| Value Class Member Function | Remarks |
-----------------------------------------
| `bool get_boolean() const` ||



| `std::int64_t get_signed() const` ||
| `std::uint64_t get_unsigned() const` ||
| `double get_double() const` ||
| `std::string& get_string()` ||
| `const std::string& get_string() const` ||
| `tao::string_view get_string_view() const` ||
| `tao::string_view get_string_type() const` | `type::STRING` and `type::STRING_VIEW` |
| `std::vector< tao::byte >& get_binary()` ||
| `const std::vector< tao::byte >& get_binary() const` ||
| `tao::binary_view get_binary_view() const` ||
| `tao::binary_view get_binary_type() const` || `type::BINARY` and `type::BINARY_VIEW` |
| `std::vector< value >& get_array()` ||
| `const std::vector< value >& get_array() const` ||
| `std::map< std::string, value >& get_object()` ||
| `const std::map< std::string, value >& get_object() const` ||
| `const value* get_value_ptr() const` ||

For particular templated code the `get<>()` accessor function that is templated over the `tao::json::type`-enumeration can be used.
It behaves just like the correspondingly named get function, i.e. `get< tao::json::type::STRING >` is the same as `get_string()`.

```c++
tao::json::value v = "hallo";
const std::string s1 = v.get_string();
const std::string s2 = v.get< tao::json::type::STRING >();
assert( s1 == s2 );  // OK
```

## Manipulating Values

Values can always (unless of type `tao::json::DESTROYED`, which should never be encountered in code that doesn't invoke unspecified or undefined behaviour) be assigned a new value using copy-assignment or move-assignment, and the contents of two Values can be swapped with the `tao::json::value::swap()` member function.

Mutable references to standard containers returned by the mutable getters above can be normally manipulated manually and with standard algorithms etc.

Like the equivalent constructor, the assignment operators that accept any type use the Type Traits to assign the Value from the argument.

The following member functions of class `tao::json::value` bypass the Type Traits and directly assign to the underlying data in a Value.

| Value Class Member Function | Remarks |
| `void assign_null() noexcept` ||
| `void assign_boolean( const bool ) noexcept` ||
| `void assign_signed( const std::int64_t ) noexcept` ||
| `void assign_unsigned( const std::uint64_t ) noexcept` ||
| `void assign_double( const double ) noexcept` ||
| `void assign_string( std::string&& ) noexcept` ||
| `void assign_string( const std::string& )` ||
| `void assign_string_view( const tao::string_view ) noexcept` ||
| `template< typename... Ts > void emplace_string( Ts&&... )` | `noexcept` depending on arguments |
| `void assign_binary( std::vector< tao::byte >&& ) noexcept` ||
| `void assign_binary( const std::vector< tao::byte >& )` ||
| `void assign_binary_view( const tao::binary_view ) noexcept` ||
| `template< typename... Ts > void emplace_binary( Ts&&... )` | `noexcept` depending on arguments |
| `void assign_array( std::vector< value >&& ) noexcept` ||
| `void assign_array( const std::vector< value >& )` ||
| `template< typename... Ts > void emplace_array( Ts&&... )` | `noexcept` depending on arguments |
| `void assign_object( std::map< std::string, value >&& ) noexcept` ||
| `void assign_object( const std::map< std::string, value >& )` ||
| `template< typename... Ts > void emplace_object( Ts&&... )` | `noexcept` depending on arguments |
| `void assign_value_ptr( const value* ) noexcept` ||
| `template< typename T > void assign_opaque_ptr( T* ) noexcept` ||
| `template< typename T > void assign_opaque_ptr( T*, const producer_t ) noexcept` ||

Remember that the functions `tao::json::value::get_array()` and `tao::json::value::get_object()` are actually `tao::json::basic_value< Traits, Base >::get_array()` and `tao::json::basic_value< Traits, Base >::get_object()`, and that the sub-values of the returned containers have the same `Traits` and `Base` class as the Value on which the method was invoked.

```c++
tao::json::value x = tao::json::empty_array;
std::vector< tao::json::value >& a = x.get_array();

tao::json::basic_value< my_traits > y = tao::json::empty_array;
std::vector< tao::json::basic_value< my_traits > >& b = y.get_array();

tao::json::basic_value< my_traits, my_base > z = tao::json::empty_array;
std::vector< tao::json::basic_value< my_traits, my_base > >& c = z.get_array();
```

Similarly, when assigning to a Value, or adding further sub-values to a Value representing an Array or Object, Values with the same Traits and Base need to be used.

## Container Functions

There are several convenience functions that simplify access and manipulation of Values representing Arrays and Objects.

The following functions have several overloads and can be called with a `std::size_t` on Values representing an Array, a `std::string` for Values representing an Object, or with a [JSON Pointer](Extended-Use-Cases#json-pointer).

When called with a [JSON Pointer](Extended-Use-Cases#json-pointer), the type of index, integer or string, must correspond to the type of container, Array or Object, on all steps along the path.

The `tao::json::value::at()` function indexes Arrays and Objects via `std::vector::at()` and `std::map::at()`, respectively.
When called with a [JSON Pointer](Extended-Use-Cases#json-pointer), the contents of the pointer must be consistent with the type of Value.

Similarly `tao::json::value::operator[]` indexes Arrays and Objects via `std::vector::operator[]` and `std::map::operator[]`, respectively.
For Objects, it keeps the semantics of the underlying `std::map` and inserts a default-constructed Value when the key string is not found.

The `tao::json::value::erase()` function erases a sub-value of an Array or Object via `std::vector::erase()` and `std::map::erase(), respectively.

The `tao::json::value::find()` function can only be called with a `std::size_t` or a `std::string` for Arrays and Objects, respectively.
It returns a plain pointer to the sub-value, or `nullptr` when no matching entry was found.
As usual there are both a const overload that returns a `const value*`, and a non-const overload that returns a `value*`.

The `tao::json::value::optional< T >()` function exists in two versions, one without arguments [explained above](#accessing-values), and one that can take any argument that `tao::json::value::find()` can.
The latter returns an empty optional when no entry is found; it does *not* return an empty optional when a JSON Null entry is found but throws an exception when the found entry can not be converted to `T` via `tao::json::value::as< T >()`.

The following functions include calls to `tao::json::value::prepare_array()` and `tao::json::value::prepare_object()` which are used on empty (`tao::json::type::UNITIALIZED` or `tao::json::type::DISCARDED`) Values to initialise to an empty array, or empty object, respectively.

The `tao::json::value::push_back()` and `tao::json::value::emplace_back()` functions for Arrays, and `tao::json::value::insert()` and `tao::json::value::emplace()` for Objects, again forward to the corresponding functions of the underlying standard container.

A more general version of `tao::json:value::insert()` inserts the JSON Value from the second argument in the position indicated by the JSON Pointer in its first argument.

To add multiple entries to an Array or Object with a single function call use `tao::json::value::append()` with Arrays, and `tao::json::value::insert()` with Objects.
Both take an initialiser-list, `tao::json::value::append()` one with only values like `static tao::json::value::array()`, and `tao::json::value::insert()` one with keys and values like the constructor that creates an Object from an initialiser-list does.

## Comparing Values

The library contains the full complement of comparison operators (`==`, `!=`, `<`, `>`, `<=`, `>=` ) to compare Values with other Values, and to compare Values with any other C++ type.

Comparison of two Values performs a structural recursive comparison ignoring both the Type Traits and the base class template parameters.
The comparison is performed at the data model level, abstracting away from implementation details and optimisations.

* Numbers are compared by value, independent of which of the possible representations they use.
* Strings and string views are compared by comparing character sequences, independent of which representation is used.
* Binary data and binary views are compared by comparing byte sequences, independent of which representation is used.
* Value Pointers are skipped, the comparison behaves "as if" the pointee Value were copied to the Value with the pointer.
* Values of different incompatible types will be ordered by the numeric Values of their type enum.

Comparison between a JSON Value and another type is performed by either

1. creating a Value object from the other type and performing the comparison between the two Value, or
2. using the [Type Traits](Type-Traits.md) to directly perform the comparison without creating a temporary Value.

See the [documentation on Type Traits](Type-Traits.md#compare-value-with-type) for details on how to enable comparison operators without temporary Values for a type.

Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
