#pragma once
#include "Common.h"
#include "IFileSystem.h"
#include "FileItem.h"
#include "StorageVolume.h"

namespace kxf
{
	class KX_API NativeFileSystem: public RTTI::Implementation<NativeFileSystem, IFileSystem, IFileIDSystem>
	{
		public:
			static FSPath GetExecutingModuleRootDirectory();
			static FSPath GetExecutingModuleWorkingDirectory();
			static bool SetExecutingModuleWorkingDirectory(const FSPath& directory);

		protected:
			StorageVolume m_LookupVolume;
			FSPath m_LookupDirectory;

		private:
			void DoAssingLookupVolume(StorageVolume volume) noexcept
			{
				m_LookupVolume = std::move(volume);
				m_LookupDirectory.SetVolume(m_LookupVolume);
			}
			void DoAssingLookupVolumeUUID(const UniversallyUniqueID& scope) noexcept
			{
				m_LookupVolume = scope;
				m_LookupDirectory.SetVolume(m_LookupVolume);
			}
			void DoAssingLookupDirectory(FSPath directory) noexcept
			{
				m_LookupVolume = directory.GetAsVolume();
				m_LookupDirectory = std::move(directory);
			}

		public:
			NativeFileSystem() = default;
			NativeFileSystem(StorageVolume volume)
			{
				DoAssingLookupVolume(std::move(volume));
			}
			NativeFileSystem(FSPath directory)
			{
				DoAssingLookupDirectory(std::move(directory));
			}
			NativeFileSystem(const UniversallyUniqueID& scope)
			{
				DoAssingLookupVolumeUUID(scope);
			}

		public:
			// IFileSystem
			bool IsNull() const override
			{
				return false;
			}

			bool IsValidPathName(const FSPath& path) const override;
			String GetForbiddenPathNameCharacters(const String& except = {}) const override;

			bool IsLookupScoped() const override
			{
				return m_LookupDirectory || m_LookupVolume;
			}
			FSPath ResolvePath(const FSPath& relativePath) const override;
			FSPath GetLookupDirectory() const override
			{
				return m_LookupDirectory;
			}

			bool ItemExist(const FSPath& path) const override;
			bool FileExist(const FSPath& path) const override;
			bool DirectoryExist(const FSPath& path) const override;

			FileItem GetItem(const FSPath& path) const override;
			Enumerator<FileItem> EnumItems(const FSPath& directory, const FSPath& query = {}, FlagSet<FSActionFlag> flags = {}) const override;
			bool IsDirectoryEmpty(const FSPath& directory) const override;

			bool CreateDirectory(const FSPath& path, FlagSet<FSActionFlag> flags = {}) override;
			bool ChangeAttributes(const FSPath& path, FlagSet<FileAttribute> attributes) override;
			bool ChangeTimestamp(const FSPath& path, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) override;

			bool CopyItem(const FSPath& source, const FSPath& destination, std::function<bool(BinarySize, BinarySize)> func = {}, FlagSet<FSActionFlag> flags = {}) override;
			bool MoveItem(const FSPath& source, const FSPath& destination, std::function<bool(BinarySize, BinarySize)> func = {}, FlagSet<FSActionFlag> flags = {}) override;
			bool RenameItem(const FSPath& source, const FSPath& destination, FlagSet<FSActionFlag> flags = {}) override;
			bool RemoveItem(const FSPath& path) override;
			bool RemoveDirectory(const FSPath& path, FlagSet<FSActionFlag> flags = {}) override;

			std::unique_ptr<IStream> GetStream(const FSPath& path,
											   FlagSet<IOStreamAccess> access,
											   IOStreamDisposition disposition,
											   FlagSet<IOStreamShare> share = IOStreamShare::Read,
											   FlagSet<IOStreamFlag> streamFlags = IOStreamFlag::None,
											   FlagSet<FSActionFlag> flags = {}
			) override;
			using IFileSystem::OpenToRead;
			using IFileSystem::OpenToWrite;

		public:
			// IFileIDSystem
			UniversallyUniqueID GetLookupScope() const override
			{
				return m_LookupVolume.GetUniqueID();
			}

			bool ItemExist(const UniversallyUniqueID& id) const override;
			bool FileExist(const UniversallyUniqueID& id) const override;
			bool DirectoryExist(const UniversallyUniqueID& id) const override;

			FileItem GetItem(const UniversallyUniqueID& id) const override;
			Enumerator<FileItem> EnumItems(const UniversallyUniqueID& id, FlagSet<FSActionFlag> flags = {}) const override;
			bool IsDirectoryEmpty(const UniversallyUniqueID& id) const override
			{
				return false;
			}

			bool ChangeAttributes(const UniversallyUniqueID& id, FlagSet<FileAttribute> attributes) override
			{
				return false;
			}
			bool ChangeTimestamp(const UniversallyUniqueID& id, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime)  override
			{
				return false;
			}

			bool CopyItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, std::function<bool(BinarySize, BinarySize)> func = {}, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}
			bool MoveItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, std::function<bool(BinarySize, BinarySize)> func = {}, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}
			bool RemoveItem(const UniversallyUniqueID& id) override
			{
				return false;
			}
			bool RemoveDirectory(const UniversallyUniqueID& id, FlagSet<FSActionFlag> flags = {}) override
			{
				return false;
			}

			std::unique_ptr<IStream> GetStream(const UniversallyUniqueID& id,
											   FlagSet<IOStreamAccess> access,
											   IOStreamDisposition disposition,
											   FlagSet<IOStreamShare> share = IOStreamShare::Read,
											   FlagSet<IOStreamFlag> streamFlags = IOStreamFlag::None,
											   FlagSet<FSActionFlag> flags = {}
			) override;
			using IFileIDSystem::OpenToRead;
			using IFileIDSystem::OpenToWrite;

		public:
			// NativeFileSystem
			StorageVolume GetLookupVolume() const noexcept
			{
				return m_LookupVolume;
			}
			void SetLookupVolume(StorageVolume volume) noexcept
			{
				DoAssingLookupVolume(std::move(volume));
			}
			void SetLookupVolume(const UniversallyUniqueID& scope) noexcept
			{
				DoAssingLookupVolumeUUID(scope);
			}
			void SetLookupDirectory(FSPath directory) noexcept
			{
				DoAssingLookupDirectory(std::move(directory));
			}

			bool IsInUse(const FSPath& path) const;
			size_t EnumStreams(const FSPath& path, std::function<bool(String, BinarySize)> func) const;

			bool CopyDirectoryTree(const FSPath& source, const FSPath& destination, std::function<bool(FSPath, FSPath, BinarySize, BinarySize)> func = {}, FlagSet<FSActionFlag> flags = {}) const;
			bool MoveDirectoryTree(const FSPath& source, const FSPath& destination, std::function<bool(FSPath, FSPath, BinarySize, BinarySize)> func = {}, FlagSet<FSActionFlag> flags = {});

		public:
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

namespace kxf
{
	class KX_API ScopedNativeFileSystem: public NativeFileSystem
	{
		public:
			ScopedNativeFileSystem() = default;
			ScopedNativeFileSystem(StorageVolume volume)
				:NativeFileSystem(std::move(volume))
			{
			}
			ScopedNativeFileSystem(FSPath directory)
				:NativeFileSystem(std::move(directory))
			{
			}
			ScopedNativeFileSystem(const UniversallyUniqueID& scope)
				:NativeFileSystem(std::move(scope))
			{
			}

		public:
			// IFileSystem
			bool IsNull() const override;
	};
}
