#pragma once

#include "Engine.h"

namespace GameBase
{
	template <typename T>
	class Matrix
	{
	public:
		Matrix(unsigned int _length, unsigned int _width, const T& defaultValue)
			: length(_length)
			, width(_width)
		{
			for (unsigned int x = 0; x < length; ++x)
			{
				matrix.Add(TArray<T>());

				for (unsigned int y = 0; y < width; ++y)
				{
					matrix[x].Add(defaultValue);
				}
			}
		}

		inline TArray<T>& operator[](unsigned int x)
		{
			return matrix[x];
		}

		inline const TArray<T>& operator[](unsigned int x) const
		{
			return matrix[x];
		}

		inline const unsigned int getLength() const { return length; }
		inline const unsigned int getWidth() const { return width; }

	private:
		TArray<TArray<T>> matrix;
		const unsigned int length;
		const unsigned int width;
	};

	template <typename T>
	class Array3D
	{
	public:
		Array3D(unsigned int length, unsigned int width, unsigned int height, const T& defaultValue)
			: length(length)
			, width(width)
			, height(height)
		{
			for (unsigned int x = 0; x < length; ++x)
			{
				matrix.Add(TArray<TArray<T>>());

				for (unsigned int y = 0; y < width; ++y)
				{
					matrix[x].Add(TArray<T>());

					for (unsigned int z = 0; z < height; ++z)
					{
						matrix[x][y].Add(defaultValue);
					}
				}
			}
		}

		inline TArray<TArray<T>>& operator[](unsigned int x)
		{
			return matrix[x];
		}

		inline const TArray<TArray<T>>& operator[](unsigned int x) const
		{
			return matrix[x];
		}

		inline const unsigned int getLength() const { return length; }
		inline const unsigned int getWidth() const { return width; }
		inline const unsigned int getHeight() const { return height; }

	private:
		TArray<TArray<TArray<T>>> matrix;
		const unsigned int length;
		const unsigned int width;
		const unsigned int height;
	};

} // namespace GameBase
