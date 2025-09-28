#pragma once

typedef struct untyped__
{
	void* data;

	template <class T>
	untyped__(T x)
	{
		data = *reinterpret_cast<void**>(&x);
	}

	operator void* () const
	{
		return data;
	}

} untyped;