#include "TePixelData.h"
#include "Image/TePixelUtil.h"
#include "Utility/TeColor.h"
#include "Math/TeVector2.h"
#include "Math/TeMath.h"

namespace te
{
	PixelData::PixelData(const PixelVolume& extents, PixelFormat pixelFormat)
		: _extents(extents)
		, _format(pixelFormat)
	{
		PixelUtil::GetPitch(extents.GetWidth(), extents.GetHeight(), extents.GetDepth(), pixelFormat, _rowPitch, _slicePitch);
	}

	PixelData::PixelData(UINT32 width, UINT32 height, UINT32 depth, PixelFormat pixelFormat)
		: _extents(0, 0, 0, width, height, depth)
		, _format(pixelFormat)
	{
		PixelUtil::GetPitch(width, height, depth, pixelFormat, _rowPitch, _slicePitch);
	}

	PixelData::PixelData(const PixelData& copy)
		: GpuResourceData(copy)
	{
		_format = copy._format;
		_rowPitch = copy._rowPitch;
		_slicePitch = copy._slicePitch;
		_extents = copy._extents;
	}

	PixelData& PixelData::operator=(const PixelData& rhs)
	{
		GpuResourceData::operator= (rhs);

		_format = rhs._format;
		_rowPitch = rhs._rowPitch;
		_slicePitch = rhs._slicePitch;
		_extents = rhs._extents;

		return *this;
	}

	UINT32 PixelData::GetRowSkip() const
	{
		UINT32 optimalRowPitch, optimalSlicePitch;
		PixelUtil::GetPitch(GetWidth(), GetHeight(), GetDepth(), _format, optimalRowPitch,
			optimalSlicePitch);

		return _rowPitch - optimalRowPitch;
	}

	UINT32 PixelData::GetSliceSkip() const
	{
		UINT32 optimalRowPitch, optimalSlicePitch;
		PixelUtil::GetPitch(GetWidth(), GetHeight(), GetDepth(), _format, optimalRowPitch,
			optimalSlicePitch);

		return _slicePitch - optimalSlicePitch;
	}

	UINT32 PixelData::GetConsecutiveSize() const
	{
		return PixelUtil::GetMemorySize(GetWidth(), GetHeight(), GetDepth(), _format);
	}

	UINT32 PixelData::GetSize() const
	{
		return _slicePitch * GetDepth();
	}

	PixelData PixelData::GetSubVolume(const PixelVolume& volume) const
	{
		if (PixelUtil::IsCompressed(_format))
		{
			if (volume.Left == GetLeft() && volume.Top == GetTop() && volume.Front == GetFront() &&
				volume.Right == GetRight() && volume.Bottom == GetBottom() && volume.Back == GetBack())
			{
				// Entire buffer is being queried
				return *this;
			}

			TE_ASSERT_ERROR(false, "Cannot return subvolume of compressed PixelBuffer", __FILE__, __LINE__);
		}

		if (!_extents.Contains(volume))
		{
			TE_ASSERT_ERROR(false, "Bounds out of range", __FILE__, __LINE__);
		}

		const size_t elemSize = PixelUtil::GetNumElemBytes(_format);
		PixelData rval(volume.GetWidth(), volume.GetHeight(), volume.GetDepth(), _format);

		rval.SetExternalBuffer(((UINT8*)GetData()) + ((volume.Left - GetLeft()) * elemSize)
			+ ((volume.Top - GetTop()) * _rowPitch)
			+ ((volume.Front - GetFront()) * _slicePitch));

		rval._format = _format;
		PixelUtil::GetPitch(volume.GetWidth(), volume.GetHeight(), volume.GetDepth(), _format, rval._rowPitch, rval._slicePitch);

		return rval;
	}

