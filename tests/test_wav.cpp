/***************************************************************************
    copyright           : (C) 2010 by Lukas Lalinsky
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

#include <wavfile.h>
#include <tpropertymap.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestWAV)

BOOST_AUTO_TEST_CASE(testPCMProperties)
{
  RIFF::WAV::File f(TEST_FILE_PATH_C("empty.wav"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3675);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 32);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 1000);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleWidth(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleFrames(), 3675);
  BOOST_CHECK_EQUAL(f.audioProperties()->format(), 1);
}

BOOST_AUTO_TEST_CASE(testALAWProperties)
{
  RIFF::WAV::File f(TEST_FILE_PATH_C("alaw.wav"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3550);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 128);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 8000);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 8);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleWidth(), 8);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleFrames(), 28400);
  BOOST_CHECK_EQUAL(f.audioProperties()->format(), 6);
}

BOOST_AUTO_TEST_CASE(testFloatProperties)
{
  RIFF::WAV::File f(TEST_FILE_PATH_C("float64.wav"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 97);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 5645);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 64);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleWidth(), 64);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleFrames(), 4281);
  BOOST_CHECK_EQUAL(f.audioProperties()->format(), 3);
}

BOOST_AUTO_TEST_CASE(testFactChunkProperties)
{
  RIFF::WAV::File f(TEST_FILE_PATH_C("pcm_with_fact_chunk.wav"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 3);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 3675);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 32);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 2);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 1000);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleWidth(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleFrames(), 3675);
  BOOST_CHECK_EQUAL(f.audioProperties()->format(), 1);
}

BOOST_AUTO_TEST_CASE(testZeroSizeDataChunk)
{
  RIFF::WAV::File f(TEST_FILE_PATH_C("zero-size-chunk.wav"));
  BOOST_CHECK(!f.isValid());
}

BOOST_AUTO_TEST_CASE(testID3v2Tag)
{
  const ScopedFileCopy copy("empty", ".wav");
  {
    RIFF::WAV::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK(!f.hasID3v2Tag());
  
    f.ID3v2Tag()->setTitle(L"Title");
    f.ID3v2Tag()->setArtist(L"Artist");
    f.save();
    BOOST_CHECK(f.hasID3v2Tag());
  }
  {
    RIFF::WAV::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK(f.hasID3v2Tag());
    BOOST_CHECK_EQUAL(f.ID3v2Tag()->title(), L"Title");
    BOOST_CHECK_EQUAL(f.ID3v2Tag()->artist(), L"Artist");
  
    f.ID3v2Tag()->setTitle(L"");
    f.ID3v2Tag()->setArtist(L"");
    f.save();
    BOOST_CHECK(!f.hasID3v2Tag());
  }
  {
    RIFF::WAV::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK(!f.hasID3v2Tag());
    BOOST_CHECK(f.ID3v2Tag()->title().isEmpty());
    BOOST_CHECK(f.ID3v2Tag()->artist().isEmpty());
  }
}

BOOST_AUTO_TEST_CASE(testInfoTag)
{
  const ScopedFileCopy copy("empty", ".wav");
  {
    RIFF::WAV::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK(!f.hasInfoTag());
  
    f.InfoTag()->setTitle(L"Title");
    f.InfoTag()->setArtist(L"Artist");
    f.save();
    BOOST_CHECK(f.hasInfoTag());
  }
  {
    RIFF::WAV::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK(f.hasInfoTag());
    BOOST_CHECK_EQUAL(f.InfoTag()->title(), L"Title");
    BOOST_CHECK_EQUAL(f.InfoTag()->artist(), L"Artist");
  
    f.InfoTag()->setTitle(L"");
    f.InfoTag()->setArtist(L"");
    f.save();
    BOOST_CHECK(!f.hasInfoTag());
  }
  {
    RIFF::WAV::File f(copy.fileName());
    BOOST_CHECK(f.isValid());
    BOOST_CHECK(!f.hasInfoTag());
    BOOST_CHECK(f.InfoTag()->title().isEmpty());
    BOOST_CHECK(f.InfoTag()->artist().isEmpty());
  }
}

BOOST_AUTO_TEST_CASE(testStripTags)
{
  const ScopedFileCopy copy("empty", ".wav");
  {
    RIFF::WAV::File f(copy.fileName());
    f.ID3v2Tag()->setTitle("test title");
    f.InfoTag()->setTitle("test title");
    f.save();
  }
  {
    RIFF::WAV::File f(copy.fileName());
    BOOST_CHECK(f.hasID3v2Tag());
    BOOST_CHECK(f.hasInfoTag());
    f.save(RIFF::WAV::File::ID3v2, true);
  }
  {
    RIFF::WAV::File f(copy.fileName());
    BOOST_CHECK(f.hasID3v2Tag());
    BOOST_CHECK(!f.hasInfoTag());
    f.ID3v2Tag()->setTitle("test title");
    f.InfoTag()->setTitle("test title");
    f.save();
  }
  {
    RIFF::WAV::File f(copy.fileName());
    BOOST_CHECK(f.hasID3v2Tag());
    BOOST_CHECK(f.hasInfoTag());
    f.save(RIFF::WAV::File::Info, true);
  }
  {
    RIFF::WAV::File f(copy.fileName());
    BOOST_CHECK(!f.hasID3v2Tag());
    BOOST_CHECK(f.hasInfoTag());
  }
}

BOOST_AUTO_TEST_CASE(testDuplicateTags)
{
  const ScopedFileCopy copy("duplicate_tags", ".wav");
  
  RIFF::WAV::File f(copy.fileName());
  BOOST_CHECK_EQUAL(f.length(), 17052);
  
  // duplicate_tags.wav has duplicate ID3v2/INFO tags.
  // title() returns "Title2" if can't skip the second tag.
  
  BOOST_CHECK(f.hasID3v2Tag());
  BOOST_CHECK_EQUAL(f.ID3v2Tag()->title(), "Title1");
  
  BOOST_CHECK(f.hasInfoTag());
  BOOST_CHECK_EQUAL(f.InfoTag()->title(), "Title1");
  
  f.save();
  BOOST_CHECK_EQUAL(f.length(), 15898);
  BOOST_CHECK_EQUAL(f.find("Title2"), -1);
}

BOOST_AUTO_TEST_CASE(testFuzzedFile1)
{
  RIFF::WAV::File f1(TEST_FILE_PATH_C("infloop.wav"));
  BOOST_CHECK(!f1.isValid());
}

BOOST_AUTO_TEST_CASE(testFuzzedFile2)
{
  RIFF::WAV::File f2(TEST_FILE_PATH_C("segfault.wav"));
  BOOST_CHECK(f2.isValid());
}

BOOST_AUTO_TEST_CASE(testStripAndProperties)
{
  const ScopedFileCopy copy("empty", ".wav");
  {
    RIFF::WAV::File f(copy.fileName());
    f.ID3v2Tag()->setTitle("ID3v2");
    f.InfoTag()->setTitle("INFO");
    f.save();
  }
  {
    RIFF::WAV::File f(copy.fileName());
    BOOST_CHECK_EQUAL(f.properties()["TITLE"].front(), "ID3v2");
    f.strip(RIFF::WAV::File::ID3v2);
    BOOST_CHECK_EQUAL(f.properties()["TITLE"].front(), "INFO");
    f.strip(RIFF::WAV::File::Info);
    BOOST_CHECK(f.properties().isEmpty());
  }
}

BOOST_AUTO_TEST_SUITE_END()
