#pragma once

#include "Common/Common.h"
#include <Windows.h>

namespace Wanted
{

	class WANTED_API Vector2
	{
	public:
		Vector2();
		Vector2(int x, int y);
		~Vector2();

		// 문자열로 변환해서 return하는 함수.
		const char* ToString();

		// Operator overloading.
		Vector2 operator+(const Vector2& other) const;
		Vector2 operator-(const Vector2& other) const;

		bool operator==(const Vector2& other) const;
		bool operator!=(const Vector2& other) const;

		// 형변환 연산자 overloading.
		operator COORD() const;

		// Vector 기본 값.
		static Vector2 Zero;
		static Vector2 One;
		static Vector2 Up;
		static Vector2 Right;

	public:
		// x, y 좌표.
		int x = 0;
		int y = 0;

	private:
		// Vector 값을 문자열로 변환할 때 사용할 변수.
		char* string = nullptr;
	};


}