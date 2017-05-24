/***************************************************************************
    copyright           : (C) 2007 by Lukas Lalinsky
    email               : lukas@oxygene.sk
 ***************************************************************************/

/***************************************************************************
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#include <fileref.h>
#include <tfilestream.h>
#include <aifffile.h>
#include <apefile.h>
#include <asffile.h>
#include <flacfile.h>
#include <mp4file.h>
#include <mpcfile.h>
#include <mpegfile.h>
#include <oggflacfile.h>
#include <speexfile.h>
#include <trueaudiofile.h>
#include <vorbisfile.h>
#include <wavfile.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

namespace
{
  class DummyResolver : public FileRef::FileTypeResolver
  {
  public:
    virtual File *createFile(FileName fileName, bool, AudioProperties::ReadStyle) const
    {
      return new Ogg::Vorbis::File(fileName);
    }
  };

  template <typename T>
  void fileRefSave(const string &filename, const string &ext)
  {
    const ScopedFileCopy copy(filename, ext);
    {
      FileRef f(copy.fileName());
      BOOST_CHECK(dynamic_cast<T*>(f.file()));
      BOOST_CHECK(!f.isNull());
      f.tag()->setArtist("test artist");
      f.tag()->setTitle("test title");
      f.tag()->setGenre("Test!");
      f.tag()->setAlbum("albummmm");
      f.tag()->setTrack(5);
      f.tag()->setYear(2020);
      f.save();
    }
    {
      FileRef f(copy.fileName());
      BOOST_CHECK(!f.isNull());
      BOOST_CHECK_EQUAL(f.tag()->artist(), "test artist");
      BOOST_CHECK_EQUAL(f.tag()->title(), "test title");
      BOOST_CHECK_EQUAL(f.tag()->genre(), "Test!");
      BOOST_CHECK_EQUAL(f.tag()->album(), "albummmm");
      BOOST_CHECK_EQUAL(f.tag()->track(), 5);
      BOOST_CHECK_EQUAL(f.tag()->year(), 2020);
      f.tag()->setArtist("ttest artist");
      f.tag()->setTitle("ytest title");
      f.tag()->setGenre("uTest!");
      f.tag()->setAlbum("ialbummmm");
      f.tag()->setTrack(7);
      f.tag()->setYear(2080);
      f.save();
    }
    {
      FileRef f(copy.fileName());
      BOOST_CHECK(!f.isNull());
      BOOST_CHECK_EQUAL(f.tag()->artist(), "ttest artist");
      BOOST_CHECK_EQUAL(f.tag()->title(), "ytest title");
      BOOST_CHECK_EQUAL(f.tag()->genre(), "uTest!");
      BOOST_CHECK_EQUAL(f.tag()->album(), "ialbummmm");
      BOOST_CHECK_EQUAL(f.tag()->track(), 7);
      BOOST_CHECK_EQUAL(f.tag()->year(), 2080);
    }
    {
      FileStream fs(copy.fileName());
      FileRef f(&fs);
      BOOST_CHECK(dynamic_cast<T*>(f.file()));
      BOOST_CHECK(!f.isNull());
      BOOST_CHECK_EQUAL(f.tag()->artist(), "ttest artist");
      BOOST_CHECK_EQUAL(f.tag()->title(), "ytest title");
      BOOST_CHECK_EQUAL(f.tag()->genre(), "uTest!");
      BOOST_CHECK_EQUAL(f.tag()->album(), "ialbummmm");
      BOOST_CHECK_EQUAL(f.tag()->track(), 7);
      BOOST_CHECK_EQUAL(f.tag()->year(), 2080);
    }
  }
}

BOOST_AUTO_TEST_SUITE(testFileRef)

BOOST_AUTO_TEST_CASE(testAIFF_1)
{
  fileRefSave<RIFF::AIFF::File>("empty", ".aiff");
}

BOOST_AUTO_TEST_CASE(testAIFF_2)
{
  fileRefSave<RIFF::AIFF::File>("alaw", ".aifc");
}

BOOST_AUTO_TEST_CASE(testAPE)
{
  fileRefSave<APE::File>("mac-399", ".ape");
}

BOOST_AUTO_TEST_CASE(testASF)
{
  fileRefSave<ASF::File>("silence-1", ".wma");
}

BOOST_AUTO_TEST_CASE(testFLAC)
{
  fileRefSave<FLAC::File>("no-tags", ".flac");
}

BOOST_AUTO_TEST_CASE(testMP3)
{
  fileRefSave<MPEG::File>("xing", ".mp3");
}

BOOST_AUTO_TEST_CASE(testMP4_1)
{
  fileRefSave<MP4::File>("has-tags", ".m4a");
}

BOOST_AUTO_TEST_CASE(testMP4_2)
{
  fileRefSave<MP4::File>("no-tags", ".m4a");
}

BOOST_AUTO_TEST_CASE(testMP4_3)
{
  fileRefSave<MP4::File>("no-tags", ".3g2");
}

BOOST_AUTO_TEST_CASE(testMP4_4)
{
  fileRefSave<MP4::File>("blank_video", ".m4v");
}

BOOST_AUTO_TEST_CASE(testMusepack)
{
  fileRefSave<MPC::File>("click", ".mpc");
}

BOOST_AUTO_TEST_CASE(testOGA_FLAC)
{
  fileRefSave<Ogg::FLAC::File>("empty_flac", ".oga");
}

BOOST_AUTO_TEST_CASE(testOGA_Vorbis)
{
  fileRefSave<Ogg::Vorbis::File>("empty_vorbis", ".oga");
}

BOOST_AUTO_TEST_CASE(testSpeex)
{
  fileRefSave<Ogg::Speex::File>("empty", ".spx");
}

BOOST_AUTO_TEST_CASE(testTrueAudio)
{
  fileRefSave<TrueAudio::File>("empty", ".tta");
}

BOOST_AUTO_TEST_CASE(testVorbis)
{
  fileRefSave<Ogg::Vorbis::File>("empty", ".ogg");
}

BOOST_AUTO_TEST_CASE(testWav)
{
  fileRefSave<RIFF::WAV::File>("empty", ".wav");
}

BOOST_AUTO_TEST_CASE(testUnsupported)
{
  const FileRef f1(TEST_FILE_PATH_C("no-extension"));
  BOOST_CHECK(f1.isNull());
  
  const FileRef f2(TEST_FILE_PATH_C("unsupported-extension.xx"));
  BOOST_CHECK(f2.isNull());
}

BOOST_AUTO_TEST_CASE(testFileResolver)
{
  {
    const FileRef f(TEST_FILE_PATH_C("xing.mp3"));
    BOOST_CHECK(dynamic_cast<MPEG::File *>(f.file()));
  }
  
  DummyResolver resolver;
  FileRef::addFileTypeResolver(&resolver);
  
  {
    const FileRef f(TEST_FILE_PATH_C("xing.mp3"));
    BOOST_CHECK(dynamic_cast<Ogg::Vorbis::File *>(f.file()));
  }
}

BOOST_AUTO_TEST_SUITE_END()