	Color PixelData::SampleColorAt(const Vector2& coords, TextureFilter filter) const
	{
		Vector2 pixelCoords = coords * Vector2((float)_extents.GetWidth(), (float)_extents.GetHeight());

		INT32 maxExtentX = std::max(0, (INT32)_extents.GetWidth() - 1);
		INT32 maxExtentY = std::max(0, (INT32)_extents.GetHeight() - 1);

		if (filter == TF_BILINEAR)
		{
			pixelCoords -= Vector2(0.5f, 0.5f);

			UINT32 x = (UINT32)Math::Clamp(Math::FloorToInt(pixelCoords.x), 0, maxExtentX);
			UINT32 y = (UINT32)Math::Clamp(Math::FloorToInt(pixelCoords.y), 0, maxExtentY);

			float fracX = pixelCoords.x - x;
			float fracY = pixelCoords.y - y;

			x = Math::Clamp(x, 0U, (UINT32)maxExtentX);
			y = Math::Clamp(y, 0U, (UINT32)maxExtentY);

			INT32 x1 = Math::Clamp(x + 1, 0U, (UINT32)maxExtentX);
			INT32 y1 = Math::Clamp(y + 1, 0U, (UINT32)maxExtentY);

			Color color = Color::ZERO;
			color += (1.0f - fracX) * (1.0f - fracY) * GetColorAt(x, y);
			color += fracX * (1.0f - fracY) * GetColorAt(x1, y);
			color += (1.0f - fracX) * fracY * GetColorAt(x, y1);
			color += fracX * fracY * GetColorAt(x1, y1);

			return color;
		}
		else
		{
			UINT32 x = (UINT32)Math::Clamp(Math::FloorToInt(pixelCoords.x), 0, maxExtentX);
			UINT32 y = (UINT32)Math::Clamp(Math::FloorToInt(pixelCoords.y), 0, maxExtentY);

			return GetColorAt(x, y);
		}
	}

	Color PixelData::GetColorAt(UINT32 x, UINT32 y, UINT32 z) const
	{
		Color cv;

		UINT32 pixelSize = PixelUtil::GetNumElemBytes(_format);
		UINT32 pixelOffset = z * _slicePitch + y * _rowPitch + x * pixelSize;
		PixelUtil::UnpackColor(&cv, _format, (unsigned char*)GetData() + pixelOffset);

		return cv;
	}

	void PixelData::SetColorAt(const Color& color, UINT32 x, UINT32 y, UINT32 z)
	{
		UINT32 pixelSize = PixelUtil::GetNumElemBytes(_format);
		UINT32 pixelOffset = z * _slicePitch + y * _rowPitch + x * pixelSize;
		PixelUtil::PackColor(color, _format, (unsigned char*)GetData() + pixelOffset);
	}

	Vector<Color> PixelData::GetColors() const
	{
		UINT32 depth = _extents.GetDepth();
		UINT32 height = _extents.GetHeight();
		UINT32 width = _extents.GetWidth();

		UINT32 pixelSize = PixelUtil::GetNumElemBytes(_format);
		UINT8* data = GetData();

		Vector<Color> colors(width * height * depth);
		for (UINT32 z = 0; z < depth; z++)
		{
			UINT32 zArrayIdx = z * width * height;
			UINT32 zDataIdx = z * _slicePitch;

			for (UINT32 y = 0; y < height; y++)
			{
				UINT32 yArrayIdx = y * width;
				UINT32 yDataIdx = y * _rowPitch;

				for (UINT32 x = 0; x < width; x++)
				{
					UINT32 arrayIdx = x + yArrayIdx + zArrayIdx;
					UINT32 dataIdx = x * pixelSize + yDataIdx + zDataIdx;

					UINT8* dest = data + dataIdx;
					PixelUtil::UnpackColor(&colors[arrayIdx], _format, dest);
				}
			}
		}

		return colors;
	}

	template<class T>
	void PixelData::SetColorsInternal(const T& colors, UINT32 numElements)
	{
		UINT32 depth = _extents.GetDepth();
		UINT32 height = _extents.GetHeight();
		UINT32 width = _extents.GetWidth();

		UINT32 totalNumElements = width * height * depth;
		if (numElements != totalNumElements)
		{
			TE_DEBUG("Unable to set colors, invalid array size.", __FILE__, __LINE__);
			return;
		}

		UINT32 pixelSize = PixelUtil::GetNumElemBytes(_format);
		UINT8* data = GetData();

		for (UINT32 z = 0; z < depth; z++)
		{
			UINT32 zArrayIdx = z * width * height;
			UINT32 zDataIdx = z * _slicePitch;

			for (UINT32 y = 0; y < height; y++)
			{
				UINT32 yArrayIdx = y * width;
				UINT32 yDataIdx = y * _rowPitch;

				for (UINT32 x = 0; x < width; x++)
				{
					UINT32 arrayIdx = x + yArrayIdx + zArrayIdx;
					UINT32 dataIdx = x * pixelSize + yDataIdx + zDataIdx;

					UINT8* dest = data + dataIdx;
					PixelUtil::PackColor(colors[arrayIdx], _format, dest);
				}
			}
		}
	}

