/*
 *  Copyright (C) 2023 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "PictureFolderImageFileLoader.h"

#include "FileItem.h"
#include "FileItemList.h"
#include "Picture.h"
#include "ServiceBroker.h"
#include "TextureCache.h"
#include "filesystem/Directory.h"
#include "guilib/Texture.h"
#include "imagefiles/ImageFileURL.h"
#include "playlists/PlayListFileItemClassify.h"
#include "settings/Settings.h"
#include "settings/SettingsComponent.h"
#include "utils/FileExtensionProvider.h"

using namespace KODI;
using namespace XFILE;

bool CPictureFolderImageFileLoader::CanLoad(const std::string& specialType) const
{
  return specialType == "picturefolder";
}

std::unique_ptr<CTexture> CPictureFolderImageFileLoader::Load(
    const IMAGE_FILES::CImageFileURL& imageFile) const
{
  CFileItemList imagesInFolder;
  CDirectory::GetDirectory(imageFile.GetTargetFile(), imagesInFolder,
                           CServiceBroker::GetFileExtensionProvider().GetPictureExtensions(),
                           DIR_FLAG_NO_FILE_DIRS);

  for (int i = 0; i < imagesInFolder.Size();)
  {
    if (!imagesInFolder[i]->IsPicture() || imagesInFolder[i]->IsZIP() ||
        imagesInFolder[i]->IsRAR() || PLAYLIST::IsPlayList(*imagesInFolder[i]))
    {
      imagesInFolder.Remove(i);
    }
    else
      i++;
  }
  if (imagesInFolder.IsEmpty())
  {
    return {};
  }

  imagesInFolder.Randomize();
  std::vector<std::string> imagesToTile;
  int size = std::min(4, imagesInFolder.Size());
  imagesToTile.reserve(size);
  for (int thumb = 0; thumb < size; thumb++)
  {
    imagesToTile.push_back(imagesInFolder[thumb]->GetPath());
  }

  return CPicture::CreateTiledThumb(imagesToTile);
}
