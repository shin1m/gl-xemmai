#ifndef XEMMAIX__GL__ARRAY_H
#define XEMMAIX__GL__ARRAY_H

#include "gl.h"

namespace xemmaix::gl
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
		if (a_index >= v_size) f_throw(L"out of range.");
	}

public:
	t_array_of(t_scoped&& a_bytes) : v_bytes(std::move(a_bytes)),
	v_size(f_as<t_bytes&>(v_bytes).f_size() / sizeof(T)),
	v_values(reinterpret_cast<T*>(&f_as<t_bytes&>(v_bytes)[0]))
	{
	}
	t_array_of(t_scoped&& a_bytes, size_t a_offset) : v_bytes(std::move(a_bytes))
	{
		if (a_offset % sizeof(T) > 0) f_throw(L"offset must be multiple of element size.");
		t_bytes& bytes = f_as<t_bytes&>(v_bytes);
		if (a_offset > bytes.f_size()) f_throw(L"out of range.");
		v_size = (bytes.f_size() - a_offset) / sizeof(T);
		v_values = reinterpret_cast<T*>(&bytes[0] + a_offset);
	}
	t_array_of(t_scoped&& a_bytes, size_t a_offset, size_t a_size) : v_bytes(std::move(a_bytes)), v_size(a_size)
	{
		if (a_offset % sizeof(T) > 0) f_throw(L"offset must be multiple of element size.");
		t_bytes& bytes = f_as<t_bytes&>(v_bytes);
		if (a_offset + v_size * sizeof(T) > bytes.f_size()) f_throw(L"out of range.");
		v_values = reinterpret_cast<T*>(&bytes[0] + a_offset);
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

namespace xemmai
{

template<typename T>
struct t_type_of<xemmaix::gl::t_array_of<T>> : t_underivable<t_holds<xemmaix::gl::t_array_of<T>>>
{
	typedef xemmaix::gl::t_extension t_extension;

	static void f_define(t_extension* a_extension, const std::wstring& a_name);

	using t_type_of::t_base::t_base;
	static void f_do_scan(t_object* a_this, t_scan a_scan);
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
	static size_t f_do_get_at(t_object* a_this, t_stacked* a_stack);
	static size_t f_do_set_at(t_object* a_this, t_stacked* a_stack);
};

template<typename T>
void t_type_of<xemmaix::gl::t_array_of<T>>::f_define(t_extension* a_extension, const std::wstring& a_name)
{
	using namespace xemmaix::gl;
	t_define<t_array_of<T>, t_object>(a_extension, a_name)
		(L"BYTES_PER_ELEMENT", sizeof(T))
		(L"size", t_member<size_t(t_array_of<T>::*)() const, &t_array_of<T>::f_size>())
	;
}

template<typename T>
void t_type_of<xemmaix::gl::t_array_of<T>>::f_do_scan(t_object* a_this, t_scan a_scan)
{
	a_scan(f_as<xemmaix::gl::t_array_of<T>&>(a_this).v_bytes);
}

template<typename T>
t_scoped t_type_of<xemmaix::gl::t_array_of<T>>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_overload<
		t_construct<t_scoped&&>,
		t_construct<t_scoped&&, size_t>,
		t_construct<t_scoped&&, size_t, size_t>
	>::t_bind<xemmaix::gl::t_array_of<T>>::f_do(this, a_stack, a_n);
}

template<typename T>
size_t t_type_of<xemmaix::gl::t_array_of<T>>::f_do_get_at(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	f_check<size_t>(a0.v_p, L"index");
	a_stack[0].f_construct(f_as<const xemmaix::gl::t_array_of<T>&>(a_this).f_get_at(f_as<size_t>(a0.v_p)));
	return -1;
}

template<typename T>
size_t t_type_of<xemmaix::gl::t_array_of<T>>::f_do_set_at(t_object* a_this, t_stacked* a_stack)
{
	t_destruct<> a0(a_stack[2]);
	t_destruct<> a1(a_stack[3]);
	f_check<size_t>(a0.v_p, L"index");
	f_check<T>(a1.v_p, L"value");
	a_stack[0].f_construct(f_as<xemmaix::gl::t_array_of<T>&>(a_this).f_set_at(f_as<size_t>(a0.v_p), f_as<T>(a1.v_p)));
	return -1;
}

}

#endif