	template TE_CORE_EXPORT void PixelData::SetColorsInternal(Color* const&, UINT32);
	template TE_CORE_EXPORT void PixelData::SetColorsInternal(const Vector<Color>&, UINT32);

	void PixelData::SetColors(const Vector<Color>& colors)
	{
		SetColorsInternal(colors, (UINT32)colors.size());
	}

	void PixelData::SetColors(Color* colors, UINT32 numElements)
	{
		SetColorsInternal(colors, numElements);
	}

	void PixelData::SetColors(const Color& color)
	{
		UINT32 depth = _extents.GetDepth();
		UINT32 height = _extents.GetHeight();
		UINT32 width = _extents.GetWidth();

		UINT32 pixelSize = PixelUtil::GetNumElemBytes(_format);
		UINT32 packedColor[4];
		assert(pixelSize <= sizeof(packedColor));

		PixelUtil::PackColor(color, _format, packedColor);

		UINT8* data = GetData();
		for (UINT32 z = 0; z < depth; z++)
		{
			UINT32 zDataIdx = z * _slicePitch;

			for (UINT32 y = 0; y < height; y++)
			{
				UINT32 yDataIdx = y * _rowPitch;

				for (UINT32 x = 0; x < width; x++)
				{
					UINT32 dataIdx = x * pixelSize + yDataIdx + zDataIdx;

					UINT8* dest = data + dataIdx;
					memcpy(dest, packedColor, pixelSize);
				}
			}
		}
	}

	float PixelData::GetDepthAt(UINT32 x, UINT32 y, UINT32 z) const
	{
		UINT32 pixelSize = PixelUtil::GetNumElemBytes(_format);
		UINT32 pixelOffset = z * _slicePitch + y * _rowPitch + x * pixelSize;
		return PixelUtil::UnpackDepth(_format, (unsigned char*)GetData() + pixelOffset);;
	}

	Vector<float> PixelData::GetDepths() const
	{
		UINT32 depth = _extents.GetDepth();
		UINT32 height = _extents.GetHeight();
		UINT32 width = _extents.GetWidth();

		UINT32 pixelSize = PixelUtil::GetNumElemBytes(_format);
		UINT8* data = GetData();

		Vector<float> depths(width * height * depth);
		for (UINT32 z = 0; z < depth; z++)
		{
			UINT32 zArrayIdx = z * width * height;
			UINT32 zDataIdx = z * _slicePitch;

			for (UINT32 y = 0; y < height; y++)
			{
				UINT32 yArrayIdx = y * width;
				UINT32 yDataIdx = y * _rowPitch;

				for (UINT32 x = 0; x < width; x++)
				{
					UINT32 arrayIdx = x + yArrayIdx + zArrayIdx;
					UINT32 dataIdx = x * pixelSize + yDataIdx + zDataIdx;

					UINT8* dest = data + dataIdx;
					depths[arrayIdx] = PixelUtil::UnpackDepth(_format, dest);
				}
			}
		}

		return depths;
	}

	SPtr<PixelData> PixelData::Create(const PixelVolume& extents, PixelFormat pixelFormat)
	{
		SPtr<PixelData> pixelData = te_shared_ptr_new<PixelData>(extents, pixelFormat);
		pixelData->AllocateInternalBuffer();

		return pixelData;
	}

	SPtr<PixelData> PixelData::Create(UINT32 width, UINT32 height, UINT32 depth, PixelFormat pixelFormat)
	{
		SPtr<PixelData> pixelData = te_shared_ptr_new<PixelData>(width, height, depth, pixelFormat);
		pixelData->AllocateInternalBuffer();

		return pixelData;
	}

	UINT32 PixelData::GetInternalBufferSize() const
	{
		return GetSize();
	}
}
