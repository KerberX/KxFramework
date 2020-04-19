#pragma once
#include "Common.h"
#include "Kx/General/String.h"
#include "Kx/General/Version.h"
#include "Kx/FileSystem/FSPath.h"

namespace KxFramework
{
	enum class ExecutableVersionField
	{
		FileVersion,
		ProductVersion,
		FileDescription,
		InternalName,
		OriginalFileName,
		CompanyName,
		ProductName,
		LegalCopyright,
		LegalTrademarks,
		PrivateBuild,
		SpecialBuild,
	};
}

namespace KxFramework
{
	class KX_API ExecutableVersionResource final
	{
		private:
			std::vector<uint8_t> m_Buffer;
			uint16_t m_LangID = 0;
			uint16_t m_CodePage = 0;

		private:
			void Load(const FSPath& filePath);

		public:
			ExecutableVersionResource() noexcept = default;
			ExecutableVersionResource(const FSPath& filePath)
			{
				Load(filePath);
			}
			ExecutableVersionResource(ExecutableVersionResource&&) noexcept = default;
			ExecutableVersionResource(const ExecutableVersionResource&) = delete;

		public:
			// General
			bool IsNull() const noexcept;

			Version GetFileVersion() const;
			Version GetProductVersion() const;
			wxDateTime GetFileDate() const;
			String GetStringField(ExecutableVersionField fieldName) const;

			String GetAnyVersion() const;
			String GetAnyFileVersion() const;
			String GetAnyProductVersion() const;

		public:
			ExecutableVersionResource& operator=(ExecutableVersionResource&&) noexcept = default;
			ExecutableVersionResource& operator=(const ExecutableVersionResource&) = delete;

			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}
	};
}
