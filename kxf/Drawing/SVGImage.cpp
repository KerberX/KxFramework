#include "stdafx.h"
#include "SVGImage.h"
#include "BitmapImage.h"
#include "kxf/IO/IStream.h"
#include "kxf/IO/StreamReaderWriter.h"
#include "kxf/General/DateTime.h"
#include <lunasvg/svgdocument.h>

namespace
{
	constexpr double g_DefaultDPI = 96.0;

	constexpr bool IsValidDPI(int dpi) noexcept
	{
		return dpi > 0;
	}
	constexpr double ToSVGDPI(int dpi) noexcept
	{
		return IsValidDPI(dpi) ? dpi : g_DefaultDPI;
	}

	int FromSVGTime(double time) noexcept
	{
		return static_cast<int>(time * 1000.0);
	}
	double ToSVGTime(int time)
	{
		return time / 1000.0;
	}
}

namespace kxf
{
	void SVGImage::AllocExclusive()
	{
		auto svg = std::make_shared<lunasvg::SVGDocument>();
		svg->loadFromData(m_Document->toString());

		m_Document = std::move(svg);
	}

	std::unique_ptr<IImage2D> SVGImage::CloneImage2D() const
	{
		if (m_Document)
		{
			auto clone = std::make_unique<SVGImage>(*this);
			clone->AllocExclusive();

			return clone;
		}
		return nullptr;
	}

	// IImage2D
	void SVGImage::Create(const Size& size)
	{
		m_Document = std::make_shared<lunasvg::SVGDocument>();
	}
	bool SVGImage::Load(IInputStream& stream, const UniversallyUniqueID& format, size_t index)
	{
		m_Document = nullptr;

		if ((format == ImageFormat::SVG || format == ImageFormat::Any) && (index == 0 || index == npos))
		{
			IO::InputStreamReader reader(stream);

			auto document = std::make_shared<lunasvg::SVGDocument>();
			if (document->loadFromData(reader.ReadStringUTF8(stream.GetSize().ToBytes()).ToStdString()))
			{
				m_Document = std::move(document);
				return true;
			}
		}
		return false;
	}
	bool SVGImage::Save(IOutputStream& stream, const UniversallyUniqueID& format) const
	{
		if (m_Document && format != ImageFormat::None && format != ImageFormat::Any)
		{
			if (format == ImageFormat::SVG)
			{
				IO::OutputStreamWriter writer(stream);
				writer.WriteStringUTF8(String::FromUTF8(m_Document->toString()));
			}
			else if (BitmapImage bitmap = SVGImage::Rasterize(SVGImage::GetSize()))
			{
				return bitmap.Save(stream, format);
			}
		}
		return false;
	}

	Size SVGImage::GetSize() const
	{
		return m_Document ? Size(m_Document->documentWidth(ToSVGDPI(m_DPI)), m_Document->documentHeight(ToSVGDPI(m_DPI))) : Size::UnspecifiedSize();
	}

	std::optional<int> SVGImage::GetOptionInt(const String& name) const
	{
		if (m_Document)
		{
			if (name == ImageOption::Resolution || name == ImageOption::DPI)
			{
				if (IsValidDPI(m_DPI))
				{
					return m_DPI;
				}
			}
			else if (name == ImageOption::SVG::HasAnimation)
			{
				return m_Document->hasAnimation() ? 1 : 0;
			}
			else if (name == ImageOption::SVG::AnimationDuration)
			{
				return FromSVGTime(m_Document->animationDuration());
			}
			else if (name == ImageOption::SVG::CurrentTime)
			{
				return FromSVGTime(m_Document->currentTime());
			}
		}
		return {};
	}
	void SVGImage::SetOption(const String& name, int value)
	{
		if (name == ImageOption::Resolution || name == ImageOption::DPI)
		{
			m_DPI = value;
		}
		else if (name == ImageOption::SVG::CurrentTime)
		{
			m_Document->setCurrentTime(ToSVGTime(value), true);
		}
	}

	// IVectorImage
	Rect SVGImage::GetBoundingBox() const
	{
		if (m_Document)
		{
			auto box = m_Document->getBBox(ToSVGDPI(m_DPI));
			return Rect(box.x, box.y, box.width, box.height);
		}
		return {};
	}
	BitmapImage SVGImage::Rasterize(const Size& size) const
	{
		if (m_Document)
		{
			lunasvg::Bitmap svgBitmap;
			if (size.IsFullySpecified())
			{
				svgBitmap = m_Document->renderToBitmap(size.GetWidth(), size.GetHeight(), ToSVGDPI(m_DPI));
			}
			else
			{
				svgBitmap = m_Document->renderToBitmap(0, 0, ToSVGDPI(m_DPI));
			}

			if (const auto sourceData = svgBitmap.data())
			{
				// If we have no size specified set it to the SVG's default size
				Geometry::BasicSize<size_t> actualSize = {svgBitmap.width(), svgBitmap.height()};
				if (size.IsFullySpecified())
				{
					actualSize.SetWidth(size.GetWidth());
					actualSize.SetHeight(size.GetHeight());
				}

				static_assert(sizeof(PackedRGBA<uint8_t>) == 4 && alignof(PackedRGBA<uint8_t>) == alignof(uint8_t));

				BitmapImage image(actualSize);
				image.SetPixelDataRGBA(reinterpret_cast<const PackedRGBA<uint8_t>*>(sourceData));
				return image;
			}
		}
		return {};
	}

	SVGImage& SVGImage::operator=(const SVGImage& other)
	{
		m_Document = other.m_Document;
		m_DPI = other.m_DPI;

		return *this;
	}
	SVGImage& SVGImage::operator=(SVGImage&& other) noexcept
	{
		m_Document = std::move(other.m_Document);
		m_DPI = std::move(other.m_DPI);

		return *this;
	}
}
