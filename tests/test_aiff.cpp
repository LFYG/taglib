/***************************************************************************
    copyright           : (C) 2009 by Lukas Lalinsky
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

#include <aifffile.h>
#include <boost/test/unit_test.hpp>
#include "utils.h"
#include "loghelpers.h"

using namespace TagLib;

BOOST_AUTO_TEST_SUITE(TestAIFF)

BOOST_AUTO_TEST_CASE(testAiffProperties)
{
  RIFF::AIFF::File f(TEST_FILE_PATH_C("empty.aiff"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 67);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 706);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 1);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleWidth(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleFrames(), 2941);
  BOOST_CHECK(!f.audioProperties()->isAiffC());
}

BOOST_AUTO_TEST_CASE(testAiffCProperties)
{
  RIFF::AIFF::File f(TEST_FILE_PATH_C("alaw.aifc"));
  BOOST_CHECK(f.audioProperties());
  BOOST_CHECK_EQUAL(f.audioProperties()->length(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInSeconds(), 0);
  BOOST_CHECK_EQUAL(f.audioProperties()->lengthInMilliseconds(), 37);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitrate(), 355);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleRate(), 44100);
  BOOST_CHECK_EQUAL(f.audioProperties()->channels(), 1);
  BOOST_CHECK_EQUAL(f.audioProperties()->bitsPerSample(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleWidth(), 16);
  BOOST_CHECK_EQUAL(f.audioProperties()->sampleFrames(), 1622);
  BOOST_CHECK(f.audioProperties()->isAiffC());
  BOOST_CHECK_EQUAL(f.audioProperties()->compressionType(), "ALAW");
  BOOST_CHECK_EQUAL(f.audioProperties()->compressionName(), "SGI CCITT G.711 A-law");
}

BOOST_AUTO_TEST_CASE(testSaveID3v2)
{
  const ScopedFileCopy copy("empty", ".aiff");
  {
    RIFF::AIFF::File f(copy.fileName());
    BOOST_CHECK(!f.hasID3v2Tag());
  
    f.tag()->setTitle(L"TitleXXX");
    f.save();
    BOOST_CHECK(f.hasID3v2Tag());
  }
  {
    RIFF::AIFF::File f(copy.fileName());
    BOOST_CHECK(f.hasID3v2Tag());
    BOOST_CHECK_EQUAL(f.tag()->title(), L"TitleXXX");
  
    f.tag()->setTitle("");
    f.save();
    BOOST_CHECK(!f.hasID3v2Tag());
  }
  {
    RIFF::AIFF::File f(copy.fileName());
    BOOST_CHECK(!f.hasID3v2Tag());
  }
}

BOOST_AUTO_TEST_CASE(testDuplicateID3v2)
{
  const ScopedFileCopy copy("duplicate_id3v2", ".aiff");
  
  // duplicate_id3v2.aiff has duplicate ID3v2 tag chunks.
  // title() returns "Title2" if can't skip the second tag.
  
  RIFF::AIFF::File f(copy.fileName());
  BOOST_CHECK(f.hasID3v2Tag());
  BOOST_CHECK_EQUAL(f.tag()->title(), "Title1");
  
  f.save();
  BOOST_CHECK_EQUAL(f.length(), 7030);
  BOOST_CHECK_EQUAL(f.find("Title2"), -1);
}

BOOST_AUTO_TEST_CASE(testFuzzedFile1)
{
  RIFF::AIFF::File f(TEST_FILE_PATH_C("segfault.aif"));
  BOOST_CHECK(!f.isValid());
}

BOOST_AUTO_TEST_CASE(testFuzzedFile2)
{
  RIFF::AIFF::File f(TEST_FILE_PATH_C("excessive_alloc.aif"));
  BOOST_CHECK(!f.isValid());
}

BOOST_AUTO_TEST_SUITE_END()
