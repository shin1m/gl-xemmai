#ifndef GL__XEMMAI__ARRAY_H
#define GL__XEMMAI__ARRAY_H

#include "gl.h"

namespace gl
{

namespace xemmai
{

template<typename T>
class t_array_of
{
	friend struct xemmai::t_type_of<t_array_of>;

	t_slot v_bytes;
	size_t v_size;
	T* v_values;

	void f_check(size_t a_index) const
	{
		if (a_index >= v_size) t_throwable::f_throw(L"out of range.");
	}

public:
	t_array_of(t_scoped&& a_bytes) : v_bytes(std::move(a_bytes)),
	v_size(f_as<t_bytes&>(v_bytes).f_size() / sizeof(T)),
	v_values(reinterpret_cast<T*>(&f_as<t_bytes&>(v_bytes)[0]))
	{
	}
	t_array_of(t_scoped&& a_bytes, size_t a_offset) : v_bytes(std::move(a_bytes))
	{
		if (a_offset % sizeof(T) > 0) t_throwable::f_throw(L"offset must be multiple of element size.");
		f_check(a_offset);
		t_bytes& bytes = f_as<t_bytes&>(v_bytes);
		v_size = (bytes.f_size() - a_offset) / sizeof(T);
		v_values = reinterpret_cast<T*>(&bytes[0] + a_offset);
	}
	t_array_of(t_scoped&& a_bytes, size_t a_offset, size_t a_size) : v_bytes(std::move(a_bytes))
	{
		if (a_offset % sizeof(T) > 0) t_throwable::f_throw(L"offset must be multiple of element size.");
		f_check(a_offset);
		f_check(a_offset + a_size * sizeof(T));
		v_size = a_size;
		v_values = reinterpret_cast<T*>(&f_as<t_bytes&>(v_bytes)[0] + a_offset);
	}
	size_t f_size() const
	{
		return v_size;
	}
	T f_get_at(size_t a_index) const
	{
		f_check(a_index);
		return v_values[a_index];
	}
	T f_set_at(size_t a_index, T a_value)
	{
		f_check(a_index);
		return v_values[a_index] = a_value;
	}
};

}

}

namespace xemmai
{

using gl::xemmai::t_array_of;

template<typename T>
struct t_type_of<t_array_of<T>> : t_type
{
	typedef gl::xemmai::t_extension t_extension;

	static void f_define(t_extension* a_extension, const std::wstring& a_name);

	using t_type::t_type;
	virtual t_type* f_derive(t_object* a_this);
	virtual void f_scan(t_object* a_this, t_scan a_scan);
	virtual void f_finalize(t_object* a_this);
	virtual t_scoped f_construct(t_object* a_class, t_slot* a_stack, size_t a_n);
	virtual void f_get_at(t_object* a_this, t_slot* a_stack);
	virtual void f_set_at(t_object* a_this, t_slot* a_stack);
};

template<typename T>
void t_type_of<t_array_of<T>>::f_define(t_extension* a_extension, const std::wstring& a_name)
{
	t_define<t_array_of<T>, t_object>(a_extension, a_name)
		(L"BYTES_PER_ELEMENT", sizeof(T))
		(L"size", t_member<size_t (t_array_of<T>::*)() const, &t_array_of<T>::f_size>())
	;
}

template<typename T>
t_type* t_type_of<t_array_of<T>>::f_derive(t_object* a_this)
{
	return nullptr;
}

template<typename T>
void t_type_of<t_array_of<T>>::f_scan(t_object* a_this, t_scan a_scan)
{
	a_scan(f_as<t_array_of<T>&>(a_this).v_bytes);
}

template<typename T>
void t_type_of<t_array_of<T>>::f_finalize(t_object* a_this)
{
	delete &f_as<t_array_of<T>&>(a_this);
}

template<typename T>
t_scoped t_type_of<t_array_of<T>>::f_construct(t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return t_overload<
		t_construct<t_scoped&&>,
		t_construct<t_scoped&&, size_t>,
		t_construct<t_scoped&&, size_t, size_t>
	>::t_bind<t_array_of<T>>::f_do(a_class, a_stack, a_n);
}

template<typename T>
void t_type_of<t_array_of<T>>::f_get_at(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	f_check<t_array_of<T>>(a_this, L"this");
	t_scoped a0 = std::move(a_stack[1]);
	f_check<size_t>(a0, L"index");
	a_stack[0].f_construct(f_as<const t_array_of<T>&>(a_this).f_get_at(f_as<size_t>(a0)));
	context.f_done();
}

template<typename T>
void t_type_of<t_array_of<T>>::f_set_at(t_object* a_this, t_slot* a_stack)
{
	t_native_context context;
	f_check<t_array_of<T>>(a_this, L"this");
	t_scoped a0 = std::move(a_stack[1]);
	t_scoped a1 = std::move(a_stack[2]);
	f_check<size_t>(a0, L"index");
	f_check<T>(a1, L"value");
	a_stack[0].f_construct(f_as<t_array_of<T>&>(a_this).f_set_at(f_as<size_t>(a0), f_as<T>(a1)));
	context.f_done();
}

}

#endif
