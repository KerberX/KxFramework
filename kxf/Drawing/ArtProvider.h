#pragma once
#include "Common.h"
#include "IArtProvider.h"
#include "kxf/General/OptionalPtr.h"

namespace kxf
{
	class Bitmap;
	class Icon;
	class Image;
	class ImageBundle;
}

namespace kxf::ArtProviderClient
{
	extern const String ToolBar;
	extern const String Menu;
	extern const String List;
	extern const String Button;
	extern const String FrameIcon;
	extern const String HelpBrowser;
	extern const String CommonDialog;
	extern const String MessageBox;
}

namespace kxf::ArtProvider
{
	void PushProvider(optional_ptr<IArtProvider> artProvider);
	void RemoveProvider(const IArtProvider& artProvider);
}

namespace kxf::ArtProvider
{
	Bitmap GetResource(const ResourceID& id, const String& clientID = {}, const Size& size = Size::UnspecifiedSize());
	ImageBundle GetResourceBundle(const ResourceID& id, const String& clientID = {});

	ResourceID GetMessageBoxResourceIDs(StdIcon iconID);
	Icon GetMessageBoxResource(StdIcon iconID);
}
