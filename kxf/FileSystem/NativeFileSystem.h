#pragma once
#include "Common.h"
#include "IFileSystem.h"
#include "FileItem.h"

namespace kxf
{
	class NativeFileSystem: public RTTI::ImplementInterface<NativeFileSystem, IFileSystem, IFileIDSystem>
	{
		public:
			using TCopyDirectoryTreeFunc = std::function<bool(FSPath, FSPath, BinarySize, BinarySize)>;
			using TEnumStreamsFunc = std::function<bool(String, BinarySize)>;

		private:
			UniversallyUniqueID m_LookupScope;

		public:
			NativeFileSystem(const UniversallyUniqueID& scope  = {}) noexcept
				:m_LookupScope(scope)
			{
			}

		public:
			// IFileSystem
			bool DoesItemExist(const FSPath& path) const override;
			FileItem GetItem(const FSPath& path) const override;
			size_t EnumItems(const FSPath& directory, TEnumItemsFunc func, const FSPathQuery& query = {}, FlagSet<FSEnumItemsFlag> flags = {}) const override;
			
			bool CreateDirectory(const FSPath& path) override;
			bool ChangeAttributes(const FSPath& path, FileAttribute attributes) override;
			bool ChangeTimestamp(const FSPath& path, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime) override;

			bool CopyItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) override;
			bool MoveItem(const FSPath& source, const FSPath& destination, TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) override;
			bool RenameItem(const FSPath& source, const FSPath& destination, FlagSet<FSCopyItemFlag> flags = {}) override;
			bool RemoveItem(const FSPath& path) override;

			// IFileIDSystem
			UniversallyUniqueID GetLookupScope() const override
			{
				return m_LookupScope;
			}

			bool DoesItemExist(const UniversallyUniqueID& id) const override;
			FileItem GetItem(const UniversallyUniqueID& id) const override;
			size_t EnumItems(const UniversallyUniqueID& id, TEnumItemsFunc func, FlagSet<FSEnumItemsFlag> flags = {}) const override
			{
				return 0;
			}

			bool ChangeAttributes(const UniversallyUniqueID& id, FileAttribute attributes) override
			{
				return false;
			}
			bool ChangeTimestamp(const UniversallyUniqueID& id, DateTime creationTime, DateTime modificationTime, DateTime lastAccessTime)  override
			{
				return false;
			}

			bool CopyItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) override
			{
				return false;
			}
			bool MoveItem(const UniversallyUniqueID& source, const UniversallyUniqueID& destination, TCopyItemFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) override
			{
				return false;
			}
			bool RemoveItem(const UniversallyUniqueID& id) override
			{
				return false;
			}

		public:
			bool IsInUse(const FSPath& path) const;
			size_t EnumStreams(const FSPath& path, TEnumStreamsFunc func) const;

			bool RemoveDirectoryTree(const FSPath& path);
			bool CopyDirectoryTree(const FSPath& source, const FSPath& destination, TCopyDirectoryTreeFunc func = {}, FlagSet<FSCopyItemFlag> flags = {}) const;
			bool MoveDirectoryTree(const FSPath& source, const FSPath& destination, TCopyDirectoryTreeFunc func = {}, FlagSet<FSCopyItemFlag> flags = {});

			FSPath GetExecutableDirectory() const;
			FSPath GetWorkingDirectory() const;
			bool SetWorkingDirectory(const FSPath& directory) const;
	};
}
