#pragma once
#include "Common.h"
#include "kxf/General/String.h"
#include "kxf/Drawing/Geometry.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/Localization/Locale.h"
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/cursor.h>

namespace kxf
{
	class GDIIcon;
	class GDIBitmap;
	class GDICursor;
	class ImageBundle;

	enum class DynamicLibraryFlag: uint32_t
	{
		None = 0,

		Resource = 1 << 0,
		ImageResource = 1 << 1,

		SearchUserDirectories = 1 << 8,
		SearchSystemDirectories = 1 << 9,
		SearchApplicationDirectory = 1 << 10,
		SearchLibraryDirectory = 1 << 11,
		SearchDefaultDirectories = SearchUserDirectories|SearchSystemDirectories|SearchApplicationDirectory,

		Exclusive = 1 << 16
	};
	KxFlagSet_Declare(DynamicLibraryFlag);
}

namespace kxf
{
	class KX_API DynamicLibrary final
	{
		public:
			class SearchDirectory;

		public:
			static DynamicLibrary GetCurrentModule() noexcept;
			static DynamicLibrary GetExecutingModule() noexcept;
			static DynamicLibrary GetLoadedModule(const String& name) noexcept;

			static SearchDirectory* AddSearchDirectory(const FSPath& path);
			static bool RemoveSearchDirectory(SearchDirectory& handle);

		private:
			std::optional<void*> m_Handle;
			FlagSet<DynamicLibraryFlag> m_LoadFlags;
			bool m_ShouldUnload = false;

		public:
			DynamicLibrary() noexcept = default;
			DynamicLibrary(const FSPath& path, FlagSet<DynamicLibraryFlag> flags = {})
			{
				Load(path, flags);
			}
			DynamicLibrary(DynamicLibrary&& other) noexcept
			{
				*this = std::move(other);
			}
			DynamicLibrary(const DynamicLibrary&) = delete;
			~DynamicLibrary() noexcept
			{
				Unload();
			}

		public:
			// General
			bool IsNull() const noexcept
			{
				return !m_Handle || m_Handle == nullptr;
			}
			bool ShouldUnload() const noexcept
			{
				return !IsNull() && m_ShouldUnload;
			}
			void* GetHandle() const noexcept
			{
				return m_Handle.value_or(nullptr);
			}
			FSPath GetFilePath() const;

			bool Load(const FSPath& path, FlagSet<DynamicLibraryFlag> flags = {});
			void Unload() noexcept;

			void AttachHandle(void* handle, bool takeOwnership = false) noexcept
			{
				Unload();

				m_Handle = handle;
				m_ShouldUnload = takeOwnership;
				m_LoadFlags = DynamicLibraryFlag::None;
			}
			void* DetachHandle() noexcept
			{
				void* handle = m_Handle.value_or(nullptr);
				m_Handle = {};
				m_ShouldUnload = false;
				m_LoadFlags = DynamicLibraryFlag::None;

				return handle;
			}

			// Functions
			size_t EnumExportedFunctionNames(std::function<bool(String)> func) const;

			void* GetExportedFunctionAddress(const char* name) const;
			void* GetExportedFunctionAddress(const wchar_t* name) const;
			void* GetExportedFunctionAddress(const String& name) const
			{
				return GetExportedFunctionAddress(name.wx_str());
			}
			void* GetExportedFunctionAddress(size_t ordinal) const;

			template<class TFunc, class T>
			TFunc* GetExportedFunction(T&& name) const
			{
				static_assert(std::is_function_v<std::remove_pointer_t<std::remove_reference_t<TFunc>>>, "free function type required");

				return reinterpret_cast<TFunc*>(GetExportedFunctionAddress(std::forward<T>(name)));
			}

			// Dependencies
			size_t EnumDependencyModuleNames(std::function<bool(String)> func) const;

			// Resources
			bool IsDataFile() const noexcept;
			bool IsImageResource() const noexcept;
			bool IsAnyResource() const noexcept;

			bool IsResourceExist(const String& resType, const String& resName, const Locale& locale = {}) const;
			size_t EnumResourceTypes(std::function<bool(String)> func, const Locale& locale = {}) const;
			size_t EnumResourceNames(const String& resType, std::function<bool(String)> func, const Locale& locale = {}) const;
			size_t EnumResourceLanguages(const String& resType, const String& resName, std::function<bool(Locale)> func) const;
			wxScopedCharBuffer GetResource(const String& resType, const String& resName, const Locale& locale = {}) const;

			size_t GetIconResourceCount(const String& name, const Locale& locale = {}) const;
			GDIIcon GetIconResource(const String& name, const Size& size = Size::UnspecifiedSize(), const Locale& locale = {}) const;
			GDIIcon GetIconResource(const String& name, size_t index, const Locale& locale = {}) const;
			ImageBundle GetIconBundleResource(const String& name, const Locale& locale = {}) const;

			GDIBitmap GetBitmapResource(const String& name, const Locale& locale = {}) const;
			GDICursor GetCursorResource(const String& name, const Locale& locale = {}) const;
			String GetStringResource(const String& name, const Locale& locale = {}) const;
			String GetMessageResource(uint32_t messageID, const Locale& locale = {}) const;

		public:
			DynamicLibrary& operator=(DynamicLibrary&& other) noexcept
			{
				m_Handle = std::move(other.m_Handle);

				m_LoadFlags = other.m_LoadFlags;
				other.m_LoadFlags = DynamicLibraryFlag::None;

				m_ShouldUnload = other.m_ShouldUnload;
				other.m_ShouldUnload = false;

				return *this;
			}
			DynamicLibrary& operator=(const DynamicLibrary&) = delete;

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
