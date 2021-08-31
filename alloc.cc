#include <new>
#include <cstdlib>
#include <cstddef>
#include <climits>
#include <iostream>

namespace My
{
	//************************************
	// Method:    _allocate
	// Desc:	  分配空间的函数，实际上是调用了operator new，稍后将会讲述operator new
	// Author:    
	// Date: 	  2019/07/17
	// FullName:  My::_allocate
	// Access:    public 
	// Returns:   T*
	// Qualifier:
	// Parameter: ptrdiff_t size：	分配空间的大小
	// Parameter: T *：				类型
	//************************************
	template <class T>
	inline T* _allocate(ptrdiff_t size, T*)
	{
		set_new_handler(0);
        
        //operator new只是用来申请内存空间，并没有调用构造函数
		T* TMP = (T *)(::operator new((size_t)(size * sizeof(T))));
        
		if (TMP == 0)
		{
			cerr << "out of memory" << endl;
			exit(1);
		}
		return TMP;
	}

	//************************************
	// Method:    _deallocator
	// Desc:	  回收空间的函数
	// Author:    
	// Date: 	  2019/07/17
	// FullName:  My::_deallocator
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: T * buffer：	buffer指向已经想回收的地址
	//************************************
	template <class T>
	inline void _deallocator(T* buffer)
	{
		::operator delete(buffer);
	}

	//************************************
	// Method:    _construct
	// Desc:	  调用类的构造函数，稍后讲述new
	// Author:    
	// Date: 	  2019/07/17
	// FullName:  My::_construct
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: T1 * p：			指向实际存储空间的指针
	// Parameter: const T2 & value：	该类型的值
	//************************************
	template <class T1, class T2>
	inline void _construct(T1 *p, const T2& value)
	{
        //placement new用于调用构造函数
		new (p)T1(value);
	}

	//************************************
	// Method:    _destroy
	// Desc:	  调用析构函数
	// Author:    
	// Date: 	  2019/07/17
	// FullName:  My::_destroy
	// Access:    public 
	// Returns:   void
	// Qualifier:
	// Parameter: T * ptr：		指向地址的指针
	//************************************
	template <class T>
	inline void _destroy(T* ptr)
	{
		ptr->~T();
	}


	//allocator类实际上是上面几个函数的封装，相似的函数不再详细讲述
	template <class T>
	class allocator
	{
	public:
		typedef T			value_type;
		typedef T*			pointer;
		typedef const T*	const_pointer;
		typedef T&			reference;
		typedef const T&	const_reference;
		typedef ptrdiff_t	difference_type;
		typedef size_t		size_type;

		template <class U>
		struct rebind
		{
			typedef	allocator<U> other;
		};

		pointer allocate(size_type n, const void* hint = 0)
		{
			return _allocate((difference_type)n, (pointer)0);
		}

		void deallocate(pointer p, size_t n)
		{
			_deallocator(p);
		}

		void construct(pointer p, const T& value)
		{
			_construct(p, value);
		}

		void destroy(pointer p)
		{
			_destroy(p);
		}


		//************************************
		// Method:    address
		// Desc:	  返回地址
		// Author:    
		// Date: 	  2019/07/17
		// FullName:  My::allocator<T>::address
		// Access:    public 
		// Returns:   pointer
		// Qualifier:
		// Parameter: reference x
		//************************************
		pointer address(reference x)
		{
			return (pointer)&x;
		}

		const_pointer const_address(const_reference x)
		{
			return (const_pointer)&x;
		}

		size_type max_size() const
		{
			return size_type(UINT_MAX / sizeof(T));
		}

	};
}
————————————————
版权声明：本文为CSDN博主「qq_36253569」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/qq_36253569/article/details/96309285